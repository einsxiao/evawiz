/****************************/
#include<evawiz>          
#include"TemplateModule.h" 
using namespace std;
using namespace evawiz;
/****************************/
__DEBUG_MAIN__("input.ev");
ModuleInitialize
{
  dout<<"Module "<<__ModuleNameStr(ModuleName)<<" is constructing."<<endl;

}

ModuleFinalize
{
  dout<<"Module "<<__ModuleNameStr(ModuleName)<<" is deconstructing."<<endl;

}

/****************************/
DefineFunction(TestFunction,"A test Function can be called both from c/c++/cudac(int TestFunction_Eva(Object&,Evaluation*))"
    "and eva kernel (TestFunction[...])."){
  cout<<"Module Template is created from the standard template."<<endl;
  Matrix arr;
  arr.Init(2,3,3);
  arr = 3.14;
  cout<<arr<<endl;
  arr.HostToDevice();
#pragma evawiz launch kernel<<<i:3,j:3>>>(Matrix mat:arr)
  {
    mat(i,j)*=3;
  }
  arr.DeviceToHost();
  cout<<arr<<endl;
  ReturnNull;
}


