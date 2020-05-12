#ifndef M_DER_V1_HPP
#define M_DER_V1_HPP


template <typename dT>
void matrix<dT>::m_der_1d_v1(matrix<dT> &res){
  int i0;
  res.initialize(nDim, size0-1);
  for(i0=0;i0<size0-1;i0++){
    res.data[i0]=data[i0+1]-data[i0];
  }
}

template <typename dT>
void matrix<dT>::m_der_2d_dim0_v1(matrix<dT> &res){
  int i0,i1,index,index2;
  res.initialize(nDim, size0-1,size1); 
  for(i1=0;i1<size1;i1++){
    for(i0=0;i0<size0-1;i0++){
      index=i0+size0*i1;
      index2=i0+(size0-1)*i1;
      res.data[index2]=data[index+1]-data[index];
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_2d_dim1_v1(matrix<dT> &res){
  int i0,i1,index;
  res.initialize(nDim, size0,size1-1); 
  for(i1=0;i1<size1-1;i1++){
    for(i0=0;i0<size0;i0++){
      index=i0+size0*i1;
      res.data[index]=data[index+size0]-data[index];
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_3d_dim0_v1(matrix<dT> &res){
  int i0,i1,i2,index,index2;
  res.initialize(nDim, size0-1,size1,size2); 
  for(i2=0;i2<size2;i2++){
    for(i1=0;i1<size1;i1++){
      for(i0=0;i0<size0-1;i0++){
        index=i0+size0*(i1+size1*i2);
        index2=i0+(size0-1)*(i1+size1*i2);
        res.data[index2]=data[index+1]-data[index];
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_3d_dim1_v1(matrix<dT> &res){
  int i0,i1,i2,index,index2;
  res.initialize(nDim, size0,size1-1,size2);
  for(i2=0;i2<size2;i2++){
    for(i1=0;i1<size1-1;i1++){
      for(i0=0;i0<size0;i0++){
        index=i0+size0*(i1+size1*i2);
        index2=i0+size0*(i1+(size1-1)*i2);
        res.data[index2]=data[index+size0]-data[index];
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_3d_dim2_v1(matrix<dT> &res){
  int i0,i1,i2,index;
  res.initialize(nDim, size0,size1,size2-1); 
  for(i2=0;i2<size2-1;i2++){
    for(i1=0;i1<size1;i1++){
      for(i0=0;i0<size0;i0++){
        index=i0+size0*(i1+size1*i2);
        res.data[index]=data[index+(size1*size0)]-data[index];
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim0_v1(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0-1,size1,size2,size3); 
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0-1;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          index2=i0+(size0-1)*(i1+size1*(i2+size2*i3));          
          res.data[index2]=data[index+1]-data[index];
        }
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim1_v1(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0,size1-1,size2,size3); 
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1-1;i1++){
        for(i0=0;i0<size0;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          index2=i0+size0*(i1+(size1-1)*(i2+size2*i3));
          res.data[index2]=data[index+size0]-data[index];
        }
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim2_v1(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0,size1,size2-1,size3); 
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2-1;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          index2=i0+size0*(i1+size1*(i2+(size2-1)*i3));
          res.data[index2]=data[index+(size1*size0)]-data[index];
        }
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim3_v1(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0,size1,size2,size3-1); 
  for(i3=0;i3<size3-1;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          res.data[index]=data[index+(size2*size1*size0)]-data[index];
        }
      }
    }
  }
}










template <typename dT>
void matrix<dT>::m_der_1d_v2(matrix<dT> &res){
  int i0;
  res.initialize(nDim, size0-2);
  for(i0=0;i0<size0-2;i0++){
    res.data[i0]=(data[i0+2]-data[i0])/2;
  }
}

template <typename dT>
void matrix<dT>::m_der_2d_dim0_v2(matrix<dT> &res){
  int i0,i1,index,index2;
  res.initialize(nDim, size0-2,size1); 
  for(i1=0;i1<size1;i1++){
    for(i0=0;i0<size0-2;i0++){
      index=i0+size0*i1;
      index2=i0+(size0-2)*i1;
      res.data[index2]=(data[index+2]-data[index])/2;
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_2d_dim1_v2(matrix<dT> &res){
  int i0,i1,index;
  res.initialize(nDim, size0,size1-2); 
  for(i1=0;i1<size1-2;i1++){
    for(i0=0;i0<size0;i0++){
      index=i0+size0*i1;
      res.data[index]=(data[index+2*size0]-data[index])/2;
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_3d_dim0_v2(matrix<dT> &res){
  int i0,i1,i2,index,index2;
  res.initialize(nDim, size0-2,size1,size2); 
  for(i2=0;i2<size2;i2++){
    for(i1=0;i1<size1;i1++){
      for(i0=0;i0<size0-2;i0++){
        index=i0+size0*(i1+size1*i2);
        index2=i0+(size0-2)*(i1+size1*i2);
        res.data[index2]=(data[index+2]-data[index])/2;
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_3d_dim1_v2(matrix<dT> &res){
  int i0,i1,i2,index,index2;
  res.initialize(nDim, size0,size1-2,size2);
  for(i2=0;i2<size2;i2++){
    for(i1=0;i1<size1-2;i1++){
      for(i0=0;i0<size0;i0++){
        index=i0+size0*(i1+size1*i2);
        index2=i0+size0*(i1+(size1-2)*i2);
        res.data[index2]=(data[index+2*size0]-data[index])/2;
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_3d_dim2_v2(matrix<dT> &res){
  int i0,i1,i2,index;
  res.initialize(nDim, size0,size1,size2-2); 
  for(i2=0;i2<size2-2;i2++){
    for(i1=0;i1<size1;i1++){
      for(i0=0;i0<size0;i0++){
        index=i0+size0*(i1+size1*i2);
        res.data[index]=(data[index+2*(size1*size0)]-data[index])/2;
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim0_v2(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0-2,size1,size2,size3); 
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0-2;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          index2=i0+(size0-2)*(i1+size1*(i2+size2*i3));          
          res.data[index2]=(data[index+2]-data[index])/2;
        }
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim1_v2(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0,size1-2,size2,size3); 
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1-2;i1++){
        for(i0=0;i0<size0;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          index2=i0+size0*(i1+(size1-2)*(i2+size2*i3));
          res.data[index2]=(data[index+2*size0]-data[index])/2;
        }
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim2_v2(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0,size1,size2-2,size3); 
  
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2-2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          index2=i0+size0*(i1+size1*(i2+(size2-2)*i3));
          res.data[index2]=(data[index+2*(size1*size0)]-data[index])/2;
        }
      }
    }
  }
}

template <typename dT>
void matrix<dT>::m_der_4d_dim3_v2(matrix<dT> &res){
  int i0,i1,i2,i3,index,index2;
  res.initialize(nDim, size0,size1,size2,size3-2); 
  for(i3=0;i3<size3-2;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          index=i0+size0*(i1+size1*(i2+size2*i3));
          res.data[index]=(data[index+2*(size2*size1*size0)]-data[index])/2;
        }
      }
    }
  }
}

#endif
