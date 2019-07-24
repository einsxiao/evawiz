#include"evawiz.h"
#include"SystemModule.h"
#include"eva_oldmacro.h"
#include<sys/time.h>

using namespace std;
using namespace evawiz;

__DEBUG_MAIN__("input.ev");

#undef ModuleNameStr
#define ModuleNameStr "System"
__CREATE_MODULE__(System)
SystemModule::SystemModule(__CONSTRUCTION_PARA__){
  ///////////////////////////////////////////////////////////
  // special form
#define AP(sym) EvaKernel->SetAttribute(#sym,AttributeType::Protected)
  AP(Null); AP(Listable); AP(Flat); AP(Orderless); AP(OneIdentity); AP(HoldFirst); AP(HoldRest);
  AP(HoldAll); AP(HoldCompleted); AP( SequenceHold); AP(Constant); AP(Protected); AP(Locked);
  AP( MAXENUM);
  AP( I );
#undef AP
  Object left,right;
  left.SetSymbol("I"); right.SetComplex( complex(0,1) ); EvaKernel->InsertValue(left,right);

  // systematic functions

  {
    RegisterFunction("Evaluate",Evaluate);
  }
  {
    RegisterFunction("Set",Set);
    AddAttribute("Set",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("SetDelayed",SetDelayed);
    AddAttribute("SetDelayed",AttributeType::HoldAll);
  }
  {
    RegisterFunction("CompoundExpression",CompoundExpression);
    AddAttribute("CompoundExpression",AttributeType::HoldAll);
  }
  {
    RegisterFunction("Replace",Replace);
  }
  {
    RegisterFunction("ReplaceAll",ReplaceAll);
  }
  {
    RegisterFunction("ReplaceAllRepeated",ReplaceRepeated);
  }
  {
    RegisterFunction("In",In);
    AddAttribute("In",AttributeType::Listable);
  }
  {
    RegisterFunction("Out",Out);
    AddAttribute("Out",AttributeType::Listable);
  }
  {
    RegisterFunction("GetModule",GetModule); 
    AddAttribute("GetModule",AttributeType::Listable);
  }
  {
    RegisterFunction("RemoveModule",RemoveModule); 
    AddAttribute("RemoveModule",AttributeType::Listable);
  }
  {
    RegisterFunction("ShowModuleFunctions",ShowModuleFunctions);
    AddAttribute("ShowModuleFunctions",AttributeType::Listable);
  }
  {
    RegisterFunction("ShowFunctionsOfModule",ShowModuleFunctions);
    AddAttribute("ShowFunctionsOfModule",AttributeType::Listable);
  }
  {
    RegisterFunction("ShowModules",ShowModules);
  }
  ////////////////////////////////////////////////////////////////
  {
    RegisterFunction("Options",Options);
  }
  {
    RegisterFunction("Attributes",Attributes); 
    AddAttribute("Attributes",AttributeType::Listable);
  }
  {
    RegisterFunction("SetAttributes",SetAttribute_Eva);
    AddAttribute("SetAttributes",AttributeType::HoldAll);
    AddAttribute("Attributes",AttributeType::Setable);
  }
  {
    RegisterFunction("AddAttribute",AddAttribute_Eva);
    AddAttribute("AddAttribute",AttributeType::HoldAll);
  }
  ///////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////
  // pattern system
  {
    RegisterFunction("MatchQ",MatchQ);
  }
  {
    RegisterFunction("UnifyRule",UnifyRule);
  }
  {
    RegisterFunction("UnifiedMatchQ",UnifiedMatchQ);
  }
  ////////////////////////////////////////////////////////////
  {
    AddAttribute("Pattern",AttributeType::HoldFirst);
    AddAttribute("Pattern",AttributeType::Protected);
  }
  {
    AddAttribute("RuleDelayed",AttributeType::HoldRest);
    AddAttribute("RuleDelayed",AttributeType::Protected);
  }
  ////////////////////////////////////////////////////////////
  {
    RegisterFunction("Hold",Hold);
    AddAttribute("Hold",AttributeType::HoldAll);
  }
  {
    RegisterFunction("HoldOnce",HoldOnce);
    AddAttribute("HoldOnce",AttributeType::HoldAll);
  }
  {
    RegisterFunction("HoldPattern",HoldPattern);
    AddAttribute("HoldPattern",AttributeType::HoldAll);
  }
  ///////////////////////////////////////////////////////////
  {
    RegisterFunction("Module",GrammarModule);
    AddAttribute("Module",AttributeType::HoldAll);
    AddAttribute("Module",AttributeType::GrammarUnit);
  }
  {
    RegisterFunction("Function",Function);
    AddAttribute("Function",AttributeType::DelayFunction);
    AddAttribute("Function",AttributeType::HoldAll);
    AddAttribute("Function",AttributeType::GrammarUnit);
  }
  {
    RegisterFunction("PureFunction",PureFunction);
    AddAttribute("PureFunction",AttributeType::DelayFunction);
    AddAttribute("PureFunction",AttributeType::HoldAll);
  }
  {
    RegisterFunction("Conjunct",Conjunct);
    AddAttribute("Conjunct",AttributeType::HoldAll );
  }
  ///////////////////////////////////////////////////////////
  {
    RegisterFunction("If",If);
    AddAttribute("If",AttributeType::HoldAll);
  }
  {
    RegisterFunction("While",While);
    AddAttribute("While",AttributeType::HoldAll);
  }
  {
    RegisterFunction("For",For);
    AddAttribute("For",AttributeType::HoldAll);
  }
  {
    RegisterFunction("Foreach",Foreach);
    AddAttribute("Foreach",AttributeType::HoldAll);
    AddAttribute("Foreach",AttributeType::GrammarUnit);
  }
  {
    RegisterFunction("Do",Do);
    AddAttribute("Do",AttributeType::HoldAll);
    AddAttribute("Do",AttributeType::GrammarUnit);
  }
  {
    RegisterFunction("System",System);
    RegisterFunction("SystemWithoutOutput",SystemWithoutOutput);
  }
  ///////////////////////////////////////////////////////////
  RegisterFunction("SetOutputFormat",SetOutputFormat,"Set output format for numbers.\nSetOutputFormat[precison,is_scientific]"); 
  RegisterFunction("SystemArgv",SystemArgv,"Get system input argument variables.");
  ///////////////////////////////////////////////////////////
  RegisterFunction("SetRunningMode",SetRunningMode,"Set running mode for following code.");
  RegisterFunction("GetRunningMode",GetRunningMode,"Get current running mode .");
  RegisterFunction("GPUDeviceQ",GPUDeviceQ,"Get GPU Device List.");
  RegisterFunction("SetCudaThreadsNumberPerBlock",SetCudaThreadsNumberPerBlock,"Set cuda thread number per block when launching evawiz kernel on cuda device.");
  RegisterFunction("SetEvawizCpuKernelThreadsNumber",SetEvawizCpuKernelThreadsNumber,"Set thread number when launching evawiz cpu kernel.");
  RegisterFunction("GetCudaThreadsNumberPerBlock",GetCudaThreadsNumberPerBlock,"Get cuda thread number per block when launching evawiz kernel on cuda device.");
  RegisterFunction("GetEvawizCpuKernelThreadsNumber",GetEvawizCpuKernelThreadsNumber,"Get thread number when launching evawiz cpu kernel.");
  ///////////////////////////////////////////////////////////
  RegisterFunction("Help",ShowHelpInfo,"Show help information of module functions.");
  RegisterFunction("Clear",Clear,"Clear defination relating to symbol(s)."); AddAttribute("Clear",AttributeType::HoldAll);
  RegisterFunction("Protect",Protect_Eva,"Add Protect Attribute to a Symbol.");
  AddAttribute("Protect",AttributeType::HoldAll);
  RegisterFunction("UnProtect",UnProtect_Eva,"Remove Protect Attribute of a Symbol if exist.");
  AddAttribute("UnProtect",AttributeType::HoldAll);
  ///////////////////////////////////////////////////////////
  RegisterFunction("AbsoluteTime",AbsoluteTime,"Get Absolute time from 1970.");
  RegisterFunction("DateList",DateList,"Give out the Date Object .");
  RegisterFunction("Timing",Timing,"Calculate the time a command consumed."); AddAttribute("Timing",AttributeType::HoldAll);
  RegisterFunction("CpuTiming",CpuTiming,"Calculate the Cpu time a command consumed."); AddAttribute("CpuTiming",AttributeType::HoldAll);
  RegisterFunction("Sleep",Sleep,"Sleep for some seconds. Then continue to run.");
  RegisterFunction("ToString",SystemModule::ToString_Eva);
  RegisterFunction("ToExpression",SystemModule::ToExpression);
  RegisterFunction("Print",SystemModule::Print);
};

SystemModule::~SystemModule(){};
//ModuleFinalize{};


int SystemModule::Evaluate(Object &Argv){
  ReturnIfNotEqual(1);
  EvaKernel->Evaluate(Argv[1],false);
  Argv.SetTo(1);
  ReturnNormal;
}

int SystemModule::Options(Object & Argv){
  ReturnHold;
}

int SystemModule::SetOptions(Object & Argv){
  ReturnHold;
}

int SystemModule::SetAttribute(Object &left,Object&attris){
  if ( not attris.ListQ_List() )
    ThrowError("SetAttribute","Attributes should be list in a List.");
  for ( u_int i =1 ;i<= attris.Size();i++){
    if ( not (attris)[i].StringQ() )
      ThrowError("SetAttibute","Attibutes should be Strings.");
  }
  bool *attri;
  if ( left.ListQ( SYMBOL_ID_OF_Attributes) ){
    if ((left).Size() != 1 ) ThrowError("SetAttributes","Attributes requires an Object as argument.");
    if ( not left[1].SymbolQ() ) ThrowError("SetAttribute","Only Symbol can assign attributes to.");
    attri = EvaKernel->GetAttributes( (left)[1].Key() );
  }else{
    if ( not left.SymbolQ() ) ThrowError("SetAttribute","Only Symbol can assign attributes to.");
    attri = EvaKernel->GetAttributes( left );
  }
  if ( attri == NULL )
    ThrowError("SetAttributes","Can not create new attributes.");
  
  AttributeType type;
  for (u_int i=1; i<=attris.Size(); i++ ){
    if ( not attris[i].StringQ() ) ThrowError("SetAttributes","A attribute string is required.");
    type = String2AttributeType( attris[i].Key() );
    if ( type == AttributeType::Null ) ThrowError("SetAttribute",(string)"Unkonwn Attribute \""+attris[i].Key()+"\".");
    evawiz::SetAttribute(attri,type);
  }
  return 0;
};

int SystemModule::SetAttribute_Eva(Object &Argv){
  CheckArgsShouldEqual(Argv,2);
  SetAttribute( Argv[1], Argv[2]);
  Argv.SetTo(2);
  ReturnNormal;
};

int SystemModule::AddAttribute_Eva(Object &Argv){
  CheckShouldEqual(2);
  CheckShouldBeSymbol(1);
  EvaRecord *rec = EvaKernel->GetOrNewEvaRecord( Argv[1] );
  if ( rec == NULL )
    ThrowError("AddAttribute","Can not get Symbol Attributes Object .");
  if ( AttributeQ(rec->attributes,AttributeType::Protected) )
    ThrowError("AddAttribute",(string)"Symbol "+Argv[1].Key()+" is protected.");
  if ( Argv[2].SymbolQ() || Argv[2].StringQ() ){
    AttributeType attriType = String2AttributeType( Argv[2].Key() );
    if ( attriType == AttributeType::Null )
      ThrowError("AddAttribute","Attribute type specified is not right.");
    evawiz::SetAttribute(rec->attributes, attriType );
    ReturnNull;
  }else if ( Argv[2].ListQ_List() ){
    AttributeType attriType;
    for ( u_int i =1; i<=Argv[2].Size() ; i++){
      attriType = String2AttributeType( Argv[2][i].Key() );
      if ( attriType == AttributeType::Null )
        ThrowError("AddAttribute","Attribute type specified is not right.");
      evawiz::SetAttribute( rec->attributes, attriType );
    }
    ReturnNull;
  }
  ThrowError("AddAttribute","Attributes specified is not in the right form.");
};

int SystemModule::RemoveAttributes_Eva(Object &Argv){
  CheckShouldEqual(2);
  CheckShouldBeSymbol(1);
  EvaRecord *rec = EvaKernel->GetOrNewEvaRecord( Argv[1] );
  if ( rec == NULL )
    ThrowError("RemoveAttribute","Can not get Symbol Attributes Object .");
  if ( AttributeQ(rec->attributes,AttributeType::Protected) )
    ThrowError("RemoveAttribute",(string)"Symbol "+Argv[1].Key()+" is protected.");
  if ( Argv[2].SymbolQ() || Argv[2].StringQ() ){
    AttributeType attriType = String2AttributeType( Argv[2].Key() );
    if ( attriType == AttributeType::Null )
      ThrowError("RemoveAttribute","Attribute type specified is not right.");
    evawiz::SetAttribute( rec->attributes, attriType,false);
    ReturnNull;
  }else if ( Argv[2].ListQ_List() ){
    AttributeType attriType;
    for ( u_int i =1; i<=Argv[2].Size() ; i++){
      attriType = String2AttributeType( Argv[2][i].Key() );
      if ( attriType == AttributeType::Null )
        ThrowError("RemoveAttribute","Attribute type specified is not right.");
      evawiz::SetAttribute( rec->attributes, attriType,false );
    }
    ReturnNull;
  }
  ThrowError("RemoveAttribute","Attributes specified is not in the right form.");
};

int SystemModule::Protect_Eva(Object &Argv){
  CheckShouldEqual(1);
  CheckShouldBeSymbol(1);
  EvaRecord *rec = EvaKernel->GetOrNewEvaRecord( Argv[1] );
  if ( rec == NULL )
    ThrowError("RemoveAttribute","Can not get Symbol Attributes Object .");
  evawiz::SetAttribute( rec->attributes, AttributeType::Protected, true);
  ReturnNull;
};

int SystemModule::UnProtect_Eva(Object &Argv){
  CheckShouldEqual(1);
  CheckShouldBeSymbol(1);
  EvaRecord *rec = EvaKernel->GetOrNewEvaRecord( Argv[1] );
  if ( rec == NULL )
    ThrowError("RemoveAttribute","Can not get Symbol Attributes Object .");
  evawiz::SetAttribute( rec->attributes, AttributeType::Protected,false);
  ReturnNull;
};

int SystemModule::Attributes(Object & Argv){
  Set_Context(Attributes){
    return SetAttribute_Eva(Argv);
  }
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeSymbol(1);
  Object arg = Argv(1);
  bool *attri = EvaKernel->GetAttributes( arg.Key() );
  Argv.SetList_List();
  if ( attri ){
    for ( int i = (int)AttributeType::Null+1; i< (int)AttributeType::MAXENUM; i++ ){ 
      if ( attri[i] )
        Argv.PushBackString( AttributeType2String((AttributeType)i) );
    }
  }
  ReturnNormal;
}

int SystemModule::Parenthesis(Object & Argv){
  for ( auto iter = Argv.Begin(); iter != Argv.End(); iter++){
    EvaKernel->Evaluate(*iter);
  }
  if ( Argv.Size() == 1 ){
    Argv.SetTo(1);
    ReturnHold;
  }
  if ( Argv.Size() == 2 and Argv[1].NumberQ() and Argv[2].NumberQ() ){
    Argv[0].SetSymbol( SYMBOL_ID_OF_Complex );
    ReturnHold;
  }
  ReturnHold;
}

int SystemModule::CompoundExpression(Object & Argv){
  for (auto iter = Argv.Begin(); iter!= Argv.End() ; iter++){
    EvaKernel->Evaluate(*iter);
  }
  Argv.SetTo( Argv.Size() );
  ReturnNormal;
}

static int specialCharReplace(string &str,string ori,string rep){
  int pos = 0;
  while ( (pos <(int)str.size())&&pos>=0 ){
    pos = str.find(ori);
    if (pos>=0) str.replace(pos,ori.size(),rep);
  }
  return 0;
}

static int specialCharReplacement(string &str){
  specialCharReplace(str,"\\n","\n");
  specialCharReplace(str,"\\t","\t");
  specialCharReplace(str,"\\r","\r");
  specialCharReplace(str,"\\\\","\\");
  specialCharReplace(str,"\\\"","$QUOTATION_MARK$");
  specialCharReplace(str,"\"","");
  specialCharReplace(str,"$QUOTATION_MARK$","\"");
  return 0;
}

int SystemModule::Print(Object & Argv){
  if ( Argv.Size()>0 ){
    for (auto iter=Argv.Begin(); iter!=Argv.End(); iter++){
      EvaKernel->Evaluate(*iter);
      if(  (*iter).NullQ() ) continue;
      string str = (*iter).ToString();
      specialCharReplacement(str);
      cout<< str;
    }
    if ( not Argv.Last().NullQ() || Argv.Size() == 1 )
      cout<<"\n";
  }else
    cout<<"\n";
  ReturnNull;
}

int SystemModule::ToString_Eva(Object& Argv)
{
  CheckShouldEqual(1);
  string content = Argv[1].ToString();
  Argv.SetString( content );
  ReturnNormal;
}

/*
int SystemModule::ToExpression_Eva(Object& Argv, Evaluation *Eva )
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  Object t;
  t = ImportList::ToExpression( Argv[1].Key() );
  Argv = t;
  ReturnNormal;
}
*/

int SystemModule::MatchQ(Object & Argv){
  CheckArgsShouldEqual(Argv,2);
  Object&l=Argv(1),&pattern = Argv(2);
  Pattern::UnifyRuleLeft( pattern );
  Argv.SetNumber( Pattern::UnifiedMatchQ( l, pattern ) );
  ReturnNormal;
}

int SystemModule::UnifiedMatchQ(Object & Argv){
  CheckArgsShouldEqual(Argv,2);
  Object&l=Argv(1),&pattern = Argv(2);
  Argv.SetNumber( Pattern::UnifiedMatchQ(l,pattern));
  ReturnNormal;
}

int SystemModule::UnifyRule(Object & Argv){
  CheckArgsShouldEqual(Argv,1);
  Object&pattern = Argv(1);
  if ( (pattern).Size() < 2 ){
    ThrowError("UnifyRule","Rule form is right.");
  }
  Pattern::UnifyRule( (pattern)[1], (pattern)[2] );
  Argv.SetTo(1);
  ReturnNormal;
}

int SystemModule::Hold(Object & Argv){
  for ( auto iter = Argv.Begin(); iter!= Argv.End(); iter++){
    EvaKernel->Evaluate( *iter, true );
  }
  ReturnHold;
}

int SystemModule::HoldPattern(Object & Argv){
  CheckShouldEqual( 1 );
  EvaKernel->Evaluate( Argv[1], true );
  ReturnHold;
}

int SystemModule::HoldOnce(Object & Argv){
  CheckShouldEqual(1);
  EvaKernel->Evaluate( Argv[1], true );
  Argv.SetTo(1);
  ReturnNormal;
}


int SystemModule::Replace(Object & Argv){
  CheckArgsShouldEqual(Argv,2);
  Object&l = Argv(1);
  Object&rules = Argv(2);
  EvaKernel->Evaluate(rules);
  Pattern::ReplaceAll(l,rules,false);
  Argv.SetTo(1);
  EvaKernel->Evaluate(Argv);
  ReturnNormal;
}

int SystemModule::ReplaceAll(Object & Argv){
  CheckArgsShouldEqual(Argv,2);
  Object&l = Argv(1); 
  Object&rules = Argv(2);
  EvaKernel->Evaluate(rules);
  if ( not rules.ListQ( SYMBOL_ID_OF_List ) ){
    Pattern::ReplaceAll(l,rules,true);
    Argv.SetTo(1);
    EvaKernel->Evaluate(Argv);
    ReturnNormal;
  }
  if ( rules.ListQ( SYMBOL_ID_OF_List) ){
    for (u_int i = 1; i<= rules.Size() ; i++)
      Pattern::ReplaceAll( l, (rules)[i] ,true);
    Argv.SetTo(1);
    EvaKernel->Evaluate(Argv);
    ReturnNormal;
  }
  ThrowError("ReplaceAll","Second parameter should be a Rule or a Argv of rules.");
}

int SystemModule::ReplaceRepeated(Object & Argv){
  CheckArgsShouldEqual(Argv,2);
  Object l = Argv(1);
  Object rules = Argv(2);
  EvaKernel->Evaluate(rules);
  while ( Pattern::ReplaceAll(l,rules,true) );
  Argv.SetTo(1);
  EvaKernel->Evaluate(Argv);
  ReturnNormal;
}

int SystemModule::GrammarModule(Object &Argv){
  CheckArgsShouldNoLessThan(Argv,2);
  // create the local variables;
  EvaKernel->newGrammarModule();
  Object variables = Argv(1);
  if ( not (variables).ListQ( SYMBOL_ID_OF_List ) )
    ThrowError("Module","Local variable specification should a Object ."); 
  for (Object::iterator iter = variables.Begin(); iter!=variables.End(); iter++){
    if ( (*iter).SymbolQ() ){
      Object rvalue; rvalue.SetObject(*iter);
      //rvalue.KeyPrepend( "$" );
      EvaKernel->InstantInsertOrUpdateValue(*iter,rvalue);
      continue;
    }
    if ( (*iter).ListQ( SYMBOL_ID_OF_Set ) ){
      EvaKernel->InstantInsertOrUpdateValue( (*iter)[1],(*iter)[2] );
      continue;
    }
    if ( (*iter).ListQ( SYMBOL_ID_OF_SetDelayed ) ){
       EvaKernel->Evaluate( (*iter)[2]);
       EvaKernel->InstantInsertOrUpdateValue( (*iter)[1],(*iter)[2]);
       continue;
    }
    ThrowError("Module","Local variable specification "+variables.ToString()+" contains "+(*iter).ToString()+" which is not a atom symbol or an assignment to a symbol.");
  }
  //evaluate the returned Argv
  for (Object::iterator iter = Argv.Begin()+1; iter!=Argv.End(); iter++){
    EvaKernel->Evaluate( *iter);
  }
  EvaKernel->deleteGrammarModule();
  Argv.SetTo( Argv.Size() );
  ReturnNormal;
}

int PureFunctionApply(Object&p_expr,Object&Argv){
  if ( p_expr.NullQ() ) return 0;
  switch (p_expr.type() ){
  case EvaType::Number: return 0;
  case EvaType::String: return 0;
  case EvaType::Symbol:{
    if ( p_expr.ids() == SYMBOL_ID_OF___Variable ){
      u_int id = p_expr.re();
      if ( id == 0 or id > Argv.Size() )
        ThrowError("PureFunction","PureFunction variable id exceed the number of arguments.");
      p_expr=Argv[id];
    }
    return 0;
  }
  case EvaType::List:{
    for (auto iter = p_expr.ElementsBegin(); iter != p_expr.ElementsEnd(); iter++){
      PureFunctionApply(*iter,Argv);
    }
  }
  }
  return 0;
}

int SystemModule::PureFunction(Object &Argv){
  CheckShouldNoLessThan(1);
  CheckArgsShouldEqual(Argv[0],1);
  Object expr = Argv[0][1];
  PureFunctionApply(expr,Argv);
  Argv = expr;
  EvaKernel->Evaluate( Argv );
  ReturnNormal;
}


int SystemModule::Function(Object &Argv){
  CheckShouldNoLessThan(1);
  Object&func = Argv[0];
  CheckArgsShouldBeWithin(func,1,2);
  if ( func.Size() == 1 ){
    return PureFunction(Argv);
  }else{
    Object &varlist=func[1];
    if ( varlist.SymbolQ() ){
      if ( Argv.Size() != 1 )
        ThrowError("Function","Function variable list is different from arguments applied to.");
      EvaKernel->newGrammarModule();
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(varlist,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Function","Can not create innner variable.");
      Object&vobj = pairobj[2];
      vobj = Argv[1];
      Object res = func[2];
      EvaKernel->Evaluate( res );
      Argv = res;
      EvaKernel->deleteGrammarModule();
      ReturnNormal;
    }else if ( varlist.ListQ_List() ){
      if ( Argv.Size() != varlist.Size() )
        ThrowError("Function","Function variable list is different from arguments applied to.");
      EvaKernel->newGrammarModule();
      for(u_int i = 1; i <= varlist.Size() ; i++){
        if ( not varlist[i].SymbolQ() ) 
          ThrowError("Function","First Arguments is required to be a Symbol or a List of Symbols.");
        Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(varlist[i],false,false,true);
        if ( pairobj.NullQ() ) ThrowError("Function","Can not create innner variable.");
        Object&vobj = pairobj[2];
        vobj = Argv[i];
      }
      Object res = func[2];
      EvaKernel->Evaluate(res);
      Argv = res;
      EvaKernel->deleteGrammarModule();
      ReturnNormal;
    }else
      ThrowError("Function","First Arguments is required to be a Symbol or a List of Symbols.");
  }

  ReturnNormal;
}

int SystemModule::Conjunct(Object &Argv){
  CheckShouldEqual(2);
  //Dealing 1st argument evaluate until to a conjunctable expr
  Object&fobj = Argv[1];
  EvaKernel->Evaluate( fobj, false, true );
  if ( not fobj.SimpleListQ() ){
    ReturnHold;
  }
  if ( not fobj[0].ValuedSymbolQ() ){
    ReturnHold;
  }
  Object pairlist( fobj[0].idx() );
  int recId = pairlist.ids();
  EvaRecord *rec = EvaKernel->evaRecordTable[ recId ];
  if ( rec != NULL ){
    if ( AttributeQ(rec->attributes, AttributeType::Conjunctable ) ){
      if ( rec->staticFunction ){
        return( (rec->staticFunction)(Argv)  ) ;
      }else if ( rec->module ){
        return( ((rec->module)->*(rec->memberFunction))(Argv) );
      }
      ReturnNormal;
    }
    ReturnHold;
  }
  ReturnHold;
}

int SystemModule::If(Object &Argv){
  ReturnIfNotWithin(2,3);
  EvaKernel->Evaluate(Argv[1]);
  char res = Argv[1].Boolean();
  if (  res > 0 ){
    EvaKernel->Evaluate(Argv[2]);
    Argv.SetTo(2);
  }else if (  res == 0 ){
    if ( Argv.Size() == 3){
      EvaKernel->Evaluate(Argv[3]);
      Argv.SetTo(3);
    }else{// return Null
      Argv.SetNull();
    }
  }
  // condition result not known Boolean() == -1
  //dprintf("If returned with hold");
  ReturnHold;
}
int SystemModule::While(Object &Argv){
  ReturnIfNotEqual(2);
  Object&condition = Argv(1);
  Object&expression = Argv(2);
  EvaKernel->PreEvaluate( condition );
  EvaKernel->PreEvaluate( expression );
  Object cond,expr;
  for(;;){
    cond = NullObject;
    cond.SetObject(condition);
    EvaKernel->Evaluate(cond);
    if ( cond.Boolean() > 0 ){ //evaluate expr
      expr = NullObject;
      expr.SetObject(expression);
      EvaKernel->Evaluate( expr);
      continue;
    }
    break;
  }
  ReturnNull;
}

int SystemModule::For(Object &Argv){
  ReturnIfNotEqual(4);
  Object&start= Argv(1);
  EvaKernel->Evaluate( start);
  Object&test= Argv(2);
  Object&incr= Argv(3);
  Object&body= Argv(4);
  EvaKernel->PreEvaluate( test );
  EvaKernel->PreEvaluate( incr );
  EvaKernel->PreEvaluate( body );
  Object ltest,lincr,lbody;
  for(;;){
    ltest = NullObject;
    ltest.SetObject(test);
    EvaKernel->Evaluate( ltest);
    if ( ltest.Boolean() > 0 ){ //evaluate expr
      lbody = NullObject;
      lbody.SetObject(body);
      EvaKernel->Evaluate( lbody);
      lincr = NullObject;
      lincr.SetObject(incr);
      EvaKernel->Evaluate(lincr);
      continue;
    }
    break;
  }
  ReturnNull;
}

int SystemModule::Foreach(Object&Argv){
  ReturnIfNotEqual(3);
  Object&vars  = Argv[1];
  EvaKernel->Evaluate( Argv[2] );
  Object&lists = Argv[2];
  Object&expr  = Argv[3];
  if ( not lists.ListQ_List() )
    ThrowError("Foreach","Second argument is required to be a List.");
  EvaKernel->newGrammarModule();
  if ( vars.SymbolQ() ){
    Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(vars,false,false,true);
    if ( pairobj.NullQ() ) ThrowError("Do","Can not create innner variable.");
    Object&vobj = pairobj[2];
    Object newexpr;
    EvaKernel->PreEvaluate( expr );
    for (auto iter= lists.Begin(); iter != lists.End(); iter++){
      vobj.SetObject( *iter );
      newexpr = NullObject;
      newexpr.SetObject( expr );
      EvaKernel->Evaluate( newexpr );
    }
  }else if ( vars.ListQ() ){
    Object vobj_list; vobj_list.SetList_List();
    for ( auto iter = vars.Begin(); iter != vars.End(); iter ++ ){
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(*iter,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Do","Can not create innner variable.");
      vobj_list.PushBackRef( pairobj[2] );
    }
    Object newexpr;
    for ( auto iter = lists.Begin(); iter != lists.End(); iter++ ){
      if ( iter->Size() != vobj_list.Size() )
        ThrowError("Foreach","shape","List shape is not consistent with variable list.");
      for ( auto viter=vobj_list.Begin(),liter=(*iter).Begin();viter!=vobj_list.End();viter ++,liter++){
        (*viter).SetObject( *liter );
      }
      newexpr = NullObject;
      newexpr.SetObject( expr );
      EvaKernel->Evaluate( newexpr );
    }
  }else
    ThrowError("Foreach","Iterator should be a Symbol or a List of Symbols.");
  EvaKernel->deleteGrammarModule();
  ReturnNull;
  return 0;
}

int SystemModule::Do(Object &Argv){
  ReturnIfLessThan(2);
  while ( Argv.Size() >2 ){
    Object newtable; newtable.SetList( SYMBOL_ID_OF_Table ); 
    newtable.PushBackRef( Argv(1) );
    newtable.PushBackRef( Argv(2) );
    Argv.Delete(2);
    Argv(1) = newtable;
  }
  Object&expr = Argv(1);
  Object&iter = Argv(2);
  if ( not (iter).ListQ( SYMBOL_ID_OF_List ) || iter.Size()<1 || iter.Size()>4)
    ThrowError("Do", (iter).ToString()+" is not a valid iterator Argv form.");
  //iteratorToList(iter,newlist);
  if ( iter.Size() == 1 ){
    Object&num = (iter)(1);
    EvaKernel->Evaluate(num);
    if ( num.NumberQ() ){
      int N = num.Number() ;
      Object newexpr; 
      EvaKernel->PreEvaluate( expr );
      for ( int i = 1; i<=N; i++ ){
        newexpr = NullObject;
        newexpr.SetObject(expr );
        EvaKernel->Evaluate(newexpr);
      }
      Argv.SetNull();
      ReturnNormal ;
    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  if ( iter.Size() == 2 ){
    Object&var = (iter)(1);
    if ( !var.SymbolQ() ) ThrowError("Do",var.ToString()+" cannot be used as an iterator.");
    Object&num = (iter)(2);
    EvaKernel->Evaluate(num);
    EvaKernel->newGrammarModule();
    Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(var,false,false,true);
    if ( pairobj.NullQ() ) ThrowError("Do","Can not create innner variable.");
    Object&vobj = pairobj[2];
    if ( num.NumberQ() ){
      int N =  num.Number();
      Object newexpr;
      EvaKernel->PreEvaluate( expr );
      for ( int i = 1; i<=N; i++ ){
        newexpr = NullObject;
        newexpr.SetObject( expr );
        vobj.SetNumber(i);
        EvaKernel->Evaluate(newexpr);
      }
      EvaKernel->deleteGrammarModule();
      ReturnNull;
    }else if ( (num).ListQ( SYMBOL_ID_OF_List ) ){
      Object newexpr;
      EvaKernel->PreEvaluate( expr );
      for ( u_int i = 1; i<=num.Size(); i++ ){
        newexpr = NullObject;
        newexpr.SetObject( expr );
        vobj =  num[i];
        EvaKernel->Evaluate(newexpr);
      }
      EvaKernel->deleteGrammarModule();
      ReturnNull;
    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  if ( iter.Size() == 3 ){
    Object&var = (iter)(1);
    if ( !var.SymbolQ() ) ThrowError("Do",var.ToString()+" cannot be used as an iterator.");
    Object&num1 = (iter)(2);
    Object&num2 = (iter)(3);
    EvaKernel->Evaluate(num1);
    EvaKernel->Evaluate(num2);
    if ( num1.NumberQ() && num2.NumberQ() ){
      int Nbegin =  num1.Number();
      int Nend =  num2.Number();
      Object newexpr;
      EvaKernel->newGrammarModule();
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(var,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Do","Can not create innner variable.");
      Object&vobj = pairobj[2];
      EvaKernel->PreEvaluate( expr );
      for ( int i = Nbegin; i<=Nend; i++ ){
        newexpr = NullObject;
        newexpr.SetObject(expr);
        vobj.SetNumber(i);
        EvaKernel->Evaluate(newexpr);
      }
      EvaKernel->deleteGrammarModule();
      ReturnNull;
    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  if ( iter.Size() == 4 ){
    Object var = (iter)(1);
    if ( !var.SymbolQ() ) ThrowError(Argv.Key(),var.ToString()+" cannot be used as an iterator.");
    Object num1 = (iter)(2);
    Object num2 = (iter)(3);
    Object incr = (iter)(4);
    EvaKernel->Evaluate(num1);
    EvaKernel->Evaluate(num2);
    EvaKernel->Evaluate(incr);
    if ( num1.NumberQ() && num2.NumberQ() && incr.NumberQ() ){
      double Nbegin = num1.Number( );
      double Nend = num2.Number( );
      double Nincr = incr.Number( );
      if ( Nincr == 0 ) ThrowError("Do","Increment should not be a zero.");
      int steps = (Nend-Nbegin)/Nincr;
      Object newexpr;
      EvaKernel->newGrammarModule();
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(var,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Do","Can not create innner variable.");
      Object&vobj = pairobj[2];
      EvaKernel->PreEvaluate( expr );
      for ( int i = 0 ;i<=steps;i++){
        newexpr = NullObject;
        newexpr.SetObject(expr);
        vobj.SetNumber(Nbegin+i*Nincr);
        EvaKernel->Evaluate(newexpr);
      }
      EvaKernel->deleteGrammarModule();
      ReturnNull;
    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  ReturnHold;
}

int SystemModule::ToExpression(Object & Argv){
  string s;
  if ( not Argv.StringQ() ){
    CheckArgsShouldEqual(Argv,1);
    EvaKernel->Evaluate(Argv[1]);
    CheckArgsShouldBeString(Argv,1);
    s = Argv[1].Key();
  }else{
    s = Argv.Key();
  }
  Object tobj = ImportList::ToExpression(s);
  if ( tobj.NullObjectQ() )
    ThrowError("ToExpression","Ivalid string to transform to Evawiz expression.");
  Argv = tobj;
  EvaKernel->Evaluate(Argv);
  ReturnNormal;
}

int system_with_print(string cmd, vector<string>&result){
  result.clear();
  FILE *fp;
  char line[1024];
  fp = popen(cmd.c_str(),"r");
  if ( fp == NULL ){
    cerr<<"  Error: "<<cmd<<" cannot be excuted"<<endl;
    return -1;
  }
  while ( fgets(line,sizeof(line),fp) != NULL ){
    if ( line[ strlen(line)-1 ] == '\n' ){
      line[ strlen(line)-1 ] = '\0';
    }
    result.push_back(line);
  }
  pclose(fp);
  return result.size();
}

int SystemModule::System(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  EvaKernel->Evaluate( Argv[1] );
  if ( !Argv[1].StringQ() ) ThrowError(Argv.Key(),"First argument should be a string.");
  vector<string> strs;
  if ( system_with_print( Argv[1].Key(),strs ) <0 )
    ThrowError(Argv.Key(),"cmd "+Argv[1].ToString()+" execute error.");
  Argv.SetList_List();
  for ( int i=0; i<(int)strs.size(); i++){
    Argv.PushBackString( strs[i].c_str() );
  }
  ReturnNormal;
}

int SystemModule::SystemWithoutOutput(Object &Argv)
{
  CheckArgsShouldEqual(Argv,1);
  EvaKernel->Evaluate( Argv[1] );
  if ( !Argv[1].StringQ() ) ThrowError(Argv.Key(),"First argument should be a string.");
  if ( system( Argv[1].Key() ) <0 )
    ThrowError(Argv.Key(),"cmd "+Argv[1].ToString()+" execute error.");
  ReturnNull;
}

const u_char Set_Temp_State = 1;
int SystemModule::Set( Object&Argv){
  //if not symbol then evaluate its head such as a[x],,, a == Dict or Matrix or List or something
  CheckShouldEqual(2);
  Object&left = Argv[1];
  Object&right = Argv[2];
  if ( Argv.state() != Set_Temp_State ){
    EvaKernel->Evaluate( right );
  }
  if ( left.ListQ() ){
    EvaKernel->Evaluate(left[0],false,true);
  }
  if ( left.ListQ_List() ){
    if ( right.ListQ_List() ){
      if ( left.Size() != right.Size() ) ThrowError("Set","Left List size should match size of right List.");
      for ( u_int i = 1; i <= left.Size(); i++ ){
        Object tSet; tSet.SetList( SYMBOL_ID_OF_Set );
        tSet.set_state( 1 );
        tSet.PushBackRef( left[i] ); tSet.PushBackRef( right[i] );
        Set( tSet );
      }
    }else{
      for ( u_int i = 1; i <= left.Size(); i++ ){
        Object tSet; tSet.SetList( SYMBOL_ID_OF_Set );
        tSet.PushBackRef( left[i] ); tSet.PushBackRef( right );
        Set( tSet );
      }
    }
    if ( Argv.state() != Set_Temp_State ){
      Argv.SetTo(2);
    }
    return 1;
  }
  //normal
  if ( left.NumberQ() or left.StringQ()  ){
    ThrowError("Set","Left value of set is not allowed to be a Number or String.");
  }
  //check if the left is a Setable or DelaySetable List
  if ( left.ListQ() ){
    //Conjunct
    if ( left[0].SymbolQ( SYMBOL_ID_OF_Conjunct ) ){ // Conjunct
      EvaKernel->Evaluate( left );
      if ( not left.ListQ( SYMBOL_ID_OF_Conjunct ) ){
        left.SetObject( right );
        if ( Argv.state() != Set_Temp_State )
          Argv.SetTo(2);
        ReturnNormal;
      }
    }else{
      //Setable
      if ( left[0].SymbolQ() and left[0].idx() != Index0 ){
        Object refpair( left[0].idx() );
        if ( refpair.ids() != 0 ){//check whether Setable
          EvaRecord * rec = EvaKernel->evaRecordTable[ refpair.ids() ];
          if ( rec != NULL and AttributeQ( rec->attributes , AttributeType::Setable ) ){
            if ( rec->staticFunction ){
              return  ( rec->staticFunction )( Argv );
            }else if ( rec->module ){
              return ( ((rec->module)->*(rec->memberFunction))(Argv) );
            }
          }
        }
      }
      //DelaySetable
      if ( left[0].SimpleListQ() and left[0][0].idx() != Index0 ){
        Object refpair( left[0][0].idx() );
        if ( refpair.ids() != 0 ){//check whether Setable
          EvaRecord * rec = EvaKernel->evaRecordTable[ refpair.ids() ];
          if ( rec!=NULL and AttributeQ(rec->attributes ,AttributeType::DelaySetable) ){
            if ( rec->staticFunction ){
              return ( (rec->staticFunction)( Argv ) );
            }else if ( rec->module ){
              return ( ((rec->module)->*(rec->memberFunction))(Argv) );
            }
          }
        }
      }
    }
  }
  //get left
  //check whether pattern
  Object res = EvaKernel->GetOrNewValuePairRefAtTop(left); // the List pair
  if ( not res.NullQ() ){//is not a pattern
    if ( res.ids() != 0 ){//check whether protected
      if (  EvaKernel->AttributeQ(res.ids() ,AttributeType::Protected) ){
        ThrowError("Set",(left).ToString() +(string)" is Protected.");
      }
    }
    res[2].SetObject(right);
  }else{//is a pattern
    EvaKernel->UpdateOrInsertValueAtTop(left,right,false,true,true);
  }
  if ( Argv.state() != Set_Temp_State )
    Argv.SetTo(2);
  return 1;
}

/*
int SystemModule::Set(Object & Argv){
  CheckShouldEqual(2);
  EvaKernel->Evaluate( Argv[2] );
  Set(Argv[1],Argv[2],Eva);
  Argv.SetTo(2);
  ReturnNormal;
}
*/

int SystemModule::SetDelayed(Object & Argv){
  CheckShouldEqual(2);
  Object&left = Argv(1);  
  Object&right = Argv(2);
  if ( left.ValuedSymbolQ() ){
    Object( left.idx() )[2].SetObject( right );
    ReturnNull;
  }
  CheckShouldNotBeNumber(1);
  CheckShouldNotBeString(1);
  //get left
  //check whether pattern
  if ( not Pattern::PatternListQ(left ) ){
    Object res = EvaKernel->GetOrNewValuePairRefAtTop(left,false,true,true);
    if ( res.NullQ() )
      ThrowError("Set","Can not get or new value from Kernel.");
    if ( res.ids() != 0 ){//check whether protected
      if (  EvaKernel->AttributeQ( res.ids(),AttributeType::Protected) ){
        ThrowError("Set",(left).ToString() +(string)" is Protected.");
      }
    }
    res[2].SetObject(right);
    ReturnNull;
  }else{//is a pattern
    EvaKernel->UpdateOrInsertValueAtTop(left,right,false,true,true);
    ReturnNull;
  }
}

int SystemModule::GetModule(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  string modulename = Argv[1].Key();
  EvaKernel->GetModule(modulename);
  ReturnNull;
};

int SystemModule::RemoveModule(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  if ( EvaKernel->RemoveModule(Argv[1].Key()) < 0 ) ReturnError;
  ReturnNull;
}



int SystemModule::Out(Object &Argv){
  if ( EvaKernel->superList.NullQ() )
    ThrowError("Out","Out is not avaliable in this evaluation.");
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeNumber(1);
  int id = (int)Argv[1];
  if ( id >= (int)EvaKernel->superList.Size() || id<1 )
    ThrowError("Out",Math::OrderForm(id)+" output is out of range.");
  Argv = EvaKernel->superList.ElementAt(id);
  EvaKernel->Evaluate(Argv);
  ReturnNormal;
}

int SystemModule::In(Object &Argv){
  if ( EvaKernel->inputList.NullQ() )
    ThrowError("In","In is not avaliable in this particular evaluation.");
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeNumber(1);
  int id =(double) Argv[1];
  if ( id >= (int)EvaKernel->superList.Size() || id<1 )
    ThrowError("In",Math::OrderForm(id)+" input is out of range.");

  Argv = EvaKernel->inputList.ElementAt(id);
  EvaKernel->Evaluate(Argv);
  ReturnNormal;
}

int SystemModule::ShowModuleFunctions(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  ReturnNormal;
}

int SystemModule::ShowModules(Object &Argv){
  CheckArgsShouldEqual(Argv,0);
  ReturnNormal;
}

int SystemModule::SetOutputFormat(Object &Argv){
  CheckShouldBeWithin(1,2);
  CheckShouldBeNumber(1);
  if ( Argv.Size() == 2){
    CheckShouldBeNumber(2);
    EvaSettings::SetFormat((double)Argv[1],(double)Argv[2]);
  } else {
    EvaSettings::SetFormat((double)Argv[1]);
  }
  ReturnNull;
}

int SystemModule::SystemArgv(Object &Argv)
{
  CheckShouldNoMoreThan(1);
  if ( Argv.Size() == 0 ){
    EvaKernel->argc;
    Argv.SetList_List();
    for (int i=0; i<EvaKernel->argc; i++ ){
      Argv.PushBackString( EvaKernel->argv[i] );
    }
    ReturnNormal;
  }
  CheckShouldBeNumber(1);
  int p = (int)Argv[1];
  if ( p < 0 ) ThrowError("SystemArgv","Input should be a non-negative Number.");
  if ( p >= EvaKernel->argc ) ReturnString("");
  ReturnString( EvaKernel->argv[p] );
}

int SystemModule::SetRunningMode(Object&Argv)
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  if ( strcasecmp( Argv[1].Key(), "gpu" ) == 0 ){
    EvaSettings::SetRunningMode( RunningModeGpu );
  }else if ( strcasecmp( Argv[1].Key(), "cpu" ) == 0 ){
    EvaSettings::SetRunningMode( RunningModeCpu );
  }else{
    ThrowError("SetRunningMode","Running mode "+Argv[1].ToString()+" is not recongnized.");
  }
  ReturnNull;
}

int SystemModule::GetRunningMode(Object&Argv)
{
  CheckShouldEqual(0);
  if ( EvaSettings::runningMode == RunningModeGpu ){
    ReturnString("Gpu");
  }
  ReturnString("Cpu");
}

int SystemModule::GPUDeviceQ(Object&Argv)
{
  if ( Execute("nvidia-smi -L >/dev/null 2>&1") != 0 ){
    ReturnString("No nVIDIA GPU Driver found.");
  }
  vector<string> res;
  evawiz::System("nvidia-smi -L",res) ;
  Argv.SetList_List();
  for ( auto li: res){
    Argv.PushBackString( li);
  }
  ReturnNormal;
}

int SystemModule::SetCudaThreadsNumberPerBlock(Object&Argv)
{
  CheckShouldEqual(1);
  CheckShouldBeNumber(1);
  int num = (int)Argv[1];
  if ( num <= 0 and num%32 != 0 and num > 1024 )
    ThrowError("SetCudaThreadsNumberPerBlock","Thread number per block is suggested to be multiple of 32 and a number with 0 and 1024.");
  EvaSettings::cudaThreadsNumberPerBlock = num;
  ReturnNull;
}

int SystemModule::SetEvawizCpuKernelThreadsNumber(Object&Argv)
{
  CheckShouldEqual(1);
  CheckShouldBeNumber(1);
  int num = (int)Argv[1];
  if ( num <= 0 )
    ThrowError("SetEvawizCpuKernelThreadsNumber","Thread number should be a positive number.");
  EvaSettings::SetEvawizCpuKernelThreadsNumber( num );
  ReturnNull;
}

int SystemModule::GetCudaThreadsNumberPerBlock(Object&Argv)
{
  CheckShouldEqual(0);
  ReturnNumber( EvaSettings::cudaThreadsNumberPerBlock );
}

int SystemModule::GetEvawizCpuKernelThreadsNumber(Object&Argv)
{
  CheckShouldEqual(0);
  ReturnNumber( EvaSettings::evawizCpuKernelThreadsNumber );
}

int SystemModule::ShowHelpInfo(Object &Argv){
  CheckShouldEqual(1);
  EvaRecord*ev = EvaKernel->GetEvaRecord(Argv(1));
  if ( ev == NULL ){
    cout<<"No help info found for "<<Argv[1].ToString()<<endl;
  }else{
    cout<<ev->description<<endl;
  }
  ReturnNull;
}

int SystemModule::Clear(Object&Argv){
  if ( Argv.Size() == 0 ) ReturnNull;
  for ( u_int i=1;i<=Argv.Size();i++){
    if ( not Argv[i].SymbolQ() )
      ThrowError("Clear","Arguments of Clear is required to be Symbol.");
    if ( EvaKernel->Clear(Argv[i]) < 0 )
      ThrowError("Clear",(string)Argv[i].Key()+" is Protected. Clear can not applied. Use UnProtect to unlock.");
  }
  ReturnNull;
}

int SystemModule::AbsoluteTime(Object &Argv){
  struct timeval tvs;
  gettimeofday(&tvs,NULL);
  Argv.SetNumber( tvs.tv_sec+tvs.tv_usec/1000000.0);
  ReturnNormal;
}

int SystemModule::CpuTiming(Object &Argv){
  ReturnIfNotEqual(1);
  clock_t start,finish;
  start = clock();
  EvaKernel->Evaluate(Argv[1]);
  finish = clock();
  Argv[0].SetSymbol( SYMBOL_ID_OF_List );
  Argv.InsertNumber( 1,( (1.0*finish-start)/CLOCKS_PER_SEC ) );
  ReturnNormal;
}

int SystemModule::Timing(Object &Argv){
  ReturnIfNotEqual(1);
  struct timeval start,finish;
  gettimeofday( &start,NULL );
  EvaKernel->Evaluate(Argv[1]);
  gettimeofday( &finish,NULL );
  Argv[0].SetSymbol( SYMBOL_ID_OF_List );
  Argv.InsertNumber( 1,finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec)/1000000.0 );
  ReturnNormal;
}

int SystemModule::DateList(Object &Argv){
  Argv.SetList_List();
  ReturnNormal;
}

int SystemModule::Sleep(Object&Argv){
  if ( Argv.NumberQ() ){
    sleep( Argv.Number() );
  }
  CheckShouldEqual(1);
  CheckShouldBeNumber(1);
  sleep(Argv[1].Number());
  ReturnNull;
}
