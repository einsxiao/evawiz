#include<eva_headers.h>
#include<signal.h>

using namespace std;
using namespace evawiz;

EvaRecord::EvaRecord(){
  memset(attributes,0,sizeof(bool)*24);
};

EvaRecord::~EvaRecord(){
};

EvaRecord::EvaRecord(const EvaRecord&er){
  for ( int i=0;i< 24;i++)
    attributes[i] = er.attributes[i];
  staticFunction = er.staticFunction;
  module = er.module;
  memberFunction = er.memberFunction;
  description = er.description;
}

void EvaRecord::Update(const EvaRecord&er){
  for ( int i=0;i< 24;i++)
    attributes[i] = attributes[i] or er.attributes[i];
  int seted = false;
#define deal(x) if ( seted ){ x = er.x; if (x != NULL) seted = true; }else{ x= NULL; }
  deal(staticFunction);
  module = er.module;
  memberFunction = er.memberFunction;
  deal(memberFunction);
#undef deal
  description = er.description;
}

void EvaRecord::ClearValues(){
  staticFunction = NULL;
  module = NULL;
  memberFunction = NULL;
  description = "";
}

void EvaRecord::ClearAll(){
  memset(attributes,0,sizeof(bool)*24);
  staticFunction = NULL;
  module = NULL;
  memberFunction = NULL;
  description = "";
}

void signal_action_ctrl_c( int sig )
{
  ThrowError("Evawiz","Termination signal ^C is captured.");
}


Evaluation::Evaluation(bool debug){
  // EvaKernel
  EvaKernel = this;

  signal(SIGINT,signal_action_ctrl_c);
  //check env var, if not set set to default ones
  string str;
#define check(ev,dp) ({str = evawiz::GetEnv(ev); if(str=="")ThrowError("System","Evawiz runing environment is not set properly. Please source /opt/evawiz/evawiz.source and run the program again.");})
  check("EVAWIZ_ROOT","/opt/evawiz");
  check("EVAWIZ","/opt/evawiz/evawiz");
  check("HOME_EVAWIZ","$HOME/evawiz");

  /////////////
  DebugMode = debug;
  valueTables.push_back( pair<ValueTable*,bool>(new ValueTable(),false) );
  currentValueTable = (*valueTables.begin()).first;
  evaRecordTable.push_back(NULL);
  evaRecordTable.push_back(NULL); //Get Value will return a value ( > 2) when is a eva record
  superList.SetList_List();
  inputList.SetList_List();
  ///////////////////////////////
  if ( evawiz::GPUDeviceQ() ){
    EvaSettings::SetRunningMode( RunningModeGpu );
  }else{
    EvaSettings::SetRunningMode( RunningModeCpu );
  }
  ///////////////////////////////
  EvaluateRecursionDepth=-1;
  Object temp; temp.SetSymbol("MaxRecursionDepth");
  maxRecursionDepth = this->GetOrNewValuePairRef( temp )[2];
  maxRecursionDepth.SetNumber( MaxRecursionDepth );
  ///////////////////////////////
  GetModule("System");
  GetModule("List");
  GetModule("Algebraic");
  GetModule("Arithmatics");
  GetModule("Matrix");

  GetModule("Class");
  GetModule("MPI");
  GetModule("OS");
  GetModule("String");
  GetModule("Python"); 
  ///////////////////////////
  string matlab_root = GetEnv("MATLAB_ROOT");
  if (  matlab_root != "" ){ 
    GetModule("MatlabEngine");
  }
  string mathematica_root = GetEnv("MATHEMATICA_ROOT");
  if (  mathematica_root != "" ){ 
    GetModule("Mathematica");
  }

}

Evaluation::~Evaluation(){
  // deconstruct the many Modules or other malloced memory.
  while ( moduleTable.begin()!=moduleTable.end() ){
    // module itself will erase the module and delete the module with its type
    if ( moduleTable.begin()->second!=NULL )
      delete moduleTable.begin()->second;
    moduleTable.erase( moduleTable.begin() );
  }
  while ( evaRecordTable.begin()!=evaRecordTable.end() ){
    if ( *evaRecordTable.begin()  != NULL )
      delete *evaRecordTable.begin();
    evaRecordTable.erase( evaRecordTable.begin() );
  }
  while ( valueTables.begin()!=valueTables.end() ){
    if ( ((*valueTables.begin()).first !=NULL ) && ((*valueTables.begin()).second == false ) )
      delete (*valueTables.begin()).first ;
    valueTables.erase( valueTables.begin() );
  }
}

int Evaluation::InsertModule(string moduleName,Module*module){
  pair< map<string,Module*>::iterator, bool > ret;
  ret = moduleTable.insert( pair<string,Module*>(moduleName,module) );
  if ( !ret.second )
    return false;
  return true;
}

Module *Evaluation::ModulePtr(string moduleName){
  map<string,Module*>::iterator iter;
  iter = moduleTable.find( moduleName );
  if ( iter == moduleTable.end() ){
    GetModuleLib(moduleName);
    iter = moduleTable.find(moduleName);
    if ( iter == moduleTable.end() )
      return NULL;
  }
  return iter->second;
}

int Evaluation::GetModuleDependence(string moduleName){
  string module_dir;
  string depend_file;
  string content;
  vector<string> words;
  module_dir =  GetEnv("HOME_EVAWIZ")+"/"+moduleName;
  if ( CheckFileType(module_dir,"DIR") ){
    depend_file =  module_dir + "/.evadepend";
    if ( CheckFileType( depend_file,"REG" ) ){
      content = evawiz::GetFileContent( depend_file );
      evawiz::StringSplit( content, words);
      for ( u_int i=0; i< words.size(); i++ ){
        GetModule( words[i] );
      }
    }
  }
  module_dir =  GetEnv("EVAWIZ")+"/modules/"+moduleName;
  if ( CheckFileType(module_dir,"DIR") ){
    depend_file =  module_dir + "/.evadepend";
    if ( CheckFileType( depend_file,"REG" ) ){
      content = evawiz::GetFileContent( depend_file );
      evawiz::StringSplit( content, words);
      for ( u_int i=0; i< words.size(); i++ ){
        GetModule( words[i] );
      }
    }
  }
  return 0;
}

int Evaluation::GetModulePathReady(string moduleName){
  /*
  //PATH LD_LIBRARY_PATH
  //#define __ADD_PATH(dir) {if(FileExistQ(dir)){evawiz::SetEnv("PATH=$PATH:"+dir);}}
  //#define __ADD_LIB(dir) {if(FileExistQ(dir)){evawiz::SetEnv("LD_LIBRARY_PATH",dir+":"+evawiz::GetEnv("LD_LIBRARY_PATH"),true);}}
  string evawiz_dir = GetEnv("EVAWIZ");
  string home_evawiz_dir = GetEnv("HOME_EVAWIZ");
  if ( not CheckFileType(home_evawiz_dir,"DIR" ) )
    ThrowError("System","Local settings not ready. Please refer to Evawiz setup tips.");
  __ADD_LIB( GetEnv("EVAWIZ")+"/modules/"+moduleName+"/lib" );
  __ADD_LIB( GetEnv("EVAWIZ")+"/modules/"+moduleName+"/lib" );
  __ADD_LIB( home_evawiz_dir+"/"+moduleName+"/lib" );
  __ADD_LIB( home_evawiz_dir+"/"+moduleName+"/lib" );
#undef __ADD_PATH
  //cout<<"After add LD_LIBRARY_PATH is "<<GetEnv("LD_LIBRARY_PATH")<<endl;
  */
  return 0;
}

int Evaluation::GetModuleEvaScript(string moduleName){
  string sysdir,filename;
#define __GET_PATH(dir) {                                               \
  filename=dir+"/"+moduleName+".eva";                                   \
  if ( FileExistQ(filename) )                                           \
    return EvaluateFile(filename);                                      \
  }
  __GET_PATH( GetCwd() );
  __GET_PATH( GetEnv("HOME_EVAWIZ")+"/"+moduleName );
  __GET_PATH( GetEnv("EVAWIZ")+"/modules/"+moduleName );
  __GET_PATH( GetEnv("EVAWIZ")+"/lib/"+moduleName );
#undef __GET_PATH
  return 0;
}

int Evaluation::GetModuleMatPathReady(string moduleName){
  //Matlab path
#define __ADD_PATH(dir) {                         \
    if( FileExistQ(dir) ) {                       \
      Object  expr; expr.SetList( "MatAddPath" ); \
      expr.PushBackString( (dir).c_str() );       \
      Evaluate( expr );                           \
    }                                             \
  }
  __ADD_PATH( GetEnv("EVAWIZ")+"/modules/"+moduleName );
  __ADD_PATH( GetEnv("HOME_EVAWIZ")+"/"+moduleName );
#undef __ADD_PATH
  return 0;
}

int Evaluation::GetModulePyPathReady(string moduleName){
  //Matlab path
#define __ADD_PATH(dir) {                         \
    if( FileExistQ(dir) ) {                       \
      Object  expr; expr.SetList( "PyAddPath" ); \
      expr.PushBackString( (dir).c_str() );       \
      Evaluate( expr );                           \
    }                                             \
  }
  __ADD_PATH( GetEnv("EVAWIZ")+"/modules/"+moduleName );
  __ADD_PATH( GetEnv("HOME_EVAWIZ")+"/"+moduleName );
#undef __ADD_PATH
  return 0;
}

int Evaluation::CreateModule(string moduleName){
  return 0;
}

int Evaluation::GetModuleLib(string moduleName){
  //checking moduleName spell
  if ( moduleTable.find( moduleName ) != moduleTable.end() ){ // already initialized
    Warning("GetModule","Module "+moduleName+" has already been got.");
    return -1;
  }
  ////// load the share library of the module
  map<string,void*>::iterator iter = libHandlerTable.find(moduleName);
  bool module_find = false;
  void *lib_Handler = NULL;
  string module_dir = "";
  if ( iter == libHandlerTable.end() ){
    // load the shared lib into the kernel from different directory
    // self own modules
    string current_dir = GetDirectory();
    string evawiz_dir = GetEnv("EVAWIZ");
    string home_evawiz_dir = GetEnv("HOME_EVAWIZ");
    if ( not CheckFileType(home_evawiz_dir,"DIR" ) ) {
      if ( FileExistQ(home_evawiz_dir) ) System("\\rm -rf "+home_evawiz_dir);
      System("\\mkdir "+home_evawiz_dir);
    }
    string temp_dir = home_evawiz_dir + "/.temp";
    if ( not CheckFileType(temp_dir,"DIR" ) ) {
      if ( FileExistQ(temp_dir) ) System("\\rm -rf "+temp_dir);
      System("\\mkdir "+temp_dir);
    }
    if ( home_evawiz_dir == "" || evawiz_dir == "" )
      ThrowError("Evawiz Environment variables are not properly set");
    string libname="lib"+moduleName+"Module.so",devlibname="lib"+moduleName+"Module-dev.so",so_file,temp_so_file;
    string culibname="lib"+moduleName+"Module_cuda.so",cudevlibname="lib"+moduleName+"Module_cuda-dev.so";
    bool nvcc_status = evawiz::CommandExist("nvcc");

#define TRY_LOAD_SO_IN(_module_dir,libname)                              \
    if ( not lib_Handler and CheckFileType(_module_dir,"DIR") ){        \
      module_find = true;                                               \
      so_file =  _module_dir+"/"+libname;                               \
      temp_so_file = temp_dir+"/"+libname;                              \
      if ( FileExistQ(so_file) ){                                       \
        if ( !FileExistQ(temp_so_file) ||                               \
             GetFileMD5(so_file) != GetFileMD5(temp_so_file) )          \
          System("rmcp "+so_file+" "+temp_dir );                        \
        lib_Handler=dlopen(temp_so_file.c_str(),RTLD_NOW|RTLD_GLOBAL ); \
        if (!lib_Handler) {                                             \
          ThrowError("GetModule","Fail to get binary '"                 \
                     + moduleName+"' in.\n \t"+dlerror());              \
        }                                                               \
        module_dir = _module_dir;                                       \
        break;                                                          \
      }                                                                 \
    }
    for(;;){
      if ( DebugMode ){
        if ( nvcc_status ){
          TRY_LOAD_SO_IN(current_dir+"/",cudevlibname);
          TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,cudevlibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,cudevlibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/lib",cudevlibname);

          TRY_LOAD_SO_IN(current_dir+"/",culibname);
          TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,culibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,culibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/lib",culibname);
        }
        TRY_LOAD_SO_IN(current_dir+"/",devlibname);
        TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,devlibname);
        TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,devlibname);
        TRY_LOAD_SO_IN(evawiz_dir+"/lib",devlibname);

        TRY_LOAD_SO_IN(current_dir+"/",libname);
        TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,libname);
        TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,libname);
        TRY_LOAD_SO_IN(evawiz_dir+"/lib",libname);
      }else{
        if ( nvcc_status ){
          TRY_LOAD_SO_IN(current_dir+"/",culibname);
          TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,culibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,culibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/lib",culibname);

          TRY_LOAD_SO_IN(current_dir+"/",cudevlibname);
          TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,cudevlibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,cudevlibname);
          TRY_LOAD_SO_IN(evawiz_dir+"/lib",cudevlibname);
        }
        TRY_LOAD_SO_IN(current_dir+"/",libname);
        TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,libname);
        TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,libname);
        TRY_LOAD_SO_IN(evawiz_dir+"/lib",libname);

        
        TRY_LOAD_SO_IN(current_dir+"/",devlibname);
        TRY_LOAD_SO_IN(home_evawiz_dir+"/"+moduleName,devlibname);
        TRY_LOAD_SO_IN(evawiz_dir+"/modules/"+moduleName,devlibname);
        TRY_LOAD_SO_IN(evawiz_dir+"/lib",devlibname);
      }
      break;
    }
#undef TRY_LOAD_SO_IN
    // module not find
    if (not lib_Handler) {
      if ( not module_find ){
        Warning("GetModule","'"+moduleName+"' located in '"+module_dir+"' may not not load correctly.");
      }
      return -1;
    }
    libHandlerTable[moduleName] = lib_Handler; // store handle for dlclose
  }else{
    lib_Handler = iter->second;
  }
  /////////////////////////////////////////////////////////////////////
  ///// new create and destroy handle
  /////////////////////////////////////////////////////////////////////
  type_create* __create = (type_create*)dlsym(lib_Handler,("__create_"+moduleName+"Module").c_str() );
  const char* dlsym_error=dlerror();
  if ( __create == NULL || dlsym_error ){
    ThrowError("GetModule", "Macro to init module '"+moduleName+"' not found. Please make sure you are following the standard template.");
  }
  type_destroy*__destroy = (type_destroy*)dlsym(lib_Handler,("__destroy_"+moduleName+"Module").c_str() );
  dlsym_error=dlerror();
  if ( __destroy == NULL || dlsym_error ){ThrowError("GetModule", "Macro to delete '"+moduleName+"' not found. Please make sure you are following the standard tempalte.");}
  moduleCreateHandleTable[moduleName]=__create;
  moduleDestroyHandleTable[moduleName]=__destroy;//for destroy
  /////////////////////////////////////////////////////////////////////
  ///// Create module
  // registering to eva will happen once the new module being constructed.
  Module * new_module = __create(moduleName,module_dir);
  if ( new_module == NULL ){ ThrowError("GetModule"," Error while new module."); }
  // register functions created by DefineFunction(func)
  type_return_functions *__return_functions = (type_return_functions*)dlsym(lib_Handler,("__return_functions_"+moduleName).c_str() );
  dlsym_error=dlerror();
  // if ( dlsym_error )
  //   Warning("dlsym",((string)"error :"+dlsym_error).c_str() );

  if ( __return_functions != NULL ){ // write in template style,handle registered functions
    type_regsterfunction*regfunc;
    Object  funclist  ; string func;
    funclist.SetObjectRef(__return_functions());
    for (u_int i=1;i<=funclist.Size();i++){
      func = funclist[i].Key();
      regfunc = (type_regsterfunction*)dlsym(lib_Handler,(func+"_Eva_Register_"+moduleName).c_str() );
      dlsym_error=dlerror();
      if ( regfunc == NULL || dlsym_error ) Warning("GetModule","Failed to register function "+func+". Please check your code style for proper usage of DefineFunction(func).");
      else regfunc(new_module);
    }
  }
  dlsym_error=dlerror();
  if ( dlsym_error )
    Warning("dlsym",((string)"error at end of GetLib:"+dlsym_error).c_str() );
  return 1;
}

int Evaluation::GetModule(string moduleName){
  if ( moduleName.size() == 0 )
    return -1;
  //cout<<"get module "<<moduleName<<endl;
  //System Module or None System Module
  try{
    if ( moduleName[0] < 'A' or moduleName[0] > 'Z' ){
      GetModuleDependence(moduleName);
      GetModulePathReady(moduleName);
      GetModuleEvaScript(moduleName);
      GetModuleMatPathReady(moduleName);
      GetModulePyPathReady(moduleName);

    }
    GetModuleLib(moduleName);
  }catch ( const Error&err ){
    cerr<<err.what()<<endl;
  }catch ( const exception &err){
    cerr<<"Sourcecode::Error: "<<err.what()<<endl;
  }catch ( ... ){
    cerr<<"System::Error: Unexpected error occured."<<endl;
  }

  return 0;
}

int Evaluation::RemoveModule(string moduleName){
  //moduleTable
  map<string,Module*>::iterator itermodule;
  itermodule=moduleTable.find( moduleName );
  // already initialized
  if ( itermodule == moduleTable.end() ){ ThrowError("RemoveModule","Module "+moduleName+" has not been got yet.");}
  //moduleDestroyHandleTable
  map<string,type_destroy*>::iterator iterdestroy;
  iterdestroy = moduleDestroyHandleTable.find(moduleName);
  if ( iterdestroy != moduleDestroyHandleTable.end() ){ // already initialized
    iterdestroy->second( itermodule->second );
    moduleDestroyHandleTable.erase(iterdestroy);
  }
  map<string,void*>::iterator iterlibhandle;
  iterlibhandle = libHandlerTable.find("lib"+moduleName+"Module.so");
  if ( iterlibhandle != libHandlerTable.end() ){
    dlclose( iterlibhandle->second );
    libHandlerTable.erase(iterlibhandle);
  }
  moduleTable.erase(itermodule);
  return 0;
}

int Evaluation::newGrammarModule(){
  valueTables.push_back( pair<ValueTable*,bool>(new ValueTable(),false) );
  currentValueTable = (*valueTables.rbegin()).first;
  return 0;
}

int Evaluation::newGrammarModule(ValueTable*vt){
  //vt->upValueTable =  (*valueTables.rbegin()).first;
  valueTables.push_back( pair<ValueTable*,bool>(vt ,true) );
  currentValueTable = (*valueTables.rbegin()).first;
  return 0;
}

int Evaluation::deleteGrammarModule(){
  if ( valueTables.size() > 1 ){
    if ( (*valueTables.rbegin()).first != NULL && (*valueTables.rbegin()).second == false ){
      delete (*valueTables.rbegin()).first;
    }
    valueTables.pop_back();
    currentValueTable = (*valueTables.rbegin()).first;
  }else{
    ThrowError("Evaluation","Top Grammar Unit reaches, fail to deleteGrammerModule");
  }
  return 0;
}

int Evaluation::ClearValueTablesTo(u_int N){
  if ( N < 1 ) ThrowError("ClearValueTableTo","Argument N is smaller than 1.");
  while ( valueTables.size() > N ){
    if ( (*valueTables.rbegin()).first != NULL && (*valueTables.rbegin()).second == false ){
      delete (*valueTables.rbegin()).first;
    }
    valueTables.pop_back();
  }
  currentValueTable = (*valueTables.rbegin()).first;
  return 0;
}

int Evaluation::PreEvaluate(Object&list){//prebuild value table
  if ( list.NullQ() ) return 0;
  switch ( list.type() ){ 
  case EvaType::Number: case EvaType::String: return 0;
  case EvaType::Symbol:{
    Object res = GetOrNewValuePairRefAtTop(list);
    if ( not res.NullQ() ){
      list.set_idx( res.objid );
      return 1;
    }
    return 0;
  }
  case EvaType::List:
    //Function  Module Compound and so on will no go deep 
    if ( list.ElementsSize() == 0 )
      ThrowError("List","Uncomplete List.");
    //deal head
    PreEvaluate(list[0]);
    if ( list[0].SymbolQ() ){
      if ( list[0].idx() != Index0 and list[0].ids() != SYMBOL_ID_OF___Variable and list[0].ids() != SYMBOL_ID_OF_SerialCode ){
        Object pairlist( list[0].idx() );
        int recId = pairlist.ids();
        EvaRecord *rec = evaRecordTable[ recId ];
        if ( rec != NULL and evawiz::AttributeQ( rec->attributes, AttributeType::GrammarUnit ) ){   
          return 5;
        }
      }//situation which is capable to apply PreEvaluate. do PreEvaluate to Elements
      for ( auto iter = list.Begin() ; iter != list.End(); iter++){
        PreEvaluate( *iter);
      }
      return 1;
    }
    //otherwize list[0] may change to a GrammarUnit value once Evaluated
    return 5;
  }
  return 0;
}

void Evaluation::EvaluateHead(Object&list,bool isHold){
  Evaluate(list[0], isHold);
}

void Evaluation::EvaluateFirst(Object&list,bool isHold){
  if ( list.Size() < 1) return;
  Evaluate( list[1],isHold);
}

void Evaluation::EvaluateRest(Object&list,bool isHold){
  if ( list.Size() < 2 ) return;
  for ( auto iter = list.Begin()+1;iter!=list.End(); iter++){
    Evaluate(*iter, isHold);
  }
}

void Evaluation::EvaluateAll(Object&list,bool isHold){
  if ( list.Size() < 1 ) return;
  for ( auto iter = list.Begin();iter!=list.End(); iter++){
    Evaluate(*iter, isHold);
  }
}

int Evaluation::ListableEvaluate(Object &Argv,bool isHold){
  if ( Argv.Size() == 0 ) return 0;
  //Check if condition
  u_int size = 0;
  for ( auto iter = Argv.Begin(); iter != Argv.End(); iter++ ){
    if ( iter->ListQ_List() ){
      if ( size == 0 ){
        size = (*iter).Size();
      }
      else{
        if ( iter->Size() != size )
          ThrowError( Argv.Key(),"shape","Try to apply listable attribute, but the shape does not match.");
      }
    }
  }
  if ( size == 0 ) return 0;
  //condition met
  Object obj; obj.SetList_List();
  for ( u_int i=1;i<=size;i++){
    Object chobj;
    chobj.SetList( Argv[0].ids() );
    chobj[0].set_idx( Argv[0].idx() );
    for ( auto iter = Argv.Begin(); iter != Argv.End(); iter++ ){
      if ( iter->ListQ_List() ){
        chobj.PushBackRef( (*iter)[i] );
      }else{
        chobj.PushBack( (*iter) );
      }
    }
    Evaluate( chobj );
    obj.PushBackRef( chobj );
  }
  Argv = obj;
  return 1;
}

int Evaluation::AttributeProcessing(Object&list,bool *attri,bool isHold){
  if ( not list.ListQ() or list.Size() == 0 )
    return 0;
  if ( attri == NULL ){
    list.FlattenSequence();
    EvaluateAll(list,isHold);
    return 0;
  }
  // flat and sequence
  if ( evawiz::AttributeQ(attri,AttributeType::Flat) ){
    if ( evawiz::AttributeQ(attri,AttributeType::SequenceHold) )
      list.Flatten();
    else
      list.FlattenListAndSequence();
  }else{
    if ( not evawiz::AttributeQ(attri,AttributeType::SequenceHold) )
      list.FlattenSequence();
  }
  // Listable
  if ( evawiz::AttributeQ(attri,AttributeType::Listable) ){
    if ( ListableEvaluate( list )>0) return 1;
  }
  // Hold Some
  if ( list.Size()>0 && ( not evawiz::AttributeQ(attri,AttributeType::HoldAll) ) ){
    if ( not evawiz::AttributeQ(attri,AttributeType::HoldFirst) ){
      EvaluateFirst(list,isHold);
    }
    if ( not evawiz::AttributeQ(attri,AttributeType::HoldRest) ){
      EvaluateRest(list,isHold);
    }
  }//HoldAll
  // Orderless
  if ( list.Size()>1 && evawiz::AttributeQ(attri,AttributeType::Orderless) )
    list.ExpressionSort();
  //DelayFunction like Function PureFunction
  if ( evawiz::AttributeQ(attri,AttributeType::DelayFunction) )
    return 1;
  return 0;
}

// return value and its meaning
// 0 nothing done ( hold the same )
// 1 normal return
// -1 some error occur, but nothing matter

#define EvaReturn(r) {                          \
    EvaluateRecursionDepth--;                   \
    return r;                                   \
  }
#define EvaReturnAfter(expr) {                  \
    int res = expr;                             \
    EvaluateRecursionDepth--;                   \
    return res;                                 \
  }

int Evaluation::Evaluate(Object&list,bool isHold,bool isRef){ // outList need to be an empty pointer
  if ( list.NullQ() )
    return 0;
  EvaluateRecursionDepth++;
  //dout<<"Try Evaluate "<<list.objid<<" in "<<EvaluateRecursionDepth<<" level"<<endl;
  if( EvaluateRecursionDepth > (int)this->maxRecursionDepth )
    ThrowError("Evaluate","Recursion depth exceeds "+ToString( (int)maxRecursionDepth )+".");
  if ( isHold ){
    switch ( list.type() ){
    case EvaType::Number: case EvaType::String: EvaReturn(0);
    case EvaType::Symbol: EvaReturn(0);
    case EvaType::List:
      //if Atom
      if ( list.ListQ( evawiz::SYMBOL_ID_OF_Evaluate ) ){
        if ( list.Size() != 1 ) ThrowError("Evaluate","1 Argument is required.");
        EvaReturnAfter( Evaluate(list[1],false) );
      }
      //deal head
      EvaluateHead(list,true);
      EvaluateRest(list,true);
      //according to the attribute get from head
      //if ( list[0]
      EvaReturn(0);
    }
    EvaReturn(0);
  }
  switch ( list.type() ){
  case EvaType::Number: case EvaType::String:
    EvaReturn(0);
  case EvaType::Symbol:{
    //dout<<" try deal this symbol "<<list<<endl;
    if ( list.idx() != Index0 and list.ids() != SYMBOL_ID_OF___Variable and list.ids() != SYMBOL_ID_OF_SerialCode ){
      Object pair_value( list.idx() );
      if ( not pair_value[2].SymbolQ( list.ids() ) ){
        if ( isRef ){
          list.SetObjectRef( pair_value[2] );
        }else{
          list.SetObject( pair_value[2] );
        }
        EvaReturnAfter( Evaluate(list) );
      }else{
        EvaReturn( 5 );
      }
    }else{ //for a normal Symbol
      if ( list.ids() == 0 ) return 0;
      if ( list.ids() == SYMBOL_ID_OF_Exit ) throw ExceptionQuit( 0 );
      //dout<<"  treated as a normal symbol"<<endl;
      // a single sysmbol can not be an pattern, so get ref is ok
      Object pair_value = GetValuePairRef( list ); 
      //dout<<"  value pair get: "<<pair_value<<endl;
      if ( not pair_value.NullQ() ){
        if ( pair_value[2].SymbolQ( list.ids() ) ){
          //dout<<list.objid<<" attried symbol with pair"<<pair_value.objid<<" "<<pair_value<<endl;
          list.set_idx( pair_value.objid );
          EvaReturn( 5 );
        }else{ //
          //dout<<list.objid<<" get new "<<pair_value[2].objid<<" level"<<endl;
          if ( isRef ){
            list.SetObjectRef( pair_value[2] );
          }else{
            list.SetObject( pair_value[2] );
          }
          EvaReturnAfter( Evaluate(list) );
        }
      }else{
        EvaReturn(0);//Nothing need to do more
      }
    }
  }
  case EvaType::List:
    //dout<<" try deal this list "<<list<<endl;
    if ( list.SimpleListQ() ){
      switch ( list[0].SymbolId() ){
      case SYMBOL_ID_OF_Black: EvaReturn(0); 
      case SYMBOL_ID_OF_BlackSequence: EvaReturn(0); //BlackSequence
      case SYMBOL_ID_OF_BlackNullSequence: EvaReturn(0); //BlackNullSequence
      case SYMBOL_ID_OF_Slot: EvaReturn(0); //Slot
      case SYMBOL_ID_OF_Exit:
        if ( list.Size() > 0 ){
          if ( list[1].NumberQ() ) throw ExceptionQuit( (int)list[1] );
          else throw ExceptionQuit(1);
        }else{
          throw ExceptionQuit(0);
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////
#define CASE(__module,__key) case SYMBOL_ID_OF_##__key:                 \
        static Index _id##_##__module##_##__key = GetOrNewValuePairRefAtTop(#__key).objid; \
        list[0].set_idx( _id##_##__module##_##__key   );                \
        break                                                 
        ////////////////////////////////////////////////////////////////////////////////////////////////
        CASE(ListModule,List); CASE(ListModule,Range);
        CASE(SystemModule,Evaluate); CASE(SystemModule,Set); CASE(SystemModule,SetDelayed); CASE(SystemModule,ToExpression); CASE(SystemModule,Parenthesis); CASE(SystemModule,CompoundExpression); CASE(SystemModule,Print); CASE(SystemModule,Replace); CASE(SystemModule,ReplaceAll); CASE(SystemModule,ReplaceRepeated); CASE(SystemModule,System);
        CASE(SystemModule,Function); CASE(SystemModule,PureFunction); CASE(SystemModule,Conjunct); 
        ////////////////////////////////////////////////////////////////////////////////////////////////
        CASE(SystemModule,If); CASE(SystemModule,While); CASE(SystemModule,Do); CASE(SystemModule,For); CASE(SystemModule,Foreach);
        ////////////////////////////////////////////////////////////////////////////////////////////////
        CASE(ArithmaticsModule,Plus); CASE(ArithmaticsModule,Times); CASE(ArithmaticsModule,Power); CASE(ArithmaticsModule,Log); CASE(ArithmaticsModule,Complex);
        ////////////////////////////////////////////////////////////////////////////////////////////////
        CASE(ArithmaticsModule,AddTo); CASE(ArithmaticsModule,SubstractFrom); CASE(ArithmaticsModule,TimesBy); CASE(ArithmaticsModule,DivideBy); CASE(ArithmaticsModule,PowerWith); CASE(ArithmaticsModule,Increment); CASE(ArithmaticsModule,Decrement); CASE(ArithmaticsModule,PreIncrement); CASE(ArithmaticsModule,PreDecrement);
        ////////////////////////////////////////////////////////////////////////////////////////////////
        CASE(ArithmaticsModule,Greater); CASE(ArithmaticsModule,GreaterEqual); CASE(ArithmaticsModule,Less); CASE(ArithmaticsModule,LessEqual); CASE(ArithmaticsModule,Equal); CASE(ArithmaticsModule,UnEqual); CASE(ArithmaticsModule,And); CASE(ArithmaticsModule,Or); CASE(ArithmaticsModule,Not); CASE(ArithmaticsModule,Random);
#undef CASE
      default:
        Evaluate( list[0] );
      }
    }else{
      Evaluate( list[0] );
    }
    if ( list[0].ValuedSymbolQ() ){// is a unChanged Symbol or something with EvaRecordTable id
      //dprintf("head has something when eval %s",list.ToString().c_str());
      Object pairlist( list[0].idx() );
      int recId = pairlist.ids();
      EvaRecord *rec = evaRecordTable[ recId ];
      if ( rec != NULL ){
        if ( AttributeProcessing(list,rec->attributes) > 0 )
          EvaReturn(1);
        if ( rec->staticFunction ){
          EvaReturnAfter( (rec->staticFunction)(list)  ) ;
        }else if ( rec->module ){
          EvaReturnAfter( ((rec->module)->*(rec->memberFunction))(list) );
        }
      }else{
        AttributeProcessing(list,NULL);
      }
      //dprintf("nothing done when eval %s afterall its head",list.ToString().c_str());
    }else{
      AttributeProcessing(list,NULL);
    }
    //after Processing list will surely be a lilst.
    if ( list[0].ListQ() and list[0][0].ValuedSymbolQ() ){
      Object pairlist( list[0][0].idx() );
      int recId = pairlist.ids();
      EvaRecord *rec = evaRecordTable[ recId ];
      if ( rec != NULL ){
        if ( evawiz::AttributeQ(rec->attributes, AttributeType::DelayFunction) ){
          if ( rec->staticFunction ){
            EvaReturnAfter( (rec->staticFunction)(list)  ) ;
          }else if ( rec->module ){
            EvaReturnAfter( ((rec->module)->*(rec->memberFunction))(list) );
          }
        }
      }
    }
    Object nobj;
    if ( GetValueRef( list,nobj ) > 0 ){//get a new Value
      if ( list != nobj ){
        if ( isRef ){
          list.SetObjectRef(nobj);
        }else{
          list.SetObject(nobj);
        }
        EvaReturnAfter( Evaluate(list) );
      }else{
        EvaReturn(0);
      }
    }
    // from pattern table
    //dprintf("Try get %s from pattern.",list.ToString().c_str() );
    if ( GetValue( list,nobj,true) == 0 ){
      //dprintf("no any value for %s.",list.ToString().c_str() );
      EvaReturn(0);
    }
    list.SetObjectRef( nobj );
    EvaReturnAfter( Evaluate( list ) );
  }
  EvaReturn(0);
}
#undef EvaReturn
//#undef EvaReturnSimplify

///////////////////////////////////////////////////////////
Object Evaluation::Evaluate(string expr){
  Object  list;
  list = ImportList::ToExpression(expr);
  Evaluate(list);
  //if ( not list.NullQ() ) cout<<list.ToString()<<"\n";
  return list;
}

int Evaluation::EvaluateFile(string filename,bool willBroken){
  ifstream ifs( filename.c_str() , ios::in );
  if ( !ifs ){
    Warning("Get","file "+filename+" cannot be opened.");
    return -1;
  }
  istreambuf_iterator<char> beg(ifs),end;
  string instr(beg,end);
  ifs.close();
  ////////////////////////////////////////////////////
  Object superListTemp,inputListTemp,superListThis,inputListThis;
  superListThis.SetList_List(); inputListThis.SetList_List();
  int RecursionDepth; u_int ValueTableDepth;
  superListTemp = this->superList; inputListTemp=this->inputList;
  RecursionDepth = this->EvaluateRecursionDepth;
  ValueTableDepth = this->valueTables.size();
  this->superList = superListThis; this->inputList= inputListThis;
  this->EvaluateRecursionDepth = -1;
  int printQuan=0,result=0;
  ImportList importList(instr);
  while ( !importList.End() ){
    try{
      result = 0;
      if ( importList.GetList( superListThis , 0) < 0 ){
        continue; //get list
      }
      if ( (int)superListThis.Size() > printQuan ){
        printQuan++;
        Object list = superListThis.Last(); // list is an alias of superList->Last()
        //record in histlist
        inputListThis.PushBack(list);
        ( result=this->Evaluate(list) );
        if ( result >= NormalQuitCode ) break;
        if ( result <= ErrorQuitCode ) break;
        //if no error, print the result if result will be printed out (ie. without ';' follow the clause)
        if ( (list).ListQ( SYMBOL_ID_OF_FullForm ) ){
          if ( list.Size()!=1 ){
            ThrowError(list.Key(),(string)list.Key()+" called with "+ToString( list.Size() )+" arguments; 1 argument is expected.");
          }
          string strout = (list)[1].ToFullFormString();
          if ( strout != "" ) cout<<strout<<"\n";
        }else{
          string strout = list.ToString();
          if ( (list).NullQ() ) strout = "";
          if ( strout != "" ) cout<<strout<<"\n";
        }
      }
    }catch ( const ExceptionQuit&err ){
      break;
    }catch ( const Error&err){
      cerr<<endl<<err<<endl;
      if ( willBroken )
        break;
      else
        this->EvaluateRecursionDepth = -1;
    }catch ( const exception &err){
      cerr<<"Sourcecode::Error: "<<err.what()<<endl;
      if ( willBroken )
        break;
      else
        this->EvaluateRecursionDepth = -1;
    }catch ( ... ){
      cerr<<"System::Error: Unexpected error occured."<<endl;
      if ( willBroken )
        break;
      else
        this->EvaluateRecursionDepth = -1;
    }
  }
  this->superList = superListTemp; this->inputList= inputListTemp;
  this->EvaluateRecursionDepth = RecursionDepth;
  this->ClearValueTablesTo( ValueTableDepth );
  return 0;
};

////////////////////////////////////////////////////////
int Evaluation::GetValueRef(Object &obj,Object&ref){
  switch ( obj.type() ){
  case EvaType::Number: case EvaType::String:
    ref.SetNull();
    return 0;
  case EvaType::Symbol:
    for ( auto iter = valueTables.rbegin(); iter!= valueTables.rend(); iter++){
      int res =  ((*iter).first)->GetValueRef(obj,ref);
      if (  res > 0 ) return 1;
      else if ( res < 0 ) return -1;
    }
    ref.SetNull();
    return 0;
  case EvaType::List: 
    if ( obj[0].SymbolQ( SYMBOL_ID_OF_Part ) ){
      ref.SetObject(obj);
      static StaticFunction func_part = NULL;
      if ( func_part == NULL ){
        EvaRecord *rec = GetEvaRecord( "Part" ); 
        if ( rec == NULL ) ThrowError("EvaKernel","Function 'Part' is not loaded properly.");
        func_part  =  rec->staticFunction;
      }
      (*func_part)(ref);
      return 1;
    }
    for ( auto iter = valueTables.rbegin(); iter!= valueTables.rend(); iter++){
      int res =  ((*iter).first)->GetValueRef(obj,ref);
      if (  res > 0 ) return 1;
      else if ( res < 0 ) return -1;
    }
    ref.SetNull();
    return 0;
  }
  ThrowError("GetValueRef","Switch error.");
}

//////////////////////////////////////////////////
int Evaluation::GetValue(Object &list,Object &value,bool onlyCheckPatternTable){
  switch (list.type()){
  case EvaType::Number: case EvaType::String:
    return 0;
  case EvaType::Symbol: case EvaType::List: 
    for ( auto iter = valueTables.rbegin(); iter!= valueTables.rend(); iter++){
      return_if_not( (iter->first)->GetValue(list,value, onlyCheckPatternTable) , 0);
    }
    return 0;
  }
  ThrowError("GetValue","swtich error");
};

int Evaluation::GetValue(Object &list){
  return GetValue( list,list);
};

bool Evaluation::GetValue(string key,string&str){//??????
  Object  list; list.SetSymbol( key.c_str() );
  Object  value = GetValuePairRef( list );
  if ( value.NullQ() or value[1] == value[2] ){return false;}
  if ( value[2].StringQ() ){
    str = value[2].Key();
  }else{
    str = value[2].ToString();
  }
  return true;
}

bool Evaluation::GetValue(string key,double&num){
  Object  list; list.SetSymbol( key.c_str() );
  Object  value = GetValuePairRef( list );
  if ( value.NullQ() or value[1] == value[2] ){return false;}
  if ( !value[2].NumberQ() ){return false;}
  num = value[2].Number();
  return true;
}

bool Evaluation::GetValue(string key,float&num){
  Object  list; list.SetSymbol( key.c_str() );
  Object  value = GetValuePairRef( list );
  if ( value.NullQ() or value[1] == value[2] ){return false;}
  if ( !value[2].NumberQ() ){return false;}
  num = value[2].Number();
  return true;
}

bool Evaluation::GetValue(string key,int&num){
  Object  list; list.SetSymbol( key.c_str() );
  Object  value = GetValuePairRef( list );
  if ( value.NullQ() or value[1] == value[2] ){return false;}
  if ( !value[2].NumberQ() ){return false;}
  num = value[2].Number();
  return true;
}

bool Evaluation::GetValue(string key,bool&num){
  Object  list; list.SetSymbol( key.c_str() );
  Object  value = GetValuePairRef( list );
  if ( value.NullQ() or value[1] == value[2] ){return false;}
  if ( !value[2].NumberQ() ){return false;}
  num = value[2].Number();
  return true;
}

// onlyGet==true
// onlyGet==false,
//     atTop == false
//     atTop == true
// return value is the pair Object from ValueTable
Object Evaluation::GetOrNewValuePairRefAtTop(Object&obj,bool onlyGet,bool atTop,bool isCheckedNoPattern)
{
  if ( obj.NumberQ() or obj.StringQ() )
    return NullObject;
  //dout<<"Try Get or New ValueRef of Object "<<obj<<endl;
  if ( obj.ValuedSymbolQ() ){//already a get value r
    //dout<<"Object is a ValuedSymbol"<<endl;
    return Object( obj.idx() );
  }
  Object res; 
  for ( auto iter = valueTables.rbegin(); iter!= valueTables.rend()-1; iter++){
    //dout<<"Try Only Get '"<<obj<<"' in valueTable "<<(*iter)<<endl;
    res.SetObjectRef( (iter->first)->GetOrNewValuePairRef(obj,true,isCheckedNoPattern) );
    if ( not res.NullQ() )
      return res;
  }
  if ( onlyGet ){
    //dout<<"Try Only Get '"<<obj<<"' in valueTable "<<( *valueTables.begin() )<<endl;
    return  (*valueTables.begin()).first->GetOrNewValuePairRef(obj,true,isCheckedNoPattern);
  }
  if ( atTop ){//Get if SomeWhere or NewAtTop
    //dout<<"Try New or Get '"<<obj<<"' in top valueTable "<< (*valueTables.begin() )<<endl;
   res =  (*valueTables.begin()).first->GetOrNewValuePairRef(obj,false,isCheckedNoPattern);
    return res;
  }else{//Get in SomeWhere or New At Bottom
    if ( valueTables.size() > 1 ){
      //dout<<"Try Get '"<<obj<<"' in top valueTable "<< (*valueTables.begin() )<<". Top is different from bottom."<<endl;
      res =  (*valueTables.begin()).first->GetOrNewValuePairRef(obj,true,isCheckedNoPattern);
      if ( not res.NullQ() )
        return res;
    }
    //dout<<"Try New or Get '"<<obj<<"' in from valueTable "<< (*valueTables.rbegin() ) <<endl;
    return (*valueTables.rbegin()).first->GetOrNewValuePairRef(obj,false,isCheckedNoPattern);
  }
}

Object Evaluation::GetOrNewValuePairRefAtTop(const char*sym_str)
{
  Object obj; obj.SetSymbol( sym_str );
  return GetOrNewValuePairRefAtTop( obj );
}

Object Evaluation::GetValuePairRef(Object &list,bool isCheckedNoPattern)
{
  return GetOrNewValuePairRefAtTop(list,true,true,isCheckedNoPattern);
};

Object Evaluation::GetOrNewValuePairRef(Object &list,bool isCheckedNoPattern){
  return GetOrNewValuePairRefAtTop(list,false,false,isCheckedNoPattern);
};

//onlyUpdate
//atTop
bool Evaluation::UpdateOrInsertValueAtTop(Object&left,Object&right,bool onlyUpdate,bool atTop,bool isCheckedPattern){
  if ( left.ValuedSymbolQ() ){
    Object( left.idx() ).SetObject( right );
    return true;
  }
  for ( auto iter = valueTables.rbegin(); iter != valueTables.rend()-1; iter++){
    bool result = (iter->first)->UpdateOrInsert(left,right,true,isCheckedPattern);
    if ( result ) return result;
  }
  if ( onlyUpdate ) return valueTables[0].first->UpdateOrInsert(left,right,true,isCheckedPattern);;
  if ( atTop ){
    return valueTables[0].first->UpdateOrInsert(left,right,false,isCheckedPattern);
  }else{
    valueTables[0].first->UpdateOrInsert(left,right,true,isCheckedPattern);
    return (*valueTables.rbegin()).first->UpdateOrInsert(left,right,false,isCheckedPattern);;
  }
}
                                     
bool Evaluation::InsertValue(Object &left,Object &right){
  return UpdateOrInsertValueAtTop(left,right,false,true);
}

bool Evaluation::InstantInsertOrUpdateValue(Object &left,Object &right){
  if ( left.ValuedSymbolQ() ){
    Object( left.idx() ).SetObject( right );
    return true;
  }
  return (*valueTables.rbegin()).first->UpdateOrInsert(left,right);
};

char Evaluation::Boolean(Object &list){
  Object tobj; tobj.SetObject(list);
  Evaluate(tobj);
  return tobj.Boolean();
}

bool* Evaluation::SetAttribute(Object &list,AttributeType attriType,bool val){
  register EvaRecord* rec = GetOrNewEvaRecord(list);
  rec->attributes[ (u_char)attriType] = val;
  return rec->attributes;
}

bool* Evaluation::SetAttribute(const char*key,AttributeType attriType,bool val){
  Object  list; list.SetSymbol(key);
  return SetAttribute(list,attriType,val);
}


bool* Evaluation::SetAttribute(const unsigned int id,AttributeType attriType,bool val){
  if ( id > 0 and id < evaRecordTable.size() ){
    register EvaRecord*rec = evaRecordTable[id];
    if ( rec == NULL ) ThrowError("SetAttributes","Try to set attributes record which is deleted.");
    rec->attributes[(u_char)attriType] = val;
    return rec->attributes;
  }
  ThrowError("SetAttributes","Try to set attributes record out of range.");
}

bool *Evaluation::GetAttributes(const unsigned int id){
  if ( id > 0 and id < evaRecordTable.size() ){
    register EvaRecord*rec = evaRecordTable[id];
    if ( rec == NULL ) ThrowError("GetAttributes","Try to get attributes record which is deleted.");
    return rec->attributes;
  }
  ThrowError("GetAttributes","Try to get attributes record out of range.");
}
bool *Evaluation::GetAttributes(Object &list){
  register EvaRecord*rec = GetOrNewEvaRecord(list);
  if ( rec == NULL ) return NULL;
  return rec->attributes;
}

bool *Evaluation::GetAttributes(const char* key){
  Object  list; list.SetSymbol(key);
  return GetAttributes(list);
}

bool Evaluation::AttributeQ(Object &list, AttributeType intype){
  bool *attris = GetAttributes( list );
  if ( attris == NULL ) return false;
  return evawiz::AttributeQ(attris,intype);
}

bool Evaluation::AttributeQ(const char*inkey, AttributeType intype){
  Object  list; list.SetSymbol(inkey);
  return AttributeQ(list,intype);
}

bool Evaluation::AttributeQ(const unsigned int id, AttributeType intype){
  bool *attris = GetAttributes(id);
  return evawiz::AttributeQ(attris,intype);
}

bool Evaluation::SetEvaRecord(Object&list, EvaRecord&rec){
  Object listpair = currentValueTable->GetOrNewValuePairRef( list );
  if ( listpair.ids() > 0 ){
    *evaRecordTable[ listpair.ids() ] = rec;
  }else{
    if ( freeEvaRecord.size() > 0 ){
      if ( evaRecordTable[ *freeEvaRecord.rbegin() ] != NULL ){
        evaRecordTable[ *freeEvaRecord.rbegin() ]->Update( rec );
      }else{
        evaRecordTable[ *freeEvaRecord.rbegin() ] =new EvaRecord(rec);
        freeEvaRecord.pop_back();
      }
    }else{
      listpair.set_ids( evaRecordTable.size() );
      evaRecordTable.push_back(new EvaRecord(rec) );
    }
  }
  return true;
}

bool Evaluation::ClearEvaRecord(int id){
  if ( id <= 0 ) return true;
  if ( id >= (int)evaRecordTable.size() )
    ThrowError("ClearEvaRecord","Try to clear EvaRecord out of range.");
  if ( evaRecordTable[id] == NULL ) return false;
  delete evaRecordTable[id];
  evaRecordTable[id] = NULL;
  freeEvaRecord.push_back(id);
  return true;
}

int Evaluation::Clear(Object&obj,bool repeated){
  for ( auto iter = valueTables.rbegin(); iter != valueTables.rend(); iter++){
    int tes =  (iter->first)->Clear(obj,this);
    if ( tes < 0 )
      return -1;
    if (  tes > 0 and not repeated )
      return 1;
  }
  return 0;
}

int Evaluation::Call( const char *funcName, Object &Argv)
{
  EvaRecord*rec = GetEvaRecord(funcName);
  if ( rec == NULL ) 
    ThrowError("EvaKernel",(string)"Function '"+funcName+"' is not loaded in yet.");
  if ( rec->staticFunction )
    return (*rec->staticFunction)(Argv);
  if ( rec->module and rec->memberFunction )
    return ((rec->module)->*(rec->memberFunction))(Argv);
  ThrowError("EvaKernel",(string)"Function '"+funcName+"' is not properly loaded.");
}

int Evaluation::Call(const EvaRecord*rec,Object&Argv)
{
  if ( rec == NULL ) 
    ThrowError("EvaKernel","Call",(string)"EvaRecord '"+ToString((void*)rec)+"' is NULL.");
  if ( rec->staticFunction )
    return (*rec->staticFunction)(Argv);
  if ( rec->module and rec->memberFunction )
    return ((rec->module)->*(rec->memberFunction))(Argv);
  ThrowError("EvaKernel",(string)"EvaRecord'"+ToString((void*)rec)+"' has a bad value.");
}

EvaRecord *Evaluation::GetEvaRecord(const char*str)
{
  Object obj; obj.SetSymbol( str );
  return GetEvaRecord( obj );
}

EvaRecord *Evaluation::GetEvaRecord(Object&list){
  if ( list.ValuedSymbolQ() ){
    Object temppair(list.idx());
    if ( temppair.ids() > 0 )
      return evaRecordTable[ temppair.ids() ];
    return NULL;
  }
  for ( auto iter = valueTables.rbegin(); iter != valueTables.rend(); iter++){
    Object pairlist = (iter->first)->GetOrNewValuePairRef( list,true );
    if ( pairlist.NullQ() ) continue;
    if ( pairlist.ids() > 0 ){
      return evaRecordTable[ pairlist.ids() ];
    }
  }
  return NULL;
}

EvaRecord *Evaluation::GetOrNewEvaRecord(Object&list){
  Object pairlist = currentValueTable->GetOrNewValuePairRef( list );
  if ( pairlist.ids() > 0 ){
    return evaRecordTable[ pairlist.ids() ];
  }else{
    EvaRecord *rec = new EvaRecord();
    if ( freeEvaRecord.size() > 0 ){
      pairlist.set_ids( *freeEvaRecord.rbegin() );
      evaRecordTable[ *freeEvaRecord.rbegin() ] = rec;
      freeEvaRecord.pop_back();
    }else{
      pairlist.set_ids( evaRecordTable.size() );
      evaRecordTable.push_back(rec);
    }
    return rec;
  }
}

int Evaluation::SetMaxRecursionDepth(int n){
  if (n<=0) return -1;
  maxRecursionDepth.set_re( n );
  return 0;
}

// Matrix things

Matrix *Evaluation::GetMatrix(string name){
  return GlobalPool.Matrices.GetMatrix(name);
}

ComplexMatrix *Evaluation::GetComplexMatrix(string name){
  return GlobalPool.ComplexMatrices.GetMatrix(name);
}

Matrix *Evaluation::GetOrNewMatrix(string name){
  return GlobalPool.Matrices.GetOrNewMatrix(name);
}

ComplexMatrix *Evaluation::GetOrNewComplexMatrix(string name){
  return GlobalPool.ComplexMatrices.GetOrNewMatrix(name);
}

int Evaluation::RemoveMatrix(string name){
  return GlobalPool.Matrices.RemoveMatrix(name);
}

int Evaluation::RemoveComplexMatrix(string name){
  return GlobalPool.ComplexMatrices.RemoveMatrix(name);
}

/////////////////////////////////////////////

