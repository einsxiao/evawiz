#ifndef __EVACC__
#define __EVACC__

#include<iostream>
#include<vector>
#include<string>
#include<regex>
#include"evawiz.h"
using namespace std;
using namespace evawiz;
#include"evacc_cmdline.h"
////////////////////////////////
//#include"syntax_launch_kernel.h"
#include"evacc_syntax.h"
////////////////////////////////
#include"evacc_engine.h"



class cc_err:public Error{
public:
  int lN,cN;
  string file,message;
  cc_err(string _file,int _lN,int _cN,string _mes){
    file = _file;
    lN = _lN;
    cN = _cN;
    message = _mes;
  }
  virtual const string info()const throw(){
    if ( file != "" ){
      return (file+":"+ToString(lN+1)+":"+ToString(cN+1)+": error: "+message);
    }else{
      return ("evacc:: error: "+message);
    }
  }
};

inline void warning(string mes){ cerr<<"evacc:  warning:  "<<mes<<endl; }
#define warningAtPos(idx,mes) cerr<<(filePath+":"+ToString((idx).row+1)+":"+ToString((idx).column+1)+": warning: "+(mes)+"\n")
#define warnErrorAtPos(idx,mes) cerr<<(filePath+":"+ToString((idx).row+1)+":"+ToString((idx).column+1)+": error: "+(mes)+"\n")

#define throwCCError(mes) ({throw cc_err(filePath,(idx).row,(idx).column,mes);})
#define throwCCErrorAtPos(idx,mes) ({throw cc_err(filePath,(idx).row,(idx).column,mes);})



#endif
