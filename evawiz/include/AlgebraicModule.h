#ifndef AlgebraicModule_Eins_H
#define AlgebraicModule_Eins_H

namespace evawiz{
  class AlgebraicModule:public Module{
  private:
    Object SSRules;
  public:
    AlgebraicModule(string,string);
    ~AlgebraicModule();
    /////////////////////////////////
    int SimpleSimplify(Object&);
  };
};


#endif

