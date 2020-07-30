#ifndef OTHERS_HPP
#define OTHERS_HPP

//#define cIdx1D(i1) (i2*r1+i1)
#define cIdx2D(i1,i0,r0) (i1*r0+i0)
#define cIdx3D(i2,i1,i0,r1,r0) ((i2*r1+i1)*r0+i0)
#define cIdx4D(i3,i2,i1,i0,r2,r1,r0) (((i3*r2+i2)*r1+i1)*r0+i0)

template <typename dT>
double matrix<dT>::PSNR(matrix &other){
  
  if(size0!=other.size0){
    cout<<"matrix :: PSNR : Size0's do not match."<<endl;
    cout<<"  lhs.size0="<<size0<<"  rhs.size0="<<other.size0<<endl;
    assert(0);
  }
  if(size1!=other.size1){
    cout<<"matrix :: PSNR : Size1's do not match."<<endl;
    cout<<"  lhs.size1="<<size1<<"  rhs.size1="<<other.size1<<endl;
    assert(0);
  }
  if(size2!=other.size2){
    cout<<"matrix :: PSNR : Size2's do not match."<<endl;
    cout<<"  lhs.size2="<<size2<<"  rhs.size1="<<other.size2<<endl;
    assert(0);
  }
  if(size3!=other.size3){
    cout<<"matrix :: PSNR : Size3's do not match."<<endl;
    cout<<"  lhs.size3="<<size3<<"  rhs.size1="<<other.size3<<endl;
    assert(0);
  }
  
  int i0;
  double min, max;
  min=data[0];
  max=data[0];
  double sum=0;
  int n=size0*size1*size2*size3;
  for(i0=0;i0<n;i0++){
    if(min>data[i0])
      min=data[i0];
    if(max<data[i0])
      max=data[i0];
    double temp=data[i0]-other.data[i0];
    sum+=(temp*temp);
  }
  
  double rmse = sqrt(sum/n); 
  return 20 * log10 ( (max-min) / rmse );
  
}

template <typename dT>
dT matrix<dT>::maxDiff(matrix &other){  //The maximum difference between two matrices
  
  if(size0!=other.size0){
    cout<<"matrix :: maxDiff : Size0's do not match."<<endl;
    cout<<"  lhs.size0="<<size0<<"  rhs.size0="<<other.size0<<endl;
    assert(0);
  }
  if(size1!=other.size1){
    cout<<"matrix :: maxDiff : Size1's do not match."<<endl;
    cout<<"  lhs.size1="<<size1<<"  rhs.size1="<<other.size1<<endl;
    assert(0);
  }
  if(size2!=other.size2){
    cout<<"matrix :: maxDiff : Size2's do not match."<<endl;
    cout<<"  lhs.size2="<<size2<<"  rhs.size1="<<other.size2<<endl;
    assert(0);
  }
  if(size3!=other.size3){
    cout<<"matrix :: maxDiff : Size3's do not match."<<endl;
    cout<<"  lhs.size3="<<size3<<"  rhs.size1="<<other.size3<<endl;
    assert(0);
  }
  
  int i0;
  dT maxDiff_;
  maxDiff_=fabs(data[0]-other.data[0]);

  int n=size0*size1*size2*size3;
  for(i0=0;i0<n;i0++){
    if(maxDiff_<fabs(data[i0]-other.data[i0]))
      maxDiff_=fabs(data[i0]-other.data[i0]);
  }
  
  return maxDiff_;
  
}

template <typename dT>
double matrix<dT>::sobolevNorm_s1_p2(matrix &rhs){  //The sobolevNorm between two matrices
  
  if(size0!=rhs.size0){
    cout<<"matrix :: maxDiff : Size0's do not match."<<endl;
    cout<<"  lhs.size0="<<size0<<"  rhs.size0="<<rhs.size0<<endl;
    assert(0);
  }
  if(size1!=rhs.size1){
    cout<<"matrix :: maxDiff : Size1's do not match."<<endl;
    cout<<"  lhs.size1="<<size1<<"  rhs.size1="<<rhs.size1<<endl;
    assert(0);
  }
  if(size2!=rhs.size2){
    cout<<"matrix :: maxDiff : Size2's do not match."<<endl;
    cout<<"  lhs.size2="<<size2<<"  rhs.size1="<<rhs.size2<<endl;
    assert(0);
  }
  if(size3!=rhs.size3){
    cout<<"matrix :: maxDiff : Size3's do not match."<<endl;
    cout<<"  lhs.size3="<<size3<<"  rhs.size1="<<rhs.size3<<endl;
    assert(0);
  }
  
  int i0,i1,i2,i3;
  int n0,n1,n2,n3; //0123 correspond to xyzt respectively
  int n=0;
  double result=0;
  
  matrix<dT> ldx,ldy,ldz,ldt;  //Partial derivatives of left hand side
  matrix<dT> rdx,rdy,rdz,rdt;  //Partial derivatives of right hand side
 
  if(nDim==1){
    n0=size0;
    n=n0-2;
    m_der_1d_v2(ldx);
    rhs.m_der_1d_v2(rdx);
    for(i0=0;i0<n0-2;i0++){
      result+= (data[i0+1]-rhs.data[i0+1])*(data[i0+1]-rhs.data[i0+1]);
      result+= (ldx.data[i0]-ldx.data[i0])*(ldx.data[i0]-ldx.data[i0]);
    }
  }else if(nDim==2){
    n0=size0;
    n1=size1;
    n=(n0-2)*(n1-2);
    m_der_2d_dim0_v2(ldx);
    m_der_2d_dim1_v2(ldy);
    rhs.m_der_2d_dim0_v2(rdx);
    rhs.m_der_2d_dim1_v2(rdy);
    for(i1=0;i1<n1-2;i1++){
      for(i0=0;i0<n0-2;i0++){
        result+= (data[cIdx2D(i1+1,i0+1,n0)]-rhs.data[cIdx2D(i1+1,i0+1,n0)])*(data[cIdx2D(i1+1,i0+1,n0)]-rhs.data[cIdx2D(i1+1,i0+1,n0)]);
        result+= (ldx.data[cIdx2D(i1+1,i0,n0-2)]-ldx.data[cIdx2D(i1+1,i0,n0-2)])*(ldx.data[cIdx2D(i1+1,i0,n0-2)]-ldx.data[cIdx2D(i1+1,i0,n0-2)]);
        result+= (ldy.data[cIdx2D(i1,i0+1,n0)]-ldy.data[cIdx2D(i1,i0+1,n0)])*(ldy.data[cIdx2D(i1,i0+1,n0)]-ldy.data[cIdx2D(i1,i0+1,n0)]);
      }
    }
  }else if(nDim==3){
    n0=size0;
    n1=size1;
    n2=size2;
    n=(n0-2)*(n1-2)*(n2-2);
    m_der_3d_dim0_v2(ldx);
    m_der_3d_dim1_v2(ldy);
    m_der_3d_dim2_v2(ldz);
    rhs.m_der_3d_dim0_v2(rdx);
    rhs.m_der_3d_dim1_v2(rdy);
    rhs.m_der_3d_dim2_v2(rdz);
    for(i2=0;i1<n2-2;i2++){
      for(i1=0;i1<n1-2;i1++){
        for(i0=0;i0<n0-2;i0++){
          result+= (data[cIdx3D(i2+1,i1+1,i0+1,n1,n0)]-rhs.data[cIdx3D(i2+1,i1+1,i0+1,n1,n0)])*(data[cIdx3D(i2+1,i1+1,i0+1,n1,n0)]-rhs.data[cIdx3D(i2+1,i1+1,i0+1,n1,n0)]);
          result+= (ldx.data[cIdx3D(i2+1,i1+1,i0,n1,n0-2)]-ldx.data[cIdx3D(i2+1,i1+1,i0,n1,n0-2)])*(ldx.data[cIdx3D(i2+1,i1+1,i0,n1,n0-2)]-ldx.data[cIdx3D(i2+1,i1+1,i0,n1,n0-2)]);
          result+= (ldy.data[cIdx3D(i2+1,i1,i0+1,n1-2,n0)]-ldy.data[cIdx3D(i2+1,i1,i0+1,n1-2,n0)])*(ldy.data[cIdx3D(i2+1,i1,i0+1,n1-2,n0)]-ldy.data[cIdx3D(i2+1,i1,i0+1,n1-2,n0)]);
          result+= (ldz.data[cIdx3D(i2,i1+1,i0+1,n1,n0)]-ldz.data[cIdx3D(i2,i1+1,i0+1,n1,n0)])*(ldz.data[cIdx3D(i2,i1+1,i0+1,n1,n0)]-ldz.data[cIdx3D(i2,i1+1,i0+1,n1,n0)]);
        }
      }
    }
  }else if(nDim==4){
    n0=size0;
    n1=size1;
    n2=size2;
    n3=size3;
    n=(n0-2)*(n1-2)*(n2-2)*(n3-2);
    m_der_4d_dim0_v2(ldx);
    m_der_4d_dim1_v2(ldy);
    m_der_4d_dim2_v2(ldz);
    m_der_4d_dim3_v2(ldt);
    rhs.m_der_4d_dim0_v2(rdx);
    rhs.m_der_4d_dim1_v2(rdy);
    rhs.m_der_4d_dim2_v2(rdz);
    rhs.m_der_4d_dim3_v2(rdt);
    for(i3=0;i3<n3-2;i3++){
      for(i2=0;i2<n2-2;i2++){
        for(i1=0;i1<n1-2;i1++){
          for(i0=0;i0<n0-2;i0++){
            result+= (data[cIdx4D(i3+1,i2+1,i1+1,i0+1,n2,n1,n0)]-rhs.data[cIdx4D(i3+1,i2+1,i1+1,i0+1,n2,n1,n0)])*(data[cIdx4D(i3+1,i2+1,i1+1,i0+1,n2,n1,n0)]-rhs.data[cIdx4D(i3+1,i2+1,i1+1,i0+1,n2,n1,n0)]);
            result+= (ldx.data[cIdx4D(i3+1,i2+1,i1+1,i0,n2,n1,n0-2)]-ldx.data[cIdx4D(i3+1,i2+1,i1+1,i0,n2,n1,n0-2)])*(ldx.data[cIdx4D(i3+1,i2+1,i1+1,i0,n2,n1,n0-2)]-ldx.data[cIdx4D(i3+1,i2+1,i1+1,i0,n2,n1,n0-2)]);
            result+= (ldy.data[cIdx4D(i3+1,i2+1,i1,i0+1,n2,n1-2,n0)]-ldy.data[cIdx4D(i3+1,i2+1,i1,i0+1,n2,n1-2,n0)])*(ldy.data[cIdx4D(i3+1,i2+1,i1,i0+1,n2,n1-2,n0)]-ldy.data[cIdx4D(i3+1,i2+1,i1,i0+1,n2,n1-2,n0)]);
            result+= (ldz.data[cIdx4D(i3+1,i2,i1+1,i0+1,n2-2,n1,n0)]-ldz.data[cIdx4D(i3+1,i2,i1+1,i0+1,n2-2,n1,n0)])*(ldz.data[cIdx4D(i3+1,i2,i1+1,i0+1,n2-2,n1,n0)]-ldz.data[cIdx4D(i3+1,i2,i1+1,i0+1,n2-2,n1,n0)]);
            result+= (ldt.data[cIdx4D(i3,i2+1,i1+1,i0+1,n2,n1,n0)]-ldt.data[cIdx4D(i3,i2+1,i1+1,i0+1,n2,n1,n0)])*(ldt.data[cIdx4D(i3,i2+1,i1+1,i0+1,n2,n1,n0)]-ldt.data[cIdx4D(i3,i2+1,i1+1,i0+1,n2,n1,n0)]);
          }
        }
      }
    } 
  }
  
  result /= n;
    
  result=sqrt(result);

  return result;
  
}
#endif
