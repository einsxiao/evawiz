#ifndef EVAWIZ_Class_H
#define EVAWIZ_Class_H

namespace evawiz{

  class ClassModule:public Module{
    INIT_SYMBOL_ID_OF( DefClass );
    INIT_SYMBOL_ID_OF( Init );
    INIT_SYMBOL_ID_OF( New );
    INIT_SYMBOL_ID_OF( Delete );
    DeclareModule(Class);
    //vector<ValueTable*> classes;
  public:
    Index valuePairIndexOfClass;
    DeclareFunction(Class);
    DeclareFunction(DefClass);
  };
};
#endif
