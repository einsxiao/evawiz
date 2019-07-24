#include<evawiz.h>
#include"ClassModule.h"
using namespace std;
using namespace evawiz;
#define ModuleName Class
__DEBUG_MAIN__("input.ev");

class __EvaClassTable:public __EvaTable< pair<ValueTable,int> >{
public:
  __EvaClassTable():__EvaTable(1024){
    tableName = "ClassTable";
  };
};

static __EvaClassTable ClassPool;


ModuleInitialize{
  {
    AddAttribute("Class",AttributeType::HoldAll);
    AddAttribute("Class",AttributeType::Conjunctable);  // For Member ref or call Member function
    //AddAttribute("Class",AttributeType::DelayFunction); // Class[*][*]
    //AddAttribute("Class",AttributeType::DelaySetable);  // Class[*][*] = 
    Object temp; temp.SetSymbol(SYMBOL_ID_OF_Class);
    valuePairIndexOfClass = EvaKernel->GetValuePairRef( temp ).objid;
  }
  {
    AddAttribute("DefClass",AttributeType::HoldAll);
  }
  //classes.push_back( NULL );
}

ModuleFinalize{

}

DefineFunction(Class){//Argv.Size() Argv[1]
  Conjunct_Context(Class){ // Class[$$_*].* 
    Object&claobj = Argv[1];
    CheckArgsShouldNoLessThan(claobj,1);
    if ( not claobj[1].SymbolQ(SYMBOL_ID_OF_SerialCode ) )
      ThrowError("Class","Only an installation of Class can access their members.");
    pair< ValueTable,int>&rec = ClassPool.Get( claobj[1].idx() );
    if ( rec.second <= 0 )
      ThrowError("Class","Conjunct","Class is not available.");
    ValueTable&cla = rec.first;
    ////check second 1
    Object &oper= Argv[2];
    if ( oper.NumberQ() or oper.StringQ() )
      ThrowError("Class","Conjunct","A member of a class can only be a Symbol or a Functio.");
    ////////////////////////////////////////
    ////////////////////////////////////////
    Conjunct_Case( New ){
      Index sncode = ClassPool.New();
      pair< ValueTable,int>&rec = ClassPool.Get(sncode); rec.second = 1;
      ValueTable&newcla = rec.first;
      newcla = cla;
      oper[0].SetSymbol( SYMBOL_ID_OF_Init );
      if ( newcla.GetValue( oper,oper ) <= 0 ){
        if ( oper.Size()  > 0 ){
          ThrowError("Class","No construction function matches ",oper.ToString()+".");
        }
      }else{
        EvaKernel->newGrammarModule( &newcla );
        EvaKernel->Evaluate( oper );
        EvaKernel->deleteGrammarModule();
      }
      Object sn;
      sn.SetSymbol( SYMBOL_ID_OF_SerialCode );
      sn.set_idx( sncode ); 
      Argv.SetList( SYMBOL_ID_OF_Class );
      Argv.idx(); 
      Argv.PushBack( sn );
      ReturnNormal;
    }
    Conjunct_Case( Delete ){
      rec.second --;
      if ( rec.second == 0 ){
        ClassPool.Free( claobj[1].idx() );
      }
      ReturnNull;
    }
    ////////////////////////////////////////

    /////normal situation
    if ( oper.SymbolQ() ){
      Object res = cla.GetOrNewValuePairRef(oper,false,true);
      if ( res.NullQ() )
        ThrowError("Class","Can not new member variable in a Class installation.");
      Argv.SetObjectRef( res[2] );
    }else{
      EvaKernel->EvaluateAll( oper );
      Object res;
      cla.GetValue( oper, res );
      EvaKernel->newGrammarModule(&cla);
      EvaKernel->Evaluate( res );
      EvaKernel->deleteGrammarModule();
      Argv.SetObjectRef( res );
    }
    ReturnNormal;
  }
  ReturnHold;
}

DefineFunction( DefClass,"Define a Class." ){
  Index sncode = ClassPool.New();
  pair< ValueTable,int>&rec = ClassPool.Get(sncode); rec.second = 1;
  ValueTable&cla = rec.first;

  EvaKernel->newGrammarModule( &cla );
  for (auto iter = Argv.Begin(); iter != Argv.End(); iter++ ){
    if ( (*iter).NumberQ() or (*iter).StringQ() )
      continue;
    if ( (*iter).SymbolQ() ){
      cla.GetOrNewValuePairRef( (*iter),false,true );
      continue;
    }
    if ( (*iter).ListQ(SYMBOL_ID_OF_Set) ){
      CheckArgsShouldEqual( (*iter) ,2 );
      EvaKernel->Evaluate( (*iter)[2] );
      cla.UpdateOrInsert( (*iter)[1], (*iter)[2] );
      continue;
    }
    if ( (*iter).ListQ( SYMBOL_ID_OF_SetDelayed) ){
      CheckArgsShouldEqual( (*iter) ,2 );
      cla.UpdateOrInsert( (*iter)[1], (*iter)[2] );
      continue;
    }
    EvaKernel->Evaluate( *iter );
  }
  EvaKernel->deleteGrammarModule();
  Object sn;
  sn.SetSymbol( SYMBOL_ID_OF_SerialCode );
  sn.set_idx( sncode ); 
  Argv.SetList( SYMBOL_ID_OF_Class );
  Argv[0].set_idx( valuePairIndexOfClass );
  Argv.PushBack( sn );
  ReturnNormal;
}
