/*******/
#include<evawiz>          
#include"MathematicaModule.h" 
#include"mathlink.h"
using namespace std;
using namespace evawiz;
/****************************/
__DEBUG_MAIN__("input.ev");

#define LINK MLINK(link)
ModuleInitialize
{
  env  = NULL;
  link = NULL;
}

ModuleFinalize
{
  Close();
}

/****************************/
bool MathematicaModule::On()
{
  return link;
}

bool MathematicaModule::Open()
{
  if ( not On() ){
    int argc = 4; 
    const char*argv[5]={"-linkname","math -mathlink","-linkmode","launch", NULL};
    env = (void*)MLInitialize(NULL);
    link = (void*)MLOpen(argc,const_cast<char**>(argv));
  }
  return link;
}

bool MathematicaModule::Close()
{
  if ( link != NULL ){
    MLClose(LINK);
    MLDeinitialize( (MLENV)env);
  }
  return true;
}

/****************************/
DefineFunction(Mathematica,"Mathematica Mathlink. Evaluate as String return as String.")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  ReturnString( Evaluate(Argv[1].Key() ) );
}

int MathematicaModule::PutObject(Object&obj){
  switch ( obj.type() ){
  case EvaType::Number:{
    MLPutDouble(LINK, obj.Number() );
    return 0;
  }
  case EvaType::Symbol:{
    MLPutSymbol(LINK, obj.Key() );
    return 0;
  }
  case EvaType::String:{
    MLPutString(LINK, obj.Key() );
    return 0;
  }
  case EvaType::List:{
    if ( not obj[0].SymbolQ() ){
      Close();
      ThrowError("PutObject","Complex List with Non-Symbol head is not supported.");
    }
    MLPutFunction(LINK,obj[0].Key(),obj.Size() );
    for ( u_int i=1; i <= obj.Size(); i++ ){
      PutObject( obj[i] );
    }
    return 0;
  }
  }
  Close();
  ThrowError("PutObject","There is something wrong with the input.");
}

DefineFunction(MLPutValue,"Put value to MathLink Kernel.")
{
  CheckShouldEqual(2);
  CheckShouldBeString(1);
  //////////////////////////////
  PutValue( Argv[1].Key(), Argv[2] );
  ReturnNull;
}
               

#include"SystemModule.h"
DefineFunction(MLGetValue,"Get value from MathLink Kernel.")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  Argv = GetValue( Argv[1].Key() );
  ReturnNormal;
}

int MathematicaModule::PutMatrix(string varname,Matrix&mat){
  Open();
  static long int dim[10];
  static const char *heads[10]={"List","List","List","List","List","List","List","List","List","List"};
  for ( int i=0; i< mat.DimN(); i++ ){
    dim[i] = mat.Dim(i+1);
  }
  
  //////////////////////////////
  MLPutFunction(LINK,"EvaluatePacket",1);{
    MLPutFunction(LINK,"CompoundExpression",2);{
      MLPutFunction(LINK,"Set",2);{
        MLPutSymbol(LINK,varname.c_str() );
        ///
        MLPutDoubleArray(LINK, mat.Data, dim, const_cast<char**>(heads), mat.DimN() );
      }
      MLPutDouble(LINK,2.0);
    }
  }MLEndPacket(LINK);

  
  int pkt;
  while ( ( pkt = MLNextPacket(LINK) ) && ( pkt != RETURNPKT ) )
    MLNewPacket(LINK);
  if ( !pkt ) {
    Close();
    ThrowError("Mathematica","Evluation errors occured.");
  }
 
  double x;
  MLGetDouble(LINK,&x);
  return 0;
}

DefineFunction(MLPutMatrix,"Put Matrix to MathLink Kernel.")
{
  CheckShouldEqual(2);
  CheckShouldBeString(1);
  CheckShouldBeList(2);
  /////////////////////////////
  CheckShouldBeListWithHead(2,Matrix);
  CheckArgsShouldEqual(Argv[2],1);
  CheckArgsShouldBeString(Argv[2],1);

  Matrix*mat=EvaKernel->GetMatrix(Argv[2][1].Key());

  PutMatrix(Argv[1].Key(), *mat);

  ReturnNull;
}

string MathematicaModule::Evaluate(string expr){
  Open();
  MLPutFunction(LINK,"EvaluatePacket",1);{
    MLPutFunction(LINK,"ToString",1);{
      MLPutFunction(LINK,"ToExpression",1);{
        MLPutString(LINK,expr.c_str() );
      }
    }
  }MLEndPacket(LINK);

  int pkt;
  while ( ( pkt = MLNextPacket(LINK) ) && ( pkt != RETURNPKT ) )
    MLNewPacket(LINK);
  if ( !pkt ){
    Close();
    ThrowError("Mathematica","Evluation errors occured.");
  }
  const char *res;
  MLGetString(LINK,&res);
  ////////////////////
  return res;
}

int MathematicaModule::PutValue(string name,Object value){
  Open();
  //////////////////////////////
  MLPutFunction(LINK,"EvaluatePacket",1);{
    MLPutFunction(LINK,"CompoundExpression",2);{
      MLPutFunction(LINK,"Set",2);{
        MLPutSymbol(LINK, name.c_str() );
        PutObject(value);
      }
      MLPutDouble(LINK,2.0);
    }
  }MLEndPacket(LINK);

  int pkt;
  while ( ( pkt = MLNextPacket(LINK) ) && ( pkt != RETURNPKT ) )
    MLNewPacket(LINK);
  if ( !pkt ) {
    Close();
    ThrowError("Mathematica","Evluation errors occured.");
  }
 
  double x;
  MLGetDouble(LINK,&x);
  return 0;
}

Object MathematicaModule::GetValue(string name){
  Open();
  //////////////////////////////
  MLPutFunction(LINK,"EvaluatePacket",1);{
    MLPutFunction(LINK,"ToString",1);{
      MLPutSymbol( LINK, name.c_str() );
    }
  }MLEndPacket(LINK);

  int pkt;
  while ( ( pkt = MLNextPacket(LINK) ) && ( pkt != RETURNPKT ) )
    MLNewPacket(LINK);
  if ( !pkt ){
    Close();
    ThrowError("Mathematica","Evluation errors occured.");
  }

  const char *res;
  MLGetString(LINK,&res);
  Object obj;
  obj.SetString( res );
  static EvaRecord *rec = NULL;
  if ( rec == NULL ){
    rec = EvaKernel->GetEvaRecord("ToExpression");
    if ( rec == NULL or rec->staticFunction == NULL )
      ThrowError("EvaKernel","ToExpression from 'System' is not loaded properly.");
  }
  (*rec->staticFunction)(obj);
  return obj;
}
