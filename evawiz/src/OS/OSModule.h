#ifndef OSModule_Eins_H
#define OSModule_Eins_H

namespace evawiz{
  class OSModule: public Module{
    /////////////////////////////////
    DeclareModule(OS);
  public:
    /////////////////////////////////
    static int Import(string filename,bool willBroken=false);
    /////////////////////////////////
    DeclareFunction(Get);
    DeclareFunction(Import);
    DeclareFunction(Put);
    DeclareFunction(Export);
    /////////////////////////////////
    DeclareFunction(ReadList);
    /////////////////////////////////
    DeclareFunction(FileExistQ);
    DeclareFunction(Directory);
    DeclareFunction(SetDirectory);
    DeclareFunction(ParentDirectory);
    DeclareFunction(SetToParentDirectory);
    DeclareFunction(CreateDirectory);
    DeclareFunction(DeleteDirectory);
    /////////////////////////////////
    DeclareFunction(TouchFile);
    DeclareFunction(RenameFile);
    DeclareFunction(CopyFile);
    DeclareFunction(DeleteFile);
    DeclareFunction(FileBaseName);
    DeclareFunction(FileDirectoryName);
    DeclareFunction(FileNameJoin);
    DeclareFunction(FileNameSplit);
    map<string, fstream*> fileTable;
    DeclareFunction(File);
    /////////////////////////////////
    DeclareFunction(GetEnv);
    DeclareFunction(SetEnv);


  };
};


#endif
