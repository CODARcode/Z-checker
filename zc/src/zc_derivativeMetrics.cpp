

#include <cmath>
#include <matrix.hpp>

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
