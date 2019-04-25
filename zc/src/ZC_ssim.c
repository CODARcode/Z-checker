/*
 *  Copyright (c) 2010 The VP8 project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 * 
 *  Copyright (c) 2018 The Z-checker project copyright: also BSD-style license.
 * 
 * This file is modified based on ssim.c in VP8 project: converting RGB image SSIM to floating-point version.
 * 
 * Authors: VP8 project authors and Sheng Di
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "zc.h"

// Google version of SSIM
// SSIM
#define KERNEL 3
#define KERNEL_SIZE  (2 * KERNEL + 1)

static const int K[KERNEL_SIZE] =
{
    1, 4, 11, 16, 11, 4, 1    // 16 * exp(-0.3 * i * i)
};
static const double ki_w = 1. / 2304.;  // 1 / sum(i:0..6, j..6) K[i]*K[j]
double zc_get_ssim_float(const float *org, const float *rec,
                 int xo, int yo, size_t W, size_t H, const double valueRange)
{
	size_t stride = W;
    int y, x;

    const int ymin = (yo - KERNEL < 0) ? 0 : yo - KERNEL;
    const int ymax = (yo + KERNEL > H - 1) ? H - 1 : yo + KERNEL;
    const int xmin = (xo - KERNEL < 0) ? 0 : xo - KERNEL;
    const int xmax = (xo + KERNEL > W - 1) ? W - 1 : xo + KERNEL;
    // worst case of accumulation is a weight of 48 = 16 + 2 * (11 + 4 + 1)
    // with a diff of 255, squares. That would a max error of 0x8ee0900,
    // which fits into 32 bits integers.
    double w = 0, xm = 0, ym = 0, xxm = 0, xym = 0, yym = 0;
    org += ymin * stride;
    rec += ymin * stride;

    for (y = ymin; y <= ymax; ++y, org += stride, rec += stride)
    {
        const double Wy = K[KERNEL + y - yo];

        for (x = xmin; x <= xmax; ++x)
        {
            const double Wxy = Wy * K[KERNEL + x - xo];
            // TODO(skal): inlined assembly
            w   += Wxy;
            xm  += Wxy * org[x];            
            ym  += Wxy * rec[x];
            xxm += Wxy * org[x] * org[x];
            xym += Wxy * org[x] * rec[x];
            yym += Wxy * rec[x] * rec[x];
        }
    }

    double range2 = valueRange*valueRange;

    {
        const double iw = 1. / w;
        const double iwx = xm * iw;
        const double iwy = ym * iw;
        double sxx = xxm * iw - iwx * iwx;
        double syy = yym * iw - iwy * iwy;

        // small errors are possible, due to rounding. Clamp to zero.
        if (sxx < 0.) sxx = 0.;

        if (syy < 0.) syy = 0.;

        {
            const double sxsy = sqrt(sxx * syy);
            const double sxy = xym * iw - iwx * iwy;
            double C11 = (0.01 * 0.01) * range2;
            double C22 = (0.03 * 0.03) * range2;
            double C33 = (0.015 * 0.015) * range2;
            double l = (2. * iwx * iwy + C11) / (iwx * iwx + iwy * iwy + C11);
            double c = (2. * sxsy      + C22) / (sxx + syy + C22);

            double s = (sxy + C33) / (sxsy + C33);
            return l * c * s;

        }
    }

}

double zc_get_ssimfull_kernel_float(const float *org, const float *rec,
                            int xo, int yo, size_t W, size_t H, const double valueRange)
{
	size_t stride = W;
    int y_, x_;
    double xm = 0, ym = 0, xxm = 0, xym = 0, yym = 0;
    org += (yo - KERNEL) * stride;
    org += (xo - KERNEL);
    rec += (yo - KERNEL) * stride;
    rec += (xo - KERNEL);

    for (y_ = 0; y_ < KERNEL_SIZE; ++y_, org += stride, rec += stride)
    {
        const int Wy = K[y_];

        for (x_ = 0; x_ < KERNEL_SIZE; ++x_)
        {
            const int Wxy = Wy * K[x_];
            // TODO(skal): inlined assembly
            const double org_x = org[x_];
            const double rec_x = rec[x_];
            xm  += Wxy * org_x;
            ym  += Wxy * rec_x;
            xxm += Wxy * org_x * org_x;
            xym += Wxy * org_x * rec_x;
            yym += Wxy * rec_x * rec_x;
        }
    }
    
    double range2 = valueRange*valueRange;

    {
        const double iw = ki_w;
        const double iwx = xm * iw;
        const double iwy = ym * iw;
        double sxx = xxm * iw - iwx * iwx;
        double syy = yym * iw - iwy * iwy;

        // small errors are possible, due to rounding. Clamp to zero.
        if (sxx < 0.) sxx = 0.;

        if (syy < 0.) syy = 0.;

        {
            const double sxsy = sqrt(sxx * syy);
            const double sxy = xym * iw - iwx * iwy;
            double C11 = (0.01 * 0.01) * range2;
            double C22 = (0.03 * 0.03) * range2;
            double C33 = (0.015 * 0.015) * range2;
            double l = (2. * iwx * iwy + C11) / (iwx * iwx + iwy * iwy + C11);
            double c = (2. * sxsy      + C22) / (sxx + syy + C22);
            double s = (sxy + C33) / (sxsy + C33);
            return l * c * s;
        }
    }
}

double zc_calc_ssim_2d_float(const float *org, const float *rec,
                  const size_t r2, const size_t r1)
{
	size_t totalSize = r2*r1;
	size_t image_width = r1;
	size_t image_height = r2;
	
    size_t j, i;
    double SSIM = 0.;
    double min = org[0], max = org[0];
    
    for(i=0;i<image_height;i++)
		for(j=0;j<image_width;j++)
		{
			double v = org[i*image_width+j];
			if(min>v) min = v;
			if(max<v) max = v;
		}
		
	double range = max - min;

    for (j = 0; j < KERNEL; ++j)
    {
        for (i = 0; i < image_width; ++i)
        {
            SSIM += zc_get_ssim_float(org, rec, i, j, image_width, image_height, range);
        }
    }

    for (j = KERNEL; j < image_height - KERNEL; ++j)
    {
        for (i = 0; i < KERNEL; ++i)
        {
            SSIM += zc_get_ssim_float(org, rec, i, j, image_width, image_height, range);
        }

        for (i = KERNEL; i < image_width - KERNEL; ++i)
        {
            SSIM += zc_get_ssimfull_kernel_float(org, rec, i, j,
                                         image_width, image_height, range);
        }

        for (i = image_width - KERNEL; i < image_width; ++i)
        {
            SSIM += zc_get_ssim_float(org, rec, i, j, image_width, image_height, range);
        }
    }

    for (j = image_height - KERNEL; j < image_height; ++j)
    {
        for (i = 0; i < image_width; ++i)
        {
            SSIM += zc_get_ssim_float(org, rec, i, j, image_width, image_height, range);
        }
    }

    return SSIM/totalSize;
}


double zc_get_ssim_double(const double *org, const double *rec,
                 int xo, int yo, size_t W, size_t H, const double valueRange)
{
	size_t stride = W;
    int y, x;

    const int ymin = (yo - KERNEL < 0) ? 0 : yo - KERNEL;
    const int ymax = (yo + KERNEL > H - 1) ? H - 1 : yo + KERNEL;
    const int xmin = (xo - KERNEL < 0) ? 0 : xo - KERNEL;
    const int xmax = (xo + KERNEL > W - 1) ? W - 1 : xo + KERNEL;
    // worst case of accumulation is a weight of 48 = 16 + 2 * (11 + 4 + 1)
    // with a diff of 255, squares. That would a max error of 0x8ee0900,
    // which fits into 32 bits integers.
    double w = 0, xm = 0, ym = 0, xxm = 0, xym = 0, yym = 0;
    org += ymin * stride;
    rec += ymin * stride;

    for (y = ymin; y <= ymax; ++y, org += stride, rec += stride)
    {
        const double Wy = K[KERNEL + y - yo];

        for (x = xmin; x <= xmax; ++x)
        {
            const double Wxy = Wy * K[KERNEL + x - xo];
            // TODO(skal): inlined assembly
            w   += Wxy;
            xm  += Wxy * org[x];            
            ym  += Wxy * rec[x];
            xxm += Wxy * org[x] * org[x];
            xym += Wxy * org[x] * rec[x];
            yym += Wxy * rec[x] * rec[x];
        }
    }

    double range2 = valueRange*valueRange;

    {
        const double iw = 1. / w;
        const double iwx = xm * iw;
        const double iwy = ym * iw;
        double sxx = xxm * iw - iwx * iwx;
        double syy = yym * iw - iwy * iwy;

        // small errors are possible, due to rounding. Clamp to zero.
        if (sxx < 0.) sxx = 0.;

        if (syy < 0.) syy = 0.;

        {
            const double sxsy = sqrt(sxx * syy);
            const double sxy = xym * iw - iwx * iwy;
            double C11 = (0.01 * 0.01) * range2;
            double C22 = (0.03 * 0.03) * range2;
            double C33 = (0.015 * 0.015) * range2;
            double l = (2. * iwx * iwy + C11) / (iwx * iwx + iwy * iwy + C11);
            double c = (2. * sxsy      + C22) / (sxx + syy + C22);

            double s = (sxy + C33) / (sxsy + C33);
            return l * c * s;

        }
    }

}

double zc_get_ssimfull_kernel_double(const double *org, const double *rec,
                            int xo, int yo, size_t W, size_t H, const double valueRange)
{
	size_t stride = W;
    int y_, x_;
    double xm = 0, ym = 0, xxm = 0, xym = 0, yym = 0;
    org += (yo - KERNEL) * stride;
    org += (xo - KERNEL);
    rec += (yo - KERNEL) * stride;
    rec += (xo - KERNEL);

    for (y_ = 0; y_ < KERNEL_SIZE; ++y_, org += stride, rec += stride)
    {
        const int Wy = K[y_];

        for (x_ = 0; x_ < KERNEL_SIZE; ++x_)
        {
            const int Wxy = Wy * K[x_];
            // TODO(skal): inlined assembly
            const double org_x = org[x_];
            const double rec_x = rec[x_];
            xm  += Wxy * org_x;
            ym  += Wxy * rec_x;
            xxm += Wxy * org_x * org_x;
            xym += Wxy * org_x * rec_x;
            yym += Wxy * rec_x * rec_x;
        }
    }
    
    double range2 = valueRange*valueRange;

    {
        const double iw = ki_w;
        const double iwx = xm * iw;
        const double iwy = ym * iw;
        double sxx = xxm * iw - iwx * iwx;
        double syy = yym * iw - iwy * iwy;

        // small errors are possible, due to rounding. Clamp to zero.
        if (sxx < 0.) sxx = 0.;

        if (syy < 0.) syy = 0.;

        {
            const double sxsy = sqrt(sxx * syy);
            const double sxy = xym * iw - iwx * iwy;
            double C11 = (0.01 * 0.01) * range2;
            double C22 = (0.03 * 0.03) * range2;
            double C33 = (0.015 * 0.015) * range2;
            double l = (2. * iwx * iwy + C11) / (iwx * iwx + iwy * iwy + C11);
            double c = (2. * sxsy      + C22) / (sxx + syy + C22);
            double s = (sxy + C33) / (sxsy + C33);
            return l * c * s;
        }
    }
}

double zc_calc_ssim_2d_double(const double *org, const double *rec,
                  const size_t r2, const size_t r1)
{
	size_t totalSize = r1*r2;
	size_t image_width = r1;
	size_t image_height = r2;
	
    size_t j, i;
    double SSIM = 0.;
    double min = org[0], max = org[0];
    
    for(i=0;i<image_height;i++)
		for(j=0;j<image_width;j++)
		{
			double v = org[i*image_width+j];
			if(min>v) min = v;
			if(max<v) max = v;
		}
		
	double range = max - min;

    for (j = 0; j < KERNEL; ++j)
    {
        for (i = 0; i < image_width; ++i)
        {
            SSIM += zc_get_ssim_double(org, rec, i, j, image_width, image_height, range);
        }
    }

    for (j = KERNEL; j < image_height - KERNEL; ++j)
    {
        for (i = 0; i < KERNEL; ++i)
        {
            SSIM += zc_get_ssim_double(org, rec, i, j, image_width, image_height, range);
        }

        for (i = KERNEL; i < image_width - KERNEL; ++i)
        {
            SSIM += zc_get_ssimfull_kernel_double(org, rec, i, j,
                                         image_width, image_height, range);
        }

        for (i = image_width - KERNEL; i < image_width; ++i)
        {
            SSIM += zc_get_ssim_double(org, rec, i, j, image_width, image_height, range);
        }
    }

    for (j = image_height - KERNEL; j < image_height; ++j)
    {
        for (i = 0; i < image_width; ++i)
        {
            SSIM += zc_get_ssim_double(org, rec, i, j, image_width, image_height, range);
        }
    }

    return SSIM/totalSize;
}


void zc_calc_ssim_3d_float(float *org, float *rec, size_t r3, size_t r2, size_t r1, double *min_ssim, double* avg_ssim, double* max_ssim)
{
	size_t i = 0, j = 0, counter = 0;
	size_t intv = 1, offset = 0;
	*min_ssim = 1;
	*max_ssim = 0;
	*avg_ssim = 0;
	//just work on 10 uniformly sampled layers if r3 is a large number
	if(r3>20)
		intv = r3/20;
	size_t nbEle = r2*r1;
	float *p, *q;
	//perform calculation of ssim for each layer
	for(i=0;i<r3;i+=intv)
	{
		offset = i*nbEle;
		p = org+offset;
		q = rec+offset;
		double min = p[0], max = min;
		for(j=0;j<nbEle;j++)
		{
			if(min>p[j])min = p[j];
			if(max<p[j])max = p[j];
		}	
		if(min==max) //skip the constant dataset
			continue;
		counter++;
		double ssim = zc_calc_ssim_2d_float(p, q, r2, r1);
		if(*min_ssim>ssim) *min_ssim=ssim;
		if(*max_ssim<ssim) *max_ssim=ssim;
		*avg_ssim += ssim;
	}
	*avg_ssim=*avg_ssim/counter;
}

void zc_calc_ssim_3d_double(double *org, double *rec, size_t r3, size_t r2, size_t r1, double *min_ssim, double* avg_ssim, double* max_ssim)
{
	size_t i = 0, j = 0, counter = 0;
	size_t intv = 1, offset = 0;
	*min_ssim = 1;
	*avg_ssim = 0;
	*max_ssim = 0;
	//just work on 10 uniformly sampled layers if r3 is a large number
	if(r3>20)
		intv = r3/20;
	size_t nbEle = r2*r1;
	double *p, *q;		
	//perform calculation of ssim for each layer
	for(i=0;i<r3;i+=intv)
	{
		offset = i*nbEle;
		p = org+offset;
		q = rec+offset;
		double min = p[0], max = min;
		for(j=0;j<nbEle;j++)
		{
			if(min>p[j])min = p[j];
			if(max<p[j])max = p[j];
		}	
		if(min==max) //skip the constant dataset
			continue;
		counter++;		
		double ssim = zc_calc_ssim_2d_double(p, q, r2, r1);
		if(*min_ssim>ssim) *min_ssim=ssim;
		if(*max_ssim<ssim) *max_ssim=ssim;
		*avg_ssim += ssim;
	}
	*avg_ssim=*avg_ssim/counter;
}

//TODO: 1d SSIM
double zc_calc_ssim_1d_float(const float *org, const float *rec, const size_t r1)
{
	return 0;
}

double zc_calc_ssim_1d_double(const double *org, const double *rec, const size_t r1)
{
	return 0;
}


//MPI
#ifdef HAVE_MPI
double zc_calc_ssim_2d_double_online(const double *org, const double *rec, const size_t r2, const size_t r1)
{
	double global_ssim = 0;
	double local_ssim = zc_calc_ssim_2d_double(org, rec, r2, r1);
	MPI_Reduce(&local_ssim, &global_ssim, 1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
	if(myRank==0)
		global_ssim /= nbProc;
	
	return global_ssim;
}

double zc_calc_ssim_2d_float_online(const float *org, const float *rec, const size_t r2, const size_t r1)
{
	double global_ssim = 0;
	double local_ssim = zc_calc_ssim_2d_float(org, rec, r2, r1);
	MPI_Reduce(&local_ssim, &global_ssim, 1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
	if(myRank==0)
		global_ssim /= nbProc;
	
	return global_ssim;
}

void zc_calc_ssim_3d_float_online(float *org, float *rec, size_t r3, size_t r2, size_t r1, double *global_min_ssim, double* global_avg_ssim, double* global_max_ssim)
{
	double local_min_ssim = 0, local_max_ssim = 0, local_avg_ssim = 0;
	zc_calc_ssim_3d_float(org, rec, r3, r2, r1, &local_min_ssim, &local_avg_ssim, &local_max_ssim);
	MPI_Reduce(&local_min_ssim, global_min_ssim, 1, MPI_DOUBLE, MPI_MIN, 0, ZC_COMM_WORLD);
	MPI_Reduce(&local_avg_ssim, global_avg_ssim, 1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
	MPI_Reduce(&local_max_ssim, global_max_ssim, 1, MPI_DOUBLE, MPI_MAX, 0, ZC_COMM_WORLD);
	
	if(myRank==0)
		*global_avg_ssim = *global_avg_ssim/nbProc;
}

void zc_calc_ssim_3d_double_online(double *org, double *rec, size_t r3, size_t r2, size_t r1, double *global_min_ssim, double* global_avg_ssim, double* global_max_ssim)
{
	double local_min_ssim = 0, local_max_ssim = 0, local_avg_ssim = 0;
	zc_calc_ssim_3d_double(org, rec, r3, r2, r1, &local_min_ssim, &local_avg_ssim, &local_max_ssim);
	MPI_Reduce(&local_min_ssim, global_min_ssim, 1, MPI_DOUBLE, MPI_MIN, 0, ZC_COMM_WORLD);
	MPI_Reduce(&local_avg_ssim, global_avg_ssim, 1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
	MPI_Reduce(&local_max_ssim, global_max_ssim, 1, MPI_DOUBLE, MPI_MAX, 0, ZC_COMM_WORLD);
	
	if(myRank==0)
		*global_avg_ssim = *global_avg_ssim/nbProc;	
}

#endif
