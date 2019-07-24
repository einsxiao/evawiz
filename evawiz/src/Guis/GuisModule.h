#ifndef GuisModule_Evawiz_H
#define GuisModule_Evawiz_H

namespace evawiz{

  
  class GuisModule:public Module
  {
    DeclareModule(Guis);
  public:
    DeclareFunction(Notepad);
    DeclareFunction(Runmod);
  };
};
#endif
