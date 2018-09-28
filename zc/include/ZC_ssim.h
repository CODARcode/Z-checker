#ifndef _ZC_SSIM_H
#define _ZC_SSIM_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
double zc_calc_ssim_1d_float(const float *org, const float *rec, const size_t r1);
double zc_calc_ssim_1d_double(const double *org, const double *rec, const size_t r1);

double zc_calc_ssim_2d_float(const float *org, const float *rec,
                  const size_t r2, const size_t r1); //r2 is height, r1 is width
double zc_calc_ssim_2d_double(const double *org, const double *rec,
                  const size_t r2, const size_t r1);
                  
void zc_calc_ssim_3d_float(float *org, float *rec, size_t r3, size_t r2, size_t r1, double *min_ssim, double* avg_ssim, double* max_ssim);
void zc_calc_ssim_3d_double(double *org, double *rec, size_t r3, size_t r2, size_t r1, double *min_ssim, double* avg_ssim, double* max_ssim);

//mpi interfaces
double zc_calc_ssim_2d_float_online(const float *org, const float *rec, const size_t r2, const size_t r1);
double zc_calc_ssim_2d_double_online(const double *org, const double *rec, const size_t r2, const size_t r1);

void zc_calc_ssim_3d_float_online(float *org, float *rec, size_t r3, size_t r2, size_t r1, double *global_min_ssim, double* global_avg_ssim, double* global_max_ssim);
void zc_calc_ssim_3d_double_online(double *org, double *rec, size_t r3, size_t r2, size_t r1, double *global_min_ssim, double* global_avg_ssim, double* global_max_ssim);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_SSIMF_H  ----- */
