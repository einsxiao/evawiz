#ifndef Evaluation_Evawiz_H
#define Evaluation_Evawiz_H

namespace evawiz{
  class EvaRecord{
  public:
    bool            attributes[24];
    StaticFunction  staticFunction =NULL ;
    Module*         module =NULL ;
    MemberFunction  memberFunction =NULL ;
    string          description ;
    EvaRecord();   ~EvaRecord();
    EvaRecord(const EvaRecord&);
    // update do get something from old record
    //   attributes sum up
    //   others set to newone, old clear up
    //   description set to newone
    void   Update(const EvaRecord&); 
    void   ClearValues(); //clear values from staticFunction from the last, attributes will hold
    void   ClearAll(); //clear values from staticFunction from the last, attributes will hold
  };
  class Evaluation{
  public:
    friend class ValueTable;
    friend class Kernel;
    friend class SystemModule;
    friend class MeioModule;
    friend class ClassModule;
  private:
    Object                       maxRecursionDepth;
    int                          EvaluateRecursionDepth;
    bool                         DebugMode;
    bool                         InteractivelyMode;
    //////////////////////////////////////////////////////////////
    ///////  moduleTables
    map<string, Module*>				 moduleTable;
    map<string, type_create*>    moduleCreateHandleTable;
    map<string, type_destroy*>   moduleDestroyHandleTable;
    map<string, void*>           libHandlerTable;
    //string Context;
    //////////////////////////////////////////////////////////////
    ///////  id to others
    vector<EvaRecord*>           evaRecordTable;
    vector<int>                  freeEvaRecord;
    //////////////////////////////////////////////////////////////
    /////// valueTables 
    vector<pair<ValueTable*,bool> >valueTables; //second true: not need to clear, false: need to
    ValueTable                  *currentValueTable;
    //////////////////////////////////////////////////////////////
    /////// specific affairs
    Object                       superList,inputList;
  public:
    int                          argc;
    char                       **argv;
    // MatrixModule                *matrixModule;
    // AlgebraicModule             *algebraicModule;
    // MPIModule                   *mpiModule;
    //////////////////////////////////////////////////////////////
  public:
    Evaluation(bool debug = false);  ~Evaluation();
    //////////////////////////////////////////////////////////////
    ///// Evaluate something
    int             AttributeProcessing(Object&obj,bool *attri,bool isHold=false);
    int             PreEvaluate(Object&obj);
    void            EvaluateHead(Object&obj,bool isHold=false); 
    void            EvaluateFirst(Object&obj,bool isHold=false); 
    void            EvaluateRest(Object&obj,bool isHold=false); 
    void            EvaluateAll(Object&obj,bool isHold=false); 
    int             ListableEvaluate(Object &,bool isHold=false);
    int             Evaluate(Object&obj,bool isHold=false,bool isRef=false); //PostEvaluate
    /////////////////////////////////////////////////////////////
    Object          Evaluate(string expr); // no return value
    int             EvaluateFile(string filename,bool willBroken = false); //no return value
    int  	          SetMaxRecursionDepth(int n); 
    //////////////////////////////////////////////////////////////
    ///// Context
    int             GetCurrentContext();
    int             NewInContext(const char*instance);
    int             InContext(const char*instance);
    int             OutContext();
    int             ClearContext(const char*instance);
    //////////////////////////////////////////////////////////////
    ///// Modules
    int             GetModuleDependence(string moduleName);
    int             GetModuleLib(string moduleName);
    int             GetModuleEvaScript(string moduleName);
    int             GetModulePathReady(string moduleName);
    int             GetModuleMatPathReady(string moduleName);
    int             GetModulePyPathReady(string moduleName);
    int             GetModule(string moduleName);//c form module
    int  	          CreateModule(string moduleName);
    //////////////////////////////////////////////////////////////
    int             RemoveModule(string moduleName);
    int             InsertModule(string moduleName,Module* module);
    Module*         ModulePtr(string moduleName);//if none return NULL
    //////////////////////////////////////////////////////////////
    ///// EvaRecord Info operations
    EvaRecord      *GetEvaRecord(Object&obj);
    EvaRecord      *GetEvaRecord(const char*);
    EvaRecord      *GetOrNewEvaRecord(Object&obj);
    bool            SetEvaRecord(Object&obj,EvaRecord&rec);
    bool            ClearEvaRecord(int i);
    int             Clear(Object&obj,bool repeated = false);
    //////////////////////////////////////////////////////////////
    int             Call(const char*funcName,Object&Argv);
    int             Call(const EvaRecord *funcEvaRecord,Object&Argv);
    //////////////////////////////////////////////////////////////
    ///// ValueTable operations
    // 0: get nothing,  other: get a new value
    Object          GetOrNewValuePairRefAtTop(Object&obj,bool onlyGet=false,bool atTop=true,bool isCheckedNoPattern=false);//search and get
    Object          GetOrNewValuePairRefAtTop(const char*sym_str);//search and get
    bool            UpdateOrInsertValueAtTop(Object&left,Object&right,bool onlyUpdate=false,bool AtTop=true,bool isCheckedPattern=false);
    //////////////////////////////////////////////////////////////
    Object          GetValuePairRef(Object  &obj,bool isCheckedNoPattern=false );//search and get value pair
    Object          GetOrNewValuePairRef(Object &obj,bool isCheckedNoPattern=false);//search get or new value pair
    //////////////////////////////////////////////////////////////////////////////////////
    bool            InsertValue(Object&left,Object&right);// Short for previous function
    bool            InstantInsertOrUpdateValue(Object&left,Object&right);
    //////////////////////////////////////////////////////////////////////////////////////
    int             GetValueRef(Object&obj,Object&ref); // -1 pro&f 0 not f 1: unPro&f
    //////////////////////////////////////////////////////////////////////////////////////
    int             GetValue(Object&obj,Object&value,bool onlyCheckPatternTable=false);
    int             GetValue(Object&obj);
    template<class type>
    bool            GetValue(string key,type&num){
      Object  list; list.SetSymbol( key.c_str() );
      Object  value = GetValuePairRef( list );
      if ( value.NullQ() or value[1] == value[2] ){return false;}
      if ( !value[2].NumberQ() ){return false;}
      num = value[2].Number();
      return true;
    }
    bool            GetValue(string,string&);
    bool            GetValue(string,double&);
    bool            GetValue(string,float&);
    bool            GetValue(string,complex&);
    bool            GetValue(string,int&);
    bool            GetValue(string,bool&);
    ///////////
    //////////////////////////////////////////////////////////////
    char            Boolean(Object&obj);
    //////////////////////////////////////////////////////////////
    ///// new ValueTable 
    int             newGrammarModule(); 
    int             newGrammarModule(ValueTable*); 
    int             deleteGrammarModule();
    int             ClearValueTablesTo(u_int);
    //////////////////////////////////////////////////////////////
    ///// Attributes operations Attributes['Set']={'HoldFirst'}
    bool           *SetAttribute(Object&obj,AttributeType attriType,bool val=true);
    bool           *SetAttribute(const char*key,AttributeType attriType,bool val=true);
    bool           *SetAttribute(const unsigned int id,AttributeType attriType,bool val=true);
    bool           *GetAttributes(Object&obj); 
    bool           *GetAttributes(const char *); 
    bool           *GetAttributes(const unsigned int id); 
    bool            AttributeQ(Object&obj,AttributeType intype);
    bool            AttributeQ(const char*inkey,AttributeType intype);
    bool            AttributeQ(const  unsigned int id,AttributeType intype);
    ////////////////////////////////////////
    ////// Imbeded Matrix
    Matrix          *GetMatrix(string matrixName);
    ComplexMatrix   *GetComplexMatrix(string matrixName);
    Matrix          *GetOrNewMatrix(string matrixName);
    ComplexMatrix   *GetOrNewComplexMatrix(string matrixName);
    int              RemoveMatrix(string matrixName);
    int              RemoveComplexMatrix(string matrixName);
  };
#define INIT_VALUETABLEPAIR_ID_OF(sym) Index VALUETABLEPAIR_ID_OF_##sym = EvaKernel->GetOrNewValuePairRefAtTop(#sym).objid

};

#endif
