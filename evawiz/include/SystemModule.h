#ifndef Gisp_SystemModule_H
#define Gisp_SystemModule_H

namespace evawiz{
  class SystemModule:public Module{
  public:
    ///////////////////////////////
    SystemModule(string,string);
    ~SystemModule();
    ///////////////////////////////
    //Evaluate
    static int Evaluate(Object &);
    //Options && Attributes
    static int Options(Object &);
    static int SetOptions(Object &);
    static int CurrentValue(Object &);
    ///////////////////////////////
    static int Attributes(Object &);
    static int SetAttribute(Object&left,Object&attris);
    static int SetAttribute_Eva(Object &);
    static int AddAttribute_Eva(Object &);
    static int AddLocalAttribute_Eva(Object &);
    static int RemoveAttributes_Eva(Object &);
    static int RemoveLocalAttributes_Eva(Object &);
    static int Protect_Eva(Object &);
    static int UnProtect_Eva(Object &);
    ///////////////////////////////
    static int Parenthesis(Object &);
    static int CompoundExpression(Object &);
    static int Print(Object &);
    ///////////////////////////////
    static int ToString_Eva(Object &);
    static int ToExpression(Object &);
    ///////////////////////////////
    static int Hold(Object &);
    static int HoldOnce(Object &);
    static int HoldPattern(Object &);
    static int UnifyRule(Object &);
    static int UnifiedMatchQ(Object &);
    static int MatchQ(Object &);
    static int PatternCompare(Object &);
    static int Cases(Object &);
    static int Position(Object &);
    static int Count(Object &);
    static int Replace(Object &);
    static int ReplaceAll(Object &);
    static int ReplaceRepeated(Object &);
    static int DeleteCases(Object &);
    ////////////////////////////////////////
    ///////////////////////////////
    static int GrammarModule(Object &);
    static int Function(Object &);
    static int PureFunction(Object &);
    ////////////////////////////////////////
    static int Conjunct(Object &);
    ///////////////////////////////
    static int If(Object &);
    static int While(Object &);
    static int For(Object &);
    static int Do(Object &);
    ///////////////////////////////
    static int Foreach(Object &); //Foreach[i,{...},expr]
    ///////////////////////////////
    static int System(Object &);
    static int SystemWithoutOutput(Object &);
    ///////////////////////////////
    static int Set(Object  &Argv);
    static int SetDelayed(Object  &Argv);
    ///////////////////////////////
    static int GetModule(Object  &);
    static int RemoveModule(Object  &);
    static int ShowModuleFunctions(Object &);
    static int ShowModules(Object &);
    static int In(Object  &ArgvEva);
    static int Out(Object  &ArgvEva);
    static int SetOutputFormat(Object  &ArgvEva);
    static int Input(Object&ArgvEva);
    static int SystemArgv(Object&ArgvEva);
    ///////////////////////////////
    static int SetRunningMode(Object&ArgvEva);
    static int GetRunningMode(Object&ArgvEva);
    static int GPUDeviceQ(Object&ArgvEva);
    static int SetCudaThreadsNumberPerBlock(Object&ArgvEva);
    static int GetCudaThreadsNumberPerBlock(Object&ArgvEva);
    static int SetEvawizCpuKernelThreadsNumber(Object&ArgvEva);
    static int GetEvawizCpuKernelThreadsNumber(Object&ArgvEva);
    ///////////////////////////////
    static int ShowHelpInfo(Object &ArgvEva);
    static int Clear(Object &ArgvEva);
    ///////////////////////////////
    static int AbsoluteTime(Object &ArgvEva);
    static int DateList(Object &ArgvEva);
    static int Timing(Object &ArgvEva);
    static int CpuTiming(Object &ArgvEva);
    static int Sleep(Object&);
  };
};
#endif
