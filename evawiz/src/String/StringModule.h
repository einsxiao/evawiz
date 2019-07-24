#ifndef StringModule_Eins_H
#define StringModule_Eins_H

namespace evawiz{
  class StringModule:public Module{
  public:
    DeclareModule(String);
    /////////////////////////////////
  public:
    DeclareFunction( StringSplit );
    DeclareFunction( RandomString );
    DeclareFunction( StringJoin );
  };
};

#endif

