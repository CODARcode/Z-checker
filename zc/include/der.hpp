#ifndef DER_HPP
#define DER_HPP


template <typename dT>
inline double matrix<dT>::der_1d(int idx0){
  return (data[idx0+1]-data[idx0-1])/2;
}

template <typename dT>
inline double matrix<dT>::der_2d_dim0(int idx0,int idx1){
  return (data[idx0+1+size0*idx1]-data[idx0-1+size0*idx1])/2;
}

template <typename dT>
inline double matrix<dT>::der_2d_dim1(int idx0,int idx1){
  return (data[idx0+size0*(idx1+1)]-data[idx0+size0*(idx1-1)])/2;
}

template <typename dT>
inline double matrix<dT>::der_3d_dim0(int idx0,int idx1,int idx2){
  return (data[idx0+1+size0*(idx1+size1*idx2)]-data[idx0-1+size0*(idx1+size1*idx2)])/2;
}

template <typename dT>
inline double matrix<dT>::der_3d_dim1(int idx0,int idx1,int idx2){
  return (data[idx0+size0*((idx1+1)+size1*idx2)]-data[idx0+size0*((idx1-1)+size1*idx2)])/2;
}

template <typename dT>
inline double matrix<dT>::der_3d_dim2(int idx0,int idx1,int idx2){
  return (data[idx0+size0*(idx1+size1*(idx2+1))]-data[idx0+size0*(idx1+size1*(idx2-1))])/2;
}

template <typename dT>
inline double matrix<dT>::der_4d_dim0(int idx0,int idx1,int idx2,int idx3){
  return (data[idx0+1+size0*(idx1+size1*(idx2+size2*idx3))]-data[idx0-1+size0*(idx1+size1*(idx2+size2*idx3))])/2;
}

template <typename dT>
inline double matrix<dT>::der_4d_dim1(int idx0,int idx1,int idx2,int idx3){
  return (data[idx0+size0*((idx1+1)+size1*(idx2+size2*idx3))]-data[idx0+size0*((idx1-1)+size1*(idx2+size2*idx3))])/2;
}

template <typename dT>
inline double matrix<dT>::der_4d_dim2(int idx0,int idx1,int idx2,int idx3){
  return (data[idx0+size0*(idx1+size1*((idx2+1)+size2*idx3))]-data[idx0+size0*(idx1+size1*((idx2-1)+size2*idx3))])/2;
}

template <typename dT>
inline double matrix<dT>::der_4d_dim3(int idx0,int idx1,int idx2,int idx3){
  return (data[idx0+size0*(idx1+size1*(idx2+size2*(idx3+1)))]-data[idx0+size0*(idx1+size1*(idx2+size2*(idx3-1)))])/2;
}




#endif
