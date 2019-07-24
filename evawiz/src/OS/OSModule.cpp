#include"evawiz.h"
#include"OSModule.h"
using namespace evawiz;
#define ModuleName OS

__DEBUG_MAIN__("input.ev");

ModuleInitialize{
  AddAttribute("File",AttributeType::Conjunctable);

}

ModuleFinalize{
  for(auto iter = fileTable.begin(); iter != fileTable.end(); iter ++ ){
    if ( iter->second  != NULL )
      iter->second->close();
  }

};

int OSModule::Import(string filename,bool willBroken){
  return EvaKernel->EvaluateFile(filename,willBroken);
}

DefineFunction(Import)
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  if ( !Argv[1].StringQ() ) ThrowError(Argv.Key(),(string)"Argument of "+Argv.Key()+" should be string.");
  Import( Argv[1].Key() );
  ReturnNull;
}

DefineFunction(Export)
{
  CheckShouldEqual(2);
  CheckShouldBeString (1);
  if ( Argv[1].StringQ( "" ) ){
    ThrowError("Export","Empty filename is not allowed.");
  }
  SetFileContent( Argv[1].Key(), Argv[2].ToString() );
  Argv.SetTo(1);
  ReturnNormal;
}

  

DefineFunction(Get)
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  if ( !Argv[1].StringQ() ) ThrowError(Argv.Key(),(string)"Argument of "+Argv.Key()+" should be string.");
  Import( Argv[1].Key(), true);
  ReturnNull;
}

DefineFunction(Put)
{
  CheckShouldBeString( Argv.Size() );
  ofstream ofs;

  ofs.open( Argv.Last().Key() );
  if ( not ofs ){
    ThrowError("Put",Argv.Last().ToString()+" can not be opened.");
  }
  
  for(u_int i=1; i< Argv.Size(); i++ ){
    ofs<<Argv[i]<<endl;
  }
  ofs.close();
  ReturnNull;
}

DefineFunction(ReadList)
{
  CheckArgsShouldEqual(Argv,1);
  if ( !Argv[1].StringQ() ) ThrowError(Argv.Key(),(string)"Argument of "+Argv.Key()+" should be string.");
  ifstream ifs( Argv[1].Key() , ios::in );
  if ( !ifs ) ThrowError(Argv.Key(),(string)"file "+Argv[1].Key()+" cannot be opened.");
  string line; Object tl;
  Argv.SetList_List();
  while ( getline(ifs,line) ){
    dout<<line<<endl;
    if ( (tl = ImportList::ToExpression(line) ).NullObjectQ() ) Warning(Argv.Key(),"Invalid input when reading"+line+".");
    if ( tl.NullQ() ){
      EvaKernel->Evaluate(tl);
      Argv.PushBack(tl);
    }
  }
  ReturnNormal;
}



DefineFunction(FileExistQ)
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  if ( evawiz::FileExistQ( Argv[1].Key() ) )
    Argv.SetSymbol(SYMBOL_ID_OF_True);
  else
    Argv.SetSymbol(SYMBOL_ID_OF_False);
  ReturnNormal;
}


DefineFunction(Directory,"")
{
  CheckShouldNoMoreThan(0);
  char *buffer;  
  if( (buffer = getcwd(NULL, 0)) == NULL) 
    ThrowError("Directory","Can not get current working directory.");
  Argv.SetString( buffer );
  free( buffer );
  ReturnNormal;
}

DefineFunction(SetDirectory,"")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  if ( chdir( Argv[1].Key() ) != 0 )
    ThrowError("SetDirectory",(string)"Failed to Set Directory to \""+Argv[1].Key()+"\".");
  Argv.SetTo(1);
  ReturnNormal;
}

DefineFunction(ParentDirectory,"")
{
  CheckShouldNoMoreThan(0);
  char *buffer;  
  if( (buffer = getcwd(NULL, 0)) == NULL) 
    ThrowError("Directory","Can not get current working directory.");
  int ind = strrchr(buffer,'/') - buffer;
  if ( ind <= 0 ){
    ReturnNull;
  }
  buffer[ind]='\0';
  Argv.SetString( buffer );
  free( buffer );
  ReturnNormal;
}

DefineFunction(SetToParentDirectory,"")
{
  CheckShouldNoMoreThan(0);
  char *buffer;  
  if( (buffer = getcwd(NULL, 0)) == NULL) 
    ThrowError("Directory","Can not get current working directory.");
  int ind = strrchr(buffer,'/') - buffer + 1;
  if ( ind <= 0 )
    ReturnNull;
  buffer[ind]='\0';
  Argv.SetString(buffer );
  chdir( buffer );
  buffer[ind-1]='\0';
  ReturnNormal;
}

DefineFunction(CreateDirectory,"")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  System((string)"mkdir "+Argv[1].Key() );
  ReturnNull;
}

DefineFunction(DeleteDirectory,"")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  System((string)"rm -r "+Argv[1].Key() );
  ReturnNull;
}

DefineFunction(TouchFile,"Create an empty file in current directory.")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  System((string)"touch "+Argv[1].Key() );
  ReturnNull;
}

DefineFunction(RenameFile,"Rename file in current directory.")
{
  CheckShouldEqual(2);
  CheckShouldBeString(1);
  CheckShouldBeString(2);
  System((string)"mv "+Argv[1].Key()+" "+Argv[2].Key() );
  ReturnNull;
}

DefineFunction(CopyFile,"Copy file.")
{
  CheckShouldEqual(2);
  CheckShouldBeString(1);
  CheckShouldBeString(2);
  System((string)"cp -f "+Argv[1].Key()+" "+Argv[2].Key() );
  ReturnNull;
}

DefineFunction(DeleteFile)
{
  for (size_t i=1; i<= Argv.Size(); i++ ){
    if ( Argv[i].StringQ() ){
      System( (string)"rm -f "+Argv[i].Key() );
    }
  }
  ReturnNull;
}

DefineFunction(FileBaseName,"Get the base name of a file path.")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  Argv.SetString( GetBaseName( Argv[1].Key() ) );
  ReturnNormal;
}

DefineFunction(FileDirectoryName,"Get the directory of a file path.")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  Argv.SetString( GetDirName( Argv[1].Key() ) );
  ReturnNormal;
}

DefineFunction(GetEnv,"Get environment variable")
{
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  ReturnString( evawiz::GetEnv( Argv[1].Key() ) );
}

DefineFunction(SetEnv,"Get environment variable")
{
  CheckShouldEqual(2);
  CheckShouldBeString(1);
  CheckShouldBeString(2);
  if ( evawiz::SetEnv( Argv[1].Key(), Argv[2].Key(), true ) < 0 )
    ThrowError("SetEnv","Environment set failed.");
  ReturnNull;
}
ios_base::openmode modeString2Int(string mode)
{
  static vector<string> modes;
  StringSplit(mode,modes,"|");
  ios_base::openmode mod,tmod;
  for (auto iter = modes.begin(); iter != modes.end(); iter++){
    if ( *iter == "Append" or *iter == "append" )
      tmod = ios::app;
    else if ( *iter == "Binary" or *iter == "binary")
      tmod= ios::binary;
    else if ( *iter == "In" or *iter == "in")
      tmod = ios::in;
    else if ( *iter == "Out" or *iter == "out" )
      tmod = ios::out;
    else if ( *iter == "Trunc" or *iter == "trunc" )
      tmod = ios::trunc;
    else
      ThrowError("File","Unkown File open mode.");
    if ( iter == modes.begin() )
      mod = tmod;
    else
      mod = mod | tmod;
  }
  return mod;
};

DefineFunction(File,"File Handler")
{
  static INIT_SYMBOL_ID_OF( File );
  // static INIT_SYMBOL_ID_OF( Open );
  static INIT_SYMBOL_ID_OF( Close );
  static INIT_SYMBOL_ID_OF( Read );
  static INIT_SYMBOL_ID_OF( ReadLines );
  static INIT_SYMBOL_ID_OF( Write );
  Conjunct_Context(File){
    Object&file = Argv[1];
    Object&operobj = Argv[2];
    CheckArgsShouldEqual( file, 1);
    CheckArgsShouldBeString( file, 1);
    string filename = file[1].Key();
    fstream **fs;

    map<string,fstream*>::iterator iter;
    if ( (iter = fileTable.find( filename ) ) == fileTable.end() )
      ThrowError("File","File '"+filename+"' is not constructed yet");
    fs = &(iter->second);
    if ( *fs == NULL )
      ThrowError("File","File '"+filename+"' is not constructed yet");

    Conjunct_Case( Close ){
      (**fs).close();
      ReturnNull;
    }
    
    string res;
    Conjunct_Case( Read ){
      if ( ! (**fs ) ) ReturnNull;
      if ( operobj.Size() == 0 ){
        stringstream buffer;
        buffer << (**fs).rdbuf();
        ReturnString( buffer.str() );
      }else if ( operobj.Size() ==  1 ){
        EvaKernel->Evaluate( operobj[1] );
        CheckArgsShouldBeNumber(operobj, 1 );
        int N = operobj[1].Number();
        while ( N > 0 and not (**fs).eof() ){
          res+=  (**fs).get();
          N--;
        }
        ReturnString( res );
      }
    }
    Conjunct_Case( ReadLines ){
      if ( ! (**fs ) ) ReturnNull;
      Object lres; lres.SetList_List();
      if ( operobj.Size() == 0 ){
        while ( getline( **fs, res ) )
          lres.PushBackString( res );
        ReturnObject( lres );
      }else{
        EvaKernel->Evaluate( operobj[1] );
        CheckArgsShouldBeNumber(operobj, 1 );
        int N = operobj[1].Number();
        for ( int i=0; i<N; i++ ){
          if ( getline( (**fs),res) )
            lres.PushBackString( res );
          else
            break;
        }
        ReturnObject( lres );
      }
    }
    Conjunct_Case( Write ){
      if ( ! (**fs ) ) ReturnNull;
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeString(operobj,1);
      (**fs) << operobj[1].Key();
      Argv.SetTo(1);
      ReturnNormal;
    }

    ReturnNormal;
  }
  CheckShouldNoLessThan(1);
  CheckShouldBeString(1);
  if ( Argv.Size() == 1 ){
    string filename = Argv[1].Key();
    fstream** fs= &fileTable[filename];
    if ( *fs == NULL ){
      *fs = new fstream;
      (**fs).open(filename, ios::in|ios::out);
    }
    ReturnNormal;
  }
  CheckShouldEqual(2);
  CheckShouldBeString(2);
  string filename = Argv[1].Key();
  string mode = Argv[2].Key(); 
  ios_base::openmode mod= modeString2Int( mode );
 
  fstream** fs= &fileTable[filename];
  // mode of values Append Binary In Out NoCreate NoReplace Trunc
  if ( *fs != NULL ){
    (**fs).close();
    (**fs).clear();
  }else{
    *fs = new fstream;
  }
  (**fs).open(filename,mod);
  Argv.Delete(2);
  ReturnNormal;
}

