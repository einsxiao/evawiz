#include"eva_headers.h"

using namespace std;
using namespace evawiz;

ImportList::ImportList(istream *istr_in,bool interactively){
  buffer = "";
  pos = 0;
  istr = istr_in;
  this->interactively = interactively;
  inputEnd = false;
}
ImportList::ImportList(string buffer){
  this->buffer = buffer;
  pos=0;
  istr = NULL;
  interactively = false;
  inputEnd = false;
}

ImportList::~ImportList(){};

bool ImportList::End(){
  return inputEnd;
}

bool ImportList::getChar( char &ch){ // read in a char, and move the position pointer forward
  //the next char which should be get. At the very beginning, position 0 has no element and need to get from istr;
  if ( pos >= (int)buffer.length() ){
    if ( istr && istr->get(ch) ) {
      buffer.append(1,ch);
      pos++;
      return true;
    } else {
      inputEnd = true;
      ch = '\0';
      return false; 
    }
  }// another situation is that the char can be got from the buffer
  ch = buffer[pos];
  pos++;
  return true;
}

bool ImportList::getChar( char &ch, int n ){// will not change the position pointer
  for ( int i=buffer.length(); i<= pos+n-1; i++){
    if ( istr && istr->get(ch) ) {
      buffer.append(1,ch);
      return true;
    }else{
      ch = '\0';
      return false;
    }
  }
  if ( pos + n < 0 ){
    ch = '\0';
    return false;
  }
  ch = buffer[ pos + n - 1];
  return true;
}

bool ImportList::getRecentNumberString(string &str,char ch){
  //no dot dot may contain other meanings ( need seperate treatment )
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
  if ( ch != '`' ) rollBack(1);
  return true;
}

char ImportList::getRecentNonSpace(){// will not change the position pointer
  char ch = ' ';
  int p=pos;
  while ( p< (int)buffer.length() ){
    ch = buffer[p];
    if (ch != ' ') return ch; 
    p++;
  }
  while ( istr && istr->get(ch) ) {
    buffer.append(1,ch);
    if ( ch == ' ')
      continue; 
    return ch;
  }
  return ch;
}


bool ImportList::rollBack( int n){// rollBack the last n-th chars to buffer n>=0;
  if ( n > pos ) return false; 
  pos -= n;
  return true;
}

//if state != 0 the specail operator
static const u_int RUnit = 1;
static const u_int RLeft = 2;
static const u_int RRight = 3;
static const u_int RConnect = 4;

//the first operator is Seperator Null for operatorPositions is ok.
#define OPERATOR_PUSH(op,con){                  \
    Object newop; newop.SetSymbol( op );        \
    newop.set_state( con );                     \
    listStack.PushBackRef( newop );             \
  }

int ImportList::GetList( Object &parentList ,int depth ){ // the return value "0" or "error code and detail
  //depth indicates the depth of the expression
  /////////////////////////////////////
  static Object timesOp;
  if (timesOp.NullQ()){
    timesOp.SetSymbol(SYMBOL_ID_OF_Times);
    timesOp.set_state(RConnect);
  }
  Object listStack;
  listStack.SetList_List();
  char ch;
  bool expressionEnd;
  bool listEnd = false;
  string currentWord;
  //try {
  while ( 1 ){
    if ( !getChar(ch) ){ //
      expressionEnd = true;
      listEnd = true;
    }else{
      currentWord="";
      expressionEnd = false;
      switch ( ch )
        {
          ///////////////////////// space
        case ' ':{// role to seperate expressions but can be absent when constructing list
          break;
        }
        case '\t':{// role to seperate expressions but can be absent when constructing list
          break;
        }
        case '\n':{//only when end with a normal list can be treated as an end to the whole list
          if ( depth == 0 ){
            if ( listStack.Size() == 0 ){
              if ( interactively )
                return 0;
              else
                break;
            }else if ( listStack.Last().state() == RConnect ){
              if (interactively)
                cout<< indent;
              break;
            }else if( depth != 0){
              break;
            }else{
              //// an expression finished
              listEnd = true;
              expressionEnd = true;
              break;
            }
          }else // not in the upmost text(superList level); just ignore it
            break;
        }
          ///////////////////////// 
          ///////////////////////// " and ' can both enclose a String
        case '\"':{
          while ( getChar(ch) && (ch != '\"'|| (ch == '\"'&&currentWord.size()>0&& *currentWord.rbegin() == '\\') ) )
            currentWord.append(1,ch);
          if ( ch != '\"' ) ThrowError("Syntax","String uncomplete");
          listStack.PushBackString( currentWord.c_str() );
          break;
        }
        case '\'':{
          while ( getChar(ch) && (ch != '\''|| (ch == '\''&&currentWord.size()>0&& *currentWord.rbegin() == '\\') ) )
            currentWord.append(1,ch);
          if ( ch != '\'' ) ThrowError("Syntax","String uncomplete");
          listStack.PushBackString(currentWord.c_str() );
          break;
        }
          ///////////////////////// numbers
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {

          getRecentNumberString(currentWord,ch);
          listStack.PushBackNumber(  ToNumber( currentWord.c_str() ) );
          break;
        }
          ///////////////////////// .
        case '.':{
          char ch1;
          bool e1 = getChar(ch1,1);
          if ( e1 && ch1>='0' && ch1<='9' ){
            currentWord = ".";
            getChar(ch);
            getRecentNumberString(currentWord,ch);
            listStack.PushBackNumber( ToNumber(currentWord.c_str() ) ) ;
            break;
          }else{ // Member of grammar, should not treated as a operator for [] is not treated as a Operator
            OPERATOR_PUSH(SYMBOL_ID_OF_Conjunct,RConnect);
            break;
          }
        }
          ///////////////////////// 
          ///////////////////////// 
        case ',':{///////////////////////// , separate atoms or list ..( comma )
          expressionEnd = true;
          break;
        }
        case ';': {/////////////////////////;
          if ( listStack.Empty() )
            break;
          OPERATOR_PUSH( SYMBOL_ID_OF_CompoundExpression,RConnect );
          if ( getRecentNonSpace()=='\n' && depth == 0 ){
            listStack.PushBackNull();
            listEnd = true;
            expressionEnd = true;
            char ch1;
            while ( getChar(ch1) && ch1!='\n' );
          }else{
            char ch1; ch1 = getRecentNonSpace();
            if ( ch1 == ']' || ch1 == ')' || ch1 == '}' || ch1 ==',' ){
              listStack.PushBackNull();
            }
          }
          break;
        }
        case '[': { ///////////////////////// [ ] indicate a composite list
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 && ch1 == '[' ){
            if ( listStack.Empty() ) ThrowError( "Syntax:","Sole [[ without a head is not allowed." );
            rollBack(-1);
            OPERATOR_PUSH( SYMBOL_ID_OF_PartArgsFollow,RConnect );
            Object argsList; argsList.SetList( SYMBOL_ID_OF_Part );
            listStack.PushBackRef( argsList );
            int r = 0;
            if ( (r=GetList( argsList, depth+1 ) ) < 0 )
              return r;
          }else{ // need to push a head list at the beginning of the elements of this list
            Object argsList;
            if ( listStack.Size() > 0 and listStack.Back().SymbolQ() )
              argsList.SetList( listStack.Back().ids() );
            else
              argsList.SetList( SYMBOL_ID_OF_List );
            if ( not ( listStack.Empty() ||
                       ( listStack.Last().state() == RConnect ||
                         listStack.Last().state() == RRight ))
                 ){
              OPERATOR_PUSH( SYMBOL_ID_OF_ArgsFollow,RConnect );
            }
            listStack.PushBackRef( argsList );
            int r = GetList( argsList, depth+1 );
            if ( r < 0 )
              return r;
          }
          break;
        }
        case ']': {// ']' can end ( { [ [[ and so on
          if ( depth == 0 ) ThrowError("Sytax"," A ] encountered without a [ ahead.");
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 && ch1 == ']' &&parentList.ListQ(evawiz::SYMBOL_ID_OF_Part )){
            rollBack(-1);
          }
          expressionEnd = true;
          listEnd = true;
          break;
        }
        case '{': {
          Object listList;
          listList.SetList( SYMBOL_ID_OF_List );
          if ( GetList( listList , depth +1)<0 ) return -1;
          if ( listList.Size() == 1 && listList[1].NullQ() )
            listList.Delete(1);
          listStack.PushBackRef( listList );
          break;
        }
        case '}': {
          if ( parentList.ListQ(SYMBOL_ID_OF_List ) ){
            expressionEnd = true;
            listEnd = true;
          }else{
            ThrowError("Syntax","} does not follow a {");
          }
          break;
        }
        case '(': {
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 && ch1 == '*' ){
            rollBack(-1);
            if ( getChar(ch) ){
              while (  ch1 !='*' || ch != ')'  ){
                currentWord.append(1,ch);
                ch1=ch;
                if (!getChar(ch) ) break;
              }
              if ( End() ) Warning("Syntax","expression does not end.\n");
              break;
            }else{
              ThrowError("Syntax","expression does not end.\n");
            }
          }
          //OPERATOR_PUSH( SYMBOL_ID_OF_ParenthesisFollow,RConnect );
          Object listList; listList.SetList( SYMBOL_ID_OF_Parenthesis );
          int r=0;
          if ( ( r = GetList( listList , depth +1) ) <0 ) return r;
          if ( listList.Size() == 0 ){
            ThrowError("Syntax","Parenthesis should contiain at least one expression in it.");
          }else if ( listList.Size() == 1) {
            listList.SetTo(1);
          }else if ( listList.Size() == 2) {
            if ( listList[1].NumberQ() && listList[2].NumberQ() ){
              listList[0].set_ids( SYMBOL_ID_OF_Complex );
            }
          }
          listStack.PushBackRef( listList );
          break;
        }
        case ')': {
          if ( parentList.ListQ(SYMBOL_ID_OF_Parenthesis ) ){
            expressionEnd = true;
            listEnd = true;
          }else{
            ThrowError("Syntax",") does not follow a (.");
          }
          break;
        }

          ///////////////////////////////////
          ///////////////////////////////////
        case '_': {///////////////////////// blacks _ __ ___
          if ( listStack.Size() > 0 and listStack.Last().SymbolQ() ) 
            OPERATOR_PUSH( SYMBOL_ID_OF_Pattern,RConnect );
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 && ch1=='_' ){ // a BlackSequence
            char ch2; bool e2=getChar(ch2,2);
            if ( e2 && ch2=='_' ){ // a BlackNullSequence
              char ch3;
              bool e3=getChar(ch3,3);
              if ( e3 && !evawiz::SpecialCharecterQ(ch3) ){
                rollBack(-2);
                OPERATOR_PUSH( SYMBOL_ID_OF_BlackNullSequence,RRight );
                break;
              }
              rollBack(-2);
              Object plist; plist.SetList( SYMBOL_ID_OF_BlackNullSequence );
              listStack.PushBackRef(plist);
              break;
            }else if (e2 && !evawiz::SpecialCharecterQ(ch2) ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_BlackSequence,RRight );
              break;
            }
            rollBack(-1);
            Object plist; plist.SetList( SYMBOL_ID_OF_BlackSequence );
            listStack.PushBackRef(plist);
            break;
          }else if ( e1 && !evawiz::SpecialCharecterQ(ch1) ){
            OPERATOR_PUSH( SYMBOL_ID_OF_Black,RRight );
            break;
          }
          Object plist; plist.SetList( SYMBOL_ID_OF_Black );
          listStack.PushBackRef(plist);
          break;
        }
        case '#':{ //comment out
          while ( getChar(ch) && ch != '\n' );
          if ( ch == '\n' )
            rollBack(1);
          break;
        }
        case '@':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '@' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_PrefixIn,RConnect );
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Prefix,RConnect );
          break;
        }
          /*
        case '`':{
          if ( !listStack.last().SymbolQ() ){
            ThrowError("Syntax","` need to connect two Atom Symbol.");
          }
          getChar(ch,1);
          if ( !evawiz::SpecialCharecterQ(ch) ){
            getChar(ch);
            while ( !evawiz::SpecialCharecterQ(ch) ){
              currentWord.append(1,ch);
              if (!getChar(ch))break;
            }
            rollBack(1);
            EvaObj  *module = listStack.Last();
            EvaObj  *function = new EvaObj(SymbolT,currentWord);
            FunctionKey *funcKey = new FunctionKey(module,function);
            listStack.Last() = funcKey;
          }else{
            ThrowError("Syntax","` need to connect two Atom Symbol.");
          }
          break;
        }
          */
          /////////////////////////////////////////////////////
        case '=':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 && ch1=='=' ){
            rollBack(-1);
            OPERATOR_PUSH( SYMBOL_ID_OF_Equal,RConnect );
            break;
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Set,RConnect );
          break;
        }
        case ':':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if ( ch1=='=' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_SetDelayed,RConnect );
              break;
            }
            if ( ch1 == '>' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_RuleDelayed,RConnect );
              break;
            }
            if ( ch1 == ':' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_Span,RConnect );
              break;
            }
          }
          if ( parentList.ListQ( SYMBOL_ID_OF_Part ) ){
            OPERATOR_PUSH( SYMBOL_ID_OF_Span,RConnect );
          }else if ( parentList.ListQ( SYMBOL_ID_OF_Dict) ){
            OPERATOR_PUSH( SYMBOL_ID_OF_Rule,RConnect );
          }else{
            OPERATOR_PUSH( SYMBOL_ID_OF_Pattern,RConnect );
          }
          break;
        }
          /////////////////////////////////////////////////////
        case '+':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if ( ch1 == '+' ){
              if ( listStack.Empty() ){
                OPERATOR_PUSH( SYMBOL_ID_OF_PreIncrement,RRight );
              }else {
                OPERATOR_PUSH( SYMBOL_ID_OF_Increment,RLeft );
              }
              rollBack(-1);
              break;
            }
            if ( ch1 == '=' ){
              OPERATOR_PUSH( SYMBOL_ID_OF_AddTo,RConnect );
              rollBack(-1);
              break;
            }
          } else if ( listStack.Empty() ) break; // if + at the first place soly, just ignore it
          OPERATOR_PUSH( SYMBOL_ID_OF_Plus,RConnect );
          break;
        }
        case '-':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '-' ){
              if ( listStack.Empty() ){
                OPERATOR_PUSH( SYMBOL_ID_OF_PreDecrement,RRight );
              } else {
                OPERATOR_PUSH( SYMBOL_ID_OF_Decrement,RLeft );
              }
              rollBack(-1);
              break;
            }
            if ( ch1 == '=' ){
              OPERATOR_PUSH( SYMBOL_ID_OF_SubstractFrom,RConnect );
              rollBack(-1);
              break;
            }
            if ( ch1 == '>' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_Rule,RConnect );
              break;
            }
            if ( ch1 == '.' or ( ch1 >= '0' and ch1 <= '9' ) ){
              if (listStack.Empty() or
                  ( listStack.Last().state() == RConnect or
                    listStack.Last().state() == RRight ) ) {
                currentWord = '-';
                rollBack(-1);
                getRecentNumberString( currentWord,ch1);
                listStack.PushBackNumber( ToNumber( currentWord.c_str() ) );
              }else{
                OPERATOR_PUSH( SYMBOL_ID_OF_Plus,RConnect );
                currentWord = '-';
                rollBack(-1);
                getRecentNumberString( currentWord,ch1);
                listStack.PushBackNumber( ToNumber( currentWord.c_str() ) );
              }
              break;
            }
          }
          // check if is a number | negative number
          if ( listStack.Empty() ){ // a minus at the beginning
            listStack.PushBackNumber(-1);
            OPERATOR_PUSH( SYMBOL_ID_OF_Times,RConnect );
            break; // if + at the first place soly, just ignore it
          }else{
            OPERATOR_PUSH( SYMBOL_ID_OF_Plus,RConnect );
            listStack.PushBackNumber( -1 );
            OPERATOR_PUSH( SYMBOL_ID_OF_Times,RConnect );
          }
          break;
        }
        case '*':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              OPERATOR_PUSH( SYMBOL_ID_OF_TimesBy,RConnect );
              rollBack(-1);
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Times,RConnect );
          break;
        }
        case '/':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              OPERATOR_PUSH( SYMBOL_ID_OF_DivideBy,RConnect );
              rollBack(-1);
              break;
            }
            if (ch1 == '.' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_ReplaceAll,RConnect );
              break;
            }
            if (ch1 == '/'){
              char ch2; bool e2 =getChar(ch2,2);
              if ( e2 && ch2 == '.' ){
                rollBack(-2);
                OPERATOR_PUSH( SYMBOL_ID_OF_ReplaceRepeated,RConnect );
                break;
              }
              OPERATOR_PUSH( SYMBOL_ID_OF_Postfix,RConnect );
              rollBack(-1);
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Times,RConnect );
          OPERATOR_PUSH( SYMBOL_ID_OF_Reciprocal,RRight );
          break;
        }
        case '\\':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if ( ch1 == '\n' ){
              rollBack(-1);// just ignore the \ and \n to join two lines.
              break;
            }else if ( ch1 == '[' ){
              listStack.PushBackSymbol("Escape");
              break;
            }
          }
          Warning("ImportList","Syntax","\\ is not allowed to use alone.");
          break;
        }
        case '%':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              OPERATOR_PUSH( SYMBOL_ID_OF_ModBy,RConnect );
              rollBack(-1);
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Mod,RConnect );
          break;
        }
          /////////////////////////
        case '^':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              OPERATOR_PUSH( SYMBOL_ID_OF_PowerWith,RConnect );
              rollBack(-1);
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Power,RConnect );
          break;
        }
          /////////////////////////
        case '!':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_UnEqual,RConnect );
              break;
            }
          }
          if ( listStack.Empty() || listStack.Last().state() == RLeft ) {
            OPERATOR_PUSH( SYMBOL_ID_OF_Not,RRight );
          }else{
            OPERATOR_PUSH( SYMBOL_ID_OF_Factorial,RLeft );
          }
          break;
        }
        case '|':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '|' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_Or,RConnect );
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Alternatives,RConnect );
          break;
        }
          /////////////////////////
        case '<':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_LessEqual,RConnect );
              break;
            }
            if ( ch1 == '<' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_Get,RRight );
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Less,RConnect );
          break;
        }
        case '>':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_GreaterEqual,RConnect );
              break;
            }
            if ( ch1 == '>' ){
              char ch2; bool e2 = getChar(ch2,2);
              if ( e2 && ch2 == '>' ){
                rollBack(-2);
                while (  getChar(ch) ){
                  currentWord.append(1,ch);
                  if ( currentWord.size() >=3 &&
                       *(currentWord.rbegin()) == '<' &&
                       *(currentWord.rbegin()+1) == '<' &&
                       *(currentWord.rbegin()+2) == '<' ){
                    currentWord.erase( currentWord.end()-3,currentWord.end() );
                    break;
                  }
                }
                if ( listStack.Size() == 0 or not listStack.Back().SymbolQ() ){
                  listStack.PushBackString( currentWord );
                  break;
                }
                Object argsList;
                argsList.SetList( listStack.Back().ids() );
                argsList.PushBackString( currentWord );
                if ( not ( listStack.Empty() ||
                           ( listStack.Last().state() == RConnect ||
                             listStack.Last().state() == RRight ))
                     ){
                  OPERATOR_PUSH( SYMBOL_ID_OF_ArgsFollow,RConnect );
                }
                listStack.PushBackRef( argsList );
                break;
              }
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_Put,RConnect );
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_Greater,RConnect );
          break;
        }
        case '&':{
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '&' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_And,RConnect );
              break;
            }
          }
          OPERATOR_PUSH( SYMBOL_ID_OF_PureFunction,RLeft);
          break;
        }
        case '?':{
          /*
          char ch1; bool e1=getChar(ch1,1);
          if ( e1 ){
            if (ch1 == '=' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_LessEqual,RConnect );
              break;
            }
            if ( ch1 == '<' ){
              rollBack(-1);
              OPERATOR_PUSH( SYMBOL_ID_OF_Get,RRight );
              break;
            }
          }
          */
          OPERATOR_PUSH( SYMBOL_ID_OF_Help,RRight);
          break;
        }

        case '$':{
          bool e=getChar(ch,1);
          if ( e && ch == '_' ){
            rollBack(-1);
            Object var; var.SetSymbol( SYMBOL_ID_OF___Variable );
            bool e=getChar(ch,1);
            if ( e && ch>='1' && ch<='9' ){
              rollBack(-1);
              while ( ch >= '0' && ch <='9' ){
                currentWord.append(1,ch);
                if (!getChar(ch)) break;
              }
              rollBack(1);
              var.set_re( ToNumber(currentWord.c_str() ) );
            }else{
              var.set_re( 1 );
            }
            listStack.PushBackRef(var);
            break;
          }else if ( e && ch == '$' ){
            char ch1;
            bool e1 = getChar(ch1,2);
            if ( e1 && ch1 == '_' ){
              rollBack( -2 );
              Object code; code.SetSymbol( SYMBOL_ID_OF_SerialCode );
              e=getChar(ch,1);
              if ( e && ch>='1' && ch<='9' ){
                rollBack(-1);
                while ( ch >= '0' && ch <='9' ){
                  currentWord.append(1,ch);
                  if (!getChar(ch)) break;
                }
                if ( ch == '_' ){
                  u_int first = ToNumber( currentWord.c_str() );
                  currentWord = "";
                  e=getChar(ch);
                  while ( e and ch >= '0' and ch <='9' ){
                    currentWord.append(1,ch );
                    getChar(ch);
                  }
                  rollBack(-1);
                  u_int second = ToNumber( currentWord.c_str() );
                  code.set_idx( Index(first,second) );
                }else{
                  rollBack(1);
                  code.set_re( ToNumber(currentWord.c_str() ) );
                }
              }else{
                code.set_re( 1 );
              }
              listStack.PushBackRef(code);
              break;
            }
          }

          //else be a normal and part of a word
          ch = '$';
        }

          /////////////////////////other charactors (forming symbols)
        default:
          { 
            if ( evawiz::SpecialCharecterQ(ch) and ch!='$' ){
              ThrowError("Syntax","Character \'"+ToString(ch)+"\' is reserved.");
            }
            currentWord.append(1,ch);
            char ch1;
            if (getChar(ch) ){
              while ( ( not evawiz::SpecialCharecterQ(ch) ) or
                      (ch == '$' and not (getChar(ch1,1) and ch1 == '_') )     ){
                currentWord.append(1,ch);
                if (!getChar(ch)) break;
              }
              //listStack.PushBack(SymbolT,currentWord);
              listStack.PushBackSymbol( currentWord.c_str() );
              rollBack(1);
              break;
            }
            listStack.PushBackSymbol(currentWord.c_str() );
            break;
          }//default
        }//end  of switch case
    }//end of if(getChar)..

    //////////////////////////// reduce the expression if can and need
    if ( expressionEnd || listEnd || End() ){//insert proper times into stack
      if ( listStack.Empty() ){
        if ( !parentList.Empty() )
          parentList.PushBackNull();
      }else{
        for ( u_int i = 1; i< listStack.Size(); i++ ){
          if ( listStack[i].state()>0 ){
            if (listStack[i].state() ==RUnit || listStack[i].state() == RLeft ){
              if ( listStack[i+1].state() > 0 ){
                if ( listStack[i+1].state() == RUnit ||
                     listStack[i+1].state() == RRight )
                  listStack.Insert(i+1,timesOp); i++;
              }else{
                listStack.Insert(i+1,timesOp); i++;
              }
            }
          }else{ // stack i != Operator
            if ( listStack[i+1].state()>0 ){
              if ( listStack[i+1].state() == RUnit || listStack[i+1].state() == RRight ){
                listStack.Insert(i+1,timesOp); i++;
              }
            }else{
              listStack.Insert(i+1,timesOp); i++;
            }
          }
        }

        ////////////////////////////////////////////// add a null to the start or end if needed
        if ( listStack[1].state()>0 && listStack[1].state() != RUnit && listStack[1].state() != RRight )
          listStack.InsertNull( 1 );
        if ( listStack.Last().state() > 0 && listStack.Last().state() != RUnit
             && listStack.Last().state() != RLeft )
          listStack.InsertNull( listStack.Size()+1 );
        //dout<<" listStack after treated: "<<listStack<<endl;
        ////////////////////////////////////////////// reducement
        u_int p = listStack.Size(),ppre,pnex;
        pair<int,int> prePri,pPri;
        while ( listStack.Size()>=2 ){ // find reduce operator according to the priority
          if ( !(listStack[p].state() > 0) ) p --;// either of the two must be an operator.
          ppre = p-1; while ( ppre >=1 && listStack[ppre].state() == 0 ) ppre --;
          pnex = p+1; while ( pnex <= listStack.Size() && listStack[pnex].state() == 0 ) pnex++;
          if ( ppre > 0 ){
            //dout<<"checking Priority..."<<endl;
            //dout<< (int)listStack[p].state() << " ?= "<< RRight<<endl;
            if( listStack[p].state() != RRight ){
              //dout<<"try to get priority"<<endl;
              prePri =  pair<int,int>(GlobalPool.Symbols.GetPriorityLeft( listStack[ppre].ids() ),
                                      GlobalPool.Symbols.GetPriorityRight( listStack[ppre].ids() ) );

              pPri =  pair<int,int>(GlobalPool.Symbols.GetPriorityLeft( listStack[p].ids() ),
                                      GlobalPool.Symbols.GetPriorityRight( listStack[p].ids() ) );
              if( prePri.second <= pPri.first  or listStack[ppre].state() == RLeft ){
                p--;
                continue;
              }
            }else{
              //dout<<"they are the same"<<endl;
            }
          }
          //dout<<"pPri = "<<pPri.first<<","<<pPri.second<<endl;
          //dout<<"prePri = "<<prePri.first<<","<<prePri.second<<endl;
          //dout<<listStack[p].ids()<<endl;
          //dout<<SYMBOL_ID_OF_Plus<<endl;
          //dout<< GlobalPool.Symbols.GetPriorityLeft( SYMBOL_ID_OF_Plus )<<endl;
          //dout<< GlobalPool.Symbols.GetPriorityRight( SYMBOL_ID_OF_Plus )<<endl;
          //dout<<"p = "<<p<<", ppre = "<<ppre<<", pnex = "<<pnex<<endl;
          //dout<<" reduce at p: "<<p<<endl;
          {//reduce at the position
            u_int oper = listStack[p].ids();
            u_char conp = listStack[p].state();
            //dout<<"  current oper is "<<listStack[p]<<" with connect code "<<(int)conp<<endl;
            //dout<<" Unit: "<<RUnit<<" Left: "<<RLeft<<" Right: "<<RRight<<" Connect: "<<RConnect<<endl;
            // special form
            if ( oper == SYMBOL_ID_OF_Reciprocal ){
              Object newlist; newlist.SetList( SYMBOL_ID_OF_Power ); 
              newlist.PushBackRef( listStack( p+1 ) );
              listStack.Delete( p ); 
              newlist.PushBackNumber( -1 );
              listStack(p) = newlist;
            }else if (oper == SYMBOL_ID_OF_ArgsFollow ){
              //listStack[p+1][0].SetObjectRef( listStack(p-1) ); 
              listStack.Delete(p);
              listStack.Delete(p-1);
            }else if (oper == SYMBOL_ID_OF_PartArgsFollow ){
              listStack[p+1].InsertRef(1, listStack[p-1] );
              listStack.Delete(p);
              listStack.Delete(p-1);
            }else if ( oper == SYMBOL_ID_OF_Postfix ){
              listStack[p+1].Deepen();
              listStack[p+1].PushBackRef( listStack[p-1] );
              listStack.Delete(p);
              listStack.Delete(p-1);
            }else if ( oper == SYMBOL_ID_OF_Prefix ){
              listStack[p-1].Deepen();
              listStack[p-1].PushBackRef( listStack[p+1] );
              listStack.Delete(p+1);
              listStack.Delete(p);
            }else if ( oper == SYMBOL_ID_OF_Span ){
              //dout(p);
              Object spanList = listStack[p];
              spanList.SetList( SYMBOL_ID_OF_Span ); spanList.set_state(0);
              if ( p == 1 ){ //::*
                spanList.PushBackSymbol( SYMBOL_ID_OF_$START$ );
              }else if ( p == 2) { //a::*
                spanList.PushBackRef( listStack[1] );
                listStack.Delete(1);
              }else{//**::* not right form
                ThrowError("ImportList","Span specification is not right."); 
              }
              if ( listStack[2].SymbolQ( SYMBOL_ID_OF_Span ) ){ //*:: ::*
                spanList.PushBackSymbol( SYMBOL_ID_OF_$END$ );
                listStack.Delete(2);
                if ( listStack.Size() == 1 ){//*:: ::
                  //spanList.PushBackNumber( 1 );
                }else{//*:: ::a
                  spanList.PushBackRef(listStack(2));
                  listStack.Delete(2);
                }
              }else{ //listStack[2] != Span   *::a*
                spanList.PushBackRef(listStack(2));
                listStack.Delete(2);
                if ( listStack.Size() == 1 ){ //*::a End
                  //spanList.PushBackNumber( 1 );
                }else if ( listStack[2].SymbolQ( SYMBOL_ID_OF_Span ) ){ //*::a::*
                  listStack.Delete(2);
                  if (listStack.Size() == 2){ //*::a::b End
                    spanList.PushBackRef(listStack(2));
                    listStack.Delete(2);
                  }else if (listStack.Size() == 1){ //*::a::b End
                    //spanList.PushBackNumber( 1 );
                  }else{ // *::a::b* Extra things Error
                    ThrowError("ImportList","Span specification is not right.");
                  }
                }
              }

            }else{ // normal form
              if ( conp == RConnect ){
                //dout<<"Step into RConnect"<<endl;
                //dout<<listStack[p]<<endl;
                listStack[p].set_state(0);
                listStack[p].ToList(); 
                //dout<<"After ToList:"<<listStack[p]<<endl;
                listStack[p].PushBackRef( listStack[p-1] );
                listStack[p].PushBackRef( listStack[p+1] );
                //dout<<"After PushBackRef listStack "<<listStack<<endl;
                listStack.Delete(p+1);
                //dout<<"listStack after treated "<<listStack<<endl;
                //dout<<"pPri = "<<pPri.first<<","<<pPri.second<<endl;
                if ( pPri.first == pPri.second && pPri.first != 0 ){
                  //dout<<"try flatten"<<endl;
                  //dout<<p+2<<","<<listStack.Size()<<endl;
                  //dout<<(int)listStack[p+1].state()<<","<<RConnect<<endl;
                  //dout<<listStack[p+1].SymbolId()<<","<<oper<<endl;
                  while ( p+2 >= listStack.Size() &&
                          listStack[p+1].state() == RConnect &&
                          listStack[p+1].SymbolId() == oper
                          ){
                    //dout<<"flatten..."<<endl;
                    listStack[p].PushBackRef( listStack(p+2) );
                    listStack.Delete(p+2);
                    listStack.Delete(p+1);
                  }
                  //dout<<"after flattern: "<<listStack<<endl;
                }
                listStack.Delete(p-1);
                //dout<<" A Connect Operator treated:"<<listStack<<endl;
              }else if ( conp == RLeft ){
                listStack[p].ToList(); listStack[p].set_state(0);
                listStack[p].PushBackRef( listStack(p-1) );
                listStack.Delete(p-1);
              }else if ( conp == RRight ){
                listStack[p].ToList(); listStack[p].set_state(0);
                listStack[p].PushBackRef( listStack(p+1) );
                listStack.Delete(p+1);
                p++;
              }else if ( conp == RUnit  ){
                listStack[p].ToList(); listStack[p].set_state(0);
              }else{
                ThrowError("ImportList","Operator connection relationship fault.");
              }
            }//normal form reduce end
            while ( p > listStack.Size() ) p--;
            ///////////
          }// end of reduce at the position
        } //end of reduce,end of while size()>1
        /////////////////////////////////////////////// get final result
        if ( listStack.Size()>1 ){
          ThrowError("Syntax","Expression cannot be reduced.");
        }
        parentList.PushBackRef( listStack(1) );// 0 is its head, 1st is its first child list
        listStack.Delete(1);
      }
      if ( listEnd || End() ){
        /*
        while ( parentList->Size()>0 && (*parentList->Last()).NullQ() )
          parentList->Delete(parentList->End()-1,parentList->End());
        */
        return 0;
      }
    }//end of if *End
  }//end of while
  /*
  }catch (const exception &err){
    ThrowError("ImportList","Interpretor can not deal with the expression.");
  }
  */

}
#undef OPERATOR_PUSH

Object ImportList::GetList(string str,int depth){
  ImportList impo(str);
  Object lsuper; lsuper.SetList( SYMBOL_ID_OF_List );
  //dout<<"Try the core GetList to fill"<<lsuper<<endl;
  impo.GetList(lsuper,depth);
  return lsuper;
}

Object ImportList::ToExpression(string str){
  //dout<<"Step into ToExpression"<<endl;
  Object newsuper = ImportList::GetList(str,1);
  //dout<<"Get Expression "<<newsuper<<endl;
  Object newlist;
  if ( newsuper.Size() >0 ){
    newlist = newsuper.ElementAt(1);
  }
  return newlist;
}







