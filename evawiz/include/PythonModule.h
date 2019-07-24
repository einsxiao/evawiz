#ifndef SuanZi_PythonModule_H
#define SuanZi_PythonModule_H

#include"Python.h"

namespace evawiz{
  class PythonModule: public Module{
  private:
    MatrixModule*matrixModule;
  public:
    PythonModule(string,string);
    ~PythonModule();
    ////////////////////////////////////////
    ////////////////////////////////////////
    int             Initialize();
    int             Finalize();
    bool            IsInitialized();
    ////////////////////////////////////////
    PyObject       *PyMain;
    ///////////////////////////////////////
    int             PyObject2List(PyObject*,Object&);
    int             List2PyObject(Object&,PyObject*&);
    ///////////////////////////////////////
    int             Evaluate(string);
    int             Evaluate(Object&);
    ///////////////////////////////////////
    int             PutValue(Object&);
    int             PutValue(string name, Object&);
    int             GetValue(Object&);
    Object          GetValue(string name);
    ///////////////////////////////////////
    int 						Import(Object&);
    bool            AddPath(string path);
    int 						AddPath(Object&);
  };
  
};
#endif
