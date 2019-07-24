#ifndef __EVACC_CMDLINE_H
#define __EVACC_CMDLINE_H

class CmdLine{
public:
  vector<string> options;
  string         fileType;
  string         filePath;
  string         tmpFilePath;
  string         fileDir;
  string         fileName;
  int            fleID = -1;
  bool           isCompile = false;
  bool           isOutputSpecified = false;
  bool           isModuleFile = false;
  bool           isForceCpp = false;
  string         moduleName;
  vector<int>    files;
  vector<int>    objects;
  CmdLine(int argc,char*argv[]);
  ~CmdLine();
  string keep_pass_construct(string);
  string construct(string);
};

#endif
