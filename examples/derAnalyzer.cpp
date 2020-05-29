#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>

#include "matrix.hpp"

//#define DATATYPE double
#define DATATYPE float

int main(int argc, char *argv[]){
  long int i;
  
  if(argc<4 || argc>7){
    cout<<"Analyze the differences in derivatives"<<endl;
    cout<<"Wrong number of arguments. Usage:"<<endl;
    cout<<"./derAnalyzer originalFile decompFile {dimensions}"<<endl;
    cout<<"dimensions: enter 1 to 4 numbers. Dimensions are in reversed order, from fastest to slowest: dim0 dim1 dim2 dim3"<<endl;
    cout<<"In C array representation: arrayName[dim3][dim2][dim1][dim0]"<<endl;
    cout<<"EXAMPLE:"<<endl;
    cout<<"./derAnalyzer testfloat_8_8_128.dat testfloat_8_8_128.dat.sz.out 128 8 8"<<endl;
    return 0;
  }
  
  int nDims,dim3,dim2,dim1,dim0;

  int compFileIdx;
  if(argc==5){
    nDims=1; dim0=atoi(argv[3]); dim1=1;             dim2=1;             dim3=1;             compFileIdx=4;
  }else if(argc==6){
    nDims=2; dim0=atoi(argv[3]); dim1=atoi(argv[4]); dim2=1;             dim3=1;             compFileIdx=5;
  }else if(argc==7){
    nDims=3; dim0=atoi(argv[3]); dim1=atoi(argv[4]); dim2=atoi(argv[5]); dim3=1;             compFileIdx=6;
  }else if(argc==8){
    nDims=4; dim0=atoi(argv[3]); dim1=atoi(argv[4]); dim2=atoi(argv[5]); dim3=atoi(argv[6]); compFileIdx=7;
  }
  
  DATATYPE *origData=(DATATYPE*)malloc(dim0*dim1*dim2*dim3*sizeof(DATATYPE));
  if(origData==NULL){
    cout<<"ERROR: origData cannot be allocated!"<<endl;;
    return 1;
  }
  ifstream origFile;
  origFile.open(argv[1],ios::in | ios::binary);
  if(!origFile.good()){
    cout<<"ERROR: There is a problem with the input file."<<endl;
    cout<<"Trying to read size: ( "<<dim0<<" x "<<dim1<<" x "<<dim2<<" x "<<dim3<<" ) from  file:"<<argv[1]<<endl;
    cout<<"(dim0,dim1,dim2,dim3)"<<endl;
    assert(0);
  } 
  origFile.read((char *)origData,dim0*dim1*dim2*dim3*sizeof(DATATYPE));
  origFile.close();
  
  DATATYPE *lossyData=(DATATYPE*)malloc(dim0*dim1*dim2*dim3*sizeof(DATATYPE));
  if(lossyData==NULL){
    cout<<"ERROR: lossyData cannot be allocated!"<<endl;;
    return 1;
  }
  ifstream lossyFile;
  lossyFile.open(argv[1],ios::in | ios::binary);
  if(!lossyFile.good()){
    cout<<"ERROR: There is a problem with the input file."<<endl;
    cout<<"Trying to read size: ( "<<dim0<<" x "<<dim1<<" x "<<dim2<<" x "<<dim3<<" ) from  file:"<<argv[1]<<endl;
    cout<<"(dim0,dim1,dim2,dim3)"<<endl;
    assert(0);
  } 
  lossyFile.read((char *)lossyData,dim0*dim1*dim2*dim3*sizeof(DATATYPE));
  lossyFile.close();
  
  int status;
  double der_Order1_PSNR_Metric=zc_calc_metric_der_order1_psnr_float(origData,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
  cout<<"der_Order1_PSNR_Metric = "<<der_Order1_PSNR_Metric<<endl;
  cout<<"status = "<<status<<endl;
  
  double der_Order1_SSIM_Metric=zc_calc_metric_der_order1_ssim_float(origData,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
  cout<<"der_Order1_SSIM_Metric = "<<der_Order1_SSIM_Metric<<endl;
  cout<<"status = "<<status<<endl; 

  double der_Order2_PSNR_Metric=zc_calc_metric_der_order2_psnr_float(origData,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
  cout<<"der_Order2_PSNR_Metric = "<<der_Order2_PSNR_Metric<<endl;
  cout<<"status = "<<status<<endl;
  
  double der_Order2_SSIM_Metric=zc_calc_metric_der_order2_ssim_float(origData,lossyData,nDims,dim3,dim2,dim1,dim0,&status);
  cout<<"der_Order2_SSIM_Metric = "<<der_Order2_SSIM_Metric<<endl;
  cout<<"status = "<<status<<endl;

  free(origData);
  free(lossyData);
  
  return 0;
}

