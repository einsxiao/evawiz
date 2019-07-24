#ifndef OldMacro_Here_Evawiz_H
#define OldMacro_Here_Evawiz_H

#define _POSSIBLE__MODULE_NAME__ 

///////////////////////////////////////////////////////////////////////old way
#define __CREATE_MODULE__( __module_name__)                             \
  extern "C" Module *__create_##__module_name__##Module(string name,string moduledir){ \
    return new __module_name__##Module(name,moduledir);                 \
  };                                                                    \
  extern "C" void __destroy_##__module_name__##Module(Module*p){        \
    delete p;                                                           \
  };                                                                    \
  static string __MODULE_NAME__ (#__module_name__);  
#define __PARA__ ) :Module(__MODULE_NAME__,
#define __CONSTRUCTION_PARA__ string __module_name__,string __module_src_dir) :Module(__module_name__,__module_src_dir
////////////

#endif
