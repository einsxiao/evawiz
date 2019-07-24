#ifndef Kernel_Xiao_H
#define Kernel_Xiao_H

namespace evawiz{

  class Kernel{
  public:
    Object       superList; //super Argv of lists readin
    Object       inputList; //historical Argv of lists readin
    bool         interactively;
    ImportList  *importList;
    Evaluation  *evaluation;
    ///////////////////////////////
    Kernel(istream *istr_in,bool interactively,int argc,char*argv[]);
    ~Kernel();
    //////////////////////////////////////////////
    int   Phrasing();
    //////////////////////////////////////////////
  }; 

};

#endif
