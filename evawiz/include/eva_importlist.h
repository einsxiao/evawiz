#ifndef Gisp_Reading_H
#define Gisp_Reading_H

namespace evawiz{

  //class Evaluation;

  class ImportList{
  public:
    ImportList(istream *istr_in,bool interactively);
    ImportList(string buffer);
    ~ImportList();
    istream	      *istr;
    string 	       buffer;
    bool	         interactively;
    string	       indent;
    //////////////////////////////////////////////
    int 	         GetList(Object&Argv,int depth);//begin with 0
    //////////////////////////////////////////////
    static Object  GetList(string,int depth); //
    static Object  ToExpression(string);
    //////////////////////////////////////////////
    //////////////////////////////////////////////
    bool	         End(); bool inputEnd;
    bool	         getChar(char &ch); int 	pos;
    bool 	         getChar(char &ch,int n);// won't move buffer pointer
    char           getRecentNonSpace();
    bool           getRecentNumberString(string&,char ch=' ');
    //////////////////////////////////////////////
    bool	         rollBack(int n);
    //////////////////////////////////////////////
    //static bool isSpecialCharArray[255+1];
  };
};

#endif
