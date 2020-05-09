#ifndef M_NORM_HPP
#define M_NORM_HPP

#include <limits>

//Available Norms:
//derOr_1_norm_1
//derOr_1_norm_2
//derOr_1_norm_INF
//derOr_2_norm_1
//derOr_2_norm_2
//derOr_2_norm_INF
//derOr_2_norm_Frobenius


//First order derivative 1-Norm. Sum(|Xi|)
template <typename dT>
void matrix<dT>::derOr_1_norm_1(matrix<dT> &norm){
  int i0,i1,i2,i3;
  if(nDim==1){
    norm.initialize(1,size0-2);
    for(i0=0;i0<size0-2;i0++)
      norm.data[i0]=vec_norm_1(der_1d(i0+1));
  }else if(nDim==2){
    norm.initialize(2,size0-2,size1-2);
    for(i1=0;i1<size1-2;i1++)
      for(i0=0;i0<size0-2;i0++)
        norm.data[i0+(size0-2)*i1]=vec_norm_1(
                                  der_2d_dim0(i0+1,i1+1),
                                  der_2d_dim1(i0+1,i1+1)
                                  );
  }else if(nDim==3){
    //cout<<"derOr_1_norm_1 : nDim = "<<nDim<<endl; //AMG
    norm.initialize(3,size0-2,size1-2,size2-2);
    //cout<<"derOr_1_norm_1 : Initialized Norm Matrix : "<<norm.size0<<" x "<<norm.size1<<" x "<<norm.size2<<endl; //AMG
    for(i2=0;i2<size2-2;i2++)
      for(i1=0;i1<size1-2;i1++)
        for(i0=0;i0<size0-2;i0++)
          norm.data[i0+(size0-2)*(i1+(size1-2)*i2)]=vec_norm_1(
              der_3d_dim0(i0+1,i1+1,i2+1),
              der_3d_dim1(i0+1,i1+1,i2+1),
              der_3d_dim2(i0+1,i1+1,i2+1)
              );
  }else if(nDim==4){
    norm.initialize(4,size0-2,size1-2,size2-2,size3-2);
    for(i3=0;i3<size3-2;i3++)
      for(i2=0;i2<size2-2;i2++)
        for(i1=0;i1<size1-2;i1++)
          for(i0=0;i0<size0-2;i0++)
            norm.data[i0+(size0-2)*(i1+(size1-2)*(i2+(size2-2)*i3))]=vec_norm_1(
                der_4d_dim0(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim1(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim2(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim3(i0+1,i1+1,i2+1,i3+1)
                );
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }

}


//First order derivative 2-Norm. Sqrt(Sum(|Xi|^2))
template <typename dT>
void matrix<dT>::derOr_1_norm_2(matrix<dT> &norm){
  int i0,i1,i2,i3;
  if(nDim==1){
    norm.initialize(1,size0-2);
    for(i0=0;i0<size0-2;i0++)
      norm.data[i0]=vec_norm_2(der_1d(i0+1));
  }else if(nDim==2){
    norm.initialize(2,size0-2,size1-2);
    for(i1=0;i1<size1-2;i1++)
      for(i0=0;i0<size0-2;i0++)
        norm.data[i0+(size0-2)*i1]=vec_norm_2(
                                  der_2d_dim0(i0+1,i1+1),
                                  der_2d_dim1(i0+1,i1+1)
                                  );
  }else if(nDim==3){
    norm.initialize(3,size0-2,size1-2,size2-2);
    for(i2=0;i2<size2-2;i2++)
      for(i1=0;i1<size1-2;i1++)
        for(i0=0;i0<size0-2;i0++)
          norm.data[i0+(size0-2)*(i1+(size1-2)*i2)]=vec_norm_2(
              der_3d_dim0(i0+1,i1+1,i2+1),
              der_3d_dim1(i0+1,i1+1,i2+1),
              der_3d_dim2(i0+1,i1+1,i2+1)
              );
  }else if(nDim==4){
    norm.initialize(4,size0-2,size1-2,size2-2,size3-2);
    for(i3=0;i3<size3-2;i3++)
      for(i2=0;i2<size2-2;i2++)
        for(i1=0;i1<size1-2;i1++)
          for(i0=0;i0<size0-2;i0++)
            norm.data[i0+(size0-2)*(i1+(size1-2)*(i2+(size2-2)*i3))]=vec_norm_2(
                der_4d_dim0(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim1(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim2(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim3(i0+1,i1+1,i2+1,i3+1)
                );
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
    
}

//First order derivative INF-Norm. Max(|Xi|)
template <typename dT>
void matrix<dT>::derOr_1_norm_INF(matrix<dT> &norm){
  int i0,i1,i2,i3;
  if(nDim==1){
    norm.initialize(1,size0-2);
    for(i0=0;i0<size0-2;i0++)
      norm.data[i0]=vec_norm_INF(der_1d(i0+1));
  }else if(nDim==2){
    norm.initialize(2,size0-2,size1-2);
    for(i1=0;i1<size1-2;i1++)
      for(i0=0;i0<size0-2;i0++)
        norm.data[i0+(size0-2)*i1]=vec_norm_INF(
                                  der_2d_dim0(i0+1,i1+1),
                                  der_2d_dim1(i0+1,i1+1)
                                  );
  }else if(nDim==3){
    norm.initialize(3,size0-2,size1-2,size2-2);
    for(i2=0;i2<size2-2;i2++)
      for(i1=0;i1<size1-2;i1++)
        for(i0=0;i0<size0-2;i0++)
          norm.data[i0+(size0-2)*(i1+(size1-2)*i2)]=vec_norm_INF(
              der_3d_dim0(i0+1,i1+1,i2+1),
              der_3d_dim1(i0+1,i1+1,i2+1),
              der_3d_dim2(i0+1,i1+1,i2+1)
              );
  }else if(nDim==4){
    norm.initialize(4,size0-2,size1-2,size2-2,size3-2);
    for(i3=0;i3<size3-2;i3++)
      for(i2=0;i2<size2-2;i2++)
        for(i1=0;i1<size1-2;i1++)
          for(i0=0;i0<size0-2;i0++)
            norm.data[i0+(size0-2)*(i1+(size1-2)*(i2+(size2-2)*i3))]=vec_norm_INF(
                der_4d_dim0(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim1(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim2(i0+1,i1+1,i2+1,i3+1),
                der_4d_dim3(i0+1,i1+1,i2+1,i3+1)
                );
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
  
}

//Second order derivative 1-Norm. Max(sum(abs(col)))
template <typename dT>
void matrix<dT>::derOr_2_norm_1(matrix<dT> &norm){
  int i0,i1,i2,i3;
  
  if(nDim==1){
    norm.initialize(1,size0-4);
    for(i0=0;i0<size0-4;i0++)
      norm.data[i0]=vec_norm_1((der_1d(i0+3)-der_1d(i0+1))/2);
  }else if(nDim==2){
    norm.initialize(2,size0-4,size1-4);
    double m[2][2];
    for(i1=0;i1<size1-4;i1++)
      for(i0=0;i0<size0-4;i0++){
        //norm[i0][i1] correspond to data[i0+2][i1+2]
        m[0][0]=(der_2d_dim0(i0+3,i1+2)-der_2d_dim0(i0+1,i1+2))/2;//dxdx
        m[0][1]=(der_2d_dim0(i0+2,i1+3)-der_2d_dim0(i0+2,i1+1))/2;//dxdy
        m[1][0]=m[0][1]; //dydx = dxdy
        m[1][1]=(der_2d_dim1(i0+2,i1+3)-der_2d_dim1(i0+2,i1+1))/2; //dydy
        
        norm.data[i0+(size0-4)*i1]=m2x2_norm_1(m);
      }
  }else if(nDim==3){
    norm.initialize(3,size0-4,size1-4,size2-4);
    double m[3][3];
    for(i2=0;i2<size2-4;i2++)
      for(i1=0;i1<size1-4;i1++)
        for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_3d_dim0(i0+3,i1+2,i2+2)-der_3d_dim0(i0+1,i1+2,i2+2))/2;//dxdx
          m[0][1]=(der_3d_dim0(i0+2,i1+3,i2+2)-der_3d_dim0(i0+2,i1+1,i2+2))/2;//dxdy
          m[0][2]=(der_3d_dim0(i0+2,i1+2,i2+3)-der_3d_dim0(i0+2,i1+2,i2+1))/2;//dxdz
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_3d_dim1(i0+2,i1+3,i2+2)-der_3d_dim1(i0+2,i1+1,i2+2))/2;//dydy
          m[1][2]=(der_3d_dim1(i0+2,i1+2,i2+3)-der_3d_dim1(i0+2,i1+2,i2+1))/2;//dydz
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_3d_dim2(i0+2,i1+2,i2+3)-der_3d_dim2(i0+2,i1+2,i2+1))/2;//dzdz
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*i2)]=m3x3_norm_1(m);
        }
  }else if(nDim==4){
    norm.initialize(4,size0-4,size1-4,size2-4,size3-4);
    double m[4][4];
    for(i3=0;i3<size3-4;i3++)
      for(i2=0;i2<size2-4;i2++)
        for(i1=0;i1<size1-4;i1++)
          for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_4d_dim0(i0+3,i1+2,i2+2,i3+2)-der_4d_dim0(i0+1,i1+2,i2+2,i3+2))/2;//dxdx
          m[0][1]=(der_4d_dim0(i0+2,i1+3,i2+2,i3+2)-der_4d_dim0(i0+2,i1+1,i2+2,i3+2))/2;//dxdy
          m[0][2]=(der_4d_dim0(i0+2,i1+2,i2+3,i3+2)-der_4d_dim0(i0+2,i1+2,i2+1,i3+2))/2;//dxdz
          m[0][3]=(der_4d_dim0(i0+2,i1+2,i2+2,i3+3)-der_4d_dim0(i0+2,i1+2,i2+2,i3+3))/2;//dxdt
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_4d_dim1(i0+2,i1+3,i2+2,i3+2)-der_4d_dim1(i0+2,i1+1,i2+2,i3+2))/2;//dydy
          m[1][2]=(der_4d_dim1(i0+2,i1+2,i2+3,i3+2)-der_4d_dim1(i0+2,i1+2,i2+1,i3+2))/2;//dydz
          m[1][3]=(der_4d_dim1(i0+2,i1+2,i2+2,i3+3)-der_4d_dim1(i0+2,i1+2,i2+2,i3+1))/2;//dydt
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_4d_dim2(i0+2,i1+2,i2+3,i3+2)-der_4d_dim2(i0+2,i1+2,i2+1,i3+2))/2;//dzdz
          m[2][3]=(der_4d_dim2(i0+2,i1+2,i2+2,i3+3)-der_4d_dim2(i0+2,i1+2,i2+2,i3+1))/2;//dzdt
          m[3][0]=m[0][3];//dtdx=dxdt
          m[3][1]=m[1][3];//dtdy=dydt
          m[3][2]=m[2][3];//dtdz=dzdt
          m[3][3]=(der_4d_dim3(i0+2,i1+2,i2+2,i3+3)-der_4d_dim3(i0+2,i1+2,i2+2,i3+1))/2;//dtdt
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*(i2+(size2-4)*i3))]=m4x4_norm_1(m);
        }
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
  
}

//Second order derivative 2-Norm. Max(Eig(M_T x M))
template <typename dT>
void matrix<dT>::derOr_2_norm_2(matrix<dT> &norm){
  int i0,i1,i2,i3;
  
  if(nDim==1){
    norm.initialize(1,size0-4);
    for(i0=0;i0<size0-4;i0++)
      norm.data[i0]=vec_norm_2((der_1d(i0+3)-der_1d(i0+1))/2);
  }else if(nDim==2){
    norm.initialize(2,size0-4,size1-4);
    double m[2][2];
    for(i1=0;i1<size1-4;i1++)
      for(i0=0;i0<size0-4;i0++){
        //norm[i0][i1] correspond to data[i0+2][i1+2]
        m[0][0]=(der_2d_dim0(i0+3,i1+2)-der_2d_dim0(i0+1,i1+2))/2;//dxdx
        m[0][1]=(der_2d_dim0(i0+2,i1+3)-der_2d_dim0(i0+2,i1+1))/2;//dxdy
        m[1][0]=m[0][1]; //dydx = dxdy
        m[1][1]=(der_2d_dim1(i0+2,i1+3)-der_2d_dim1(i0+2,i1+1))/2; //dydy
        
        norm.data[i0+(size0-4)*i1]=m2x2_norm_2(m);
      }
  }else if(nDim==3){
    norm.initialize(3,size0-4,size1-4,size2-4);
    double m[3][3];
    for(i2=0;i2<size2-4;i2++)
      for(i1=0;i1<size1-4;i1++)
        for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_3d_dim0(i0+3,i1+2,i2+2)-der_3d_dim0(i0+1,i1+2,i2+2))/2;//dxdx
          m[0][1]=(der_3d_dim0(i0+2,i1+3,i2+2)-der_3d_dim0(i0+2,i1+1,i2+2))/2;//dxdy
          m[0][2]=(der_3d_dim0(i0+2,i1+2,i2+3)-der_3d_dim0(i0+2,i1+2,i2+1))/2;//dxdz
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_3d_dim1(i0+2,i1+3,i2+2)-der_3d_dim1(i0+2,i1+1,i2+2))/2;//dydy
          m[1][2]=(der_3d_dim1(i0+2,i1+2,i2+3)-der_3d_dim1(i0+2,i1+2,i2+1))/2;//dydz
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_3d_dim2(i0+2,i1+2,i2+3)-der_3d_dim2(i0+2,i1+2,i2+1))/2;//dzdz
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*i2)]=m3x3_norm_2(m);
        }
  }else if(nDim==4){
    norm.initialize(4,size0-4,size1-4,size2-4,size3-4);
    double m[4][4];
    for(i3=0;i3<size3-4;i3++)
      for(i2=0;i2<size2-4;i2++)
        for(i1=0;i1<size1-4;i1++)
          for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_4d_dim0(i0+3,i1+2,i2+2,i3+2)-der_4d_dim0(i0+1,i1+2,i2+2,i3+2))/2;//dxdx
          m[0][1]=(der_4d_dim0(i0+2,i1+3,i2+2,i3+2)-der_4d_dim0(i0+2,i1+1,i2+2,i3+2))/2;//dxdy
          m[0][2]=(der_4d_dim0(i0+2,i1+2,i2+3,i3+2)-der_4d_dim0(i0+2,i1+2,i2+1,i3+2))/2;//dxdz
          m[0][3]=(der_4d_dim0(i0+2,i1+2,i2+2,i3+3)-der_4d_dim0(i0+2,i1+2,i2+2,i3+3))/2;//dxdt
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_4d_dim1(i0+2,i1+3,i2+2,i3+2)-der_4d_dim1(i0+2,i1+1,i2+2,i3+2))/2;//dydy
          m[1][2]=(der_4d_dim1(i0+2,i1+2,i2+3,i3+2)-der_4d_dim1(i0+2,i1+2,i2+1,i3+2))/2;//dydz
          m[1][3]=(der_4d_dim1(i0+2,i1+2,i2+2,i3+3)-der_4d_dim1(i0+2,i1+2,i2+2,i3+1))/2;//dydt
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_4d_dim2(i0+2,i1+2,i2+3,i3+2)-der_4d_dim2(i0+2,i1+2,i2+1,i3+2))/2;//dzdz
          m[2][3]=(der_4d_dim2(i0+2,i1+2,i2+2,i3+3)-der_4d_dim2(i0+2,i1+2,i2+2,i3+1))/2;//dzdt
          m[3][0]=m[0][3];//dtdx=dxdt
          m[3][1]=m[1][3];//dtdy=dydt
          m[3][2]=m[2][3];//dtdz=dzdt
          m[3][3]=(der_4d_dim3(i0+2,i1+2,i2+2,i3+3)-der_4d_dim3(i0+2,i1+2,i2+2,i3+1))/2;//dtdt
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*(i2+(size2-4)*i3))]=m4x4_norm_2(m);
        }
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
  
}

//Second order derivative INF-Norm. Max(sum(abs(row)))
template <typename dT>
void matrix<dT>::derOr_2_norm_INF(matrix<dT> &norm){
  int i0,i1,i2,i3;
  
  if(nDim==1){
    norm.initialize(1,size0-4);
    for(i0=0;i0<size0-4;i0++)
      norm.data[i0]=vec_norm_INF((der_1d(i0+3)-der_1d(i0+1))/2);
  }else if(nDim==2){
    norm.initialize(2,size0-4,size1-4);
    double m[2][2];
    for(i1=0;i1<size1-4;i1++)
      for(i0=0;i0<size0-4;i0++){
        //norm[i0][i1] correspond to data[i0+2][i1+2]
        m[0][0]=(der_2d_dim0(i0+3,i1+2)-der_2d_dim0(i0+1,i1+2))/2;//dxdx
        m[0][1]=(der_2d_dim0(i0+2,i1+3)-der_2d_dim0(i0+2,i1+1))/2;//dxdy
        m[1][0]=m[0][1]; //dydx = dxdy
        m[1][1]=(der_2d_dim1(i0+2,i1+3)-der_2d_dim1(i0+2,i1+1))/2; //dydy
        
        norm.data[i0+(size0-4)*i1]=m2x2_norm_INF(m);
      }
  }else if(nDim==3){
    norm.initialize(3,size0-4,size1-4,size2-4);
    double m[3][3];
    for(i2=0;i2<size2-4;i2++)
      for(i1=0;i1<size1-4;i1++)
        for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_3d_dim0(i0+3,i1+2,i2+2)-der_3d_dim0(i0+1,i1+2,i2+2))/2;//dxdx
          m[0][1]=(der_3d_dim0(i0+2,i1+3,i2+2)-der_3d_dim0(i0+2,i1+1,i2+2))/2;//dxdy
          m[0][2]=(der_3d_dim0(i0+2,i1+2,i2+3)-der_3d_dim0(i0+2,i1+2,i2+1))/2;//dxdz
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_3d_dim1(i0+2,i1+3,i2+2)-der_3d_dim1(i0+2,i1+1,i2+2))/2;//dydy
          m[1][2]=(der_3d_dim1(i0+2,i1+2,i2+3)-der_3d_dim1(i0+2,i1+2,i2+1))/2;//dydz
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_3d_dim2(i0+2,i1+2,i2+3)-der_3d_dim2(i0+2,i1+2,i2+1))/2;//dzdz
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*i2)]=m3x3_norm_INF(m);
        }
  }else if(nDim==4){
    norm.initialize(4,size0-4,size1-4,size2-4,size3-4);
    double m[4][4];
    for(i3=0;i3<size3-4;i3++)
      for(i2=0;i2<size2-4;i2++)
        for(i1=0;i1<size1-4;i1++)
          for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_4d_dim0(i0+3,i1+2,i2+2,i3+2)-der_4d_dim0(i0+1,i1+2,i2+2,i3+2))/2;//dxdx
          m[0][1]=(der_4d_dim0(i0+2,i1+3,i2+2,i3+2)-der_4d_dim0(i0+2,i1+1,i2+2,i3+2))/2;//dxdy
          m[0][2]=(der_4d_dim0(i0+2,i1+2,i2+3,i3+2)-der_4d_dim0(i0+2,i1+2,i2+1,i3+2))/2;//dxdz
          m[0][3]=(der_4d_dim0(i0+2,i1+2,i2+2,i3+3)-der_4d_dim0(i0+2,i1+2,i2+2,i3+3))/2;//dxdt
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_4d_dim1(i0+2,i1+3,i2+2,i3+2)-der_4d_dim1(i0+2,i1+1,i2+2,i3+2))/2;//dydy
          m[1][2]=(der_4d_dim1(i0+2,i1+2,i2+3,i3+2)-der_4d_dim1(i0+2,i1+2,i2+1,i3+2))/2;//dydz
          m[1][3]=(der_4d_dim1(i0+2,i1+2,i2+2,i3+3)-der_4d_dim1(i0+2,i1+2,i2+2,i3+1))/2;//dydt
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_4d_dim2(i0+2,i1+2,i2+3,i3+2)-der_4d_dim2(i0+2,i1+2,i2+1,i3+2))/2;//dzdz
          m[2][3]=(der_4d_dim2(i0+2,i1+2,i2+2,i3+3)-der_4d_dim2(i0+2,i1+2,i2+2,i3+1))/2;//dzdt
          m[3][0]=m[0][3];//dtdx=dxdt
          m[3][1]=m[1][3];//dtdy=dydt
          m[3][2]=m[2][3];//dtdz=dzdt
          m[3][3]=(der_4d_dim3(i0+2,i1+2,i2+2,i3+3)-der_4d_dim3(i0+2,i1+2,i2+2,i3+1))/2;//dtdt
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*(i2+(size2-4)*i3))]=m4x4_norm_INF(m);
        }
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
  
}

//Second order derivative Frobenius-Norm. Sqrt(Sum(|Xij|^2))
template <typename dT>
void matrix<dT>::derOr_2_norm_Frobenius(matrix<dT> &norm){
  int i0,i1,i2,i3;
  
  if(nDim==1){
    norm.initialize(1,size0-4);
    for(i0=0;i0<size0-4;i0++)
      norm.data[i0]=fabs((der_1d(i0+3)-der_1d(i0+1))/2); //Frobenius norm for 1x1 is just abs().
  }else if(nDim==2){
    norm.initialize(2,size0-4,size1-4);
    double m[2][2];
    for(i1=0;i1<size1-4;i1++)
      for(i0=0;i0<size0-4;i0++){
        //norm[i0][i1] correspond to data[i0+2][i1+2]
        m[0][0]=(der_2d_dim0(i0+3,i1+2)-der_2d_dim0(i0+1,i1+2))/2;//dxdx
        m[0][1]=(der_2d_dim0(i0+2,i1+3)-der_2d_dim0(i0+2,i1+1))/2;//dxdy
        m[1][0]=m[0][1]; //dydx = dxdy
        m[1][1]=(der_2d_dim1(i0+2,i1+3)-der_2d_dim1(i0+2,i1+1))/2; //dydy
        
        norm.data[i0+(size0-4)*i1]=m2x2_norm_Frobenius(m);
      }
  }else if(nDim==3){
    norm.initialize(3,size0-4,size1-4,size2-4);
    double m[3][3];
    for(i2=0;i2<size2-4;i2++)
      for(i1=0;i1<size1-4;i1++)
        for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_3d_dim0(i0+3,i1+2,i2+2)-der_3d_dim0(i0+1,i1+2,i2+2))/2;//dxdx
          m[0][1]=(der_3d_dim0(i0+2,i1+3,i2+2)-der_3d_dim0(i0+2,i1+1,i2+2))/2;//dxdy
          m[0][2]=(der_3d_dim0(i0+2,i1+2,i2+3)-der_3d_dim0(i0+2,i1+2,i2+1))/2;//dxdz
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_3d_dim1(i0+2,i1+3,i2+2)-der_3d_dim1(i0+2,i1+1,i2+2))/2;//dydy
          m[1][2]=(der_3d_dim1(i0+2,i1+2,i2+3)-der_3d_dim1(i0+2,i1+2,i2+1))/2;//dydz
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_3d_dim2(i0+2,i1+2,i2+3)-der_3d_dim2(i0+2,i1+2,i2+1))/2;//dzdz
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*i2)]=m3x3_norm_Frobenius(m);
        }
  }else if(nDim==4){
    norm.initialize(4,size0-4,size1-4,size2-4,size3-4);
    double m[4][4];
    for(i3=0;i3<size3-4;i3++)
      for(i2=0;i2<size2-4;i2++)
        for(i1=0;i1<size1-4;i1++)
          for(i0=0;i0<size0-4;i0++){
          //norm[i0][i1] correspond to data[i0+2][i1+2]
          m[0][0]=(der_4d_dim0(i0+3,i1+2,i2+2,i3+2)-der_4d_dim0(i0+1,i1+2,i2+2,i3+2))/2;//dxdx
          m[0][1]=(der_4d_dim0(i0+2,i1+3,i2+2,i3+2)-der_4d_dim0(i0+2,i1+1,i2+2,i3+2))/2;//dxdy
          m[0][2]=(der_4d_dim0(i0+2,i1+2,i2+3,i3+2)-der_4d_dim0(i0+2,i1+2,i2+1,i3+2))/2;//dxdz
          m[0][3]=(der_4d_dim0(i0+2,i1+2,i2+2,i3+3)-der_4d_dim0(i0+2,i1+2,i2+2,i3+3))/2;//dxdt
          m[1][0]=m[0][1];//dydx=dxdy
          m[1][1]=(der_4d_dim1(i0+2,i1+3,i2+2,i3+2)-der_4d_dim1(i0+2,i1+1,i2+2,i3+2))/2;//dydy
          m[1][2]=(der_4d_dim1(i0+2,i1+2,i2+3,i3+2)-der_4d_dim1(i0+2,i1+2,i2+1,i3+2))/2;//dydz
          m[1][3]=(der_4d_dim1(i0+2,i1+2,i2+2,i3+3)-der_4d_dim1(i0+2,i1+2,i2+2,i3+1))/2;//dydt
          m[2][0]=m[0][2];//dzdx=dxdz
          m[2][1]=m[1][2];//dzdy=dydz
          m[2][2]=(der_4d_dim2(i0+2,i1+2,i2+3,i3+2)-der_4d_dim2(i0+2,i1+2,i2+1,i3+2))/2;//dzdz
          m[2][3]=(der_4d_dim2(i0+2,i1+2,i2+2,i3+3)-der_4d_dim2(i0+2,i1+2,i2+2,i3+1))/2;//dzdt
          m[3][0]=m[0][3];//dtdx=dxdt
          m[3][1]=m[1][3];//dtdy=dydt
          m[3][2]=m[2][3];//dtdz=dzdt
          m[3][3]=(der_4d_dim3(i0+2,i1+2,i2+2,i3+3)-der_4d_dim3(i0+2,i1+2,i2+2,i3+1))/2;//dtdt
          
          norm.data[i0+(size0-4)*(i1+(size1-4)*(i2+(size2-4)*i3))]=m4x4_norm_Frobenius(m);
        }
  }else{
    cout<<"ERROR : Wrong nDim = "<<nDim<<endl;
    assert(0);
  }
  
}


#endif
