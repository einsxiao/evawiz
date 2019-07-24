#ifndef SuanZi_MatlabEngineModule_H
#define SuanZi_MatlabEngineModule_H

#include"engine.h"

namespace evawiz{
  class MatlabEngineModule: public Module{
  private:
    Engine *ep;
    int state;
    int List2mxCellArray(Object &Argv,mxArray*cellArray,int pdim,int parr);
    vector<string> paths;
  public:
    MatlabEngineModule(string,string);
    ~MatlabEngineModule();
    ////////////////////////////////
    bool On();
    bool Open();  //success(true)  failure(false)
    bool Close(); //success(true)  failure(false)
    bool AddPath(string path); // add matlab path
    int  MatAddPath(Object &);
    ////////////////////////////////
    ///// matrix <--> mxArray
    //mxArray->Matrix
    int mxArray2Matrix(mxArray*mxA,Matrix&matrix);
    //matrix->Mxarray
    mxArray*Matrix2mxArray(Matrix&matrix);
    ///// Argv <--> mxArray ( evaluation is needed )
    //mxArray->Object 
    static int mxArray2List(mxArray *mxA,Object &Argv);
    //Object ->mxArray
    mxArray*List2mxArray(Object &Argv);
    ////////////////////////////////
    Object  GetResult();
    int     GetResult(Object &ans);
    Object  GetValue(string var_name);
    int     GetValue(string var_name,Object &var);
    int     PutValue(string var_name,Object &var);
    int     GetMatrix(string mx_name,Matrix &matrix);
    int     PutMatrix(Matrix &matrix, string mx_name);
    int     Evaluate(string str_in);
    int     Function(string str_in,string &str_out);
    //////////////////////////////// evawiz functions
    int     MatStart(Object &);
    int     MatClose(Object &);
    int     MatEvaluate(Object &);
    int     MatEval(Object &);
    int     MatGetResult(Object &);
    int     MatCall(Object &);
    ////////////////////////////////
    int     MatPutValue(Object &);
    int     MatGetValue(Object &);
    int     MatPutMatrix(Object &);
    int     MatGetMatrix(Object &);
  };
  
};
#endif
