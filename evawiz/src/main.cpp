/*
  Author: Xiao, Hu
  Content: Evawiz main function
  Edition: X
  CopyRight: CDE
*/
#include<evawiz.h>
#include"unistd.h"

using namespace std;
using namespace evawiz;
int main(int argc,char* argv[])
{ 
  int res = 0;
  try{
    if ( argc == 1 ){
      if ( isatty( 0 ) ){
        cout<<"Evawiz X for Linux."<<endl;
        cout<<"All rights reserved."<<endl;
        Kernel kernel(&cin,true,argc,argv);
        res = kernel.Phrasing();
      }else{
        Kernel kernel(&cin,false,argc,argv);
        res = kernel.Phrasing();
      }
    }else if ( argc >= 2 ){
      string filename = argv[1];
      ifstream fcin(filename.c_str());
      if ( fcin ){
        Kernel kernel(&fcin,false,argc,argv);		
        res = kernel.Phrasing();
      }else{
        Warning("Script file "+filename+" is not found.");
        return 1;
      }
      fcin.close();
    }else{
      Warning("Evawiz","1 argument is required.");
      return 1;
    }
  }catch ( const Error&err ){
    cerr<<err.what()<<endl;
  }catch ( const exception &err){
    cerr<<"Sourcecode::Error: "<<err.what()<<endl;
  }catch ( ... ){
    cerr<<"\nSystem::Error: Unexpected error occured."<<endl;
  }
 
  return res;
}

/*/
tip 1:
	If construction function called from any place, the space of many variables will be re set. Thus the previous values set will be lost.

//*/
/*/ test code
  //////// for Dimensions of EvaObj
  EvaObj list,dim;
  ImportList::ToExpression("{1,2,3}",list);
  ImportList::ToExpression("2",list);
  ImportList::ToExpression("{{1,2},{2,3},{3,4}}",list);
  ImportList::ToExpression("{{1,2},{2,3},{3}}",list);
  cout<<EvaObj::Dimensions(list,dim)<<endl;
  cout<<list<<":"<<dim<<endl;

  TVL
  TVL(
  VL(ls)

  //cout<<"pointer null >>>"<<NULL<<"<<<"<<endl;
  EvaObj dict; dict.ToList();
  for ( int i = 0 ;i< 100;i++){
    EvaObj item;
    item.PushBackString( ToString(i)  );
    item.PushBackString( ToString(100-i)  );
    dict.DictInsert(item[1],item[2]);
  }
  dict.DictSort();
  cout<<dict<<endl;
  cout<<*dict.DictValueAt( StringList("1") )<<endl;
  dict.DictInsertOrUpdate( StringList("1"), NumberList(99) );
  cout<<*dict.DictValueAt( StringList("1") )<<endl;
  cout<<*dict.DictValueAt( StringList("77") )<<endl;
  auto res = dict.DictGetRealPtr( StringList("33") );
  cout<<(*res.first)<<"->"<<(*res.second)<<endl;
  
  complex a(3,4),b(4,5);
  double d1,d2,id1,id2;
  d1 = (a/b).re;
  d2 = 32/41.;
  id1= (a*(1/b)).im;
  id2 = 1.0/41;
  cout<<d1<<endl;
  cout<<d2<<endl;
  cout<<(d1 == d2)<<endl;
  cout<<(id1 == id2)<<endl;
  cout<<std::numeric_limits<double>::epsilon()<<endl;

  
  return 0;
//*/
 
