#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "ZC_util.h"
#include "ZC_DataProperty.h"
#include "ZC_CompareData.h"
#include "zc.h"

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
	
	double avgErr_rel = sumErr_rel/numOfElem;
	double diffRange_rel = maxDiff_rel - minDiff_rel;
	double mse_rel = sumErrSqr_rel/numOfElem_;
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

	if (autoCorrAbsErrFlag)
	{
		double *autoCorrAbsErr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));

		size_t delta;

		if (numOfElem > 4096)
		{
			double covDiff = 0;
			for (i = 0; i < numOfElem; i++)
			{
				covDiff += (diff[i] - avgDiff)*(diff[i] - avgDiff);
			}

			covDiff = covDiff/numOfElem;

			if (covDiff == 0)
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
					autoCorrAbsErr[delta] = 0;
			}
			else
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
				{
					double sum = 0;

					for (i = 0; i < numOfElem-delta; i++)
					{
						sum += (diff[i]-avgDiff)*(diff[i+delta]-avgDiff);
					}

					autoCorrAbsErr[delta] = sum/(numOfElem-delta)/covDiff;
				}
			}
		}
		else
		{
			for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
			{
				double avg_0 = 0;
				double avg_1 = 0;

				for (i = 0; i < numOfElem-delta; i++)
				{
					avg_0 += diff[i];
					avg_1 += diff[i+delta];
				}

				avg_0 = avg_0 / (numOfElem-delta);
				avg_1 = avg_1 / (numOfElem-delta);

				double cov_0 = 0;
				double cov_1 = 0;

				for (i = 0; i < numOfElem-delta; i++)
				{
					cov_0 += (diff[i] - avg_0) * (diff[i] - avg_0);
					cov_1 += (diff[i+delta] - avg_1) * (diff[i+delta] - avg_1);
				}

				cov_0 = cov_0/(numOfElem-delta);
				cov_1 = cov_1/(numOfElem-delta);

				cov_0 = sqrt(cov_0);
				cov_1 = sqrt(cov_1);


				if (cov_0*cov_1 == 0)
				{
					for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
						autoCorrAbsErr[delta] = 0;
				}
				else
				{
					double sum = 0;

					for (i = 0; i < numOfElem-delta; i++)
						sum += (diff[i]-avg_0)*(diff[i+delta]-avg_1);

					autoCorrAbsErr[delta] = sum/(numOfElem-delta)/(cov_0*cov_1);
				}
			}

		}
        
        autoCorrAbsErr[0] = 1;
		compareResult->autoCorrAbsErr = autoCorrAbsErr;
	}

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
    		valErrCorr = ee/std1/stdDiff;

		compareResult->valErrCorr = valErrCorr;
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
		
	double sumOfDiffSquare = 0;
	long numOfElem_ = 0;

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
	double max_localBuffer[4], max_globalBuffer[3];
	double min_localBuffer[4], min_globalBuffer[3];
	
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
	
	double global_avgErr_rel = global_sumErr_rel/global_numOfElem_;
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

	if (autoCorrAbsErrFlag)
	{
		//TODO
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

	free(diff);
	free(relDiff);	
}

#endif
