
#include <cmath>
#include <matrix.hpp>
#include "zc.h"

int ssimSize=7;
int ssimShift=1;

double zc_calc_metric_der_order1_ssim_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<9){
    cout<<"ERROR: dim=1, but r1<9."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<9 || r2<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<9 || r2<9 || r3<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<9 || r2<9 || r3<9 || r4<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 or r4<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<float> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<float> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<float> temp1;
  matrix<float> temp2;
  
  orig.data=data1;
  lossy.data=data2;
  
  orig.derOr_1_norm_2(temp1);
  lossy.derOr_1_norm_2(temp2);
  
  double result;
  
  switch (orig.nDim) {
    case 1:
      result=temp1.SSIM_1d_windowed(temp2,ssimSize,ssimShift);
      break;    
    case 2:
      result=temp1.SSIM_2d_windowed(temp2,ssimSize,ssimSize,ssimShift,ssimShift);
      break;
    case 3:
      result=temp1.SSIM_3d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift);
      break;
    case 4:
      result=temp1.SSIM_4d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift,ssimShift);
      break;
    default:
      cout<<"ERROR: Wrong nDim. This should not have occurred!"<<endl;
      *status=-2;
      return 0;
  }
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}

double zc_calc_metric_der_order1_psnr_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<3){
    cout<<"ERROR: dim=1, but r1<3."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<3 || r2<3)){
    cout<<"ERROR: dim=1, but { r1<3 or r2<3 }."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<3 || r2<3 || r3<3)){
    cout<<"ERROR: dim=1, but { r1<3 or r2<3 or r3<3 }."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<3 || r2<3 || r3<3 || r4<3)){
    cout<<"ERROR: dim=1, but { r1<3 or r2<3 or r3<3 or r4<3 }."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }

  //cout<<"dim="<<dim<<" r4="<<r4<<" r3="<<r3<<" r2="<<r2<<" r1="<<r1<<endl;
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<float> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<float> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  
  orig.data=data1;
  lossy.data=data2;
   
  double result=orig.derivativeOrder1_psnr_Metric(lossy);
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}

double zc_calc_metric_der_order1_ssim_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<9){
    cout<<"ERROR: dim=1, but r1<9."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<9 || r2<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<9 || r2<9 || r3<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<9 || r2<9 || r3<9 || r4<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 or r4<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<double> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<double> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<double> temp1;
  matrix<double> temp2;
  
  orig.data=data1;
  lossy.data=data2;
  
  orig.derOr_1_norm_2(temp1);
  lossy.derOr_1_norm_2(temp2);
  
  double result;
  
  switch (orig.nDim) {
    case 1:
      result=temp1.SSIM_1d_windowed(temp2,ssimSize,ssimShift);
      break;    
    case 2:
      result=temp1.SSIM_2d_windowed(temp2,ssimSize,ssimSize,ssimShift,ssimShift);
      break;
    case 3:
      result=temp1.SSIM_3d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift);
      break;
    case 4:
      result=temp1.SSIM_4d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift,ssimShift);
      break;
    default:
      cout<<"ERROR: Wrong nDim. This should not have occurred!"<<endl;
      *status=-2;
      return 0;
  }
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}

double zc_calc_metric_der_order1_psnr_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<3){
    cout<<"ERROR: dim=1, but r1<3."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<3 || r2<3)){
    cout<<"ERROR: dim=1, but { r1<3 or r2<3 }."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<3 || r2<3 || r3<3)){
    cout<<"ERROR: dim=1, but { r1<3 or r2<3 or r3<3 }."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<3 || r2<3 || r3<3 || r4<3)){
    cout<<"ERROR: dim=1, but { r1<3 or r2<3 or r3<3 or r4<3 }."<<endl;
    cout<<"REMINDER: Derivative order 1 metrics require every valid dimension to be at least 3."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<double> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<double> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  
  orig.data=data1;
  lossy.data=data2;
   
  double result=orig.derivativeOrder1_psnr_Metric(lossy);
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}

void zc_analyze_der_order1_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status,
  double *maxErrDx, double *maxErrDy, double *maxErrDz, double *maxErrDt,  //Max error of dx, dy, dx, dt
  double *psnrDx, double *psnrDy, double *psnrDz, double *psnrDt,  //PSNR of dx, dy, dx, dt
  double *ssimDx, double *ssimDy, double *ssimDz, double *ssimDt  //SSIM of dx, dy, dx, dt
  ){
  
  *status=-1;
  
  int minDim;
  if(derivativeOrder1_sep_maxDiffFlag)
    minDim=2+ssimSize;
  else
    minDim=3;
    
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return;
  }if(dim==1 && r1<minDim){
    cout<<"ERROR: dim=1, but { r1<"<<minDim<<" }."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least "<<minDim<<"."<<endl;
    *status=1;
    return;
  }else if(dim==2 && (r1<minDim || r2<minDim)){
    cout<<"ERROR: dim=1, but { r1<"<<minDim<<" or r2<"<<minDim<<" }."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least "<<minDim<<"."<<endl;
    *status=1;
    return;
  }else if(dim==3 && (r1<minDim || r2<minDim || r3<minDim)){
    cout<<"ERROR: dim=1, but { r1<"<<minDim<<" or r2<"<<minDim<<" or r3<"<<minDim<<" }."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least "<<minDim<<"."<<endl;
    *status=1;
    return;
  }else if(dim==4 && (r1<minDim || r2<minDim || r3<minDim || r4<minDim)){
    cout<<"ERROR: dim=1, but { r1<"<<minDim<<" or r2<"<<minDim<<" or r3<"<<minDim<<" or r4<"<<minDim<<" }."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least "<<minDim<<"."<<endl;
    *status=1;
    return;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<float> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<float> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<float> odx,ody,odz,odt;
  matrix<float> ldx,ldy,ldz,ldt;
  
  orig.data=data1;
  lossy.data=data2;
  
  if(dim==1){
    orig.m_der_1d_v2(odx);
    lossy.m_der_1d_v2(ldx);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_1d_windowed(ldx, ssimSize, ssimShift);
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }
  }else if(dim==2){
    orig.m_der_2d_dim0_v2(odx);
    orig.m_der_2d_dim1_v2(ody);
    lossy.m_der_2d_dim0_v2(ldx);
    lossy.m_der_2d_dim1_v2(ldy);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=ody.maxDiff(ldy);
      *maxErrDz=0;
      *maxErrDt=0;
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=ody.PSNR(ldy);
      *psnrDz=0;
      *psnrDt=0;
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_2d_windowed(ldx, ssimSize, ssimSize, ssimShift, ssimShift);
      *ssimDy=ody.SSIM_2d_windowed(ldy, ssimSize, ssimSize, ssimShift, ssimShift);
      *ssimDz=0;
      *ssimDt=0;
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }  
  }else if(dim==3){
    orig.m_der_3d_dim0_v2(odx);
    orig.m_der_3d_dim1_v2(ody);
    orig.m_der_3d_dim2_v2(odz);
    lossy.m_der_3d_dim0_v2(ldx);
    lossy.m_der_3d_dim1_v2(ldy);
    lossy.m_der_3d_dim2_v2(ldz);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=ody.maxDiff(ldy);
      *maxErrDz=odz.maxDiff(ldz);
      *maxErrDt=0;
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=ody.PSNR(ldy);
      *psnrDz=odz.PSNR(ldz);
      *psnrDt=0;
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_3d_windowed(ldx, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift);
      *ssimDy=ody.SSIM_3d_windowed(ldy, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift);
      *ssimDz=odz.SSIM_3d_windowed(ldz, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift);
      *ssimDt=0;
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }  
  }else if(dim==4){
    orig.m_der_4d_dim0_v2(odx);
    orig.m_der_4d_dim1_v2(ody);
    orig.m_der_4d_dim2_v2(odz);
    orig.m_der_4d_dim3_v2(odt);
    lossy.m_der_4d_dim0_v2(ldx);
    lossy.m_der_4d_dim1_v2(ldy);
    lossy.m_der_4d_dim2_v2(ldz);
    lossy.m_der_4d_dim3_v2(ldt);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=ody.maxDiff(ldy);
      *maxErrDz=odz.maxDiff(ldz);
      *maxErrDt=odt.maxDiff(ldt);
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=ody.PSNR(ldy);
      *psnrDz=odz.PSNR(ldz);
      *psnrDt=odt.PSNR(ldt);
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_4d_windowed(ldx, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
      *ssimDy=ody.SSIM_4d_windowed(ldy, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
      *ssimDz=odz.SSIM_4d_windowed(ldz, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
      *ssimDt=odt.SSIM_4d_windowed(ldt, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }  
  }
    
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
}

void zc_analyze_der_order1_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status,
  double *maxErrDx, double *maxErrDy, double *maxErrDz, double *maxErrDt,  //Max error of dx, dy, dx, dt
  double *psnrDx, double *psnrDy, double *psnrDz, double *psnrDt,  //PSNR of dx, dy, dx, dt
  double *ssimDx, double *ssimDy, double *ssimDz, double *ssimDt  //SSIM of dx, dy, dx, dt
  ){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return;
  }else if(dim==1 && r1<5){
    cout<<"ERROR: dim=1, but r1<5."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least 5."<<endl;
    *status=1;
    return;
  }else if(dim==2 && (r1<5 || r2<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 }."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least 5."<<endl;
    *status=1;
    return;
  }else if(dim==3 && (r1<5 || r2<5 || r3<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 or r3<5 }."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least 5."<<endl;
    *status=1;
    return;
  }else if(dim==4 && (r1<5 || r2<5 || r3<5 || r4<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 or r3<5 or r4<5 }."<<endl;
    cout<<"REMINDER: Derivative order 1 analysis require every valid dimension to be at least 5."<<endl;
    *status=1;
    return;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<double> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<double> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<double> odx,ody,odz,odt;
  matrix<double> ldx,ldy,ldz,ldt;
  
  orig.data=data1;
  lossy.data=data2;
  
  if(dim==1){
    orig.m_der_1d_v2(odx);
    lossy.m_der_1d_v2(ldx);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_1d_windowed(ldx, ssimSize, ssimShift);
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }
  }else if(dim==2){
    orig.m_der_2d_dim0_v2(odx);
    orig.m_der_2d_dim1_v2(ody);
    lossy.m_der_2d_dim0_v2(ldx);
    lossy.m_der_2d_dim1_v2(ldy);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=ody.maxDiff(ldy);
      *maxErrDz=0;
      *maxErrDt=0;
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=ody.PSNR(ldy);
      *psnrDz=0;
      *psnrDt=0;
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_2d_windowed(ldx, ssimSize, ssimSize, ssimShift, ssimShift);
      *ssimDy=ody.SSIM_2d_windowed(ldy, ssimSize, ssimSize, ssimShift, ssimShift);
      *ssimDz=0;
      *ssimDt=0;
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }  
  }else if(dim==3){
    orig.m_der_3d_dim0_v2(odx);
    orig.m_der_3d_dim1_v2(ody);
    orig.m_der_3d_dim2_v2(odz);
    lossy.m_der_3d_dim0_v2(ldx);
    lossy.m_der_3d_dim1_v2(ldy);
    lossy.m_der_3d_dim2_v2(ldz);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=ody.maxDiff(ldy);
      *maxErrDz=odz.maxDiff(ldz);
      *maxErrDt=0;
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=ody.PSNR(ldy);
      *psnrDz=odz.PSNR(ldz);
      *psnrDt=0;
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_3d_windowed(ldx, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift);
      *ssimDy=ody.SSIM_3d_windowed(ldy, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift);
      *ssimDz=odz.SSIM_3d_windowed(ldz, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift);
      *ssimDt=0;
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }  
  }else if(dim==4){
    orig.m_der_4d_dim0_v2(odx);
    orig.m_der_4d_dim1_v2(ody);
    orig.m_der_4d_dim2_v2(odz);
    orig.m_der_4d_dim3_v2(odt);
    lossy.m_der_4d_dim0_v2(ldx);
    lossy.m_der_4d_dim1_v2(ldy);
    lossy.m_der_4d_dim2_v2(ldz);
    lossy.m_der_4d_dim3_v2(ldt);
    if(derivativeOrder1_sep_maxDiffFlag){
      *maxErrDx=odx.maxDiff(ldx);
      *maxErrDy=ody.maxDiff(ldy);
      *maxErrDz=odz.maxDiff(ldz);
      *maxErrDt=odt.maxDiff(ldt);
    }else{
      *maxErrDx=0;
      *maxErrDy=0;
      *maxErrDz=0;
      *maxErrDt=0;
    }
    if(derivativeOrder1_sep_psnrFlag){
      *psnrDx=odx.PSNR(ldx);
      *psnrDy=ody.PSNR(ldy);
      *psnrDz=odz.PSNR(ldz);
      *psnrDt=odt.PSNR(ldt);
    }else{
      *psnrDx=0;
      *psnrDy=0;
      *psnrDz=0;
      *psnrDt=0;
    }
    if(derivativeOrder1_sep_ssimFlag){
      *ssimDx=odx.SSIM_4d_windowed(ldx, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
      *ssimDy=ody.SSIM_4d_windowed(ldy, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
      *ssimDz=odz.SSIM_4d_windowed(ldz, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
      *ssimDt=odt.SSIM_4d_windowed(ldt, ssimSize, ssimSize, ssimSize, ssimSize, ssimShift, ssimShift, ssimShift, ssimShift);
    }else{
      *ssimDx=0;
      *ssimDy=0;
      *ssimDz=0;
      *ssimDt=0;
    }  
  }
    
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
}

double zc_calc_metric_der_order1_sobolev_p2_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<9){
    cout<<"ERROR: dim=1, but r1<9."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<9 || r2<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<9 || r2<9 || r3<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<9 || r2<9 || r3<9 || r4<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 or r4<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<float> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<float> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<float> temp1;
  matrix<float> temp2;
  
  orig.data=data1;
  lossy.data=data2;
  
  double result=orig.sobolevNorm_s1_p2(lossy);
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;  

}

double zc_calc_metric_der_order1_sobolev_p2_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<9){
    cout<<"ERROR: dim=1, but r1<9."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<9 || r2<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<9 || r2<9 || r3<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<9 || r2<9 || r3<9 || r4<9)){
    cout<<"ERROR: dim=1, but { r1<9 or r2<9 or r3<9 or r4<9 }."<<endl;
    cout<<"REMINDER: Derivative order 1 SSIM metrics require every valid dimension to be at least 9."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<double> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<double> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<double> temp1;
  matrix<double> temp2;
  
  orig.data=data1;
  lossy.data=data2;
  
  double result=orig.sobolevNorm_s1_p2(lossy);
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;  

}

//********************************************************************************************************
//***********************************DERIVATIVE ORDER 2***************************************************
//********************************************************************************************************


double zc_calc_metric_der_order2_ssim_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<11){
    cout<<"ERROR: dim=1, but r1<11."<<endl;
    cout<<"REMINDER: Derivative order 2 SSIM metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<11 || r2<11)){
    cout<<"ERROR: dim=1, but { r1<11 or r2<11 }."<<endl;
    cout<<"REMINDER: Derivative order 2 SSIM metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<11 || r2<11 || r3<11)){
    cout<<"ERROR: dim=1, but { r1<11 or r2<11 or r3<11 }."<<endl;
    cout<<"REMINDER: Derivative order 2 SSIM metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<11 || r2<11 || r3<11 || r4<11)){
    cout<<"ERROR: dim=1, but { r1<11 or r2<11 or r3<11 or r4<11 }."<<endl;
    cout<<"REMINDER: Derivative order 2 SSIM metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  } 
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<float> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<float> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<float> temp1;
  matrix<float> temp2;
  
  orig.data=data1;
  lossy.data=data2;
  
  orig.derOr_2_norm_Frobenius(temp1);
  lossy.derOr_2_norm_Frobenius(temp2);
  
  double result;
  
  switch (orig.nDim) {
    case 1:
      result=temp1.SSIM_1d_windowed(temp2,ssimSize,ssimShift);
      break;    
    case 2:
      result=temp1.SSIM_2d_windowed(temp2,ssimSize,ssimSize,ssimShift,ssimShift);
      break;
    case 3:
      result=temp1.SSIM_3d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift);
      break;
    case 4:
      result=temp1.SSIM_4d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift,ssimShift);
      break;
    default:
      cout<<"ERROR: Wrong nDim. This should not have occurred!"<<endl;
      *status=-2;
      return 0;
  }
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}

double zc_calc_metric_der_order2_psnr_float(float *data1, float *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<5){
    cout<<"ERROR: dim=1, but r1<5."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<5 || r2<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<5 || r2<5 || r3<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 or r3<5 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<5 || r2<5 || r3<5 || r4<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 or r3<5 or r4<5 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<float> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<float> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  
  orig.data=data1;
  lossy.data=data2;
   
  double result=orig.derivativeOrder2_psnr_Metric(lossy);
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}

double zc_calc_metric_der_order2_ssim_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<11){
    cout<<"ERROR: dim=1, but r1<11."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<11 || r2<11)){
    cout<<"ERROR: dim=1, but { r1<11 or r2<11 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<11 || r2<11 || r3<11)){
    cout<<"ERROR: dim=1, but { r1<11 or r2<11 or r3<11 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<11 || r2<11 || r3<11 || r4<11)){
    cout<<"ERROR: dim=1, but { r1<11 or r2<11 or r3<11 or r4<11 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 11."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }  
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<double> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<double> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  matrix<double> temp1;
  matrix<double> temp2;
  
  orig.data=data1;
  lossy.data=data2;
  
  orig.derOr_2_norm_Frobenius(temp1);
  lossy.derOr_2_norm_Frobenius(temp2);
  
  double result;
  
  switch (orig.nDim) {
    case 1:
      result=temp1.SSIM_1d_windowed(temp2,ssimSize,ssimShift);
      break;    
    case 2:
      result=temp1.SSIM_2d_windowed(temp2,ssimSize,ssimSize,ssimShift,ssimShift);
      break;
    case 3:
      result=temp1.SSIM_3d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift);
      break;
    case 4:
      result=temp1.SSIM_4d_windowed(temp2,ssimSize,ssimSize,ssimSize,ssimSize,ssimShift,ssimShift,ssimShift,ssimShift);
      break;
    default:
      cout<<"ERROR: Wrong nDim. This should not have occurred!"<<endl;
      *status=-2;
      return 0;
  }
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}

double zc_calc_metric_der_order2_psnr_double(double *data1, double *data2,
  size_t dim, size_t r4, size_t r3, size_t r2, size_t r1, int *status){
  
  *status=-1;
  
  if(dim<1){
    cout<<"ERROR: Dimension less than 1!"<<endl;
    *status=1;
    return 0;
  }else if(dim==1 && r1<5){
    cout<<"ERROR: dim=1, but r1<5."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim==2 && (r1<5 || r2<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim==3 && (r1<5 || r2<5 || r3<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 or r3<5 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim==4 && (r1<5 || r2<5 || r3<5 || r4<5)){
    cout<<"ERROR: dim=1, but { r1<5 or r2<5 or r3<5 or r4<5 }."<<endl;
    cout<<"REMINDER: Derivative order 2 metrics require every valid dimension to be at least 5."<<endl;
    *status=1;
    return 0;
  }else if(dim>4){
    cout<<"ERROR: dim > 4 is not supported."<<endl;
    *status=1;
    return 0;
  }
  
  if(dim==1){ r2=1;r3=1;r4=1; }
  if(dim==2){ r3=1;r4=1; }
  if(dim==3){ r4=1; }
  
  matrix<double> orig;  orig.nDim=dim; orig.size0=r1; orig.size1=r2; orig.size2=r3; orig.size3=r4;
  matrix<double> lossy;  lossy.nDim=dim; lossy.size0=r1; lossy.size1=r2; lossy.size2=r3; lossy.size3=r4;
  
  orig.data=data1;
  lossy.data=data2;
   
  double result=orig.derivativeOrder2_psnr_Metric(lossy);
  
  orig.data=NULL; //To prevent deallocation from ~matrix()
  lossy.data=NULL; //To prevent deallocation from ~matrix()
  
  *status=0;
  return result;
}


