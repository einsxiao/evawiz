
#include"evacc.h"
#include"evacc_engine.h"

using namespace std;


Engine::Engine(CmdLine&_cl)
{
  cmdLine  = &_cl;
  type2CC["c"] = CCC;
  type2CC["c++"] = CPPCC;
  type2CC["cpp"] = CPPCC;
  type2CC["cu"] = NVCC;
  type2CC["fort"] = FORCC;
}

Engine::~Engine()
{
}

string subString(char*c_str,int startPos,int len){
  return string(c_str,startPos,len);
}
int Engine::processing()
{
  if ( not cmdLine->isCompile ){
    return keep_pass_through();
  }
  string cc =  type2CC[ cmdLine->fileType ];
  if ( cmdLine->fileType == "cu" and cmdLine->isForceCpp){
    cc = type2CC[ "c++" ];
  }
  if ( not CommandExist( cc ) ) ThrowError("evacc","Depended compiler '"+cc+"' is not found." );
  if ( cc == "" )
    ThrowError("do not known what to do with file type '"+cmdLine->fileType+"'.");
  string cmd;
  //prepare
  cmd = cmdLine->construct( cc );
  syntax.init( cmdLine );
  syntax.phrasing();
  syntax.output();
  FILE*fp;
  fp = popen(cmd.c_str(),"r");
  if ( fp == NULL ){
    ThrowError("can not open depended compiler '"+cc+"'.");
  }

  Regex info_pat0("^[ \t]*\n*$",0);
  //Regex info_pat1("^In file included from "+cmdLine->tmpFilePath+":(.*)$",0);
  Regex info_pat1("^(.*)from "+cmdLine->tmpFilePath+":(.*)$",0);
  Regex info_pat2("^"+cmdLine->filePath+"\\([0-9]*\\): warning: variable \".*\" was declared but never referenced",0);
  Regex::Matches match_res; 

  string info;

  while ( fgets( buffer, NLEN, fp ) != NULL ){
    if ( info_pat0.Match(buffer) ) continue;
    if ( info_pat1.Match(buffer) ) continue;
    if ( info_pat2.Match(buffer) ) continue;
    cout<<buffer;
  }
  if ( FileExistQ( cmdLine->tmpFilePath ) ){
    System("rm "+cmdLine->tmpFilePath);
  }
  return pclose(fp)/256;
}


int Engine::keep_pass_through()
{
  string cc =   type2CC[cmdLine->fileType] ;
  if ( cc == "" )
    cc = CPPCC;
  if ( cmdLine->isForceCpp )
    cc = CPPCC;
  if ( not CommandExist( cc ) ) ThrowError("evacc","Depended compiler '"+cc+"' is not found." );
  string cmd = cmdLine->keep_pass_construct(cc);
  FILE*fp;
  fp = popen(cmd.c_str(),"r");
  if ( fp == NULL ){
    ThrowError("can not open depended compiler '"+ cc +"'.");
  }
  while ( fgets( buffer, NLEN, fp ) != NULL ){
    cout<<buffer;
  }
  return pclose(fp)/256;
}
