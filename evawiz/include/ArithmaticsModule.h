#ifndef Evawiz_ArithmaticsModule_H
#define Evawiz_ArithmaticsModule_H

namespace evawiz{
  class ArithmaticsModule: public Module{
  public:
    ///////////////////////////////
    ArithmaticsModule(string,string);
    ~ArithmaticsModule();
    ///////////////////////////////
    static int Plus(Object&);
    static int Times(Object&);
    static int Power(Object&);
    static int Log(Object&);
    static int Complex(Object&);
    ///////////////////////////////
    static int AddTo(Object&);
    static int SubstractFrom(Object&);
    static int TimesBy(Object&);
    static int DivideBy(Object&);
    static int PowerWith(Object&);
    ///////////////////////////////
    static int PreIncrement(Object&);
    static int PreDecrement(Object&);
    static int Increment(Object&);
    static int Decrement(Object&);
    ///////////////////////////////
    static int Greater(Object&);
    static int GreaterEqual(Object&);
    static int Less(Object&);
    static int LessEqual(Object&);
    static int Equal(Object&);
    static int UnEqual(Object&);
    ///////////////////////////////
    static int Not(Object&);
    static int And(Object&);
    static int Or(Object&);
    ///////////////////////////////
    static int Abs(Object&);
    static int Factorial(Object&);
    ///////////////////////////////
    static int SetRandomSeed(Object&);
    static int Random(Object&);
  };
};
#endif
