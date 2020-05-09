#ifndef IDX_HPP
#define IDX_HPP

template <typename dT>
inline int matrix<dT>::idx(int dim0){
  #ifdef DEBUG_CHECK_BOUNDARIES 
    if(dim0>=size0){
      cout<<"ERROR: dim0 = "<<dim0<<" >= "<<size0<<" = size0"<<endl;
      assert(0);
    }
  #endif
  return dim0;
}

template <typename dT>
inline int matrix<dT>::idx(int dim0,int dim1){
  #ifdef DEBUG_CHECK_BOUNDARIES 
    if(dim0>=size0){
      cout<<"ERROR: dim0 = "<<dim0<<" >= "<<size0<<" = size0"<<endl;
      assert(0);
    }
    if(dim1>=size1){
      cout<<"ERROR: dim1 = "<<dim1<<" >= "<<size1<<" = size1"<<endl;
      assert(0);
    }
  #endif
  return dim0+size0*dim1;
}

template <typename dT>
inline int matrix<dT>::idx(int dim0,int dim1,int dim2){
  #ifdef DEBUG_CHECK_BOUNDARIES 
    if(dim0>=size0){
      cout<<"ERROR: dim0 = "<<dim0<<" >= "<<size0<<" = size0"<<endl;
      assert(0);
    }
    if(dim1>=size1){
      cout<<"ERROR: dim1 = "<<dim1<<" >= "<<size1<<" = size1"<<endl;
      assert(0);
    }
    if(dim2>=size2){
      cout<<"ERROR: dim2 = "<<dim2<<" >= "<<size2<<" = size2"<<endl;
      assert(0);
    }
  #endif
  return dim0+size0*(dim1+size1*dim2);
}

template <typename dT>
inline int matrix<dT>::idx(int dim0,int dim1,int dim2,int dim3){
  #ifdef DEBUG_CHECK_BOUNDARIES 
    if(dim0>=size0){
      cout<<"ERROR: dim0 = "<<dim0<<" >= "<<size0<<" = size0"<<endl;
      assert(0);
    }
    if(dim1>=size1){
      cout<<"ERROR: dim1 = "<<dim1<<" >= "<<size1<<" = size1"<<endl;
      assert(0);
    }
    if(dim2>=size2){
      cout<<"ERROR: dim2 = "<<dim2<<" >= "<<size2<<" = size2"<<endl;
      assert(0);
    }
    if(dim3>=size3){
      cout<<"ERROR: dim3 = "<<dim3<<" >= "<<size3<<" = size3"<<endl;
      assert(0);
    }
  #endif
  return dim0+size0*(dim1+size1*(dim2+size2*dim3));
}


#endif
