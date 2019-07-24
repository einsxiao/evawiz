#ifndef __EVACC_ENGINE_H
#define __EVACC_ENGINE_H

class Engine{
public:
  string                evaCC   = "evacc";

  string                CCC     = "gcc";
  string                CPPCC   = "g++";

  string                NVCC    = "nvcc";
  string                FORCC   = "gfortran";

  string                CC      = "nvcc";

  map<string,string>    type2CC;
  CmdLine              *cmdLine;
  Syntax                syntax;
  static const int      NLEN = 2048;
  char                  buffer[NLEN];
public:
  Engine(CmdLine &_cl);
  ~Engine();

  int keep_pass_through();

  int no_cuda_pass();

  int cuda_pass();

  int processing();

};



#endif
