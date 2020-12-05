#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef HAVE_MPI
#include <mpi.h>
#endif
#include "libpressio.h"
//#include "libpressio_ext/io/pressio_io.h"
#include "ZC_LibpressioOpt.h"
#include "zc.h"

struct pressio_data* ZC_constructLibPressioData(void* data, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	enum pressio_dtype dtype = pressio_float_dtype;
	switch(dataType)
	{
	case ZC_FLOAT:
		dtype = pressio_float_dtype;
		break;
	case ZC_DOUBLE:
		dtype = pressio_double_dtype;
		break;
	}
		
	size_t dims[5];
	size_t num_dimensions = (size_t)ZC_constructDimArray(r5, r4, r3, r2, r1, dims);
	
	struct pressio_data* pressioData = pressio_data_new_nonowning(dtype, data, num_dimensions, dims);
	return pressioData;
}

/**
 * compressor_id: sz or zfp
 * */
ZC_evaluation_result* search_cmpr_setting_with_CR(int compressor, void* data, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, 
const float target_cr, const float lower_bound, const float upper_bound)
{
	struct pressio_data* pressioData = ZC_constructLibPressioData(data, dataType, r5, r4, r3, r2, r1);	
	
	struct pressio_options* compressor_options = pressio_options_new();
	if(!compressor_options) {
		fprintf(stderr, "failed to allocate memory\n\n");
		exit(1);
	}
	
	char input[ZC_BUFS];
	char compressor_id[ZC_BUFS];
	switch(compressor)
	{
	case COMPRESSOR_SZ:
		strcpy(compressor_id, "sz");
		pressio_options_set_string(compressor_options,  "sz:error_bound_mode_str", "abs");
		pressio_options_set_string(compressor_options,  "sz:metric", "size");
		strcpy(input, "sz:abs_err_bound");
		break;
	case COMPRESSOR_ZFP:
		strcpy(compressor_id, "zfp");
		pressio_options_set_string(compressor_options,  "zfp:metric", "size");
		strcpy(input, "zfp:accuracy");
		break;
	default:
		printf("Error: unrecoganized compressor in search_cmpr_setting_with_CR\n");
		exit(0);
	}
	
	ZC_evaluation_result* result = search_error_setting_and_compress(target_cr, compressor_id, pressioData, compressor_options, input, lower_bound, upper_bound, "size", 0.1);
	
	return result;
}


ZC_evaluation_result* search_error_setting_and_compress(const float target_cr, const char* compressor_id, struct pressio_data* pressioData, struct pressio_options* compressor_options, const char* input, const double lower_bound, const double upper_bound, const char* metric, double global_rel_tolerance)
{	
	ZC_evaluation_result* evalResult = (ZC_evaluation_result*)malloc(sizeof(ZC_evaluation_result));
	memset(evalResult, 0, sizeof(ZC_evaluation_result));
	struct pressio* library = pressio_instance();
	struct pressio_compressor* compressor = pressio_get_compressor(library, "opt");
	if(!compressor) {
		fprintf(stderr, "failed to load compressor: %s\n", pressio_error_msg(library));
		exit(pressio_error_code(library));
	}	
	const char* inputs[] = {input};
	const char* outputs[] = {"size:compression_ratio"};
	const double lower_bounds[] = {lower_bound};
	const double upper_bounds[] = {upper_bound};
	size_t bound_dims[] = {sizeof(lower_bounds)/ sizeof(lower_bounds[0])};
	struct pressio_data* lower_bound_data = pressio_data_new_nonowning(pressio_double_dtype, (void*)lower_bounds, 1, bound_dims);
	struct pressio_data* upper_bound_data = pressio_data_new_nonowning(pressio_double_dtype, (void*)upper_bounds, 1, bound_dims);

	pressio_options_set_data(compressor_options,    "opt:lower_bound", lower_bound_data);
	pressio_options_set_data(compressor_options,    "opt:upper_bound", upper_bound_data);
	pressio_options_set_double(compressor_options,  "opt:target", target_cr);
	pressio_options_set_double(compressor_options,  "opt:global_rel_tolerance", global_rel_tolerance);
	pressio_options_set_integer(compressor_options, "opt:do_decompress", 0); //NOTE compression ratio doesn't require decompression
	pressio_options_set_string(compressor_options,  "opt:metric", "size");
	pressio_options_set_string(compressor_options,  "opt:objective_mode_name", "target");
	pressio_options_set_string(compressor_options,  "opt:search", "fraz");
	pressio_options_set_string(compressor_options,  "opt:search_metrics", "composite_search");
	pressio_options_set_strings(compressor_options, "opt:inputs", 1, inputs);
	pressio_options_set_strings(compressor_options, "opt:output", 1, outputs);	

	pressio_options_set_string(compressor_options,  "opt:compressor", compressor_id);	
	
	if(pressio_compressor_set_options(compressor, compressor_options)) {
		fprintf(stderr, "failed to set options: %s\n", pressio_compressor_error_msg(compressor));
		exit(pressio_compressor_error_code(compressor));
	}
	pressio_options_free(compressor_options);	
	
	struct pressio_data* compressed = pressio_data_new_empty(pressio_byte_dtype, 0, NULL);	
	
	if(pressio_compressor_compress(compressor, pressioData, compressed)) {
		fprintf(stderr, "failed to configure compressor: %s\n", pressio_compressor_error_msg(compressor));
		exit(pressio_compressor_error_code(compressor));
	}
	
	double achieved_cr;
	struct pressio_data* parameters = pressio_data_new_empty(pressio_double_dtype, 0, NULL);
	struct pressio_options* metrics_results = pressio_compressor_get_metrics_results(compressor);
	if(!metrics_results) {
		fprintf(stderr, "failed to get metrics results: %s\n", pressio_compressor_error_msg(compressor));
		exit(pressio_compressor_error_code(compressor));
	}

	if(pressio_options_get_double(metrics_results, "size:compression_ratio", &(evalResult->metric)) != pressio_options_key_set) {
		fprintf(stderr, "failed to get compression_ratio: %s\n", pressio_compressor_error_msg(compressor));
		exit(pressio_compressor_error_code(compressor));
	}
	if(pressio_options_get_data(metrics_results, "opt:input", &parameters) != pressio_options_key_set) {
		fprintf(stderr, "failed to get output: %s\n", pressio_compressor_error_msg(compressor));
		exit(pressio_compressor_error_code(compressor));
	}
	evalResult->error_bound = ((double*)pressio_data_ptr(parameters, NULL))[0];
	pressio_data_free(parameters);

	pressio_compressor_release(compressor);
	pressio_release(library);	
	
	return evalResult;
}


