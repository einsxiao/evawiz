/****************************/
#include"evawiz.h"
#include"TemplateModule.h" 
using namespace std;
using namespace evawiz;
/****************************/
__DEBUG_MAIN__("input.ev");
ModuleInitialize
{
  cout<< "init here"<<endl;

}

ModuleFinalize
{

}

/****************************/
DefineFunction(TestFunction){
  cout<<"Module Template is created from the standard template."<<endl;
  Object t = Argv;
  t.PushBackString("333");
  t.PushBackNumber(333);
  vector<int> vec{1,2,3,4};
  Object x,y;
  x.SetString("333");
  y.SetSymbol("xxx");
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


/*
  (du-onekey-compile) 
 */
