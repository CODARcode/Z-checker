#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "ZC_util.h"
#include "ZC_DataProperty.h"
#include "ZC_CompareData.h"
#include "zc.h"
#ifdef HAVE_FFTW3
#include "ZC_FFTW3_math.h"
#endif
#ifdef HAVE_R
#include "ZC_R_math.h"
#endif
#include "ZC_ssim.h"

#include "matrix.hpp"

void ZC_compareData_double(ZC_CompareData* compareResult, double* data1, double* data2,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i = 0;
	double minDiff = data2[0]-data1[0];
	double maxDiff = minDiff;
	double minErr = fabs(minDiff);
	double maxErr = minErr;
	double sum1 = 0, sum2 = 0, sumDiff = 0, sumErr = 0, sumErrSqr = 0;
	
	double minDiff_rel = 1E100;
	double maxDiff_rel = -1E100;
	double minErr_rel = 1E100;
	double maxErr_rel = 0;
	double sumDiff_rel = 0, sumErr_rel = 0, sumErrSqr_rel = 0;
	
	double err;
	size_t numOfElem = compareResult->property->numOfElem;
	double sumOfDiffSquare = 0, sumOfDiffSquare_rel = 0;
	size_t numOfElem_ = 0;

	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);

	double *diff = (double*)malloc(numOfElem*sizeof(double));
	double *relDiff = (double*)malloc(numOfElem*sizeof(double));

	for (i = 0; i < numOfElem; i++)
	{
		sum1 += data1[i];
		sum2 += data2[i];
		
		diff[i] = data2[i]-data1[i];
		if(minDiff > diff[i]) minDiff = diff[i];
		if(maxDiff < diff[i]) maxDiff = diff[i];
		sumDiff += diff[i];
		sumOfDiffSquare += diff[i]*diff[i];
				
		err = fabs(diff[i]);
		if(minErr>err) minErr = err;
		if(maxErr<err) maxErr = err;
		sumErr += err;
		sumErrSqr += err*err; //used for mse, nrmse, psnr
	
		if(data1[i]!=0)
		{
			numOfElem_ ++;
			relDiff[i] = diff[i]/data1[i];
			if(minDiff_rel > relDiff[i]) minDiff_rel = relDiff[i];
			if(maxDiff_rel < relDiff[i]) maxDiff_rel = relDiff[i];
			sumDiff_rel += relDiff[i];
			sumOfDiffSquare_rel += relDiff[i]*relDiff[i];
			
			err = fabs(relDiff[i]);
			if(minErr_rel>err) minErr_rel = err;
			if(maxErr_rel<err) maxErr_rel = err;
			sumErr_rel += err;
			sumErrSqr_rel += err*err;
		}	
	}
	
	ZC_DataProperty* property = compareResult->property;
	
	double zeromean_variance = property->zeromean_variance;
	double valRange = property->valueRange;
	double mean1 = sum1/numOfElem;
	double mean2 = sum2/numOfElem;
	
	double avgDiff = sumDiff/numOfElem;
	double avgErr = sumErr/numOfElem;
	
	double diffRange = maxDiff - minDiff;
	double mse = sumErrSqr/numOfElem;
	
	//double avgErr_rel = sumErr_rel/numOfElem;
	double diffRange_rel = maxDiff_rel - minDiff_rel;
	//double mse_rel = sumErrSqr_rel/numOfElem_;
	if(diffRange_rel>2*PWR_DIS_RNG_BOUND)
	{
		double avg = 0;//sumDiff_rel/numOfElem_;
		diffRange_rel = 2*PWR_DIS_RNG_BOUND;
		minDiff_rel = avg-PWR_DIS_RNG_BOUND;
		maxDiff_rel = avg+PWR_DIS_RNG_BOUND;
	}
	
	int index;
	
	if (minAbsErrFlag)
		compareResult->minAbsErr = minErr;

	if (minRelErrFlag)
		compareResult->minRelErr = minErr/valRange;

	if (maxAbsErrFlag)
		compareResult->maxAbsErr = maxErr;

	if (maxRelErrFlag)
		compareResult->maxRelErr = maxErr/valRange;

	if (avgAbsErrFlag)
		compareResult->avgAbsErr = avgErr;

	if (avgRelErrFlag)
		compareResult->avgRelErr = avgErr/valRange;
		
	compareResult->minPWRErr = minErr_rel;
	compareResult->maxPWRErr = maxErr_rel;
	compareResult->avgPWRErr = sumErr_rel/numOfElem_;

	if (absErrPDFFlag)
	{
		double interval = diffRange/PDF_INTERVALS;
		double *absErrPDF = NULL;
		if(interval==0)
		{
			absErrPDF = (double*)malloc(sizeof(double));
			*absErrPDF = 0;
		}
		else
		{
			absErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS);
			memset(absErrPDF, 0, PDF_INTERVALS*sizeof(double));

			for (i = 0; i < numOfElem; i++)
			{
				index = (int)((diff[i]-minDiff)/interval);
				if(index==PDF_INTERVALS)
					index = PDF_INTERVALS-1;
				absErrPDF[index] += 1;
			}

			for (i = 0; i < PDF_INTERVALS; i++)
				absErrPDF[i]/=numOfElem;			
		}
		compareResult->absErrPDF = absErrPDF;
		compareResult->err_interval = interval;
		compareResult->err_minValue = minDiff;		
	}

	if (pwrErrPDFFlag)
	{
		double interval = diffRange_rel/PDF_INTERVALS_REL;
		double *relErrPDF = NULL;
		if(interval==0)
		{
			relErrPDF = (double*)malloc(sizeof(double));
			*relErrPDF = 0;
		}
		else
		{
			relErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS_REL);
			memset(relErrPDF, 0, PDF_INTERVALS_REL*sizeof(double));

			for (i = 0; i < numOfElem; i++)
			{
				if(data1[i]!=0)
				{
					if(relDiff[i]>maxDiff_rel)
						relDiff[i] = maxDiff_rel;
					if(relDiff[i]<minDiff_rel)
						relDiff[i] = minDiff_rel;
					index = (int)((relDiff[i]-minDiff_rel)/interval);
					if(index==PDF_INTERVALS_REL)
						index = PDF_INTERVALS_REL-1;
					relErrPDF[index] += 1;					
				}
			}
			for (i = 0; i < PDF_INTERVALS_REL; i++)
				relErrPDF[i]/=numOfElem_;			
		}
		compareResult->pwrErrPDF = relErrPDF;
		compareResult->err_interval_rel = interval;
		compareResult->err_minValue_rel = minDiff_rel;		
	}

	if (errAutoCorrFlag)
	{
		compareResult->autoCorrAbsErr = ZC_compute_autocorrelation1D_double(diff, numOfElem, avgDiff);
	}

#ifdef HAVE_FFTW3	
	if(errAutoCorr3DFlag)
	{
		switch(dim)
		{
		case 1:
			compareResult->autoCorrAbsErr3D = autocorr_3d_double(diff, r1, 1, 1);
			break;
		case 2:
			compareResult->autoCorrAbsErr3D = autocorr_3d_double(diff, r1, r2, 1);
			break;
		case 3:
			compareResult->autoCorrAbsErr3D = autocorr_3d_double(diff, r1, r2, r3);
			break;
		case 4:
			compareResult->autoCorrAbsErr3D = autocorr_3d_double(diff, r1, r2, r3*r4);
			break;
		case 5:
			compareResult->autoCorrAbsErr3D = autocorr_3d_double(diff, r1, r2, r3*r4*r5);
			break;
		default: 
			printf("Error: wrong dimension (dim=%d)\n", dim);
			exit(0);
		}		
	}
#endif

	if (pearsonCorrFlag)
	{
		double prodSum = 0, sum1 = 0, sum2 = 0;
    	for (i = 0; i < numOfElem; i++)
    	{
    		prodSum += (data1[i]-mean1)*(data2[i]-mean2);
    		sum1 += (data1[i]-mean1)*(data1[i]-mean1);
    		sum2 += (data2[i]-mean2)*(data2[i]-mean2);
    	}

    	double std1 = sqrt(sum1/numOfElem);
    	double std2 = sqrt(sum2/numOfElem);
    	double ee = prodSum/numOfElem;
    	double pearsonCorr = 0;

    	if (std1*std2 != 0)
    		pearsonCorr = ee/std1/std2;

    	compareResult->pearsonCorr = pearsonCorr;
	}

	if (rmseFlag)
	{
		double rmse = sqrt(mse);
		compareResult->rmse = rmse;
	}

	if (nrmseFlag)
	{
		double nrmse = sqrt(mse)/valRange;
		compareResult->nrmse = nrmse;
	}

	if(snrFlag)
	{
		compareResult->snr = 10*log10(zeromean_variance/mse);
		//printf("compareResult->snr=%f ccompareResult->snr_db=%f", compareResult->snr, compareResult->snr_db); 		
	}

	if (psnrFlag)
	{
		double psnr = -20.0*log10(sqrt(mse)/valRange);
		compareResult->psnr = psnr;
	}

	//the correlation between the original data values and the compression errors
	if (valErrCorrFlag)
	{
		double prodSum = 0, sum1 = 0, sumDiff = 0;
    	for (i = 0; i < numOfElem; i++)
    	{
    		prodSum += (data1[i]-mean1)*(diff[i]-avgDiff);
    		sum1 += (data1[i]-mean1)*(data1[i]-mean1);
    		sumDiff += (diff[i]-avgDiff)*(diff[i]-avgDiff);
    	}

    	double std1 = sqrt(sum1/numOfElem);
    	double stdDiff = sqrt(sumDiff/numOfElem);
    	double ee = prodSum/numOfElem;
      	double valErrCorr = 0;

    	if (std1*stdDiff != 0)
		{
			valErrCorr = ee/std1/stdDiff;
		}
		
		compareResult->valErrCorr = valErrCorr;
	}

#ifdef HAVE_R
	if(KS_testFlag)
	{
		compareResult->ksValue = KS_test(compareResult->property->dataType, data1, data2, r5, r4, r3, r2, r1);
	}
	
	if(SSIMFlag)
	{
		double* ssimResult = SSIM(compareResult->property->dataType, data1, data2, r5, r4, r3, r2, r1);
		compareResult->lum = ssimResult[0];
		compareResult->cont = ssimResult[1];
		compareResult->struc = ssimResult[2];
		compareResult->ssim = ssimResult[3];
	}
#endif

	if(SSIMIMAGE2DFlag)
	{
		switch(dim)
		{
		case 2:
			compareResult->ssimImage2D_avg = zc_calc_ssim_2d_double(data1, data2, r2, r1);	
			break;
		case 3:
			zc_calc_ssim_3d_double(data1, data2, r3, r2, r1, &(compareResult->ssimImage2D_min), &(compareResult->ssimImage2D_avg), &(compareResult->ssimImage2D_max));
			break;
		case 4:
			zc_calc_ssim_3d_double(data1, data2, r4*r3, r2, r1, &(compareResult->ssimImage2D_min), &(compareResult->ssimImage2D_avg), &(compareResult->ssimImage2D_max));
			break;
		case 5:
			zc_calc_ssim_3d_double(data1, data2, r5*r4*r3, r2, r1, &(compareResult->ssimImage2D_min), &(compareResult->ssimImage2D_avg), &(compareResult->ssimImage2D_max));
			break;
		default: //1D data is meaningless here
			compareResult->ssimImage2D_min = 0; 
			compareResult->ssimImage2D_avg = -1;
			compareResult->ssimImage2D_max = -2;
		}
	}


	if(derivativeOrder1_sep_maxDiffFlag || derivativeOrder1_sep_psnrFlag || derivativeOrder1_sep_ssimFlag)
	{	
		int status = 0;
		zc_analyze_der_order1_double(data1, data2, dim, r4, r3, r2, r1, &status, 
		&(compareResult->maxDx), &(compareResult->maxDy), &(compareResult->maxDz), &(compareResult->maxDt),
		&(compareResult->maxErrDx), &(compareResult->maxErrDy), &(compareResult->maxErrDz), &(compareResult->maxErrDt),
		&(compareResult->psnrDx), &(compareResult->psnrDy), &(compareResult->psnrDt), &(compareResult->psnrDz),
		&(compareResult->ssimDx), &(compareResult->ssimDy), &(compareResult->ssimDz), &(compareResult->ssimDt));
	}

	//dimensions: r5, r4, r3, r2, r1, data1, data2 --> compareResult->derivativeOrder1 and compareResult->derivativeOrder2
	if(derivativeOrder1_psnrFlag)
	{
		int status = 0; //0: normal , 1: abnormal
		
		//double der_Order1_PSNR_Metric=zc_calc_metric_der_order1_psnr_double(data1,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
		compareResult->derivativeOrder1_psnr = zc_calc_metric_der_order1_psnr_double(data1, data2, dim, r4, r3, r2, r1, &status);
		if(status!=0)
			compareResult->derivativeOrder1_psnr = -1; //invalid dimension settings for this metric
	}
	
	if(derivativeOrder2_psnrFlag)
	{
		int status = 0; //0: normal , 1: abnormal
		
		//double der_Order1_PSNR_Metric=zc_calc_metric_der_order1_psnr_double(data1,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
		compareResult->derivativeOrder2_psnr = zc_calc_metric_der_order2_psnr_double(data1, data2, dim, r4, r3, r2, r1, &status);
		if(status!=0)
			compareResult->derivativeOrder2_psnr = -1; //invalid dimension settings for this metric
	}	

	if(derivativeOrder1_ssimFlag)
	{
		int status = 0; //0: normal , 1: abnormal
		
		//double der_Order1_PSNR_Metric=zc_calc_metric_der_order1_psnr_double(data1,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
		compareResult->derivativeOrder1_ssim = zc_calc_metric_der_order1_ssim_double(data1, data2, dim, r4, r3, r2, r1, &status);
		if(status!=0)
			compareResult->derivativeOrder1_ssim = -1; //invalid dimension settings for this metric
	}	
	
	if(derivativeOrder2_ssimFlag)
	{
		int status = 0; //0: normal , 1: abnormal
		
		//double der_Order1_PSNR_Metric=zc_calc_metric_der_order1_psnr_double(data1,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
		compareResult->derivativeOrder2_ssim = zc_calc_metric_der_order2_ssim_double(data1, data2, dim, r4, r3, r2, r1, &status);
		if(status!=0)
			compareResult->derivativeOrder2_ssim = -1; //invalid dimension settings for this metric
	}

	if(derivativeOrder1_sobolevFlag)
	{
		int status = 0;
		compareResult->derivative1_sobolev = zc_calc_metric_der_order1_sobolev_p2_double(data1, data2, dim, r4, r3, r2, r1, &status);
		if(status!=0)
			compareResult->derivative1_sobolev = -1;
	}

	free(diff);
	free(relDiff);

}

#ifdef HAVE_MPI

void ZC_compareData_double_online(ZC_CompareData* compareResult, double* data1, double* data2, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i = 0;
	double minDiff = data2[0]-data1[0];
	double maxDiff = minDiff;
	double minErr = fabs(minDiff);
	double maxErr = minErr;
	double sum1 = 0, sum2 = 0, sumDiff = 0, sumErr = 0, sumErrSqr = 0;
	
	double minDiff_rel = 1E100;
	double maxDiff_rel = -1E100;
	double minErr_rel = 1E100;
	double maxErr_rel = 0;
	double sumDiff_rel = 0, sumErr_rel = 0, sumErrSqr_rel = 0;
	
	double err;
	long numOfElem = ZC_computeDataLength(r5, r4, r3, r2, r1);	
		
	if(globalDataLength <= 0)
		MPI_Allreduce(&numOfElem, &globalDataLength, 1, MPI_LONG, MPI_SUM, ZC_COMM_WORLD);
			
	double sumOfDiffSquare = 0;
	long numOfElem_ = 0; //used to record the number of elements for relative-error-bound cases

	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);

	double *diff = (double*)malloc(numOfElem*sizeof(double));
	double *relDiff = (double*)malloc(numOfElem*sizeof(double));

	for (i = 0; i < numOfElem; i++)
	{
		sum1 += data1[i];
		sum2 += data2[i];
		
		diff[i] = data2[i]-data1[i];
		if(minDiff > diff[i]) minDiff = diff[i];
		if(maxDiff < diff[i]) maxDiff = diff[i];
		sumDiff += diff[i];
		sumOfDiffSquare += diff[i]*diff[i];
				
		err = fabs(diff[i]);
		if(minErr>err) minErr = err;
		if(maxErr<err) maxErr = err;
		sumErr += err;
		sumErrSqr += err*err; //used for mse, nrmse, psnr
	
		if(data1[i]!=0)
		{
			numOfElem_ ++;
			relDiff[i] = diff[i]/data1[i];
			if(minDiff_rel > relDiff[i]) minDiff_rel = relDiff[i];
			if(maxDiff_rel < relDiff[i]) maxDiff_rel = relDiff[i];
			sumDiff_rel += relDiff[i];
			
			err = fabs(relDiff[i]);
			if(minErr_rel>err) minErr_rel = err;
			if(maxErr_rel<err) maxErr_rel = err;
			sumErr_rel += err;
			sumErrSqr_rel += err*err;
		}	
	}
	
	ZC_DataProperty* property = compareResult->property;
	
	double zeromean_variance = property->zeromean_variance;
	double global_valRange = property->valueRange;
	
	double global_sum1, global_sum2, global_sumErr, global_maxDiff, global_minDiff, global_sumErrSqr, global_sumDiff;
	double global_maxErr_rel, global_minErr_rel, global_sumErr_rel, global_maxErr, global_minErr, global_maxDiff_rel, global_minDiff_rel; 
	long global_numOfElem_;
	//Compute the global sum and mean 
	
	double sum_localBuffer[6], sum_globalBuffer[6];
	double max_localBuffer[4], max_globalBuffer[4];
	double min_localBuffer[4], min_globalBuffer[4];
	
	sum_localBuffer[0] = sum1;
	sum_localBuffer[1] = sum2;
	sum_localBuffer[2] = sumErr;
	sum_localBuffer[3] = sumErrSqr;
	sum_localBuffer[4] = sumErr_rel;
	sum_localBuffer[5] = sumDiff;
	max_localBuffer[0] = maxErr;
	max_localBuffer[1] = maxDiff;
	max_localBuffer[2] = maxErr_rel;
	max_localBuffer[3] = maxDiff_rel;
	min_localBuffer[0] = minErr;
	min_localBuffer[1] = minDiff;
	min_localBuffer[2] = minErr_rel;
	min_localBuffer[3] = minDiff_rel;
	
	MPI_Allreduce(sum_localBuffer, sum_globalBuffer, 6, MPI_DOUBLE, MPI_SUM, ZC_COMM_WORLD);
	MPI_Allreduce(max_localBuffer, max_globalBuffer, 4, MPI_DOUBLE, MPI_MAX, ZC_COMM_WORLD);
	MPI_Allreduce(min_localBuffer, min_globalBuffer, 4, MPI_DOUBLE, MPI_MIN, ZC_COMM_WORLD);	
	MPI_Allreduce(&numOfElem_, &global_numOfElem_, 1, MPI_LONG, MPI_SUM, ZC_COMM_WORLD);
			
	global_sum1 = sum_globalBuffer[0];
	global_sum2 = sum_globalBuffer[1];
	global_sumErr = sum_globalBuffer[2];
	global_sumErrSqr = sum_globalBuffer[3];
	global_sumErr_rel = sum_globalBuffer[4];
	global_sumDiff = sum_globalBuffer[5];
	
	global_maxErr = max_globalBuffer[0];
	global_maxDiff = max_globalBuffer[1];
	global_maxErr_rel = max_globalBuffer[2];
	global_maxDiff_rel = max_globalBuffer[3];
	
	global_minErr = min_globalBuffer[0];
	global_minDiff = min_globalBuffer[1];
	global_minErr_rel = min_globalBuffer[2];	
	global_minDiff_rel = min_globalBuffer[3];
			
	double global_mean1 = global_sum1/globalDataLength;
	double global_mean2 = global_sum2/globalDataLength;
	
	double global_avgErr = global_sumErr/globalDataLength;
	double global_diffRange = global_maxDiff - global_minDiff;
	double global_mse = global_sumErrSqr/globalDataLength;
	
	//double global_avgErr_rel = global_sumErr_rel/global_numOfElem_;
	double global_avgDiff = global_sumDiff/globalDataLength;
	
	double global_diffRange_rel = global_maxDiff_rel - global_minDiff_rel;
		
	size_t index;
	
	if (minAbsErrFlag)
		compareResult->minAbsErr = global_minErr;

	if (minRelErrFlag)
		compareResult->minRelErr = global_minErr/global_valRange;

	if (maxAbsErrFlag)
		compareResult->maxAbsErr = global_maxErr;

	if (maxRelErrFlag)
		compareResult->maxRelErr = global_maxErr/global_valRange;

	if (avgAbsErrFlag)
		compareResult->avgAbsErr = global_avgErr;

	if (avgRelErrFlag)
		compareResult->avgRelErr = global_avgErr/global_valRange;
		
	compareResult->minPWRErr = global_minErr_rel;
	compareResult->maxPWRErr = global_maxErr_rel;
	compareResult->avgPWRErr = global_sumErr_rel/global_numOfElem_;

	if (absErrPDFFlag)
	{
		double interval = global_diffRange/PDF_INTERVALS;
		double *absErrPDF = NULL, *global_absErrPDF = NULL; 
						
		if(interval==0)
		{
			global_absErrPDF = (double*)malloc(sizeof(double));
			*global_absErrPDF = 0;
		}
		else
		{
			absErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS);
			memset(absErrPDF, 0, PDF_INTERVALS*sizeof(double));
			global_absErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS);
			memset(global_absErrPDF, 0, PDF_INTERVALS*sizeof(double));
			
			for (i = 0; i < numOfElem; i++)
			{
				index = (size_t)((diff[i]-global_minDiff)/interval);
				if(index==PDF_INTERVALS)
					index = PDF_INTERVALS-1;
				absErrPDF[index] += 1;
			}

			MPI_Reduce(absErrPDF, global_absErrPDF, PDF_INTERVALS, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
			
			free(absErrPDF);
			
			if(myRank==0)
			{
				for (i = 0; i < PDF_INTERVALS; i++)
					global_absErrPDF[i]/=globalDataLength;						
			}
		}
		if(myRank==0)
		{
			compareResult->absErrPDF = global_absErrPDF;
			compareResult->err_interval = interval;
			compareResult->err_minValue = global_minDiff;					
		}
	}

	if (pwrErrPDFFlag)
	{
		double interval = global_diffRange_rel/PDF_INTERVALS_REL;
		double *relErrPDF = NULL, *global_relErrPDF = NULL;
		if(interval==0)
		{
			relErrPDF = (double*)malloc(sizeof(double));
			*relErrPDF = 0;
		}
		else
		{			
			relErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS_REL);
			memset(relErrPDF, 0, PDF_INTERVALS_REL*sizeof(double));
			global_relErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS_REL);
			memset(global_relErrPDF, 0, PDF_INTERVALS_REL*sizeof(double));	
			
			for (i = 0; i < numOfElem; i++)
			{
				if(data1[i]!=0)
				{
					if(relDiff[i]>maxDiff_rel)
						relDiff[i] = maxDiff_rel;
					if(relDiff[i]<minDiff_rel)
						relDiff[i] = minDiff_rel;
					index = (size_t)((relDiff[i]-minDiff_rel)/interval);
					if(index==PDF_INTERVALS_REL)
						index = PDF_INTERVALS_REL-1;
					relErrPDF[index] += 1;
				}
			}
			
			MPI_Reduce(relErrPDF, global_relErrPDF, PDF_INTERVALS, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);			
			
			free(relErrPDF);
			
			if(myRank==0)
			{
				for (i = 0; i < PDF_INTERVALS_REL; i++)
					global_relErrPDF[i]/=global_numOfElem_;						
			}		
		}
		if(myRank==0)
		{
			compareResult->pwrErrPDF = relErrPDF;
			compareResult->err_interval_rel = interval;
			compareResult->err_minValue_rel = minDiff_rel;			
		}		
	}

	if (errAutoCorrFlag)
	{	
		double *autoCorrAbsErr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));
		
		double ccov[AUTOCORR_SIZE+1], gcov[AUTOCORR_SIZE+1];
		memset(ccov, 0, sizeof(double)*(AUTOCORR_SIZE+1));
		memset(gcov, 0, sizeof(double)*(AUTOCORR_SIZE+1));
		
		int delta;
		double var = 0, gvar = 0;
		for (i = 0; i < numOfElem; i++)
			var += (diff[i] - global_avgDiff)*(diff[i] - global_avgDiff);
		
		for(delta = 1; delta <= AUTOCORR_SIZE; delta++)
		{
			double cov = 0;
			for (i = 0; i < numOfElem-delta; i++)
				cov += (diff[i] - global_avgDiff)*(diff[i+delta] - global_avgDiff);
			ccov[delta] = cov;	
		}
		
		MPI_Reduce(&var, &gvar, 1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
		MPI_Reduce(ccov, gcov, AUTOCORR_SIZE+1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);		
		
		if(myRank==0)
		{	
			gvar = gvar/globalDataLength;
			if (gvar == 0)
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
					autoCorrAbsErr[delta] = 1;
			}
			else
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
				{
					autoCorrAbsErr[delta] = gcov[delta]/(globalDataLength-nbProc*delta)/gvar;				
				}
			}
			autoCorrAbsErr[0] = 1;
			compareResult->autoCorrAbsErr = autoCorrAbsErr;					
		}		
	}

	double p_localBuffer[3], p_globalBuffer[3];

	if (pearsonCorrFlag)
	{
		double prodSum = 0, sum1 = 0, sum2 = 0;
    	for (i = 0; i < numOfElem; i++)
    	{
    		prodSum += (data1[i]-global_mean1)*(data2[i]-global_mean2);
    		sum1 += (data1[i]-global_mean1)*(data1[i]-global_mean1);
    		sum2 += (data2[i]-global_mean2)*(data2[i]-global_mean2);
    	}

		p_localBuffer[0] = prodSum;
		p_localBuffer[1] = sum1;
		p_localBuffer[2] = sum2;
		
		MPI_Reduce(p_localBuffer, p_globalBuffer, 3, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
		
		if(myRank==0)
		{
			double global_std1, global_std2, global_ee, global_pearsonCorr = 0;
			global_std1 = sqrt(p_globalBuffer[1]/globalDataLength);
			global_std2 = sqrt(p_globalBuffer[2]/globalDataLength);
			global_ee = p_globalBuffer[0]/globalDataLength;
			if(global_std1*global_std2 != 0)
				global_pearsonCorr = global_ee/global_std1/global_std2;
			compareResult->pearsonCorr = global_pearsonCorr;
		}
	}

	if (rmseFlag)
	{
		double global_rmse = sqrt(global_mse);
		compareResult->rmse = global_rmse;
	}

	if (nrmseFlag)
	{
		double nrmse = sqrt(global_mse)/global_valRange;
		compareResult->nrmse = nrmse;
	}

	if(snrFlag)
	{
		compareResult->snr = 10*log10(zeromean_variance/global_mse);
	}

	if (psnrFlag)
	{
		double psnr = -20.0*log10(sqrt(global_mse)/global_valRange);
		compareResult->psnr = psnr;
	}

	if (valErrCorrFlag)
	{
		double prodSum = 0, sum1 = 0, sumDiff = 0;
    	for (i = 0; i < numOfElem; i++)
    	{
    		prodSum += (data1[i]-global_mean1)*(diff[i]-global_avgDiff);
    		sum1 += (data1[i]-global_mean1)*(data1[i]-global_mean1);
    		sumDiff += (diff[i]-global_avgDiff)*(diff[i]-global_avgDiff);
    	}

		p_localBuffer[0] = prodSum;
		p_localBuffer[1] = sum1;
		p_localBuffer[2] = sumDiff;
		
		MPI_Reduce(p_localBuffer, p_globalBuffer, 3, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
		
		if(myRank==0)
		{
			double global_std1, global_ee, global_stdDiff, global_valErrCorr = 0;				
			global_std1 = sqrt(p_globalBuffer[1]/globalDataLength);
			global_stdDiff = sqrt(p_globalBuffer[2]/globalDataLength);
			global_ee = p_globalBuffer[0]/globalDataLength;

			if (global_std1*global_stdDiff != 0)
				global_valErrCorr = global_ee/global_std1/global_stdDiff;

			compareResult->valErrCorr = global_valErrCorr;			
		}
	}

#ifdef HAVE_R
	if(KS_testFlag)
	{
		compareResult->ksValue = KS_test(compareResult->property->dataType, data1, data2, r5, r4, r3, r2, r1);
	}
	
	if(SSIMFlag)
	{
		double* ssimResult = SSIM3(compareResult->property->dataType, data1, data2, r5, r4, r3, r2, r1);
		compareResult->lum = ssimResult[0];
		compareResult->cont = ssimResult[1];
		compareResult->struc = ssimResult[2];
		compareResult->ssim = ssimResult[3];
	}
#endif

	if(SSIMIMAGE2DFlag)
	{
		switch(dim)
		{
		case 2:
			compareResult->ssimImage2D_avg = zc_calc_ssim_2d_double_online(data1, data2, r2, r1);	
			break;
		case 3:
			zc_calc_ssim_3d_double_online(data1, data2, r3, r2, r1, &(compareResult->ssimImage2D_min), &(compareResult->ssimImage2D_avg), &(compareResult->ssimImage2D_max));
			break;
		case 4:
			zc_calc_ssim_3d_double_online(data1, data2, r4*r3, r2, r1, &(compareResult->ssimImage2D_min), &(compareResult->ssimImage2D_avg), &(compareResult->ssimImage2D_max));
			break;
		case 5:
			zc_calc_ssim_3d_double_online(data1, data2, r5*r4*r3, r2, r1, &(compareResult->ssimImage2D_min), &(compareResult->ssimImage2D_avg), &(compareResult->ssimImage2D_max));
			break;
		default: //1D data is meaningless here
			compareResult->ssimImage2D_min = 0; 
			compareResult->ssimImage2D_avg = -1;
			compareResult->ssimImage2D_max = -2;
		}
	}

	free(diff);
	free(relDiff);	
}

#endif
