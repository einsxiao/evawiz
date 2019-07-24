/****************************/
#include<evawiz>
#include"TemplateModule.h"
using namespace std;
using namespace evawiz;
/****************************/
__DEBUG_MAIN__("input.ev");
ModuleInitialize{
}

ModuleFinalize{}

DefineFunction(Intro,"Function to give introduction of module 'Template'"){
  cout<<"Module Template is created from the standard template."<<endl;
  ReturnNull;
}

extern "C" void __fortmod_MOD_fortfunc(double*,double*);
DefineFunction(CFortranFunction){
  CheckShouldEqual(1);
  CheckShouldBeNumber(1);
  double n = Argv[1].Number();
  double out; 
  __fortmod_MOD_fortfunc(&n,&out);
  ReturnNumber(out);
}

