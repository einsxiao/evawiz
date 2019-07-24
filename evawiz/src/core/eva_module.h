#ifndef Modules_Evawiz_H
#define Modules_Evawiz_H

////////////////////////////////////////////////////////////////////////////////////////
namespace evawiz{
  
  class Module{
  private:
    string		 moduleName,moduleDescreption,moduleDirectory;
    int        importMethod;
  public: //
    Module(string name,string module_dir=""); 
    virtual   ~Module();
    string     GetModuleName();
    string     GetModuleDirectory();
    int        SetModuleDirectory(string dir);
    //////////////////////////////
    int        RegisterFunction(const char*funcName,MemberFunction func,const char*des="",int method = -1);
    int        RegisterFunction(const char*funcName,StaticFunction func,const char*des="",int method = -1);
    int        AddAttribute(const char*key, const char*attribute);
    int        AddAttribute(const char*key, const AttributeType type);
    //////////////////////////////
    int        RegisterVariable(const char*varName,double *var);
    int        RegisterVariable(const char*varName,complex *var);
    int        RegisterVariable(const char*varName,string *var);
    int        RegisterVariable(const char*varName,Object var);
    //////////////////////////////
    string     ModuleDescription();
    int        SetModuleDescription(string des);
    //////////////////////////////
  };
  typedef Module  *type_create(string name,string moduledir);
  typedef void     type_destroy(Module*);
  typedef Object   type_return_functions();
  typedef int      type_regsterfunction(Module*);


  /////////new way
//#define ModuleName System
////////////////////////////////////////////////////////////////////////////////////////
#define _ModuleNameStr(moduleName) #moduleName
#define __ModuleNameStr(moduleName) _ModuleNameStr(moduleName)
#define ModuleNameStr __ModuleNameStr(ModuleName)
////////////////////////////////////////////////////////////////////////////////////////
#define _ModuleType(moduleName) moduleName##Module
#define __ModuleType(moduleName) _ModuleType(moduleName)
#define ModuleType __ModuleType(ModuleName)
////////////////////////////////////////////////////////////////////////////////////////
#define _create_ModuleType(moduleName) __create_##moduleName##Module
#define __create_ModuleType(moduleName) _create_ModuleType(moduleName)
#define CreateModuleType __create_ModuleType(ModuleName)
////////////////////////////////////////////////////////////////////////////////////////
#define _destroy_ModuleType(moduleName) __destroy_##moduleName##Module
#define __destroy_ModuleType(moduleName) _destroy_ModuleType(moduleName)
#define DestroyModuleType __destroy_ModuleType(ModuleName)
////////////////////////////////////////////////////////////////////////////////////////
#define _destroy_ModuleType(moduleName) __destroy_##moduleName##Module
#define __destroy_ModuleType(moduleName) _destroy_ModuleType(moduleName)
#define DestroyModuleType __destroy_ModuleType(ModuleName)
////////////////////////////////////////////////////////////////////////////////////////
#define _ReturnFunctionsType(module) __return_functions_##module
#define __ReturnFunctionsType(module) _ReturnFunctionsType(module)
#define ReturnFunctionsType __ReturnFunctionsType(ModuleName)
////////////////////////////////////////////////////////////////////////////////////////
#define _RegisterFunctionsType(module,func) func##_Eva_Register_##module
#define RegisterFunctionsType(module,func) _RegisterFunctionsType(module,func)
////////////////////////////////////////////////////////////////////////////////////////
#define DeclareModule(module) public: module##Module(string); ~module##Module(); private: unsigned int __MODULE_DECLARE_MARK
#define DeclareFunction(func) int func##_Eva(Object&Argv)
////////////////////////////////////////////////////////////////////////////////////////
#define DefineFunction(func,description...)                             \
  int func##_Eva_init_int(__register_function(#func));                  \
  extern "C" int RegisterFunctionsType(ModuleName,func)(Module*module){ \
    return module->RegisterFunction(#func,(MemberFunction)(&ModuleType::func##_Eva),##description); \
  };                                                                    \
  int ModuleType::func##_Eva(Object&Argv)
  ////////////////////////////////////////////////////////////////////////////////////////
#define __DEBUG_MAIN__(file)                                    \
  int main(int argc,char*argv[]){                               \
    evawiz::dout<<">>>debug main to run '"<<file<<"'<<<"<<endl; \
    evawiz::Evaluation eva(true);                               \
    eva.argc = argc; eva.argv = argv;                           \
    try{                                                        \
      eva.EvaluateFile(file);                                   \
    }catch ( const ExceptionQuit&err ){                         \
      return err.code;                                          \
    }catch ( const Error&err){                                  \
      cerr<<endl<<err.what()<<endl;                             \
    }catch ( const exception &err){                             \
      cerr<<endl<<"Sourcecode::Error: "<<err.what()<<endl;      \
      return 1;                                                 \
    }catch ( ... ){                                             \
      cerr<<"\nSystem::Error: Unexpected error occured."<<endl; \
      return 1;                                                 \
    }                                                           \
    return 0;                                                   \
  }                                                                 

#define ModuleInitialize                                                \
  static Object __module_function_list;                                 \
  static int __register_function(string func){                          \
    if ( __module_function_list.NullQ() )                               \
      __module_function_list.SetList_List();                            \
    __module_function_list.PushBackSymbol( func.c_str() );              \
    return 0;                                                           \
  }                                                                     \
  extern "C" Object ReturnFunctionsType(){                              \
    return __module_function_list;                                      \
  };                                                                    \
  extern "C" Module *CreateModuleType(string name,string moduledir){    \
    return new ModuleType(moduledir);                                   \
  }                                                                     \
  extern "C" void DestroyModuleType(Module*p){ delete (ModuleType*)p; } \
  ModuleType::ModuleType(string __module_src_dir):Module(ModuleNameStr,__module_src_dir)
////////////////////////////////////////////////////////////////////////////////////////
#define ModuleFinalize ModuleType::~ModuleType()
////////////////////////////////////////////////////////////////////////////////////////
///////////////



};

#endif




