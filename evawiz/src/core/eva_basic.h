#ifndef BASIC_EVAWIZ_H
#define BASIC_EVAWIZ_H

#include<fstream>
#include<iostream>
#include<map>
using namespace std;

namespace evawiz{

////////////////////////////////////////////
  typedef       unsigned int   u_int;
  typedef       unsigned long  u_long;
  typedef       unsigned char  u_char;
  class         EvaSettings;
////////////////////////////////////////////
  struct        complex;
  struct        floatcomplex;
  struct        Index;
  enum struct   EvaType:unsigned char;
  class         Object;
////////////////////////////////////////////
  class         Evaluation;
  class         Module;
  typedef       int (Module::*MemberFunction)(Object &);
  typedef       int (*StaticFunction)(Object &);
////////////////////////////////////////////
  template<class type> class Matrix_T;
  typedef       Matrix_T<double> Matrix;
  typedef       Matrix_T<complex> ComplexMatrix;
  typedef       Matrix_T<float> FloatMatrix;
  typedef       Matrix_T<floatcomplex> FloatComplexMatrix;
  typedef       Matrix_T<int> IntMatrix;
////////////////////////////////////////////
  class         MatrixModule;
  class         TensorModule;
  class         MatlabEngineModule;
  class         PythonModule;
  class         AlgebraicModule;
  class 				ClassModule;
  class         MPIModule;
////////////////////////////////////////////
  void swap(short unsigned int&t1,short unsigned int&t2);
  void swap(int&t1,int&t2);
  void swap(u_int&t1,u_int&t2);
  void swap(char*&t1,char*&t2);
  void swap(double&t1,double&t2);
  void swap(float&t1,float&t2);
  void swap(complex&t1,complex&t2);
  void swap(long&t1,long&t2);
  void swap(u_long&t1,u_long&t2);
////////////////////////////////////////////
  template<class type> type abs(type x){ return x>=0?x:-x; }
////////////////////////////////////////////
  void sleep_micro(int sec);
  void sleep_sec(int sec);
////////////////////////////////////////////
  enum struct AttributeType: unsigned char{ Null,
      Flat,
      Listable,
      OneIdentity,
      HoldFirst,
      HoldRest,
      HoldAll,
      HoldAllCompleted,
      SequenceHold,
      Orderless,
      GrammarUnit,  
      Constant,
      DelayFunction,
      Conjunctable,
      Setable,
      DelaySetable,
      Destroyable,
      Protected,
      Locked,
      MAXENUM,
      };
////////////////////////////////////////////
  unsigned int   SimpleHashCodeOfString(const char *);
  const char    *Type2String(EvaType);
  const char    *AttributeType2String(AttributeType);
  AttributeType  String2AttributeType(const char*);
  bool           AttributeQ(bool*,AttributeType);
  bool           AttributeQ(bool*,const char*);
  void           SetAttribute(bool*,AttributeType,bool val=true);
  void           SetAttribute(bool*,const char*,bool val=true);
  bool           SpecialCharecterQ(char);
////////////////////////////////////////////
////////////////////////////////////////////
  void           Warning(string mod,string func,string mes);
  void           Warning(string func,string mes);
  void           Warning(string mes);
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
////////////////////////////////////////////
  string    ToString(int num);
  string    ToString(u_int num);
  string    ToString(u_long num);
  string    ToString(Index num);
  string    ToString(double num);
  string    ToString(complex num);
  string    ToString(float num);
  string    ToString(floatcomplex num);
  string    ToString(void *num);
  double    ToNumber(const char*);
  complex   ToComplex(const char*);
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////compare
  int       compare(double num1,double num2);
  int       compare(complex num1,complex num2);
  int       compare(string ,string );
  int       compare(const char*,const char*);
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
///////////////////////////////////////////////
////////////////////// Base on operation system
  bool     FileExistQ(string filename);
  bool     CheckFileType(string dirname,string type);//DIR  REG  LINK  FIFO
  bool     CommandExist(string cmd);
  int      SetFileContent(string filename,string content);
  string   GetFileContent(string filename);
  string   IthWord(string str,int n);
  int 	   WordCount(string str);
  int 	   StringSplit(string str,vector<string>&strs,string split);
  int 	   StringSplit(string str,vector<string>&strs);
  string   RandomString(int len);
  string   RandomNumberString(int len);
  string   RemoveFirstWord(string str,string &leftword);
  string   GetEnv(string name);
  int      SetEnv(string var,string expr,bool overwrite=false);
  string   GetDirectory();
  string   GetBaseName(string path);
  string   GetDirName(string path);
  string   TestCommand(string cmd);
  inline
  string   GetCwd(){return evawiz::GetDirectory(); };
  bool     SetDirectory(string dirname);
  string   GetParentDirectory();
  bool     SetToParentDirectory();
  string   System(string cmd);
  int      System(string cmd,vector<string>&result);
  int      Execute(string cmd);
  int      Showing(string str);
  string   GetFileMD5(string filename);
  constexpr unsigned int RSimpleHashCodeOfString(const char *s,unsigned int code = 5381, int off = 0){return (((s[off]=='\0') ? code : RSimpleHashCodeOfString(s,(code*33)^s[off],off+1)));}
  //#define KCase(key) case RSimpleHashCodeOfString(#key)
  long     GetMemInUse();
  pair<double,double> GetProcessMemUsage();//1.vm_usage,2.resident_set
  bool     GPUDeviceQ();

  ////////////////
  inline double Rand(){ return (double)rand()/RAND_MAX; }
  double GaussRand(double mean = 0.0, double variance = 1.0);
};

#endif
