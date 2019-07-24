#include"evawiz.h"
#include"MatrixModule.h"
#include"PythonModule.h"
#include"eva_oldmacro.h"

using namespace std;
using namespace evawiz;

#undef ModuleNameStr
#define ModuleNameStr "Python"
__CREATE_MODULE__(Python)


__DEBUG_MAIN__("input.ev");

PythonModule::PythonModule(__CONSTRUCTION_PARA__){
  matrixModule=(MatrixModule*) EvaKernel->ModulePtr("Matrix");
  PyMain = NULL;
  {
    RegisterFunction("Python",(MemberFunction)(&PythonModule::Evaluate));
    RegisterFunction("PyEvaluate",(MemberFunction)(&PythonModule::Evaluate));
  }
  {
    RegisterFunction("PyImport",(MemberFunction)(&PythonModule::Import));
  }
  {
    RegisterFunction("PyPutValue",(MemberFunction)(&PythonModule::PutValue));
    RegisterFunction("PyGetValue",(MemberFunction)(&PythonModule::GetValue));
  }
  {
    RegisterFunction("PyAddPath",(MemberFunction)(&PythonModule::AddPath));
  }
}

PythonModule::~PythonModule(){
  Finalize();
};

int PythonModule::Initialize(){
  if ( not IsInitialized() ){
    Py_Initialize();
    PyMain = PyImport_AddModule("__main__");
  }
  if ( not IsInitialized() )
    ThrowError("Python","Initialize","Can not Initialize Python Environment.");
  return 0;
}

int PythonModule::Finalize(){
  if ( Py_IsInitialized() )
    Py_Finalize();
  PyMain = NULL;
  return 0;
}

bool PythonModule::IsInitialized(){
  return Py_IsInitialized() and PyMain != NULL;
}

int PythonModule::Evaluate( string expr ){
  Initialize();
  PyRun_SimpleString( expr.c_str() );
  return 0;
}

int PythonModule::Evaluate(Object &Argv){
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  Initialize();
  PyRun_SimpleString(Argv[1].Key());
  ReturnNull;
}

int PythonModule::PutValue(string name,Object&obj){
  Initialize();
  PyObject *v=NULL;
  List2PyObject( obj, v );
  PyObject_SetAttrString(PyMain,name.c_str(),v);
  Py_DECREF(v);
  return 0;
}


int PythonModule::PutValue(Object&Argv){
  CheckShouldEqual(2);
  CheckShouldBeString(1);
  Initialize();
  PyObject *v=NULL;
  List2PyObject( Argv[2], v );
  PyObject_SetAttrString(PyMain,Argv[1].Key(),v);
  Py_DECREF(v);
  ReturnNull;
}

Object PythonModule::GetValue(string name){
  Initialize();
  PyObject *v = PyObject_GetAttrString(PyMain, name.c_str() );
  Object res;
  PyObject2List( v, res );
  Py_DECREF(v);
  return res;
}

int PythonModule::GetValue(Object&Argv){
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  Initialize();
  PyObject *v = PyObject_GetAttrString(PyMain, Argv[1].Key() );
  Object res;
  PyObject2List( v, res );
  Py_DECREF(v);
  Argv = res;
  ReturnNormal;
}


int PythonModule::Import(Object&Argv){
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  Initialize();
  PyImport_ImportModule( Argv[1].Key() );
  ReturnNull;
}

int PythonModule::PyObject2List(PyObject*pyo,Object &list){
  if ( pyo == NULL or pyo == Py_None ){list.SetNull(); return 1;}
  else if ( PyLong_Check(pyo) ){list.SetNumber( PyLong_AsLong(pyo) ); return 1;}
  else if ( PyBool_Check(pyo) ){if ( pyo == Py_True ){list.SetSymbol( SYMBOL_ID_OF_True );}else{list.SetSymbol( SYMBOL_ID_OF_False );} return 1;}
  else if ( PyLong_Check(pyo) ){list.SetNumber( PyLong_AsLong(pyo) ); return 1;}
  else if ( PyFloat_Check(pyo) ){list.SetNumber( PyFloat_AsDouble(pyo) ); return 1;}
  else if ( PyComplex_Check(pyo) ){list.SetComplex( complex(PyComplex_RealAsDouble(pyo), PyComplex_ImagAsDouble(pyo)) ); return 1;}
  else if ( PyByteArray_Check(pyo) ){list.SetString( PyByteArray_AsString(pyo) ); return 1;}
  else if ( PyUnicode_Check(pyo) ){
    Py_ssize_t size;
    list.SetString( PyUnicode_AsUTF8AndSize(pyo,&size) );
    return 1;
  }else if ( PyTuple_Check(pyo) ){
    u_int size = PyTuple_Size( pyo );
    list.SetList_List();
    for ( u_int i=0;i<size;i++){
      list.PushBackNull();
      PyObject2List( PyTuple_GetItem( pyo, i), list[i+1] );
    }
    return 1;
  }else if( PyList_Check(pyo) ){
    u_int size = PyList_Size( pyo );
    list.SetList_List();
    for ( u_int i=0;i<size;i++){
      list.PushBackNull();
      PyObject2List( PyList_GetItem( pyo, i), list[i+1] );
    }
    return 1;
  }else if( PyDict_Check(pyo) ){
    u_int size = PyDict_Size( pyo );
    PyObject * items = PyDict_Items( pyo );
    list.SetList(SYMBOL_ID_OF_Dict);
    for ( u_int i=0;i<size;i++){
      PyObject*item = PyList_GetItem( items, i );
      list.PushBackNull();
      Object key,value;
      PyObject2List( PyList_GetItem( item,0),key );
      PyObject2List( PyList_GetItem( item,1),value );
      list.Back().SetPairByRef( key, value );
    }
    Py_DECREF( items );
    return 1;
  }

  list.SetSymbol("$UNKNOWN$Type$PyObject$");
  return 0;
}

int PythonModule::List2PyObject(Object &list,PyObject*&pyo){
  if ( list.NullQ() ){
    pyo = Py_None;
    Py_INCREF( pyo );
    return 1;
  }
  switch ( list.type() ){
  case EvaType::Number:{
    pyo = PyFloat_FromDouble( list.Number() );
    return 1;
  }
  case EvaType::String:{
    pyo = PyUnicode_FromString( list.Key() );
    return 1;
  }
  case EvaType::Symbol:{
    ThrowError("List2PyObject","Un-determined Symbol is not allowed to transfer to Python as a value.");
  }
  case EvaType::List:{
    if ( list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      pyo = PyComplex_FromDoubles( list[1].Number(), list[2].Number() );
      return 1;
    }
    if ( list[0].SymbolQ( SYMBOL_ID_OF_List ) ){
      pyo = PyList_New(0);
      for ( u_int i = 1;i<=list.Size(); i++ ){
        PyObject *item;
        List2PyObject( list[i], item );
        PyList_Append( pyo, item );
        Py_DECREF( item );
      }
      return 1;
    }
    if ( list[0].SymbolQ( SYMBOL_ID_OF_Dict ) ){
      pyo = PyDict_New();
      for( u_int i=1; i<=list.Size(); i++ ){
        PyObject *key,*value;
        if ( not list[i].ListQ() or list[i].Size() < 2 ) continue;
        List2PyObject( list[i][1], key );
        List2PyObject( list[i][2], value );
        PyDict_SetItem( pyo, key, value );
      }
      return 1;
    }
  }
  }
  return 0;
}

bool PythonModule::AddPath( string path ){
  string cmd = "import sys\nsys.path.append(\""+path+"\")";
  Evaluate( cmd );
  return 0;
}

int PythonModule::AddPath(Object&Argv){
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  AddPath( Argv[1].Key() );
  ReturnNull;
}

