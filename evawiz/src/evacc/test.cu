#include<evawiz.h>
#include<iostream>
using namespace std;
using namespace evawiz;
#define x(name) ({#name})
int main(){

  cout<<"Hello Evawiz."<<endl;
  EvaSettings::SetRunningMode(evawiz::RunningModeCpu);
  cout<<evawiz::GPUDeviceQ()<<endl;
  Matrix arr;
  int Ni=3,Nj=2,Nk=1,Nl=2;
  arr.Init(4,Ni,Nj,Nk,Nl);
  arr = arr.Size();
  cout <<arr<<endl;

  arr.HostToDevice();
  {
    /*
#pragma evawiz launch kernel<<<i:Ni,j:Nj,k:Nk,l:Nl>>>(Matrix mat:arr,int a:6)
    {
      mat(i,j,k,l)=i+j+k+l;
    }
    */
  }
  arr.DeviceToHost();

  cout<<arr<<endl;
  cout<<"test for omp things"<<endl;
#pragma evawiz launch kernel<<<i:10>>>(int a:3)
  {
  }


  return 0; 

  
}
