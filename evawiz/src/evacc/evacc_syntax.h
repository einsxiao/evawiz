#ifndef __EVACC_SYNTAX_H__
#define __EVACC_SYNTAX_H__

struct __line_info{
  bool     deleted = false;
  bool     revised = false;
  int      row; //will never be changed once assigned 
  int      start_pos;
  int      end_pos;
  string   content;
  __line_info(const bool   _revised,
              const int    _row,
              const int    _start_pos,
              const int    _end_pos,
              const string _content){
    revised   =  _revised;
    row       =  _row;
    start_pos =  _start_pos;
    end_pos   =  _end_pos;
    content   =  _content;
  };
};

class NodeIndex{
public:
  static string *content;
  static vector< pair<int,int> > *lineInfo;
  int pos,row,column;
  NodeIndex(){ pos = 0; row=0; column = 0; };
  NodeIndex(int p,int r,int c){pos = p; row=r; column=c; };
  NodeIndex &incr(u_int n=1);
  NodeIndex &decr(u_int n=1);
  string     ToString(){return evawiz::ToString(row)+":"+evawiz::ToString(column);}
};

NodeIndex operator-(NodeIndex&ind,int num);

NodeIndex operator+(NodeIndex&ind,int num);

class Node{
public:
  static string                  *content;
  static vector< pair<int,int> > *lineInfo;
  int                 level;
  string              type;
  string              info;
  NodeIndex           start = NodeIndex(0,0,0);
  NodeIndex           end = NodeIndex(0,0,0);
  vector<Node*>       eles;

  bool operator == (Node&other){
    return ( type == other.type and getContent() == other.getContent() );
  }

  bool EqualQ(string _type,string _content){
    return type ==  _type and getContent() == _content;
  }

  Node()
  {
    level= -1; type= ""; info= "";
  }

  void Set(string _type, NodeIndex _start,NodeIndex _end, int _level){
    type = _type;
    start = _start;
    end   = _end;
    level = _level;
  }

  void Set(string _type, NodeIndex _start,NodeIndex _end){
    type  = _type;
    start = _start;
    end   = _end;
  }

  void Set(string _type, NodeIndex _start){
    type  = _type;
    start = _start;
  }

  void Set(NodeIndex _end, int _level){
    end   = _end;
    level = _level;
  }

  void Set(NodeIndex _end){
    end   = _end;
  }

  Node(int _level){
    type = "";
    info = "";
    level = _level;
  }

  Node(string _type,NodeIndex _start,int _level)
  {
    type = _type;
    start = _start;
    level = _level;
  }

  Node(string _type,NodeIndex _start,NodeIndex _end,int _level)
  {
    type = _type;
    start = _start;
    end  = _end;
    level = _level;
  }

  ~Node(){
    for ( auto node : eles){
      if ( node != NULL ){
        delete node;
        node = NULL;
      }
    }
  }
  void       output();
  string     getContent(){
    if ( NodeIndex::content == NULL ) return "";
    return NodeIndex::content->substr( start.pos, end.pos - start.pos );
  }

};

class Syntax{
  int                         conLen;
  NodeIndex                   idx;
  bool                        rollBack(const int n=1);
  bool                        rollForward(const int n=1);
  bool                        newLine();
  string                      getStr(const int n= 0);
  char                        getCharAt(int);
  bool                        getChar(char&ch);
  bool                        getRecentNumberString(string&,char);
  bool                        getWord(string &type,string &word);
  string                      wordOfNode(Node*);
public:
  string                      fileType;
  string                      filePath;
  string                      tmpFilePath;
  string                      fileContent;
  string                      moduleName;
  Node                        tree;
  struct __pragma_rec{
    Node   *topNode;
    Node   *parent;
    u_int   pos,level_pos;
    Node   *self;
    __pragma_rec(Node*t,Node*p,u_int _p,u_int _lp, Node*_self){
      topNode = t; parent = p; pos = _p; level_pos = _lp; self = _self;
    }
  };
  vector< __pragma_rec >      pragmas;
  vector< pair<int,int> >     lineInfo;
  vector< __line_info >       source;
  vector< vector<int> >       old_2_new; //old row number -> new row number
  string                      outContent;

  bool                        inPragma = false;

public:
  Syntax();
  ~Syntax();
  
  void       init(CmdLine *_cmdLine);
  bool       getWrod(string&type,string&content);
  int        processing(Node& tree,int level,bool endReturn,bool colonReturn);
  ///////////////////////////////////////////////////
  // return the row number(new) could be returned 
  int        erase_source(NodeIndex &start_pos, NodeIndex &end_pos);
  int        prepare_insert( NodeIndex &insert_pos,int insertedLinesN = 1 );
  void       deal_launch_kernel(__pragma_rec&,int serailN);
  void       rewrite();
  void       phrasing();
  void       output();
  
  
  
};








#endif
