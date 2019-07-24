#include"evacc.h"

CmdLine::CmdLine(int _argc,char*_argv[])
{
  Regex regex_c("^.+\\.c$");
  Regex regex_cpp("^.+\\.cpp$");
  Regex regex_cu("^.+\\.cu$");
  Regex regex_c_o("^.+\\.c\\.o$");
  Regex regex_cpp_o("^.+\\.cpp\\.o$");
  Regex regex_cu_o("^.+\\.cu\\.o$");
  int base = 1;
  for (int i=1;i<_argc; i++){
    if ( regex_c.Match( _argv[i] ) ){
      fileType = "c";
      files.push_back( i - base );
    }else if ( regex_cpp.Match( _argv[i] ) ){
      fileType = "c++";
      files.push_back( i - base );
    }else if ( regex_cu.Match( _argv[i] ) ){
      fileType = "cu";
      files.push_back( i - base );
      //////////////////////////////////
    }else if ( regex_c_o.Match( _argv[i] )  ){
      if ( fileType == "" and not isCompile )
          fileType = "c";
      objects.push_back( i - base );
    }else if ( regex_cpp_o.Match( _argv[i] )  ){
      if ( ( fileType == "" or fileType == "c" ) and not isCompile )
        fileType = "c++";
      objects.push_back( i - base );
    }else if ( regex_cu_o.Match( _argv[i] ) ){
      if ( not isCompile )
        fileType = "c++";
      objects.push_back( i - base );
      //////////////////////////////////
    }else if ( string(_argv[i]) == string("-c") or  string(_argv[i]) == string("-E") ){
      isCompile = true;
    }else if ( string(_argv[i]) == string("-fc++") or string(_argv[i]) == string("-fcpp") ){
      isForceCpp = true;
      base++; continue;
    }else if ( string(_argv[i]) == string("-o") ){
      isOutputSpecified = true;
    }    
    options.push_back( _argv[i] );
  }

  if ( isCompile and files.size() > 1 )
    ThrowError("evacc","evacc can only compile one source file one time.");
  if ( isCompile and files.size() < 1 )
    ThrowError("evacc","input file is not given.");
  if ( not isCompile and (files.size()>0)   )
    Warning("evacc","evacc can not do compiling and linking at the same time.");
}

CmdLine::~CmdLine()
{

}

#define add_include(path,absolute) if ( FileExistQ(path) ){ \
    out += string(" -I")+path+" ";                          \
  }

#define add_lib(path,absolute) if ( FileExistQ(path) ){ \
    out += string(" -L")+path+" ";                      \
    if ( absolute ){                                    \
      out += " "+pre+"-rpath="+path+" ";                \
    }else{                                              \
      out += " "+pre+"-rpath=$ORIGIN/"+path;            \
    }                                                   \
  }


#define add_path(path,absolute) if ( FileExistQ( path ) ){ \
    out += string(" -I")+path+" -L"+path+" ";              \
    if ( absolute ){                                       \
      out += " "+pre+"-rpath="+path+" ";                   \
    }else{                                                 \
      out += " "+pre+"-rpath=$ORIGIN/"+path+" ";           \
    }                                                      \
  }

string cmd_pre_include_string(string&cctype)
{
  static string home_evawiz,evawiz_root,evawiz;
  if ( home_evawiz == "" ){
    home_evawiz = GetEnv( "HOME_EVAWIZ" );
    if ( home_evawiz == "" ) home_evawiz = GetEnv("HOME") + "/evawiz";
  }
  if ( evawiz_root == "" ){
    evawiz_root = GetEnv( "EVAWIZ_ROOT" );
    if ( evawiz_root == "" ) evawiz_root = GetEnv("HOME") + "/.evawiz";
  }
  if ( evawiz == "" ){
    evawiz = GetEnv( "EVAWIZ" );
    if ( evawiz == "" ) evawiz = evawiz_root + "/evawiz";
  }
  string  pre = " -Wl,"; if ( cctype == "nvcc" )  pre = " -Xlinker ";
  string out = "";
  string path;

  vector<string> items;

  string evawiz_include_path = GetEnv("EVAWIZ_INCLUDE_PATH");
  StringSplit( evawiz_include_path, items,":");
  for ( auto item = items.begin(); item != items.end(); item ++ ){
    add_include( *item, true);
  }

  string evawiz_library_path = GetEnv("EVAWIZ_LIBRARY_PATH");
  StringSplit( evawiz_library_path, items,":");
  for ( auto item = items.begin(); item != items.end(); item ++ ){
    add_lib( *item, true);
  }

  add_lib(".",false);
  add_include(".",false);
  
  return out;
}

string deal_ML_item(string & item,string &cctype)
{
  static string home_evawiz,evawiz_root,evawiz;
  if ( home_evawiz == "" ){
    home_evawiz = GetEnv( "HOME_EVAWIZ" );
    if ( home_evawiz == "" ) home_evawiz = GetEnv("HOME") + "/evawiz";
  }
  if ( evawiz_root == "" ){
    evawiz_root = GetEnv( "EVAWIZ_ROOT" );
    if ( evawiz_root == "" ) evawiz_root = GetEnv("HOME") + "/.evawiz";
  }
  if ( evawiz == "" ){
    evawiz = GetEnv( "EVAWIZ" );
    if ( evawiz == "" ) evawiz = evawiz_root + "/evawiz";
  }
  string  pre = " -Wl,";  if ( cctype == "nvcc" )  pre = " -Xlinker ";
  string out = "";
  string mod = item.substr(3);

  string path;

  //test 1
  path = home_evawiz+"/"+"module_name";
  if ( FileExistQ( path ) ){
    add_lib( string("../")+mod+"/lib",false);
    add_include( string("../")+mod+"/include",false);
    add_path( string("../")+mod,false);
  }

  path = evawiz_root+"/modules/"+mod;
  if ( FileExistQ(path) ){
    add_lib(path+"/lib",true);
    add_lib(path+"/lib64",true);
    add_include(path+"/include",true);
    add_path(path,true);
  }

  path = evawiz_root+"base/"+mod;
  if ( FileExistQ(path) ){
    add_lib(path+"/lib",true);
    add_lib(path+"/lib64",true);
    add_include(path+"/include",true);
    add_path(path,true);
  }

  path = evawiz_root+"/programs/"+mod;
  if ( FileExistQ(path) ){
    add_lib(path+"/lib",true);
    add_lib(path+"/lib64",true);
    add_include(path+"/include",true);
    add_path(path,true);
  }

  return out;

}

#define OPTIONS2CMD for (auto option: options){                       \
    if ( cc != "nvcc" and option == "--compiler-options" ) continue;  \
    if ( cc != "nvcc" and option == "-G" ) continue;                  \
    if ( cc != "nvcc" and option == "-Xcompiler" ) continue;          \
    if ( option.substr(0,3) == "-ML" ){                               \
      cmd += string(" ")+deal_ML_item(option , cc) + " ";             \
      continue;                                                       \
    }                                                                 \
    cmd+= " "+option;                                                 \
  }                                                                   \
  cmd += " "+cmd_pre_include_string(cc)



string CmdLine::keep_pass_construct(string cc)//just pass forward
{
  if ( cc == "nvcc" ){
    options.push_back( "-Wno-deprecated-gpu-targets" );
  }
  options.push_back("-DEVA_OS_LINUX");
  options.push_back("--compiler-options"); options.push_back("-fopenmp");
  options.push_back("--compiler-options"); options.push_back("-pthread");
  options.push_back("-leva");
  options.push_back("-lmpi_cxx");
  options.push_back("-lmpi");
  if ( cc != "nvcc" and CommandExist("nvcc" ) ){
    options.push_back("-lcudart");
  }

  string cmd = cc; OPTIONS2CMD;

  dout<<cmd<<endl;
  return cmd + " 2>&1";
}

string CmdLine::construct(string cc) //compile and file exist for sure
{
  //test if cc is available

  filePath = options[ files[0] ];
  fileDir  = GetDirName( filePath );
  fileName = GetBaseName( filePath );
  Regex  module_pat("^(.*)Module\\.[a-zA-Z]*$",1);
  vector<string> res;
  if ( module_pat.Match( fileName, res ) ){
    isModuleFile = true;
    moduleName = res[1];
  }
  if ( fileDir == "" )
    fileDir = ".";
  tmpFilePath = "/tmp/evacc_tmp000"+RandomNumberString(8)+"_"+fileName;
  if ( isForceCpp ) tmpFilePath+=".cpp";
  while ( FileExistQ( tmpFilePath ) ){
    tmpFilePath = "/tmp/evacc_tmp000"+RandomNumberString(8)+"_"+fileName;
    if ( isForceCpp ) tmpFilePath+=".cpp";
  }
  if ( fileName == "" )
    ThrowError("source error is not found.");

  options[ files[0] ] = tmpFilePath;

  //options 
  if ( cc == "nvcc" ){
    options.push_back("-Wno-deprecated-gpu-targets" );
  }
  options.push_back("-DEVA_OS_LINUX");
  options.push_back("--compiler-options"); options.push_back("-fopenmp");
  options.push_back("--compiler-options"); options.push_back("-pthread");

  string cmd = cc; OPTIONS2CMD;

  if ( isCompile and not isOutputSpecified ){
    cmd += " -o "+fileName+".o";
  }

  cmd += " 2>&1";
  dout<<cmd<<endl;
  return cmd;
}

