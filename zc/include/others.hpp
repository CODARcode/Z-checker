#ifndef OTHERS_HPP
#define OTHERS_HPP


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
  
  int i0;
  int n;
  double result=0;
  
  matrix<dT> ldx,ldy,ldz,ldt;  //Partial derivatives of left hand side
  matrix<dT> rdx,rdy,rdz,rdt;  //Partial derivatives of right hand side
  
  if(nDim==1){
    m_der_1d_v2(ldx);
    rhs.m_der_1d_v2(rdx);
    n=ldx.size0*ldx.size1*ldx.size2*ldx.size3;
    for(i0=0;i0<n;i0++){
      result+= (data[i0]-rhs.data[i0])*(data[i0]-rhs.data[i0]);
      result+= (ldx.data[i0]-ldx.data[i0])*(ldx.data[i0]-ldx.data[i0]);
    }
  }else if(nDim==2){
    m_der_2d_dim0_v2(ldx);
    m_der_2d_dim1_v2(ldy);
    rhs.m_der_2d_dim0_v2(rdx);
    rhs.m_der_2d_dim1_v2(rdy);
    n=ldx.size0*ldx.size1*ldx.size2*ldx.size3;
    for(i0=0;i0<n;i0++){
      result+= (data[i0]-rhs.data[i0])*(data[i0]-rhs.data[i0]);
      result+= (ldx.data[i0]-ldx.data[i0])*(ldx.data[i0]-ldx.data[i0]);
      result+= (ldy.data[i0]-ldy.data[i0])*(ldy.data[i0]-ldy.data[i0]);
    }
  }else if(nDim==3){
    m_der_3d_dim0_v2(ldx);
    m_der_3d_dim1_v2(ldy);
    m_der_3d_dim2_v2(ldz);
    rhs.m_der_3d_dim0_v2(rdx);
    rhs.m_der_3d_dim1_v2(rdy);
    rhs.m_der_3d_dim2_v2(rdz);
    n=ldx.size0*ldx.size1*ldx.size2*ldx.size3;
    for(i0=0;i0<n;i0++){
      result+= (data[i0]-rhs.data[i0])*(data[i0]-rhs.data[i0]);
      result+= (ldx.data[i0]-ldx.data[i0])*(ldx.data[i0]-ldx.data[i0]);
      result+= (ldy.data[i0]-ldy.data[i0])*(ldy.data[i0]-ldy.data[i0]);
      result+= (ldz.data[i0]-ldz.data[i0])*(ldz.data[i0]-ldz.data[i0]);
    }
  }else if(nDim==4){
    m_der_4d_dim0_v2(ldx);
    m_der_4d_dim1_v2(ldy);
    m_der_4d_dim2_v2(ldz);
    m_der_4d_dim3_v2(ldt);
    rhs.m_der_4d_dim0_v2(rdx);
    rhs.m_der_4d_dim1_v2(rdy);
    rhs.m_der_4d_dim2_v2(rdz);
    rhs.m_der_4d_dim3_v2(rdt);
    n=ldx.size0*ldx.size1*ldx.size2*ldx.size3;
    for(i0=0;i0<n;i0++){
      result+= (data[i0]-rhs.data[i0])*(data[i0]-rhs.data[i0]);
      result+= (ldx.data[i0]-ldx.data[i0])*(ldx.data[i0]-ldx.data[i0]);
      result+= (ldy.data[i0]-ldy.data[i0])*(ldy.data[i0]-ldy.data[i0]);
      result+= (ldz.data[i0]-ldz.data[i0])*(ldz.data[i0]-ldz.data[i0]);
      result+= (ldt.data[i0]-ldt.data[i0])*(ldt.data[i0]-ldt.data[i0]);
    } 
  }
  
  result /= n;
    
  result=sqrt(result);

  return result;
  
}
#endif
