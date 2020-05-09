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


#endif
