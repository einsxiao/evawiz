#include<evawiz.h>
#include"AlgebraicModule.h"
#include"eva_oldmacro.h"
using namespace evawiz;

#undef ModuleNameStr
#define ModuleNameStr "Algebraic"
__CREATE_MODULE__(Algebraic)

__DEBUG_MAIN__("input.ev")

AlgebraicModule::AlgebraicModule(__CONSTRUCTION_PARA__){
  RegisterFunction("SimpleSimplify",(MemberFunction)(&AlgebraicModule::SimpleSimplify) );
  //////////////////////// Flat rules
  //Plus
  SSRules.SetList_List(); 
  //dout<<"Initialize SSRules"<<endl;
#define add_rule(rule_str) {                    \
    Object obj;                                 \
    obj =  ImportList::ToExpression(rule_str);  \
    Pattern::UnifyRule( obj );                  \
    SSRules.PushBackRef( obj );                 \
  }
  /////////////
  add_rule("HoldPattern[Plus[a_]:>a]");
  add_rule("HoldPattern[Plus[a___,Null,b___]:>Plus[a,b]]");
  add_rule("HoldPattern[Plus[a___,x_,x_,b___]:>Plus[a,Times[2,x],b]]");
  add_rule("HoldPattern[Plus[a___,x_,Times[i_Number,x_],b___]:>Plus[a,Times[Plus[1,i],x],b]]");
  add_rule("HoldPattern[Plus[a___,Times[x__],Times[i_Number,x__],b___]:>Plus[a,Times[Plus[1,i],x],b]]");
  add_rule("HoldPattern[Plus[a___,Times[i_Number,x__],Times[j_Number,x__],b___]:>Plus[a,Times[Plus[i,j],x],b]]");
  //Times Power
  //add_rule("HoldPattern[ Times[0,a_]:>0 ]");
  add_rule("HoldPattern[ Times[a_]:>a ]");
  add_rule("HoldPattern[ Times[a___,,b___]:>Times[a,b] ]");
  add_rule("HoldPattern[ Times[x_,x_]:>Power[x,2] ]");
  add_rule("HoldPattern[ Times[a___,x_,x_,b___]:>Times[a,Power[x,2],b] ]");
  add_rule("HoldPattern[ Times[a___,x_,Power[x_,i_Number],b___]:>Times[a,Power[x,Plus[1,i]],b] ]");
  add_rule("HoldPattern[ Times[a___,Power[x_,i_Number],Power[x_,j_Number],b___]:>Times[a,Power[x,Plus[i,j]],b] ]");
  ////////////////////////
  //////////////////////// normal rules
  add_rule("HoldPattern[ Power[x_,0]:>1 ]");
  add_rule("HoldPattern[ Power[x_,1]:>x ]");
  add_rule("HoldPattern[ Power[Power[x_,a_],b_]:>Power[x,Times[a,b]] ]");
  add_rule("HoldPattern[ Log[Power[x_,m_]]:>Times[m,Log[a]] ]");
#undef add_rule
  //dprintf("ssrules = %s",SSRules.ToString().c_str() );
}

AlgebraicModule::~AlgebraicModule(){
};

int AlgebraicModule::SimpleSimplify(Object&Argv){
  //return 0;
  if ( not Argv.ListQ() )return 0;
  if ( Argv.ListQ( SYMBOL_ID_OF_SimpleSimplify ) ){
    CheckArgsShouldEqual(Argv,1);
    Argv = Argv[1];
  }
  bool replaced = true;
  while (replaced){
    replaced = false;
    //dprintf("try simplify %s",Argv.ToString().c_str() );
    for ( u_int i= 1 ;i <= SSRules.Size() ; i++){
      //dprintf("simplify %s with %s",Argv.ToString().c_str(),SSRules[i].ToString().c_str() );
      if ( Pattern::UnifiedReplaceAll( Argv , SSRules[i] ,true) )
        replaced = true;
      //dprintf("-> %s",Argv.ToString().c_str() );
    }
    if ( replaced ){
      EvaKernel->Evaluate( Argv );
    }
  }
  return 0;
}






