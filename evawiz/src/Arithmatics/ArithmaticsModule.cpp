#include"evawiz.h"
#include"ArithmaticsModule.h"
#include"eva_oldmacro.h"

using namespace std;
using namespace evawiz;

__DEBUG_MAIN__("input.ev")

#undef ModuleNameStr
#define ModuleNameStr "Arithmatics"
__CREATE_MODULE__(Arithmatics)
ArithmaticsModule::ArithmaticsModule(__CONSTRUCTION_PARA__){
  ///////////////////////////////////////////////////////////
  // arithmetic functions
  {
    RegisterFunction("Plus",Plus);
    AddAttribute("Plus",AttributeType::Flat);
    //AddAttribute("Plus",AttributeType::Orderless);
    AddAttribute("Plus",AttributeType::Listable);
  }
  {
    RegisterFunction("Times",Times);
    AddAttribute("Times",AttributeType::Flat);
    AddAttribute("Times",AttributeType::Orderless);
    AddAttribute("Times",AttributeType::Listable);
  }
  {
    RegisterFunction("Power",Power);
    AddAttribute("Power",AttributeType::Listable);
  }
  {
    RegisterFunction("Power",Power);
    AddAttribute("Power",AttributeType::Listable);
  }
  {
    RegisterFunction("Log",Log);
    AddAttribute("Log",AttributeType::Listable);
  }
  ///////////////////////////////
  {
    RegisterFunction("AddTo",AddTo);
    AddAttribute("AddTo",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("SubstractFrom",SubstractFrom);
    AddAttribute("SubstractFrom",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("TimesBy",TimesBy);
    AddAttribute("TimesBy",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("DivideBy",DivideBy);
    AddAttribute("DivideBy",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("PowerWith",PowerWith);
    AddAttribute("PowerWith",AttributeType::HoldFirst);
  }
  ///////////////////////////////////////////////////////////////////////
  {
    RegisterFunction("Increment",Increment);
    AddAttribute("Increment",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("PreIncrement",PreIncrement);
    AddAttribute("PreIncrement",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("Decrement",Decrement);
    AddAttribute("Decrement",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("PreDecrement",PreDecrement);
    AddAttribute("PreDecrement",AttributeType::HoldFirst);
  }
  ///////////////////////////////////////////////////////////
  {
    RegisterFunction("Greater",Greater);
    RegisterFunction("GreaterEqual",GreaterEqual);
    RegisterFunction("Less",Less);
    RegisterFunction("LessEqual",LessEqual);
    RegisterFunction("Equal",Equal);
    RegisterFunction("UnEqual",UnEqual);
  }
  ///////////////////////////////////////////////////////////
  {
    RegisterFunction("And",And);
    AddAttribute("And",AttributeType::HoldAll);
    AddAttribute("And",AttributeType::Flat);
  }
  {
    RegisterFunction("Or",Or);
    AddAttribute("Or",AttributeType::HoldAll);
    AddAttribute("Or",AttributeType::Flat);
  }
  {
    RegisterFunction("Not",Not);
    AddAttribute("Not",AttributeType::HoldAll);
  }
  ///////////////////////////////////////////////////////////
  {
    RegisterFunction("Abs",Abs);
    AddAttribute("Abs",AttributeType::Listable);
  }
  RegisterFunction("Factorial",Factorial);
  // normal member functions
  //RegisterFunction("Random",Random,"give a uniform random number in the range of (0,1).");
  srand( (unsigned)time(NULL) );
};

ArithmaticsModule::~ArithmaticsModule(){};

int ArithmaticsModule::Plus(Object &Argv){
  //dout<<"  step into Plus"<<endl;
  ReturnIfLessThan(1);
  if ( Argv.Size() == 1 ){
    Argv.SetTo(1);
    ReturnHold;
  }
  complex sum = 0;
  u_int p = Argv.Size();
  while ( p >=1 ) {
    EvaKernel->Evaluate(Argv[p]);
    if ( Argv[p].RealQ() ){
      sum+=(double)Argv[p];
      Argv.Delete(p);
    }else if ( Argv[p].ComplexQ() ){
      sum+=(complex)Argv[p];
      Argv.Delete(p);
    }
    p--;
  }
  //dout<<"  treat all elements"<<endl;
  if ( Argv.Size() > 0 ){
    if ( sum != 0.0 ){ //!=0
      Argv.InsertNumber(1,sum);
    }
    if ( Argv.Size() == 1 ){
      Argv.SetTo(1);
      ReturnNormal;
    }
    if ( Argv.Size() >= 2 and Argv[1].StringQ() and Argv[2].StringQ() ){
      string res(string(Argv[1].Key())+Argv[2].Key());
      if ( Argv.Size() == 2 ){
        Argv.SetString( res );
        ReturnNormal;
      }else{
        Argv[1].SetString( res );
        Argv.Delete(2);
        return EvaKernel->Evaluate( Argv );
      }
      ReturnNormal;
    }
    Argv.ExpressionSort();
    ReturnHold;
  }else{
    Argv.SetNumber(sum);
    ReturnNormal;
  }
}

int ArithmaticsModule::Times(Object &Argv){
  ReturnIfLessThan(1);
  Argv.FlattenListAndSequence();
  //dprintf("Calculate %s",Argv.ToString().c_str() );
  complex res=complex(1,0);
  u_int p = Argv.Size();
  while ( p >=1 ) {
    EvaKernel->Evaluate(Argv[p]);
    if ( Argv[p].RealQ() ){
      res*=(double)Argv[p];
      Argv.Delete(p);
    }else if ( Argv[p].ComplexQ() ){
      //dprintf("multi %s to res (%lf,%lf)", Argv[p].ToString().c_str(),res.re, res.im);
      res*=(complex)Argv[p];
      //res=res*(complex)Argv[p];
      //dprintf("after %s to res: (%lf,%lf)", Argv[p].ToString().c_str(),res.re, res.im);
      Argv.Delete(p);
    }
    p--;
  }
  if ( res == 0 or Argv.Size() == 0 ){
    ReturnNumber( res );
  }
  if ( res != 1.0 ){ //!=0
    Argv.InsertNumber(1,res);
    ReturnHold;
  }
  ReturnHold;
}

int ArithmaticsModule::Power(Object &Argv){
  ReturnIfNotEqual(2);
  EvaKernel->Evaluate( Argv[1] );
  EvaKernel->Evaluate( Argv[2] );
  if ( Argv[2].RealQ() ){
    double index = (double) Argv[2];
    if ( index == 0 )
      ReturnNumber(1);
    if ( index == 1 ){
      Argv.SetTo(1);
      ReturnNormal;
    }
    if ( Argv[1].RealQ() )
      ReturnNumber( pow((double)Argv[1],index ) );
    else if ( index == -1 and Argv[1].ComplexQ() ){
      ReturnNumber( 1/ (complex)Argv[1] );
    }
  }
  ReturnHold;
}

int ArithmaticsModule::Log(Object &Argv){
  ReturnIfNotWithin(1,2);
  EvaKernel->Evaluate( Argv[1] );
  if ( not Argv[1].RealQ() ) ReturnHold;
  if ( Argv.Size() == 1 ){
    double x = (double)Argv[1];
    ReturnNumber( log(x) );
  }
  EvaKernel->Evaluate( Argv[2] );
  if ( not Argv[2].RealQ() ) ReturnHold;
  ReturnNumber( log((double)Argv[2])/log((double)Argv[1]) );
}

int ArithmaticsModule::Complex(Object &Argv){
  ReturnIfNotEqual(2);
  EvaKernel->Evaluate( Argv[1] );
  EvaKernel->Evaluate( Argv[2] );
  if ( Argv[1].RealQ() && Argv[2].RealQ() ){
    ReturnNormal;
  }
  ReturnHold;
}

int ArithmaticsModule::AddTo(Object &Argv){
  ReturnIfNotEqual(2);
  Object &var = Argv[1];
  Object &obj = Argv[2];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      EvaKernel->Evaluate( obj );
      if ( value[2].NumberQ() && obj.NumberQ() ){
        value[2].SetNumber( (complex)value[2]+(complex)obj );
        Argv.SetObject(value[2]);
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x + 1
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Plus );
  newR.PushBack( var ); newR.PushBackRef( obj );
  newS.PushBackRef( var ); newS.PushBackRef( newR );
  Argv = newS;
  return EvaKernel->Evaluate( Argv );
}

int ArithmaticsModule::SubstractFrom(Object &Argv){
  ReturnIfNotEqual(2);
  Object &var = Argv[1];
  Object &obj = Argv[2];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      EvaKernel->Evaluate( obj );
      if ( value[2].NumberQ() && obj.NumberQ() ){
        value[2].SetNumber( (complex)value[2]-(complex)obj );
        Argv.SetObject(value[2]);
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x + (-1*a)
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Plus );
  newR.PushBack( var );
  Object tobj; tobj.SetList( SYMBOL_ID_OF_Times ); tobj.PushBackNumber(-1); tobj.PushBackRef( obj );
  newR.PushBackRef( tobj );
  newS.PushBackRef( var ); newS.PushBackRef( newR );
  Argv = newS;
  return EvaKernel->Evaluate( Argv );
}

int ArithmaticsModule::TimesBy(Object &Argv){
  ReturnIfNotEqual(2);
  Object &var = Argv[1];
  Object &obj = Argv[2];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      EvaKernel->Evaluate( obj );
      if ( value[2].NumberQ() && obj.NumberQ() ){
        value[2].SetNumber( (complex)value[2]*(complex)obj );
        Argv.SetObject(value[2]);
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x * a
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Times );
  newR.PushBack( var ); newR.PushBackRef( obj );
  newS.PushBackRef( var ); newS.PushBackRef( newR );
  Argv = newS;
  return EvaKernel->Evaluate( Argv );
}

int ArithmaticsModule::DivideBy(Object &Argv){
  ReturnIfNotEqual(2);
  Object &var = Argv[1];
  Object &obj = Argv[2];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      EvaKernel->Evaluate( obj );
      if ( value[2].NumberQ() && obj.NumberQ() ){
        value[2].SetNumber( (complex)value[2]/(complex)obj );
        Argv.SetObject(value[2]);
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x + (-1*a)
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Times );
  newR.PushBack( var );
  Object tobj; tobj.SetList( SYMBOL_ID_OF_Power ); tobj.PushBackRef( obj );  tobj.PushBackNumber(-1);
  newR.PushBackRef( tobj );
  newS.PushBackRef( var ); newS.PushBackRef( newR );
  Argv = newS;
  return EvaKernel->Evaluate( Argv );
}

int ArithmaticsModule::PowerWith(Object &Argv){
  ReturnIfNotEqual(2);
  Object &var = Argv[1];
  Object &obj = Argv[2];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      EvaKernel->Evaluate( obj );
      if ( value[2].RealQ() && obj.RealQ() ){
        value[2].SetNumber( pow( (double)value[2],(double)obj ) );
        Argv.SetObject(value[2]);
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x ^ a
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Power );
  newR.PushBack( var ); newR.PushBackRef( obj );
  newS.PushBackRef( var ); newS.PushBackRef( newR );
  Argv = newS;
  return EvaKernel->Evaluate( Argv );
}



int ArithmaticsModule::Increment(Object &Argv){
  ReturnIfNotEqual(1);
  Object&var = Argv[1];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      if ( value[2].NumberQ() ){
        Argv.SetObject( value[2] );
        value[2].SetNumber( (complex)value[2]+1.0 );
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x + 1 
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Plus );
  newR.PushBackRef( var ); newR.PushBackNumber( 1.0 );
  newS.PushBack( var ); newS.PushBackRef( newR );
  EvaKernel->Evaluate(var);
  Argv.SetTo( 1 ) ;
  EvaKernel->Evaluate( newS );
  return 1;
 };

int ArithmaticsModule::Decrement(Object &Argv){
  ReturnIfNotEqual(1);
  Object&var = Argv[1];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      if ( value[2].NumberQ() ){
        Argv.SetObject( value[2] );
        value[2].SetNumber( (complex)value[2]-1.0 );
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x + 1 
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Plus );
  newR.PushBackRef( var ); newR.PushBackNumber( -1.0 );
  newS.PushBack( var ); newS.PushBackRef( newR );
  EvaKernel->Evaluate(var);
  Argv.SetTo( 1 ) ;
  EvaKernel->Evaluate( newS );
  return 1;
 };

int ArithmaticsModule::PreIncrement(Object &Argv){
  ReturnIfNotEqual(1);
  Object &var = Argv[1];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      if ( value[2].NumberQ() ){
        value[2].SetNumber( (complex)value[2]+1.0 );
        Argv.SetObject( value[2] );
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x ^ a
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Plus );
  newR.PushBackRef( var ); newR.PushBackNumber( 1.0 );
  newS.PushBack( var ); newS.PushBackRef( newR );
  Argv = newS;
  return EvaKernel->Evaluate( Argv );
};


int ArithmaticsModule::PreDecrement(Object &Argv){
  ReturnIfNotEqual(1);
  Object &var = Argv[1];
  if ( var.SymbolQ() ){
    Object value = EvaKernel->GetValuePairRef( var );
    if ( not value.NullQ() ){
      if ( value[2].NumberQ() ){
        value[2].SetNumber( (complex)value[2]-1.0 );
        Argv.SetObject( value[2] );
        ReturnNormal;
      }
    }else
      ThrowError(Argv.Key(),Argv[1].ToString()+" is not a variable with a value.");
  }
  //change to form of x = x ^ a
  Object newS; newS.SetList( SYMBOL_ID_OF_Set );
  Object newR; newR.SetList( SYMBOL_ID_OF_Plus );
  newR.PushBackRef( var ); newR.PushBackNumber( -1.0 );
  newS.PushBack( var ); newS.PushBackRef( newR );
  Argv = newS;
  return EvaKernel->Evaluate( Argv );
};

// is to deal with form like (a < b) < c or (a < b) >c
// will be transformed to    And[a<b,b<c]  And[a<b,b>c]
#define __pre_treatment                                       \
  if ( ( left.ListQ(evawiz::SYMBOL_ID_OF_Greater ) )          \
       ||left.ListQ(evawiz::SYMBOL_ID_OF_GreaterEqual )       \
       ||left.ListQ(evawiz::SYMBOL_ID_OF_Less )               \
       ||left.ListQ(evawiz::SYMBOL_ID_OF_LessEqual )          \
       ||left.ListQ(evawiz::SYMBOL_ID_OF_Equal )              \
       ||left.ListQ(evawiz::SYMBOL_ID_OF_UnEqual ) ){         \
    CheckArgsShouldEqual(left,2);                             \
    Object newleft;  newleft = left; Argv.Delete(1);          \
    Object newright; newright.SetList( Argv[0].ids() );       \
    newright.PushBack( newleft[2] );                          \
    newright.PushBackRef( Argv[1] );                          \
    Argv.SetList( evawiz::SYMBOL_ID_OF_And );                 \
    Argv.PushBackRef( newleft ); Argv.PushBackRef(newright);  \
    EvaKernel->Evaluate(Argv);                                      \
    ReturnNormal;                                             \
  }                                                           \

int ArithmaticsModule::Greater(Object &Argv){
  ReturnIfNotEqual(2);
  Object&left  = Argv(1);
  Object&right = Argv(2);
  __pre_treatment
  EvaKernel->Evaluate( left );
  EvaKernel->Evaluate( right );
  if ( left.RealQ() && right.RealQ() ){
    Argv.SetNumber( (double)left> (double)right );
    ReturnNormal;
  }
  ReturnHold;
}

int ArithmaticsModule::GreaterEqual(Object &Argv){
  ReturnIfNotEqual(2);
  Object&left = Argv(1);
  Object&right= Argv(2);
  __pre_treatment
  EvaKernel->Evaluate( left );
  EvaKernel->Evaluate( right );
  if ( left.RealQ() && right.RealQ() ){
    Argv.SetNumber(  (double)left>=double(right) );
    ReturnNormal;
  }
  ReturnHold;
}

int ArithmaticsModule::Less(Object &Argv){
  ReturnIfNotEqual(2);
  Object&left = Argv(1);
  Object&right= Argv(2);
  __pre_treatment
  EvaKernel->Evaluate( left );
  EvaKernel->Evaluate( right );
  if ( left.RealQ() && right.RealQ() ){
    Argv.SetNumber( double(left)<double(right) );
    ReturnNormal;
  }
  ReturnHold;
}

int ArithmaticsModule::LessEqual(Object &Argv){
  ReturnIfNotEqual(2);
  Object&left = Argv(1);
  Object&right= Argv(2);
  __pre_treatment
  EvaKernel->Evaluate( left );
  EvaKernel->Evaluate( right );
  if ( left.RealQ() && right.RealQ() ){
    Argv.SetNumber( double(left) <= double(right) );
    ReturnNormal;
  }
  ReturnHold;
}

int ArithmaticsModule::Equal(Object &Argv){
  ReturnIfNotEqual(2);
  Object&left = Argv(1);
  Object&right= Argv(2);
  __pre_treatment
  EvaKernel->Evaluate( left );
  EvaKernel->Evaluate( right );
  if ( left.NumberQ() && right.NumberQ() ){
    Argv.SetNumber( left == right );
    ReturnNormal;
  }else{
    if ( left == right ){
      Argv.SetNumber(true);
      ReturnNormal;
    }
  }
  ReturnHold;
}
int ArithmaticsModule::UnEqual(Object &Argv){
  ReturnIfNotEqual(2);
  Object&left = Argv(1);
  Object&right= Argv(2);
  __pre_treatment
  EvaKernel->Evaluate( left );
  EvaKernel->Evaluate( right );
  if ( left.NumberQ() && right.NumberQ() ){
    Argv.SetNumber( double(left) != double(right));
    ReturnNormal;
  }
  ReturnHold;
}

#undef __pre_treatment

int ArithmaticsModule::And(Object &Argv){
  CheckShouldNoLessThan(2);
  bool unkonwn = false;
  for ( u_int i = 1; i<=Argv.Size(); i++ ){
    EvaKernel->Evaluate( Argv[i] );
    if ( Argv[i].Boolean() == 0 ){ //false
      ReturnNumber(0);
    }else if ( Argv[i].Boolean() < 0 ){
      unkonwn = true;
    }
  }
  if ( unkonwn )
    ReturnHold;
  ReturnNumber(1);
}

int ArithmaticsModule::Or(Object &Argv){
  CheckShouldNoLessThan(2);
  int unknownExpr= false;
  for ( u_int i = 1; i<=Argv.Size(); i++ ){
    EvaKernel->Evaluate( Argv[i] );
    if ( Argv[i].Boolean() > 0 ){
      ReturnNumber(1);
    }else if ( Argv[i].Boolean()< 0 ){
      unknownExpr = true;
    }
  }
  if ( unknownExpr )
    ReturnHold;
  ReturnNumber(0);
}

int ArithmaticsModule::Not(Object &Argv){
  ReturnIfNotEqual(1);
  EvaKernel->Evaluate( Argv[1] );
  if ( Argv[1].Boolean() > 0 ){
    Argv.SetNumber(0);
  }else if ( Argv[1].Boolean() == 0 ){
    Argv.SetNumber(1);
  }
  ReturnHold;
}

int ArithmaticsModule::Factorial(Object &Argv){
  ReturnIfNotEqual(1);
  if ( not Argv[1].RealQ() ) ReturnHold ;
  long res=1, num = Argv[1].Number();
  for (int i = 2 ;i <=num; i++)
    res*=i;
  Argv.SetNumber((double)res);
  ReturnNormal ;
}

int ArithmaticsModule::Abs(Object &Argv){
  ReturnIfNotEqual(1);
  if ( Argv[1].RealQ() ){
    ReturnNumber( abs((double)Argv[1]) );
  }else if ( Argv[2].ComplexQ() ){
    complex res = (complex)Argv[1]; 
    ReturnNumber( sqrt( pow2(res.re)+pow2(res.im) ) );
  }
  ReturnHold;
}

int ArithmaticsModule::SetRandomSeed(Object &Argv){
  ReturnIfNotEqual(1);
  ReturnIfNotNumber(1);
  srand( (int)Argv[1] );
  double num;
  num = (double)rand()/RAND_MAX;
  Argv.SetNumber(num);
  ReturnNormal;
}

int ArithmaticsModule::Random(Object &Argv){
  double num;
  num = (double)rand()/RAND_MAX;
  Argv.SetNumber(num);
  ReturnNormal;
}

