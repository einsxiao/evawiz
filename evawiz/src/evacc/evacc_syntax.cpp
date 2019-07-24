#include"evacc.h"


string                  *NodeIndex::content  = NULL;
vector< pair<int,int> > *NodeIndex::lineInfo = NULL;
string                  *Node::content       = NULL;
vector< pair<int,int> > *Node::lineInfo      = NULL;

NodeIndex &NodeIndex::incr(u_int n){
  for ( u_int i=0;i<n; i++ ){
    if ( (*content)[pos] == '\n' ){
      if ( (int)lineInfo->size() == row ){
        lineInfo->push_back( pair<int,int>(row,column+1) );
      }
      row++;
      column = 0;
    }else{
      column ++;
    }
    pos++;
  }
  return *this;
};

NodeIndex &NodeIndex::decr(u_int n)
{
  for ( u_int i=0;i<n; i++ ){
    pos--;
    if ( (*content)[pos] == '\n' ){
      row --;
      column = (*lineInfo)[row].second;
      //lineInfo->pop_back();
    }else{
      column --;
    }
  }
  return *this;
};

NodeIndex operator-(NodeIndex&ind,int num){
  NodeIndex tmp = ind;
  for ( int i=0;i<num;i++)
    tmp.decr();
  return tmp;
};

NodeIndex operator+(NodeIndex&ind,int num){
  NodeIndex tmp = ind;
  for ( int i=0;i<num;i++)
    tmp.incr();
  return tmp;
};

void Node::output()
{
  cout<<string(2*(level+1),' ');
  if ( eles.size() == 0 ){
    if ( type== "word" ){
      cout<<getContent()<<endl;
    }else if ( type== "symbol"){
      cout<<"["<<getContent()<<"]"<<endl;
    }else if ( type == "newline"){
      cout<<"["<<type <<"]"<<endl;
    }else{
      cout<<"["<<type <<":"<<getContent()<<"]"<<endl;
    }
  }else{
    cout<<"["<<type;
    cout<<":"<<endl;
    for ( u_int i=0; i<eles.size(); i++ )
      eles[i]->output();
    cout<<string(2*(level+1),' ');
    cout<<"]"<<endl;
  }
}


Syntax::Syntax()
{
}

Syntax::~Syntax()
{

}

void Syntax::init(CmdLine*cmdLine)
{
  if ( cmdLine->files.size() == 0 ){
    ThrowError("there is no source file to process.");
  }
  if ( cmdLine->files.size() > 1 ){
    ThrowError("no more than one file could be processed simutaliously.");
  }
  if ( not FileExistQ( cmdLine->filePath ) ){
    ThrowError("can not find source file '"+cmdLine->filePath+"'");
  }
  fileType = cmdLine->fileType;
  filePath = cmdLine->filePath;
  tmpFilePath = cmdLine->tmpFilePath;
  fileContent = GetFileContent( filePath )+"         \n          ";
  moduleName  = cmdLine->moduleName;
  conLen = fileContent.size()-17;
}

bool Syntax::rollBack(const int n ){
  idx.decr(n);
  return true;
}

bool Syntax::rollForward(const int n ){
  idx.incr(n);
  return true;
}

string Syntax::getStr(const int n ){
  if ( n <= 0 )
    return "";
  if ( idx.pos + n > conLen )
    return fileContent.substr(idx.pos, conLen - idx.pos);
  else
    return fileContent.substr(idx.pos,n);
}

char Syntax::getCharAt(int p=1){
  return fileContent[ idx.pos + p - 1 ];
}

bool Syntax::getChar(char&ch){
  if ( idx.pos >= conLen ){
    return false;
  }
  ch = fileContent[ idx.pos ];
  idx.incr();
  return true;
}

bool Syntax::getRecentNumberString(string&str,char ch){
  while ( ch >= '0' && ch <='9' ){
    str.append(1,ch);
    if ( !getChar(ch) )
      return true;
  }
  if ( ch == '.' ){
    str.append(1,ch);
    getChar(ch);
    while ( ch >= '0' && ch <='9' ){
      str.append(1,ch);
      if (!getChar(ch))
        return true;;
    }
  }
  if ( ch == 'E' or ch == 'e' ){
    str.append(1,'E');
    getChar(ch);
    if ( ch == '-' or ch == '+' ){
      str.append(1,ch);
      getChar(ch);
    }
    while ( ch >= '0' && ch <='9' ){
      str.append(1,ch);
      if ( !getChar(ch) )
        return true;
    }
  }
  rollBack(1);
  return true;
}

bool Syntax::getWrod(string&type,string&content){

  return true;
}


// #define INFOEle(ele,pos) dout<<string(3*level,' ')<<"get "<<#ele<<" at "<<(pos).ToString()<<endl
// #define INFOEleStart(ele,pos) dout<<string(3*level,' ')<<#ele<<" start at "<<(pos).ToString()<<endl
// #define INFOEleEnd(ele,pos) dout<<string(3*level,' ')<<#ele<<" end at "<<(pos).ToString()<<endl

#define INFOEle(ele,pos) 
#define INFOEleStart(ele,pos) 
#define INFOEleEnd(ele,pos) 



int Syntax::processing(Node&parent,int level,bool endReturn = false,bool colonReturn = false)
{
  vector<Node*> &tree = parent.eles;
  char ch;
  while ( getChar(ch) ){
    switch ( ch ){
    case ' ': case '\t': {
      continue;
    }
    case '\r': case '\n':{
      if ( getStr(1) == "\n" )
        rollForward(1);
      if ( endReturn ){ //such as #include #pragma
        return 0;
      }
      if (  ( tree.size() == 0 ) or (tree.size() > 0  and tree.back()->type == "newline") ){
        continue;
      }
      tree.push_back( new Node("newline",idx-1,idx,level) );
      INFOEle(newline,idx-1);
      continue;
    }
    case '(':{ //bracket
      tree.push_back( new Node("bracket",idx-1,level) );
      processing( *tree.back(),level+1,false,false);
      tree.back()->Set(idx);
      continue;
    }
    case ')':{
      if ( parent.type != "bracket" ){
        throwCCError(" ')' is not expected here. Please check.");
      }
      return 0;
    }
    case '{':{ //block
      tree.push_back( new Node("block",idx-1,level) );
      processing( *tree.back(),level+1,false,false);
      tree.back()->Set(idx);
      continue;
    }
    case '}':{
      if ( parent.type != "block" )
        throwCCError(" '}' is not expected here. Please check.");
      return 0;
    }
    case '<':{ //<<<
      if ( getStr(2) != "<<" ) goto __default_goto_label;
      tree.push_back( new Node("<<<>>>",idx-1,level) );
      rollForward(2);
      processing( *tree.back(),level+1,false,false);
      tree.back()->Set(idx);
      continue;
    }
    case '>':{
      if ( getStr(2) != ">>" ) goto __default_goto_label;
      rollForward(2);
      return 0;
    }
    case '[':{ // squarebracket
      tree.push_back( new Node("squarebracket",idx-1,level) );
      processing( *tree.back(),level+1,false,false);
      tree.back()->Set(idx);
      continue;
    }
    case ']':{
      if ( parent.type != "squarebracket" )
        throwCCError(" ']' is not expected here. Please check.");
      return 0;
    }
    case '\\':{
      if ( getCharAt(1) == '\n' ){
        continue;
      }else{
        throwCCError("An lonely '\\' was not supposed to be here. Revise and try again please.");
      }
    }
    case '#':{
      if ( inPragma ){
        tree.push_back( new Node("sharp",idx-1,idx,level) );
        continue;
      }
      inPragma = true;
      tree.push_back( new Node("pragma",idx-1,level) );
      processing( *tree.back(), level+1, true ,false);
      tree.back()->Set(idx);  //INFOEleEnd(pragma,idx);
      inPragma = false;
      if ( tree.back()->eles.size() > 2  and
           tree.back()->eles[0]->EqualQ("word","pragma") and
           tree.back()->eles[1]->EqualQ("word","evawiz") ){
        pragmas.push_back( Syntax::__pragma_rec(this->tree.eles.back(),
                                                &parent,
                                                this->tree.eles.size()-1,
                                                parent.eles.size()-1,
                                                tree.back()));
      }
      continue;
    }
    case ';':{
      if ( colonReturn )
        return 0;
      tree.push_back( new Node("semicolon",idx-1,idx,level) );
      INFOEle(semicolon,idx-1);
      continue;
    }
    case ':':{
      tree.push_back( new Node("colon",idx-1,idx,level) );// INFOEle(semicolon,idx-1);
      continue;
    }
    case ',':{
      tree.push_back( new Node("comma",idx-1,idx,level) );
      continue;
    }
    case '\'':{
      tree.push_back( new Node("astring",idx-1,level ) );
      while ( getChar(ch) ){
        if ( ch == '\\' )
          rollForward(1);
        if ( ch == '\'' )
          break;
      }
      tree.back()->Set(idx);
      continue;
    }
    case '"':{
      tree.push_back( new Node("string",idx-1,level) );
      while ( getChar(ch) ){
        if ( ch == '\\' )
          rollForward(1);
        if ( ch == '\"' )
          break;
      }
      tree.back()->Set(idx);
      continue;
    }
    case '/':{ //let comment out directly
      if ( getStr(1) == "/" ){
        int pstart = (idx-1).pos;
        while ( getChar(ch)  ){
          if ( ch == '\n' and getCharAt(-1) != '\\' )
            break;
        }
        rollBack(1);
        int pend = (idx).pos;
        for ( int i= pstart; i<  pend; i++ )
          fileContent[i] = ' ';
        continue;
      }else if ( getStr(1) == "*" ){
        int pstart = (idx-1).pos;
        rollForward(1);
        while ( getChar(ch) ){
          if ( ch == '*' and getCharAt(1) == '/' ){
            rollForward(1);
            break;
          }
        }
        int pend = (idx).pos;
        for ( int i= pstart; i<  pend; i++ ){
          if ( fileContent[i] != '\n')
            fileContent[i] = ' ';
        }
        continue;
      }
      goto __default_goto_label;
    }
    case '0': case '2':  case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
      tree.push_back( new Node("number",idx-1,level) );
      string word;
      getRecentNumberString(word,ch);
      tree.back()->Set(idx);
      continue;
    }
    default:{
      __default_goto_label:
      if ( ('a'<=ch and ch<='z') or ('A'<=ch and ch<='Z') or ch == '_' or ch == '$' ){
        tree.push_back( new Node("word",idx-1,level) );
        string word; word.append(1,ch);
        getChar(ch);
        while ( ('a'<=ch and ch<='z') or ('A'<=ch and ch<='Z') or ch == '_' or ch == '$' or ( '0'<=ch and ch<='9' ) ){
          word.append(1,ch);
          getChar(ch);
        }
        rollBack(1);
        tree.back()->Set( idx );
        continue;
      }
      tree.push_back( new Node("symbol",idx-1,idx,level) );
      continue;
    }
    }
  }
  return 0;
}

int Syntax::erase_source(NodeIndex&start_pos,NodeIndex&end_pos){
  if ( start_pos.row == end_pos.row ){ // if only one row
    for ( auto iter = old_2_new[start_pos.row].begin(); iter != old_2_new[start_pos.row].end(); iter ++ ){
      if ( start_pos.column < source[*iter].end_pos and end_pos.column > source[*iter].start_pos ){
        for ( int i= max(source[*iter].start_pos,start_pos.column) ;
              i< min(source[*iter].end_pos, end_pos.column ) ;
              i++ ){
          source[*iter].content[ i - source[*iter].start_pos ] = ' ';
        }
      }
    }
  }else{//more than one row
    //first lines
    for ( auto iter = old_2_new[start_pos.row].begin(); iter != old_2_new[start_pos.row].end(); iter ++ ){
      if ( start_pos.column < source[*iter].end_pos ){
        for ( int p= max(source[*iter].start_pos,start_pos.column) ; p< source[*iter].end_pos ; p++ ){
          source[*iter].content[ p - source[*iter].start_pos ] = ' ';
        }
      }
    }
    //middle lines
    for ( int i = start_pos.row+1; i< end_pos.row; i++ ){
      for (auto iter = old_2_new[i].begin(); iter != old_2_new[i].end(); iter ++ ){
        for ( int p= source[*iter].start_pos ; p< source[*iter].end_pos ; p++ ){
          source[*iter].content[ p - source[*iter].start_pos ] = ' ';
        }
      }
    }
    //last line
    for ( auto iter = old_2_new[end_pos.row].begin(); iter != old_2_new[end_pos.row].end(); iter ++ ){
      if ( end_pos.column > source[*iter].start_pos ){
        for ( int p= source[*iter].start_pos; p< min(source[*iter].end_pos, end_pos.column ); p++ ){
          source[*iter].content[ p - source[*iter].start_pos ] = ' ';
        }
      }
    }
  }

  return 0;
}

//update the source constructed by line information
int Syntax::prepare_insert(NodeIndex&insert_pos,int insertedLinesN)
{
  vector<int>&rel = old_2_new[insert_pos.row];
  int source_pos = -1;
  u_int rel_pos = 0;
  //find position
  for (rel_pos = 0 ; rel_pos< rel.size(); rel_pos++ ){
    if ( source[ rel[ rel_pos] ].start_pos <= insert_pos.column and insert_pos.column <= source[ rel[rel_pos] ].end_pos ){
      source_pos = rel[ rel_pos ];
      break;
    }
  }
  if ( source_pos == -1 )
    throwCCError("internal error while processing evawiz directive '#pragma evawiz'.");
  //sure where and how to
  if ( insert_pos.column == source[source_pos].end_pos ){
    source_pos ++;
    //rel_pos ++;
  }else if ( insert_pos.column > source[source_pos].start_pos ){//break the line into two
    source.insert( source.begin()+source_pos +1,
                   __line_info(true,
                               source[source_pos].row,
                               insert_pos.column,
                               source[source_pos].end_pos,
                               source[source_pos].content.substr( insert_pos.column,
                                                                  source[source_pos].end_pos - insert_pos.column) )
                   );
    source[source_pos].content = source[source_pos].content.substr( source[source_pos].start_pos,
                                                                    insert_pos.column - source[source_pos].start_pos );
    source[source_pos].end_pos = insert_pos.column;
    insertedLinesN++;
    rel.insert( rel.begin()+rel_pos+1, source_pos);
    source_pos ++;
  }else{
    // if insert directly at the begining then this lines old_2_new should also be updated 
    rel_pos --; 
  }
  source[source_pos].revised = true;
  //update old_2_new info
  // first the insert rows
  for (u_int p = rel_pos+1; p < rel.size(); p++ ){
    rel[p] += insertedLinesN;
  }
  for ( u_int p = insert_pos.row + 1; p< old_2_new.size(); p++ ){
    for (auto iter = old_2_new[p].begin(); iter != old_2_new[p].end(); iter++ ){
      (*iter) += insertedLinesN;
    }
  }
  // then following rows
  return source_pos;
}

#include"syntax_launch_kernel.h"
void Syntax::deal_launch_kernel(__pragma_rec&pragma,int serialN){
  launch_kernel lank;
  lank.pragma = &pragma;
  lank.serial_number = serialN + 1;
  lank.filePath = filePath;
  lank.tree = &tree;
  lank.content = &fileContent;
  lank.Init();
  //insert 
  //find the source postion and see what need to do,,prepare insert  
  //declare
  int source_insert_pos;
  if ( fileType == "cu" )
    source_insert_pos = prepare_insert( lank.declare_insert_pos, 2 );
  else
    source_insert_pos = prepare_insert( lank.declare_insert_pos, 1 );
  if ( fileType == "cu" )
    source.insert( source.begin() + source_insert_pos,
                   __line_info(true, pragma.self->start.row, 0, 0, lank.declare_gpu()+";" ) );
  source.insert( source.begin() + source_insert_pos,
                 __line_info(true, pragma.self->start.row, 0, 0, lank.declare_cpu()+";" ) );

  //calling
  if ( fileType == "cu" )
    source_insert_pos = prepare_insert( lank.pragma->self->end, 3 );
  else
    source_insert_pos = prepare_insert( lank.pragma->self->end, 1 );
  source[source_insert_pos-1].deleted = true; // delete the origin line
  source.insert( source.begin() + source_insert_pos,
                 __line_info(true, pragma.self->start.row, 0, 0,
                             lank.call_cpu() ));
  if ( fileType == "cu" ){
    source.insert( source.begin() + source_insert_pos,
                   __line_info(true, pragma.self->start.row, 0, 0,
                               lank.call_gpu() +"\n}else" ));
    source.insert( source.begin() + source_insert_pos,
                   __line_info(true, pragma.self->start.row, 0, 0,
                               "if ( evawiz::EvaSettings::MatrixPosition() == evawiz::MatrixDevice ){\n") );
  }
  //define
  if ( fileType == "cu" )
    source_insert_pos = prepare_insert( lank.define_insert_pos, 3 );
  else
    source_insert_pos = prepare_insert( lank.define_insert_pos, 2 );
  if ( fileType == "cu" )
    source.insert( source.begin() + source_insert_pos,
                   __line_info(true, pragma.self->start.row, 0, 0,
                               lank.define_gpu() ));
  source.insert( source.begin() + source_insert_pos,
                 __line_info(true, pragma.self->start.row, 0, 0,
                             lank.define_cpu('s') ));
  source.insert( source.begin() + source_insert_pos,
                 __line_info(true, pragma.self->start.row, 0, 0,
                             lank.define_cpu('f') ));
  erase_source( lank.content_start_pos, lank.content_end_pos );
  
}

void Syntax::rewrite(){
  //out the evawiz pragma
  for (u_int i = 0; i < pragmas.size(); i++){
    __pragma_rec &para  = pragmas[i];
    if ( para.parent == &(this->tree) ){
      //dout<<" find evawiz at top level:"<< para.self->getContent() <<endl;
      //top level 
    }else{
      //not top level
      //dout<<" find evawiz:"<< ( para.self->getContent() )<<" within "<< para.parent->type <<endl;
    }
    if (  para.self->eles[2]->EqualQ("word","launch") ){
      //dout<<"find a launch"<<endl;
      if ( para.self->eles[3]->EqualQ("word","kernel") ){
        deal_launch_kernel( para, i);
        continue;
      }
    }
    warningAtPos(para.self->eles[2]->start,"unkonwn pragma evawiz "+para.self->eles[2]->getContent()+".");
  }

}

void Syntax::phrasing(){
  NodeIndex::content     = &fileContent;
  NodeIndex::lineInfo    = &lineInfo;
  Node::content          = &fileContent;
  Node::lineInfo         = &lineInfo;
  idx = NodeIndex(0,0,0);
  //lineInfo.push_back( pair<int,int>(0,0) );
  processing(tree,0);

  //construct the newLines and old_2_new
  int ch_count = 0;
  for (u_int i=0;i < lineInfo.size(); i++){
    old_2_new.push_back( vector<int>() );
    old_2_new.back().push_back( i );
    auto line = lineInfo[i];
    source.push_back( __line_info(false,
                                  i,
                                  0, line.second,
                                  fileContent.substr(ch_count,line.second) ) );
    ch_count += line.second;
  }
  outContent = "";
  rewrite();
}

void Syntax::output(){
  outContent = "#1 \""+filePath+"\" 1\n";
  if ( moduleName != "" ){
    outContent += "#define ModuleName "+moduleName+"\n#line 1\n";
  }

  for (auto line:source){
    if ( line.deleted ) continue;
    if ( line.revised ){
      //outContent += "#"+ToString( line.row +1 )+" \""+filePath+"\" 1\n";
      outContent += "#line "+ToString( line.row +1 )+"\n";
      outContent += string(line.start_pos,' ')+line.content;
    }else{
      outContent += string(line.start_pos,' ')+line.content;
    }
    if ( (*line.content.rbegin())!='\n' )
      outContent += "\n";
  }
  SetFileContent(tmpFilePath,outContent);
  System((string)"touch -r " + filePath  +" "+ tmpFilePath );
}


inline void index_trans(){
  return;
}
