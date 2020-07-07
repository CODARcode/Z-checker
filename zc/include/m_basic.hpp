#ifndef M_BASIC_HPP
#define M_BASIC_HPP



template <typename dT>
matrix<dT>::matrix(){
  nDim=1;
  size0=1;
  size1=1;
  size2=1;
  size3=1;
  data=NULL;
}

template <typename dT>
matrix<dT>::matrix(int _nDim, int _size0){
  nDim=_nDim;
  size0=_size0;
  size1=1;
  size2=1;
  size3=1;

  data=new dT[size0*size1*size2*size3];
}

template <typename dT>
matrix<dT>::matrix(int _nDim, int _size0, int _size1){
  nDim=_nDim;
  size0=_size0;
  size1=_size1;
  size2=1;
  size3=1;

  data=new dT[size0*size1*size2*size3];
}

template <typename dT>
matrix<dT>::matrix(int _nDim, int _size0, int _size1, int _size2){
  nDim=_nDim;
  size0=_size0;
  size1=_size1;
  size2=_size2;
  size3=1;

  data=new dT[size0*size1*size2*size3];
}

template <typename dT>
matrix<dT>::matrix(int _nDim, int _size0, int _size1, int _size2, int _size3){
  nDim=_nDim;
  size0=_size0;
  size1=_size1;
  size2=_size2;
  size3=_size3;

  data=new dT[size0*size1*size2*size3];
}

template <typename dT>
matrix<dT>::~matrix(){
  deallocate();
}

template <typename dT>
void matrix<dT>::deallocate(){
  if(data!=NULL)
    delete [] data;
  data=NULL;
}

template <typename dT>
void matrix<dT>::initialize(int _nDim, int _size0){
  initialize(_nDim,_size0,1,1,1);
}

template <typename dT>
void matrix<dT>::initialize(int _nDim, int _size0, int _size1){
  initialize(_nDim,_size0,_size1,1,1);
}

template <typename dT>
void matrix<dT>::initialize(int _nDim, int _size0, int _size1, int _size2){
  initialize(_nDim,_size0,_size1,_size2,1);
}

template <typename dT>
void matrix<dT>::initialize(int _nDim, int _size0, int _size1, int _size2, int _size3){
  nDim=_nDim;
  size0=_size0;
  size1=_size1;
  size2=_size2;
  size3=_size3;
  
  if(data!=NULL)
    delete []data;
  //cout<<"Initialize Size: "<<size0*size1*size2*size3<<endl;//AMG
  data=new dT[size0*size1*size2*size3];
  
  if(data==NULL){
    cout<<"Matrix initialization failed. Requested size was: "<<size0*size1*size2*size3<<" bytes"<<endl;
    assert(0);
  }
}
  
template <typename dT>
void matrix<dT>::readDataBin(string fileName){   
  #ifdef DEBUG_L0
    if(size0*size1*size2*size3 < 1){
      cout<<"ERROR: Bad matrix sizes."<<endl;
      cout<<"(size0,size1,size2,size3) = "<<"("<<size0<<","<<size1<<","<<size2<<","<<size3<<")"<<endl;
      assert(0);
    }
  #endif
  
  ifstream inFile;
  inFile.open(fileName,ios::in | ios::binary);
  inFile.read((char *)data,size0*size1*size2*size3*sizeof(dT));
  
  #ifdef DEBUG_CHECK_BOUNDARIES 
    if(inFile.eof()){
      cout<<"ERROR: EOF reached unexpectedly."<<endl;
      cout<<"Probably reading too long from a shorter file."<<endl;
      cout<<"Trying to read size: ( "<<size0<<" x "<<size1<<" x "<<size2<<" x "<<size3<<" ) from  file:"<<fileName<<endl;
      cout<<"(size0,size1,size2,size3)"<<endl;
      assert(0);
    }
     if(!inFile.good()){
      cout<<"ERROR: There is a problem with the input file."<<endl;
      cout<<"Trying to read size: ( "<<size0<<" x "<<size1<<" x "<<size2<<" x "<<size3<<" ) from  file:"<<fileName<<endl;
      cout<<"(size0,size1,size2,size3)"<<endl;
      assert(0);
    } 
  #endif
  
  inFile.close();
}

template <typename dT>
void matrix<dT>::writeDataBin(string fileName){
  #ifdef DEBUG_L0
    if(size0*size1*size2*size3 < 1){
      cout<<"ERROR: Bad matrix sizes."<<endl;
      cout<<"(size0,size1,size2,size3) = "<<"("<<size0<<","<<size1<<","<<size2<<","<<size3<<")"<<endl;
      assert(0);
    }
  #endif
  
  ofstream outFile;
  outFile.open(fileName,ios::out | ios::binary);
  outFile.write((char *)data,size0*size1*size2*size3*sizeof(dT));
  
  outFile.close();
}

template <typename dT>
void matrix<dT>::writeDataText_Double(string fileName){
  #ifdef DEBUG_L0
    if(size0*size1*size2*size3 < 1){
      cout<<"ERROR: Bad matrix sizes."<<endl;
      cout<<"(size0,size1,size2,size3) = "<<"("<<size0<<","<<size1<<","<<size2<<","<<size3<<")"<<endl;
      assert(0);
    }
  #endif
  
  ofstream outFile;
  outFile.open(fileName,ios::out);
  
  int i0,i1,i2,i3;
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          outFile<<(double)data[i0+size0*(i1+size1*(i2+size2*i3))]<<" ";
        }
        outFile<<endl;
      }
      outFile<<"******************************** End of: dim2 = "<<i2<<" (dim3 = "<<i3<<")"<<endl;
    }
    outFile<<"----------------------------------------------------------------"<<endl;
    outFile<<"------------------- End of: dim3 = "<<i3<<" ---------------------------"<<endl;
    outFile<<"----------------------------------------------------------------"<<endl;
  }
  
  outFile.close();
}

template <typename dT>
void matrix<dT>::print(){   
  int i0,i1,i2,i3;
  cout<<"Printing matrix. dim0 and dim1 traversels are printed as matrixes."<<endl;
  cout<<"dim2 and dim3 traversels are marked by using separator lines filled with * and -"<<endl;
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          cout<<data[i0+size0*(i1+size1*(i2+size2*i3))]<<" ";
        }
        cout<<endl;
      }
      cout<<"******************************** End of: dim2 = "<<i2<<" (dim3 = "<<i3<<")"<<endl;
    }
    cout<<"----------------------------------------------------------------"<<endl;
    cout<<"------------------- End of: dim3 = "<<i3<<" ---------------------------"<<endl;
    cout<<"----------------------------------------------------------------"<<endl;
  }
}

template <typename dT>
void matrix<dT>::print_Double(){   
  int i0,i1,i2,i3;
  cout<<"Printing matrix. dim0 and dim1 traversels are printed as matrixes."<<endl;
  cout<<"dim2 and dim3 traversels are marked by using separator lines filled with * and -"<<endl;
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          cout<<(double)data[i0+size0*(i1+size1*(i2+size2*i3))]<<" ";
        }
        cout<<endl;
      }
      cout<<"******************************** End of: dim2 = "<<i2<<" (dim3 = "<<i3<<")"<<endl;
    }
    cout<<"----------------------------------------------------------------"<<endl;
    cout<<"------------------- End of: dim3 = "<<i3<<" ---------------------------"<<endl;
    cout<<"----------------------------------------------------------------"<<endl;
  }
}

template <typename dT>
void matrix<dT>::print_Long(){   
  int i0,i1,i2,i3;
  cout<<"Printing matrix. dim0 and dim1 traversels are printed as matrixes."<<endl;
  cout<<"dim2 and dim3 traversels are marked by using separator lines filled with * and -"<<endl;
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          cout<<(long)data[i0+size0*(i1+size1*(i2+size2*i3))]<<" ";
        }
        cout<<endl;
      }
      cout<<"******************************** End of: dim2 = "<<i2<<" (dim3 = "<<i3<<")"<<endl;
    }
    cout<<"----------------------------------------------------------------"<<endl;
    cout<<"------------------- End of: dim3 = "<<i3<<" ---------------------------"<<endl;
    cout<<"----------------------------------------------------------------"<<endl;
  }
}

template <typename dT>
void matrix<dT>::printStats(){   
  int i0,i1,i2,i3;
  dT d,max, min;
  double sum=0;
  double sumAbs=0;
  
  max=data[0];
  min=data[0];
  //cout<<"Calculating matrix statistics..."<<endl;
  for(i3=0;i3<size3;i3++){
    for(i2=0;i2<size2;i2++){
      for(i1=0;i1<size1;i1++){
        for(i0=0;i0<size0;i0++){
          d=data[i0+size0*(i1+size1*(i2+size2*i3))];
          if(d>max)
            max=d;
          if(d<min)
            min=d;
          sum+=d;
          sumAbs+=fabs(d);
        }

      }
    }
  }
  int n=size0*size1*size2*size3;
  //cout<<"DONE."<<endl;
  cout<<"Printing matrix statistics:"<<endl;
  cout<<"Max   : "<<max<<endl;
  cout<<"Min   : "<<min<<endl;
  cout<<"Avg   : "<<sum/n<<endl;
  cout<<"AvgAbs: "<<sumAbs/n<<endl;
}

#endif
