/****************************/
#include"evawiz.h"
#include"TemplateModule.h" 
using namespace std;
using namespace evawiz;
/****************************/
__DEBUG_MAIN__("input.ev");
ModuleInitialize
{

}

ModuleFinalize
{

}

/****************************/
DefineFunction(TestFunction){
  cout<<"Module Template is created from the standard template."<<endl;
  Matrix arr;
  arr.Init(2,3,3);
  arr= 3.14;
  cout<<arr<<endl;
  arr.HostToDevice(); // will be ignored in pure-cpu mode
#pragma evawiz launch kernel<<<i:3,j:3>>>(Matrix mat:arr)
  {
    mat(i,j)*=3;
  }
  arr.DeviceToHost();// will be ignored in pure-cpu mode
  cout<<arr<<endl;
  ReturnNull;
}


