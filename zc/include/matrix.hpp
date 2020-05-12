#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <fstream>
#include <assert.h>
#include <math.h>

#define DEBUG_L0 1
#define DEBUG_CHECK_BOUNDARIES 1

#define K1 0.01
#define K2 0.03

using namespace std;

/*
typedef struct ssimResults{
  double luminance; //Luminance
  double contrast; //Contrast
  double structure; //Structure
  double ssim; //SSIM
}ssimResults;
*/

//dT : dataType
template <typename dT> 
class matrix{
public:
  dT *data;
  int nDim;
  int size0;
  int size1;
  int size2;
  int size3;
  
//m_basic.hpp:
  matrix();
  matrix(int _nDim, int _size0);
  matrix(int _nDim, int _size0, int _size1);
  matrix(int _nDim, int _size0, int _size1, int _size2);
  matrix(int _nDim, int _size0, int _size1, int _size2, int _size3);
  ~matrix();
  
  void deallocate();
  
  void initialize(int _nDim, int _size0);
  void initialize(int _nDim, int _size0, int _size1);
  void initialize(int _nDim, int _size0, int _size1, int _size2);
  void initialize(int _nDim, int _size0, int _size1, int _size2, int _size3);
  
  //Read and Write functions
  void readDataBin(string fileName); //Read from file
  void writeDataBin(string fileName); //Writes Data in Binary format
  void writeDataText_Double(string fileName); //Writes Data in Binary format. Very similar to print()
  void print();  //Print to screen
  void print_Double();  //Print to screen
  void print_Long();  //Print to screen
  void printStats();  //Calculate and print some statistics about the matrix
//m_basic.hpp-
  
  
  //Index calculations for the data array
  inline int idx(int dim0);
  inline int idx(int dim0,int dim1);
  inline int idx(int dim0,int dim1,int dim2);
  inline int idx(int dim0,int dim1,int dim2,int dim3);
  
  //Point-wise Derivatives:
//der.hpp:
  inline double der_1d(int idx0);
  inline double der_2d_dim0(int idx0,int idx1);
  inline double der_2d_dim1(int idx0,int idx1);
  inline double der_3d_dim0(int idx0,int idx1,int idx2);
  inline double der_3d_dim1(int idx0,int idx1,int idx2);
  inline double der_3d_dim2(int idx0,int idx1,int idx2);
  inline double der_4d_dim0(int idx0,int idx1,int idx2,int idx3);
  inline double der_4d_dim1(int idx0,int idx1,int idx2,int idx3);
  inline double der_4d_dim2(int idx0,int idx1,int idx2,int idx3);
  inline double der_4d_dim3(int idx0,int idx1,int idx2,int idx3);
//der.hpp-
  
  //Norms:
//m_norm.hpp:
  void derOr_1_norm_1(matrix &norm);//First order derivative 1-Norm. Sum(|Xi|)
  void derOr_1_norm_2(matrix &norm);//First order derivative 2-Norm. Sqrt(Sum(|Xi|^2))
  void derOr_1_norm_INF(matrix &norm);//First order derivative INF-Norm. Max(|Xi|)
  void derOr_2_norm_1(matrix &norm);//Second order derivative 1-Norm. Max(sum(abs(col)))
  void derOr_2_norm_2(matrix &norm);//Second order derivative 2-Norm. Max(Eig(M_T x M))
  void derOr_2_norm_INF(matrix &norm);//Second order derivative INF-Norm. Max(sum(abs(row)))
  void derOr_2_norm_Frobenius(matrix &norm);//Second order derivative Frobenius-Norm. Sqrt(Sum(|Xij|^2)) 
  
  //Our metrics:
  //lhs is the original, rhs is the decompressed. example usage: orig.metric_d1(decompressed)
  double derivativeOrder1_psnr_Metric(matrix &rhs);  //derOr_1_norm_2_PSNR
  double derivativeOrder2_psnr_Metric(matrix &rhs);  //derOr_2_norm_Frobenius_PSNR
//m_norm.hpp-
  
  //SSIM Calculations. Compared to "other" matrix
//ssim.hpp:
  double SSIM_4d_windowed(matrix &other, int windowSize0, int windowSize1, int windowSize2, int windowSize3, int windowShift0, int windowShift1, int windowShift2, int windowShift3);
  double SSIM_3d_windowed(matrix &other, int windowSize0, int windowSize1, int windowSize2, int windowShift0, int windowShift1, int windowShift2);
  double SSIM_2d_windowed(matrix &other, int windowSize0, int windowSize1, int windowShift0, int windowShift1);
  double SSIM_1d_windowed(matrix &other, int windowSize0, int windowShift0);
  
  //Functions that calculate SSIM of a particular window:
  double SSIM_4d_calcWindow(matrix &other, int offset0, int offset1, int offset2, int offset3,int windowSize0, int windowSize1, int windowSize2, int windowSize3);
  double SSIM_3d_calcWindow(matrix &other, int offset0, int offset1, int offset2, int windowSize0, int windowSize1, int windowSize2);
  double SSIM_2d_calcWindow(matrix &other, int offset0, int offset1, int windowSize0, int windowSize1);
  double SSIM_1d_calcWindow(matrix &other, int offset0,int windowSize0);
//ssim.hpp-

//others.hpp:
  double PSNR(matrix &other);
  dT maxDiff(matrix &other);
//others.hpp-
  
  //FOR DEBUGGING ONLY
  void multiply(int x){ //Multiplies the whole matrix with x
    int i0,i1,i2,i3;
    for(i3=0;i3<size3;i3++){
      for(i2=0;i2<size2;i2++){
        for(i1=0;i1<size1;i1++){
          for(i0=0;i0<size0;i0++){
            data[i0+size0*(i1+size1*(i2+size2*i3))]*=x;
          }
        }
      }
    }
  }
  
//m_der.hpp:
    //v1 : data[i+1]-data[i]
  void m_der_1d_v1(matrix &res);
  void m_der_2d_dim0_v1(matrix &res);
  void m_der_2d_dim1_v1(matrix &res);
  void m_der_3d_dim0_v1(matrix &res);
  void m_der_3d_dim1_v1(matrix &res);
  void m_der_3d_dim2_v1(matrix &res);
  void m_der_4d_dim0_v1(matrix &res);
  void m_der_4d_dim1_v1(matrix &res);
  void m_der_4d_dim2_v1(matrix &res);
  void m_der_4d_dim3_v1(matrix &res);

  //v2 : Central difference : (data[i+1]-data[i-1])/2
  void m_der_1d_v2(matrix &res);
  void m_der_2d_dim0_v2(matrix &res);
  void m_der_2d_dim1_v2(matrix &res);
  void m_der_3d_dim0_v2(matrix &res);
  void m_der_3d_dim1_v2(matrix &res);
  void m_der_3d_dim2_v2(matrix &res);
  void m_der_4d_dim0_v2(matrix &res);
  void m_der_4d_dim1_v2(matrix &res);
  void m_der_4d_dim2_v2(matrix &res);
  void m_der_4d_dim3_v2(matrix &res);
//m_der.hpp-



};

//zc_derivativeMetrics.cpp:
double zc_calc_metric_der_order1_ssim_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);
double zc_calc_metric_der_order1_psnr_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);
double zc_calc_metric_der_order1_ssim_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);
double zc_calc_metric_der_order1_psnr_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);
double zc_calc_metric_der_order2_ssim_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);
double zc_calc_metric_der_order2_psnr_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);
double zc_calc_metric_der_order2_ssim_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);
double zc_calc_metric_der_order2_psnr_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status);      
    



#include "m_basic.hpp"  
#include "m_der.hpp"  
#include "idx.hpp"  //UNUSED FOR NOW
#include "norm.hpp"
#include "ssim.hpp"
#include "der.hpp"
#include "m_norm.hpp"
#include "others.hpp"
#include "m_der_metrics.hpp"  

#endif
