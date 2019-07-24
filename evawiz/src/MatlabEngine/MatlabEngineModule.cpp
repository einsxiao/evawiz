#include"evawiz.h"
#include"MatrixModule.h"
#include"MatlabEngineModule.h"
#include"eva_oldmacro.h"

using namespace std;
using namespace evawiz;

__DEBUG_MAIN__("input.ev");
#undef ModuleNameStr
#define ModuleNameStr "MatlabEngine"
__CREATE_MODULE__(MatlabEngine)
MatlabEngineModule::MatlabEngineModule(__CONSTRUCTION_PARA__){
  //////////////
  RegisterFunction("MatStart",(MemberFunction)(&MatlabEngineModule::MatStart));
  RegisterFunction("MatClose",(MemberFunction)(&MatlabEngineModule::MatClose)); 
  RegisterFunction("MatAddPath",(MemberFunction)(&MatlabEngineModule::MatAddPath)); 
  //////////////
  RegisterFunction("MatEvaluate",(MemberFunction)(&MatlabEngineModule::MatEvaluate)); 
  RegisterFunction("Matlab",(MemberFunction)(&MatlabEngineModule::MatEvaluate)); 
  RegisterFunction("MatEval",(MemberFunction)(&MatlabEngineModule::MatEval)); 
  RegisterFunction("MatCall",(MemberFunction)(&MatlabEngineModule::MatCall)); 
  RegisterFunction("MatGetResult",(MemberFunction)(&MatlabEngineModule::MatGetResult));
  //////////////
  RegisterFunction("MatPutValue",(MemberFunction)(&MatlabEngineModule::MatPutValue)); 
  RegisterFunction("MatGetValue",(MemberFunction)(&MatlabEngineModule::MatGetValue));
  AddAttribute("MatPutValue",AttributeType::HoldFirst);
  AddAttribute("MatGetValue",AttributeType::HoldFirst);
  //////////////
  RegisterFunction("MatGetMatrix",(MemberFunction)(&MatlabEngineModule::MatGetMatrix));
  RegisterFunction("MatPutMatrix",(MemberFunction)(&MatlabEngineModule::MatPutMatrix)); 
  ////////////// perform like
  state=0;
  ep=NULL;
}

MatlabEngineModule::~MatlabEngineModule(){
  if ( ep !=NULL ) engClose(ep);
};

static int LocalComplexArray2List(mxArray *mxA,Object &Argv,int *dim,int pdim,int parr){
  if ( pdim > dim[0] ){
    Argv.PushBackNumber( complex( mxGetPr(mxA)[parr] ,  mxGetPi(mxA)[parr]) );
    return parr+1;
  }
  Argv.SetList_List();
  for (int i=0; i< dim[pdim]; i++){
    Argv.PushBackNull();
    parr=LocalComplexArray2List(mxA,Argv[i+1],dim,pdim+1,parr);
  }
  return parr;
}

static int LocalLogicalArray2List(mxArray *mxA,Object &Argv,int *dim,int pdim,int parr){
  if ( pdim > dim[0] ){
    if ( mxGetPr(mxA)[parr] )
      Argv.SetNumber( true );
    else
      Argv.SetNumber( false );
    return parr+1;
  }
  Argv.SetList_List();
  for (int i=0; i< dim[pdim]; i++){
    Argv.PushBackNull();
    parr=LocalLogicalArray2List(mxA,Argv[i+1],dim,pdim+1,parr);
  }
  return parr;
}

static int LocalCellArray2List(mxArray *mxA,Object &Argv,int *dim,int pdim,int parr){
  if ( pdim > dim[0] ){
    MatlabEngineModule::mxArray2List( mxGetCell(mxA,parr) , Argv );
    return parr+1;
  }
  Argv.SetList_List();
  for (int i=0; i< dim[pdim]; i++){
    Argv.PushBackNull();
    parr=LocalCellArray2List(mxA,Argv[i+1],dim,pdim+1,parr);
  }
  return 1;
}

template<class type> int LocalArray2List(type *arr,Object &Argv,int *dim,int pdim,int parr,string header){
  if ( pdim > dim[0] ){
    Argv.SetNumber( (type)arr[parr] ) ;
    return parr+1;
  }
  Argv.SetList_List( );
  for (int i=0; i< dim[pdim]; i++){
    Argv.PushBackNull();
    parr=LocalArray2List(arr,Argv[i+1],dim,pdim+1,parr,header);
  }
  return parr;
}

int MatlabEngineModule::mxArray2List(mxArray *mxA,Object &Argv){
  int *dim,dimN = (int)mxGetNumberOfDimensions(mxA); 
  dim=new int[dimN+2]; dim[0] = dimN;
  for ( int j=dimN-1, i=1; i<=dimN; i++,j-- ){
    dim[i]=(int)mxGetDimensions(mxA)[j];
    while (dim[i]==1){
      j--; dim[0]--; dimN--;
      dim[i]=(int)mxGetDimensions(mxA)[j];
      if (j==0) break;
    }
  }
  if ( mxIsCell(mxA) ){
    LocalCellArray2List(mxA,Argv,dim,1,0);
  }else if ( mxIsComplex(mxA) ){
    LocalComplexArray2List(mxA,Argv,dim,1,0);
  }else if ( mxIsDouble(mxA) ){
    MatrixModule::Array2List(Argv, mxGetPr(mxA),dim);
  }else if ( mxIsSingle(mxA) ){
    MatrixModule::Array2List(Argv,(float*) mxGetData(mxA),dim);
  }else if ( mxIsLogical(mxA) ){
    LocalLogicalArray2List( mxA,Argv,dim,1,0);
  }else if ( mxGetClassID(mxA) == mxINT32_CLASS ){
    MatrixModule::Array2List( Argv,(int*)mxGetData(mxA),dim);
  }else if ( mxGetClassID(mxA) == mxINT64_CLASS ){
    MatrixModule::Array2List(Argv, (long long*) mxGetData(mxA),dim);
  }else if ( mxIsEmpty(mxA) ){
    Argv.SetNull();
  }else{
    delete []dim;
    ThrowError("mxArray2List","mxArray type is not supported.");
  }
  delete []dim;
  return 0;
}

///////////////////////////////////
int MatlabEngineModule::GetResult(Object &ans){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  //get matlab engine result of Evaluate
  mxArray *answer=engGetVariable(ep,"ans");
  if ( answer==NULL ) ThrowError("GetResult","Cannot get result from matlab engine.");
  // indentify the type of answer an sole number, a vector, or a matrix
  mxArray2List(answer,ans);
  mxDestroyArray(answer);
  return 0;
}

Object MatlabEngineModule::GetResult(){
  if ( !Open() ){
    ThrowError("Evaluate","Engine cannot open.");
  }
  Object result;
  GetResult(result);
  return result;
}

int MatlabEngineModule::Evaluate(string str_in){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  if ( engEvalString( ep,str_in.c_str() ) ) ThrowError("Evaluate","Failed to Evaluate.");
  int pos = str_in.find_first_of("=",0);
  if ( pos< (int)str_in.size() && str_in[pos+1] != '=' ){
    // an assignment expression, do extra things to extract result in variable ans
    string left_var=str_in.substr(0,pos);
    if ( engEvalString(ep, (left_var+"+0;").c_str() ) )
      ThrowError("Evaluate","Failed to Evaluate.");
  }
  return 1;
}

bool MatlabEngineModule::On(){
  return state>0;
};

bool MatlabEngineModule::Open(){
  if ( !On() ){
    ep=engOpen(NULL); 
    if ( !ep ){
      state = -1;
      Warning("Open","Failed to open matlab engine.");
      return false;
    }
    state = 1;
    engEvalString(ep,"0");
    // add system path to engine
    string dir_root = GetEnv("EVAWIZ");
    engEvalString(ep,("addpath('"+dir_root+"/lib')").c_str());
    // add path in paths
    for ( auto iter = paths.begin(); iter!= paths.end(); iter++ ){
      engEvalString(ep,("addpath('"+(*iter)+"')").c_str());
    }
    return true;
  }
  return true;
}

bool MatlabEngineModule::Close(){
  if ( !ep ) {
    bool result=engClose(ep);
    if ( result ){
      state = 0;
      return true;
    }
    return false;
  }
  return true;
}

//////////////////////////////////
bool MatlabEngineModule::AddPath(string path){
  if ( not On() ){
    paths.push_back(path);
  }else{
    engEvalString(ep,("addpath('"+path+"')").c_str() );
  }
  return true;
}

int MatlabEngineModule::MatAddPath(Object &Argv){
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  AddPath( Argv[1].Key() );
  ReturnNull;
}
//////////////////////////////////

int MatlabEngineModule::MatStart(Object &Argv){
  if ( Open() ) Argv.SetNumber(true);
  else Argv.SetNumber(false);
  ReturnNormal;
}

int MatlabEngineModule::MatClose(Object &Argv){
  if ( Close() ) Argv.SetNumber(true);
  else Argv.SetNumber(false);
  ReturnNormal;
}

int MatlabEngineModule::MatEvaluate(Object &Argv){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  if( !Evaluate( Argv[1].Key() ) ) ReturnNormal ;
  ReturnNull;
}

int MatlabEngineModule::MatEval(Object &Argv){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  if( Evaluate( Argv[1].Key() ) > 0 ){
    string str(Argv[1].Key());
    auto iter= str.rbegin(); iter++;
    while ( *iter == ' ' || *iter=='\n' || *iter == '\t' )
      iter++;
    if ( *iter != ';' ){
      GetResult(Argv);
      ReturnNormal;
    }
    ReturnNull;
  }
  ThrowError("MatEval",Argv[1].ToString()+" can not be evaluated.");
}

int MatlabEngineModule::MatCall(Object &Argv){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckArgsShouldNoLessThan(Argv,1);
  string expr=Argv[1].ToString()+"(";
  for ( u_int i=2; i<= Argv.Size()-1; i++ ){
    expr += Argv[i].ToString()+",";
  }
  expr+=Argv.Last().ToString()+")";
  if ( Evaluate( expr )<0 ){
    ThrowError("MatCall","Error eccoured while Call Matlab functions.");
  }
  GetResult(Argv);
  ReturnNormal;
}

//////////////////////

int MatlabEngineModule::MatGetResult(Object &Argv){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckArgsShouldEqual(Argv,0);
  GetResult(Argv);
  ReturnNormal;
}

//////////////////////////////////

int MatlabEngineModule::GetValue(string var_name,Object &var){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  mxArray *mxA=engGetVariable(ep,var_name.c_str() );
  if ( mxA==NULL ) ThrowError("GetValue","Cannot get value for "+var_name+".");
  // indentify the type of mxA an sole number, a vector, or a matrix
  mxArray2List(mxA,var);
  mxDestroyArray(mxA);
  return 1;
}

Object MatlabEngineModule::GetValue(string var_name){
  if ( !Open() ){
    Warning("Evaluate","Engine cannot open."); 
    return NullObject;
  }
  Object result;
  GetValue(var_name,result);
  return result;
}

int MatlabEngineModule::MatGetValue(Object &Argv){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  GetValue(Argv[1].Key(), Argv);
  ReturnNormal;
}


///////////////////////////////////
int MatlabEngineModule::List2mxCellArray(Object &Argv,mxArray*cellArr,int pdim,int parr){
  if ( pdim == 0 ){
    mxSetCell(cellArr,parr,List2mxArray(Argv) );
    return parr+1;
  }
  for ( u_int i=1; i<=Argv.Size(); i++ ){
    parr=List2mxCellArray(Argv[i],cellArr,pdim-1,parr);
  }
  return parr;
}

mxArray *MatlabEngineModule::List2mxArray(Object &Argv){
  if ( Argv.SymbolQ() ){
    Warning("Matlab","List2mxArray","Symbol without a value is forbiden to transform to mxArray.");
    return NULL;
  }
  if ( Argv.NumberQ() ){
    return mxCreateDoubleScalar( Argv.Number() );
  }
  if ( Argv.StringQ() ){
    return mxCreateString( Argv.Key() );
  }
  mxArray*mxA;
  if ( Argv.ListQ( SYMBOL_ID_OF_Matrix) ){ // a matrix
    Matrix *mat = EvaKernel->GetMatrix( Argv[1].Key() );
    if ( mat->Data == NULL ){
      Warning("List2mxArray","No data stored in host memory in "+ Argv[1].ToString() +", please firstly move data from gpu memory or create it.");
      return NULL;
    }
    return Matrix2mxArray(*mat);
  }else if ( Argv.ListQ_List() ){
    Object dim;
    if ( MatrixModule::MatrixQ( Argv, dim ) ){ // A Argv and is a matrix
      mwSize dimN,*mxdim;
      dimN = dim.Size();
      mxdim = new mwSize[dimN];
      for ( u_int i=0; i<dimN; i++ )
        mxdim[i] = (double)dim[dimN-i];
      mxA = mxCreateNumericArray(dimN,mxdim,mxDOUBLE_CLASS, mxREAL);  
      delete[] mxdim;
      if ( mxA == NULL ){
        Warning("List2mxArray","Can not create mxArray of double type.");
        return NULL;
      } 
      MatrixModule::List2Array(Argv,mxGetPr(mxA));
      return mxA;
    }else{ // normal Argv, transform to cell array
      mwSize mxdim[1]={Argv.Size()};
      mxA = mxCreateCellArray(1,mxdim);
      if ( mxA == NULL ){
        Warning("List2mxArray","Can not create mxArray of cell type.");
        return NULL;
      }
      List2mxCellArray(Argv,mxA,1,0);
      return mxA;
    }
  }
  return NULL;
}

int MatlabEngineModule::PutValue(string var_name,Object &var){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");

  if ( var.NumberQ() ){ // a number
    string expr = var_name +"="+ var.ToString();
    return   Evaluate(expr) ;
  }

  if ( var.StringQ() ){
    string expr = var_name+"='"+var.Key() +"'";
    return Evaluate( expr );
  }
  mxArray *mxA = List2mxArray(var);
  if ( engPutVariable(ep,var_name.c_str(),mxA) )
    ThrowError("PutValue","Can not put variable to Matlab Engine.");

  mxDestroyArray(mxA);
  return 1;
}

int MatlabEngineModule::MatPutValue(Object &Argv){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckArgsShouldEqual(Argv,2);
  CheckShouldBeString(1);
  int result =PutValue(Argv[1].Key(), Argv[2]);
  if (result>0){
    ReturnNull;
  }
  ReturnHold;
}

int MatlabEngineModule::mxArray2Matrix(mxArray*mxA,Matrix&matrix){
  int *dim,dimN = (int)mxGetNumberOfDimensions(mxA); 
  dim=new int[dimN+2]; dim[0] = dimN;
  for ( int i=1; i<=dimN; i++ )
    dim[i]=(int)mxGetDimensions(mxA)[dimN-i];
  matrix.Init(dim,MatrixHost);
  memcpy(matrix.Data,mxGetPr(mxA),sizeof(double)*matrix.Size());
  delete []dim;
  return 0;
}

mxArray *MatlabEngineModule::Matrix2mxArray(Matrix&matrix){
  if ( matrix.Size() == 0 || matrix.Data==NULL ){
    Warning("Matrix2mxArray","No data in Matrix.");
    return NULL;
  }
  mxArray *mxA;
  mwSize dimN = matrix.Dim(0),*dim; 
  dim = new mwSize(dimN);
  for ( u_int i = 0 ; i< dimN; i++){
    dim[i] = matrix.Dim(dimN-i);
  }
  if ( ! (mxA=mxCreateNumericArray(dimN,dim,mxDOUBLE_CLASS, mxREAL) ) ){
    Warning("mxArray2Matrix","Can not create mxArray.");
    delete []dim;
    return NULL;
  }
  delete []dim;
  memcpy(mxGetPr(mxA), matrix.Data,sizeof(double)*matrix.Size());
  return mxA;
}

int MatlabEngineModule::GetMatrix(string mx_name,Matrix &matrix){
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  mxArray *mxA=engGetVariable(ep,mx_name.c_str());
  if ( mxA==NULL ) ThrowError("GetMatrix","Cannot get Matrix from matlab engine.");
  if ( !mxIsDouble(mxA) )
    ThrowError("GetMatrix","Function GetMatrix only supports double type mxArray.");
  return mxArray2Matrix(mxA,matrix);
}

int MatlabEngineModule::MatGetMatrix(Object &Argv){ //MatGetMatrix['a']
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  string m_name = Argv[1].Key();
  Matrix * mat = EvaKernel->GetOrNewMatrix(m_name);
  if ( mat == NULL ) ThrowError("MatGetMatrix","Matrix system cannot get or create matrix.");
  int result = GetMatrix(m_name,*mat);
  if ( result > 0 ){
    ReturnNull;
  }
  Argv.SetList(SYMBOL_ID_OF_Matrix);
  Argv.PushBackString( m_name );
  ReturnHold;
}

int MatlabEngineModule::PutMatrix(Matrix &matrix, string mx_name){
  mxArray *mxA;
  mxA=Matrix2mxArray(matrix);
  if ( mxA == NULL ) ThrowError("PutMatrix","Can not create mxArray.");
  if ( engPutVariable(ep,mx_name.c_str(),mxA) ){
    mxDestroyArray(mxA);
    ThrowError("PutMatrix","Can not put Matrix into Matlab Engine.");
  }
  mxDestroyArray(mxA);
  return 1;
}

int MatlabEngineModule::MatPutMatrix(Object &Argv){ // MatPutMatrix['a']
  if ( !Open() ) ThrowError("Evaluate","Engine cannot open.");
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  string m_name = Argv[1].Key();
  Matrix *mat=EvaKernel->GetMatrix(m_name);
  if ( mat == NULL ) ThrowError("MatPutMatrix","No Matrix "+m_name+"in system.");
  int result = PutMatrix(*mat,m_name);
  if (result>0){
    ReturnNull;
  }
  ReturnHold;
}


