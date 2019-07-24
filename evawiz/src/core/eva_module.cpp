#include"eva_headers.h"
using namespace std;
using namespace evawiz;

///////////////////////////////////////////////////////////
Module::Module(string name,string module_dir){
  this->moduleName = name;
  this->moduleDirectory = module_dir;
  EvaKernel->InsertModule(moduleName,this);
  EvaKernel->SetAttribute(moduleName.c_str(),AttributeType::Protected);
};

Module::~Module(){
};

string Module::GetModuleName(){return moduleName;}

string Module::GetModuleDirectory(){return moduleDirectory;}

int Module::SetModuleDirectory(string dir){ moduleDirectory = dir; return 0; }

int Module::RegisterFunction(const char* funcName, MemberFunction func,const char*descri,int method){
  Object temp; temp.SetSymbol( funcName );
  EvaRecord *rec = EvaKernel->GetOrNewEvaRecord(temp);
  SetAttribute(rec->attributes,AttributeType::Protected);
  rec->staticFunction = NULL;
  rec->module = this;
  rec->memberFunction = func;
  rec->description = descri;
  return 0;
}

int Module::RegisterFunction(const char* funcName, StaticFunction func,const char*descri, int method){
  Object temp; temp.SetSymbol( funcName );
  EvaRecord *rec = EvaKernel->GetOrNewEvaRecord(temp);
  SetAttribute(rec->attributes,AttributeType::Protected);
  rec->staticFunction = func;
  rec->module = NULL;
  rec->memberFunction = NULL;
  rec->description = descri;
  return 0;
}

int Module::AddAttribute(const char*funcName, const char* attribute){
  EvaKernel->SetAttribute(funcName,evawiz::String2AttributeType(attribute) );
  return 0;
}

int Module::AddAttribute(const char*funcName, const AttributeType attribute){
  EvaKernel->SetAttribute(funcName,attribute );
  return 0;
}

int Module::SetModuleDescription(string des){
  moduleDescreption = des;
  return 0;
}

string Module::ModuleDescription(){
  return moduleDescreption;
}



