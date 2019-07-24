#include"eva_headers.h"
using namespace std;
using namespace evawiz;

double Math::NaN(){
  unsigned long nan[2]={0xffffffff, 0x7fffffff}; // code representing a NaN
  return *(double*)nan;
}

bool Math::NumberQ(double num){
  return isnormal(num);
}

bool Math::IntegerQ(double num){
  return num == (int)num;
}

bool Math::FiniteQ(double num){
  return isfinite(num);
}

bool Math::InfQ(double num){
  return (num+1.0f==num);
}

bool Math::NanQ(double num){
  return !(num==num);
}

string Math::OrderForm(int id){

  int num =id; string str=ToString(id);
  num = num%100;
  if ( num<0 ) num = -num;
  if ( num >3 && num<21 ) return str+"th";
  num = num%10;
  switch (num)
    {
    case 1: return str+"st";
    case 2: return str+"nd";
    case 3: return str+"rd";
    default: return str+"th";
    }
}

bool Math::NumberQ(string str){
  int pos;
  if (str[0] == '-' || str[0] == '+' )
    str.erase(0,1);
  if ( str[str.size()-1] == '`' )
    str.erase(str.size()-1,1);
  if ( ((pos=str.find("."))>=0) )
    str.erase(pos,1);
  if ( ((pos=str.find("E"))>=0) ){
    if ( pos+1<(int)str.length() && (str[pos+1] == '+'||str[pos+1] == '-') )
      str.erase(pos+1,1);
    str.erase(pos,1);
  }
  if ( str=="" ) return false;
  while ( str!="" && '0'<=str[0] && str[0]<='9' )
    str.erase(0,1);
  if ( str=="" )
    return true;
  else
    return false;
}


int evawiz::GaussCMInverse(double *AA,double *B,int n){
  int i,j,k;
  double temp;
  double *A;
  A = new double[n*n];

  for (int i=0;i<n;i++)
    for (int j=0;j<n;j++){
      if (i==j) B[i*n+j]=1;else B[i*n+j]=0;
      A[i*n+j]=AA[i*n+j];
    }
  for (i=0;i<n;i++){
    //find the maximum one  ...
    k=i;
    temp=abs(A[i*n+i]);
    for (j=i;j<n;j++)
      if (abs(A[j*n+i])>temp){ k=j;temp=abs(A[j*n+i]); }
    //if no answer return error code -1
    if (temp==0){ delete A; return -1;}
    //change the one with the maximum one  ...
    if (i!=k)
      for (j=0;j<n;j++){
        temp=A[i*n+j];A[i*n+j]=A[k*n+j]; A[k*n+j]=temp;
        temp=B[i*n+j];B[i*n+j]=B[k*n+j]; B[k*n+j]=temp;
      }
    //dismiss 
    for (j=0;j<n;j++){
      if (j==i) continue;
      temp=A[j*n+i]/A[i*n+i];
      for (k=0;k<n;k++) {
        A[j*n+k]=A[j*n+k]-A[i*n+k]*temp;
        B[j*n+k]=B[j*n+k]-B[i*n+k]*temp;
      }
    }
    temp=A[i*n+i];
    for (j=0;j<n;j++){ A[i*n+j]=A[i*n+j]/temp;B[i*n+j]=B[i*n+j]/temp;}
  }
  delete A;
  return 0;
}

int evawiz::GaussCMInverse(double *AA,double *A, double *B,int n){
  // in which A is a temp for AA for no change in the original AA matrix
  int i,j,k;
  double temp;
  for (int i=0;i<n;i++)
    for (int j=0;j<n;j++){
      if (i==j) B[i*n+j]=1;else B[i*n+j]=0;
      A[i*n+j]=AA[i*n+j];
    }
  for (i=0;i<n;i++){
    //find the maximum one  ...
    k=i;
    temp=abs(A[i*n+i]);
    for (j=i;j<n;j++)
      if (abs(A[j*n+i])>temp){ k=j;temp=abs(A[j*n+i]); }
    //if no answer return error code -1
    //if (temp==0){ delete A; return -1;}
    if (temp==0){ return -1;}
    //change the one with the maximum one  ...
    if (i!=k)
      for (j=0;j<n;j++){
        temp=A[i*n+j];A[i*n+j]=A[k*n+j]; A[k*n+j]=temp;
        temp=B[i*n+j];B[i*n+j]=B[k*n+j]; B[k*n+j]=temp;
      }
    //dismiss 
    for (j=0;j<n;j++){
      if (j==i) continue;
      temp=A[j*n+i]/A[i*n+i];
      for (k=0;k<n;k++) {
        A[j*n+k]=A[j*n+k]-A[i*n+k]*temp;
        B[j*n+k]=B[j*n+k]-B[i*n+k]*temp;
      }
    }
    temp=A[i*n+i];
    for (j=0;j<n;j++){ A[i*n+j]=A[i*n+j]/temp;B[i*n+j]=B[i*n+j]/temp;}
  }
  return 0;
}

