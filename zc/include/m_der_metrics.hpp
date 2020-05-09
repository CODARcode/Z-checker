#ifndef M_DER_METRICS_HPP
#define M_DER_METRICS_HPP

//Derivative Order 1:
// Meaning: PSNR ( Norm_2(derOr_1(original)) , Norm_2(derOr_1(decompressed)) )
//Search for functions: derOr_1_norm_2, PSNR
template <typename dT>
double matrix<dT>::derivativeOrder1_psnr_Metric(matrix<dT> &rhs){
  int i0,i1,i2,i3;
  
  long n;
  double norm1,norm2,diff;
  double min,max;
  min=std::numeric_limits<double>::max();
  max=std::numeric_limits<double>::min();
  double sum=0;
  
  if(nDim==1){
    //norm.initialize(1,size0-2);
    n=size0-2;
    for(i0=0;i0<size0-2;i0++){
      norm1=vec_norm_2(der_1d(i0+1));
      norm2=vec_norm_2(rhs.der_1d(i0+1));
      if(min>norm1)
        min=norm1;
      if(max<norm1)
        max=norm1;
      diff=fabs(norm1-norm2);
      sum+=diff*diff;
    }
  }else if(nDim==2){
    //norm.initialize(2,size0-2,size1-2);
    n=(size0-2)*(size1-2);
    for(i1=0;i1<size1-2;i1++)
      for(i0=0;i0<size0-2;i0++){
        norm1=vec_norm_2(
                der_2d_dim0(i0+1,i1+1),
                der_2d_dim1(i0+1,i1+1)
              );
        norm2=vec_norm_2(
                rhs.der_2d_dim0(i0+1,i1+1),
                rhs.der_2d_dim1(i0+1,i1+1)
              );
        if(min>norm1)
          min=norm1;
        if(max<norm1)
          max=norm1;
        diff=fabs(norm1-norm2);
        sum+=diff*diff;
      }
  }else if(nDim==3){
    //norm.initialize(3,size0-2,size1-2,size2-2);
    n=(size0-2)*(size1-2)*(size2-0);
    for(i2=0;i2<size2-2;i2++)
      for(i1=0;i1<size1-2;i1++)
        for(i0=0;i0<size0-2;i0++){
          norm1=vec_norm_2(
                    der_3d_dim0(i0+1,i1+1,i2+1),
                    der_3d_dim1(i0+1,i1+1,i2+1),
                    der_3d_dim2(i0+1,i1+1,i2+1)
                );
          norm2=vec_norm_2(
                    rhs.der_3d_dim0(i0+1,i1+1,i2+1),
                    rhs.der_3d_dim1(i0+1,i1+1,i2+1),
                    rhs.der_3d_dim2(i0+1,i1+1,i2+1)
                );
          if(min>norm1)
            min=norm1;
          if(max<norm1)
            max=norm1;
          diff=fabs(norm1-norm2);
          sum+=diff*diff;
        }
  }else if(nDim==4){
    //norm.initialize(4,size0-2,size1-2,size2-2,size3-2);
    n=(size0-2)*(size1-2)*(size2-0)*(size3-0);
    for(i3=0;i3<size3-2;i3++)
      for(i2=0;i2<size2-2;i2++)
        for(i1=0;i1<size1-2;i1++)
          for(i0=0;i0<size0-2;i0++){
            norm1=vec_norm_2(
                der_4d_dim0(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim1(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim2(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim3(i0+1,i1+1,i2+1,i3+1)
                );
            norm2=vec_norm_2(
                rhs.der_4d_dim0(i0+1,i1+1,i2+1,i3+1),
                rhs.der_4d_dim1(i0+1,i1+1,i2+1,i3+1),
                rhs.der_4d_dim2(i0+1,i1+1,i2+1,i3+1),
                rhs.der_4d_dim3(i0+1,i1+1,i2+1,i3+1)
                );
            if(min>norm1)
              min=norm1;
            if(max<norm1)
              max=norm1;
            diff=fabs(norm1-norm2);
            sum+=diff*diff;
          }
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
  double rmse = sqrt(sum/n); 
  return 20 * log10 ( (max-min) / rmse );
}

//Derivative Order 2:
// Meaning: PSNR ( Norm_Frobenius(derOr_2(original)) , Norm_2(derOr_2(decompressed)) )
//Search for functions: derOr_2_norm_Frobenius, PSNR
template <typename dT>
double matrix<dT>::derivativeOrder2_psnr_Metric(matrix<dT> &rhs){
  int i0,i1,i2,i3;
  
  long n;
  double norm1,norm2,diff;
  double min,max;
  min=std::numeric_limits<double>::max();
  max=std::numeric_limits<double>::min();
  double sum=0;
  
  if(nDim==1){
    //norm.initialize(1,size0-4);
    n=size0-4;
    for(i0=0;i0<size0-4;i0++){
      norm1=fabs((der_1d(i0+3)-der_1d(i0+1))/2); //Frobenius norm for 1x1 is just abs().
      norm2=fabs((rhs.der_1d(i0+3)-rhs.der_1d(i0+1))/2); //Frobenius norm for 1x1 is just abs().
      if(min>norm1)
        min=norm1;
      if(max<norm1)
        max=norm1;
      diff=fabs(norm1-norm2);
      sum+=diff*diff;
    }
  }else if(nDim==2){
    //norm.initialize(2,size0-4,size1-4);
    n=(size0-4)*(size1-4);
    double m1[2][2];
    double m2[2][2];
    for(i1=0;i1<size1-4;i1++)
      for(i0=0;i0<size0-4;i0++){
        //norm[i0][i1] correspond to data[i0+2][i1+2]
        m1[0][0]=(der_2d_dim0(i0+3,i1+2)-der_2d_dim0(i0+1,i1+2))/2;//dxdx
        m1[0][1]=(der_2d_dim0(i0+2,i1+3)-der_2d_dim0(i0+2,i1+1))/2;//dxdy
        m1[1][0]=m1[0][1]; //dydx = dxdy
        m1[1][1]=(der_2d_dim1(i0+2,i1+3)-der_2d_dim1(i0+2,i1+1))/2; //dydy
        norm1=m2x2_norm_Frobenius(m1);
        
        m2[0][0]=(rhs.der_2d_dim0(i0+3,i1+2)-rhs.der_2d_dim0(i0+1,i1+2))/2;//dxdx
        m2[0][1]=(rhs.der_2d_dim0(i0+2,i1+3)-rhs.der_2d_dim0(i0+2,i1+1))/2;//dxdy
        m2[1][0]=m2[0][1]; //dydx = dxdy
        m2[1][1]=(rhs.der_2d_dim1(i0+2,i1+3)-rhs.der_2d_dim1(i0+2,i1+1))/2; //dydy
        norm2=m2x2_norm_Frobenius(m2);
        
        if(min>norm1)
          min=norm1;
        if(max<norm1)
          max=norm1;
        diff=fabs(norm1-norm2);
        sum+=diff*diff;
      }
  }else if(nDim==3){
    //norm.initialize(3,size0-4,size1-4,size2-4);
    n=(size0-4)*(size1-4)*(size2-4);
    double m1[3][3];
    double m2[3][3];
    for(i2=0;i2<size2-4;i2++)
      for(i1=0;i1<size1-4;i1++)
        for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m1[0][0]=(der_3d_dim0(i0+3,i1+2,i2+2)-der_3d_dim0(i0+1,i1+2,i2+2))/2;//dxdx
          m1[0][1]=(der_3d_dim0(i0+2,i1+3,i2+2)-der_3d_dim0(i0+2,i1+1,i2+2))/2;//dxdy
          m1[0][2]=(der_3d_dim0(i0+2,i1+2,i2+3)-der_3d_dim0(i0+2,i1+2,i2+1))/2;//dxdz
          m1[1][0]=m1[0][1];//dydx=dxdy
          m1[1][1]=(der_3d_dim1(i0+2,i1+3,i2+2)-der_3d_dim1(i0+2,i1+1,i2+2))/2;//dydy
          m1[1][2]=(der_3d_dim1(i0+2,i1+2,i2+3)-der_3d_dim1(i0+2,i1+2,i2+1))/2;//dydz
          m1[2][0]=m1[0][2];//dzdx=dxdz
          m1[2][1]=m1[1][2];//dzdy=dydz
          m1[2][2]=(der_3d_dim2(i0+2,i1+2,i2+3)-der_3d_dim2(i0+2,i1+2,i2+1))/2;//dzdz
          norm1=m3x3_norm_Frobenius(m1);
          
          m2[0][0]=(rhs.der_3d_dim0(i0+3,i1+2,i2+2)-rhs.der_3d_dim0(i0+1,i1+2,i2+2))/2;//dxdx
          m2[0][1]=(rhs.der_3d_dim0(i0+2,i1+3,i2+2)-rhs.der_3d_dim0(i0+2,i1+1,i2+2))/2;//dxdy
          m2[0][2]=(rhs.der_3d_dim0(i0+2,i1+2,i2+3)-rhs.der_3d_dim0(i0+2,i1+2,i2+1))/2;//dxdz
          m2[1][0]=m2[0][1];//dydx=dxdy
          m2[1][1]=(rhs.der_3d_dim1(i0+2,i1+3,i2+2)-rhs.der_3d_dim1(i0+2,i1+1,i2+2))/2;//dydy
          m2[1][2]=(rhs.der_3d_dim1(i0+2,i1+2,i2+3)-rhs.der_3d_dim1(i0+2,i1+2,i2+1))/2;//dydz
          m2[2][0]=m2[0][2];//dzdx=dxdz
          m2[2][1]=m2[1][2];//dzdy=dydz
          m2[2][2]=(rhs.der_3d_dim2(i0+2,i1+2,i2+3)-rhs.der_3d_dim2(i0+2,i1+2,i2+1))/2;//dzdz
          norm2=m3x3_norm_Frobenius(m2);
          
          if(min>norm1)
            min=norm1;
          if(max<norm1)
            max=norm1;
          diff=fabs(norm1-norm2);
          sum+=diff*diff;
        }
  }else if(nDim==4){
    //norm.initialize(4,size0-4,size1-4,size2-4,size3-4);
    n=(size0-4)*(size1-4)*(size2-4)*(size3-4);
    double m1[4][4];
    double m2[4][4];
    for(i3=0;i3<size3-4;i3++)
      for(i2=0;i2<size2-4;i2++)
        for(i1=0;i1<size1-4;i1++)
          for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m1[0][0]=(der_4d_dim0(i0+3,i1+2,i2+2,i3+2)-der_4d_dim0(i0+1,i1+2,i2+2,i3+2))/2;//dxdx
          m1[0][1]=(der_4d_dim0(i0+2,i1+3,i2+2,i3+2)-der_4d_dim0(i0+2,i1+1,i2+2,i3+2))/2;//dxdy
          m1[0][2]=(der_4d_dim0(i0+2,i1+2,i2+3,i3+2)-der_4d_dim0(i0+2,i1+2,i2+1,i3+2))/2;//dxdz
          m1[0][3]=(der_4d_dim0(i0+2,i1+2,i2+2,i3+3)-der_4d_dim0(i0+2,i1+2,i2+2,i3+3))/2;//dxdt
          m1[1][0]=m1[0][1];//dydx=dxdy
          m1[1][1]=(der_4d_dim1(i0+2,i1+3,i2+2,i3+2)-der_4d_dim1(i0+2,i1+1,i2+2,i3+2))/2;//dydy
          m1[1][2]=(der_4d_dim1(i0+2,i1+2,i2+3,i3+2)-der_4d_dim1(i0+2,i1+2,i2+1,i3+2))/2;//dydz
          m1[1][3]=(der_4d_dim1(i0+2,i1+2,i2+2,i3+3)-der_4d_dim1(i0+2,i1+2,i2+2,i3+1))/2;//dydt
          m1[2][0]=m1[0][2];//dzdx=dxdz
          m1[2][1]=m1[1][2];//dzdy=dydz
          m1[2][2]=(der_4d_dim2(i0+2,i1+2,i2+3,i3+2)-der_4d_dim2(i0+2,i1+2,i2+1,i3+2))/2;//dzdz
          m1[2][3]=(der_4d_dim2(i0+2,i1+2,i2+2,i3+3)-der_4d_dim2(i0+2,i1+2,i2+2,i3+1))/2;//dzdt
          m1[3][0]=m1[0][3];//dtdx=dxdt
          m1[3][1]=m1[1][3];//dtdy=dydt
          m1[3][2]=m1[2][3];//dtdz=dzdt
          m1[3][3]=(der_4d_dim3(i0+2,i1+2,i2+2,i3+3)-der_4d_dim3(i0+2,i1+2,i2+2,i3+1))/2;//dtdt
          norm1=m4x4_norm_Frobenius(m1);
          
          m2[0][0]=(rhs.der_4d_dim0(i0+3,i1+2,i2+2,i3+2)-rhs.der_4d_dim0(i0+1,i1+2,i2+2,i3+2))/2;//dxdx
          m2[0][1]=(rhs.der_4d_dim0(i0+2,i1+3,i2+2,i3+2)-rhs.der_4d_dim0(i0+2,i1+1,i2+2,i3+2))/2;//dxdy
          m2[0][2]=(rhs.der_4d_dim0(i0+2,i1+2,i2+3,i3+2)-rhs.der_4d_dim0(i0+2,i1+2,i2+1,i3+2))/2;//dxdz
          m2[0][3]=(rhs.der_4d_dim0(i0+2,i1+2,i2+2,i3+3)-rhs.der_4d_dim0(i0+2,i1+2,i2+2,i3+3))/2;//dxdt
          m2[1][0]=m2[0][1];//dydx=dxdy
          m2[1][1]=(rhs.der_4d_dim1(i0+2,i1+3,i2+2,i3+2)-rhs.der_4d_dim1(i0+2,i1+1,i2+2,i3+2))/2;//dydy
          m2[1][2]=(rhs.der_4d_dim1(i0+2,i1+2,i2+3,i3+2)-rhs.der_4d_dim1(i0+2,i1+2,i2+1,i3+2))/2;//dydz
          m2[1][3]=(rhs.der_4d_dim1(i0+2,i1+2,i2+2,i3+3)-rhs.der_4d_dim1(i0+2,i1+2,i2+2,i3+1))/2;//dydt
          m2[2][0]=m2[0][2];//dzdx=dxdz
          m2[2][1]=m2[1][2];//dzdy=dydz
          m2[2][2]=(rhs.der_4d_dim2(i0+2,i1+2,i2+3,i3+2)-rhs.der_4d_dim2(i0+2,i1+2,i2+1,i3+2))/2;//dzdz
          m2[2][3]=(rhs.der_4d_dim2(i0+2,i1+2,i2+2,i3+3)-rhs.der_4d_dim2(i0+2,i1+2,i2+2,i3+1))/2;//dzdt
          m2[3][0]=m2[0][3];//dtdx=dxdt
          m2[3][1]=m2[1][3];//dtdy=dydt
          m2[3][2]=m2[2][3];//dtdz=dzdt
          m2[3][3]=(rhs.der_4d_dim3(i0+2,i1+2,i2+2,i3+3)-rhs.der_4d_dim3(i0+2,i1+2,i2+2,i3+1))/2;//dtdt
          norm2=m4x4_norm_Frobenius(m2);
          
          if(min>norm1)
            min=norm1;
          if(max<norm1)
            max=norm1;
          diff=fabs(norm1-norm2);
          sum+=diff*diff;
        }
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
  double rmse = sqrt(sum/n); 
  return 20 * log10 ( (max-min) / rmse );
}


#endif
