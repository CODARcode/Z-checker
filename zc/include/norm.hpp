#ifndef NORM_HPP
#define NORM_HPP


//Supporting Norm functions:
template <typename dT> 
inline double _max(dT a0,dT a1){
  return a0>a1 ? a0 : a1;
}

//Sum(|Xi|)
template <typename dT> 
inline double vec_norm_1(dT a0){
  return fabs(a0);
}

template <typename dT> 
inline double vec_norm_1(dT a0, dT a1){
  return fabs(a0) + fabs(a1);
}

template <typename dT> 
inline double vec_norm_1(dT a0, dT a1, dT a2){
  return fabs(a0) + fabs(a1) + fabs(a2);
}

template <typename dT> 
inline double vec_norm_1(dT a0, dT a1, dT a2, dT a3){
  return fabs(a0) + fabs(a1) + fabs(a2) + fabs(a3);
}

//Sqrt(Sum(|Xi|^2))
template <typename dT> 
inline double vec_norm_2(dT a0){
  return fabs(a0);
}

template <typename dT> 
inline double vec_norm_2(dT a0, dT a1){
  return sqrt( pow(a0,2)+pow(a1,2) );
}

template <typename dT> 
inline double vec_norm_2(dT a0, dT a1, dT a2){
  return sqrt( pow(a0,2)+pow(a1,2)+pow(a2,2) );
}

template <typename dT> 
inline double vec_norm_2(dT a0, dT a1, dT a2, dT a3){
  return sqrt( pow(a0,2)+pow(a1,2)+pow(a2,2)+pow(a3,2) );
}

//Max(|Xi|)
template <typename dT> 
inline double vec_norm_INF(dT a0){
  return fabs(a0);
}

template <typename dT> 
inline double vec_norm_INF(dT a0, dT a1){
  return _max(fabs(a0),fabs(a1));
}

template <typename dT> 
inline double vec_norm_INF(dT a0, dT a1, dT a2){
  double max01=vec_norm_INF(a0,a1);
  double ab2=fabs(a2);
  if(max01>ab2)
    return max01;
  else
    return ab2;
}

template <typename dT> 
inline double vec_norm_INF(dT a0, dT a1, dT a2, dT a3){
  double max01=vec_norm_INF(a0,a1);
  double max23=vec_norm_INF(a2,a3);
  if(max01>max23)
    return max01;
  else
    return max23;
}

template <typename dT> 
inline double m2x2_norm_1(dT m[2][2]){ //Max(sum(fabs(col)))
  double v0=fabs(m[0][0])+fabs(m[1][0]);
  double v1=fabs(m[0][1])+fabs(m[1][1]);
  return _max(v0,v1);
}

template <typename dT> 
inline double m3x3_norm_1(dT m[3][3]){ //Max(sum(fabs(col)))
  double v0=fabs(m[0][0])+fabs(m[1][0])+fabs(m[2][0]);
  double v1=fabs(m[0][1])+fabs(m[1][1])+fabs(m[2][1]);
  double v2=fabs(m[0][2])+fabs(m[1][2])+fabs(m[2][2]);
  double max01=_max(v0,v1);
  return _max(max01,v2);
}

template <typename dT> 
inline double m4x4_norm_1(dT m[4][4]){ //Max(sum(fabs(col)))
  double v0=fabs(m[0][0])+fabs(m[1][0])+fabs(m[2][0])+fabs(m[3][0]);
  double v1=fabs(m[0][1])+fabs(m[1][1])+fabs(m[2][1])+fabs(m[3][1]);
  double v2=fabs(m[0][2])+fabs(m[1][2])+fabs(m[2][2])+fabs(m[3][2]);
  double v3=fabs(m[0][3])+fabs(m[1][3])+fabs(m[2][3])+fabs(m[3][3]);

  double max01=_max(v0,v1);
  double max23=_max(v2,v3);
  return _max(max01,max23);
}


template <typename dT> 
inline double m2x2_norm_2(dT m[2][2]){ //Max(Eig(M_T x M))
  cout<<"WARNING: Function is not implemented yet: m2x2_norm_2 : 2x2 Matrix 2-Norm"<<endl;
  cout<<"Returning -1"<<endl;
  return -1;
}
template <typename dT> 
inline double m3x3_norm_2(dT m[3][3]){ //Max(Eig(M_T x M))
  cout<<"WARNING: Function is not implemented yet: m3x3_norm_2 : 3x3 Matrix 2-Norm"<<endl;
  cout<<"Returning -1"<<endl;
  return -1;
}

template <typename dT> 
inline double m4x4_norm_2(dT m[4][4]){ //Max(Eig(M_T x M))
  cout<<"WARNING: Function is not implemented yet: m4x4_norm_2 : 4x4 Matrix 2-Norm"<<endl;
  cout<<"Returning -1"<<endl;
  return -1;
}

template <typename dT> 
inline double m2x2_norm_INF(dT m[2][2]){ //Max(sum(fabs(row)))
  double v0=fabs(m[0][0])+fabs(m[0][1]);
  double v1=fabs(m[1][0])+fabs(m[1][1]);
  return _max(v0,v1);
}

template <typename dT> 
inline double m3x3_norm_INF(dT m[3][3]){ //Max(sum(fabs(row)))
  double v0=fabs(m[0][0])+fabs(m[0][1])+fabs(m[0][2]);
  double v1=fabs(m[1][0])+fabs(m[1][1])+fabs(m[1][2]);
  double v2=fabs(m[2][0])+fabs(m[2][1])+fabs(m[2][2]);
  double max01=_max(v0,v1);
  return _max(max01,v2);
}

template <typename dT> 
inline double m4x4_norm_INF(dT m[4][4]){ //Max(sum(fabs(row)))
  double v0=fabs(m[0][0])+fabs(m[0][1])+fabs(m[0][2])+fabs(m[0][3]);
  double v1=fabs(m[1][0])+fabs(m[1][1])+fabs(m[1][2])+fabs(m[1][3]);
  double v2=fabs(m[2][0])+fabs(m[2][1])+fabs(m[2][2])+fabs(m[2][3]);
  double v3=fabs(m[3][0])+fabs(m[3][1])+fabs(m[3][2])+fabs(m[3][3]);
  double max01=_max(v0,v1);
  double max23=_max(v2,v3);
  return _max(max01,max23);
}

template <typename dT> 
inline double m2x2_norm_Frobenius(dT m[2][2]){ //Sqrt(Sum(|Xij|^2)) 
  return sqrt(pow(fabs(m[0][0]),2)+pow(fabs(m[0][1]),2)+pow(fabs(m[1][0]),2)+pow(fabs(m[1][1]),2));
}

template <typename dT> 
inline double m3x3_norm_Frobenius(dT m[3][3]){ //Sqrt(Sum(|Xij|^2)) 
  return sqrt(
   pow(fabs(m[0][0]),2)+pow(fabs(m[0][1]),2)+pow(fabs(m[0][2]),2)
  +pow(fabs(m[1][0]),2)+pow(fabs(m[1][1]),2)+pow(fabs(m[1][2]),2)
  +pow(fabs(m[2][0]),2)+pow(fabs(m[2][1]),2)+pow(fabs(m[2][2]),2)
  );
}

template <typename dT> 
inline double m4x4_norm_Frobenius(dT m[4][4]){ //Sqrt(Sum(|Xij|^2)) 
  return sqrt(
   pow(fabs(m[0][0]),2)+pow(fabs(m[0][1]),2)+pow(fabs(m[0][2]),2)+pow(fabs(m[0][3]),2)
  +pow(fabs(m[1][0]),2)+pow(fabs(m[1][1]),2)+pow(fabs(m[1][2]),2)+pow(fabs(m[1][3]),2)
  +pow(fabs(m[2][0]),2)+pow(fabs(m[2][1]),2)+pow(fabs(m[2][2]),2)+pow(fabs(m[2][3]),2)
  +pow(fabs(m[3][0]),2)+pow(fabs(m[3][1]),2)+pow(fabs(m[3][2]),2)+pow(fabs(m[3][3]),2)
  );
}



#endif
