#ifndef ListModule_Eins_H
#define ListModule_Eins_H

namespace evawiz{
  class ListModule:public Module{
  public:
    /////////////////////////////////
    
    ListModule(string,string);
    ~ListModule();
    /////////////////////////////////
    static int List(Object &);
    //Declare_ValueTablePair_Id_Of(List);
    static int Range(Object &);
    /////////////////////////////////
    static int Take(Object &);
    static int Drop(Object &);
    /////////////////////////////////
    static int Head(Object &);
    static int Size(Object &);
    static int First(Object &);
    static int Last(Object &);
    static int Most(Object &);
    static int Rest(Object &);
    /////////////////////////////////////////////
    static int Part(Object &);
    static int SetPart(Object&left,Object&right);
    static int SetPart(Object&);
    static int Append(Object &);
    static int AppendTo(Object &);
    static int Insert(Object &);
    static int InsertTo(Object &);
    static int Delete(Object &);
    static int DeleteFrom(Object &);
    static int PopBack(Object &);
    /////////////////////////////////////////////
    static int Table(Object &);
    static int Band(Object &);
    /////////////////////////////////////////////
    static int Sort(Object &);
    static int Flatten(Object &);
    static int Dimensions(Object &);
    //////////////////////////////////////////
    static int Dict(Object&);
  };
};


#endif
