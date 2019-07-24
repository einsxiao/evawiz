#include<iostream>
#include"evacc.h"
using namespace std;

int main(int argc,char* argv[]){
  try{
    CmdLine   cl(argc,argv);
    Engine    eng(cl);
    return eng.processing();
  }catch (const cc_err &err){
    cerr<< err.info() <<endl;
    return 1;
  }catch (const Error &err){
    cerr<<"evacc:  error:  "<<err.message<<endl;
    return 1;
  }catch ( const exception &err){
    cerr<<endl<<"evacc : internal fatal error: "<<err.what()<<endl;
    return 2;
  }catch ( ... ){
    cerr<<endl<<"evacc : internal fatal unexpected unknown error. "<<endl;
    return 3;
  }
  return 0;
  return 1;
}
