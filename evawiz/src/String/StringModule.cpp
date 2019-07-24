#include"evawiz.h"
#include"StringModule.h"
using namespace std;
using namespace evawiz;
#define ModuleName String 
__DEBUG_MAIN__("input.ev");
ModuleInitialize
{

}

ModuleFinalize
{

}

DefineFunction(StringSplit,"Split string into pieces by splitor specified.")
{
  CheckShouldBeWithin(1,2);
  CheckShouldBeString(1);
  string str = Argv[1].Key();
  string split;
  if (Argv.Size()>1){
    CheckShouldBeString(2);
    split = Argv[2].Key();
  }else
    split = " ";
  vector<string>result;
  evawiz::StringSplit(str,result,split);
  Argv.SetList_List();
  for (u_int i = 0; i< result.size(); i++)
    Argv.PushBackString( result[i].c_str() );
  return 0;
}

DefineFunction(RandomString,"Return a random string of given length.")
{
  CheckShouldEqual(1);
  CheckShouldBeNumber(1);
  int len = Argv[1].Number();
  ReturnString( evawiz::RandomString(len) );
}

DefineFunction(StringJoin,"Join Strings or String of Expressions into one Single String.")
{
  CheckShouldNoLessThan(1);
  string content;
  for (size_t i=1; i<= Argv.Size(); i++ ){
    if ( Argv[i].StringQ() )
      content += Argv[i].Key();
    else
      content+=Argv[i].ToString();
  }
  Argv.SetString(content);
  ReturnNormal;
}



