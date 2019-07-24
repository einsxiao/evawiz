
#include<eva_headers.h>

using namespace evawiz;

#define _obj GlobalPool.Objects.objs[ objid.second ][ objid.first ]
#define _sym GlobalPool.Symbols.GetKey( ids() )
#define _str GlobalPool.Strings.Get( idx() )
#define _list GlobalPool.Lists.Get( idx() )

#define __check(t) if ( objid == Index0  ) ThrowError("Object",(string)"Function '"+__FUNCTION__+"' is only applied to none Null Object."); if ( type() != EvaType::t ) ThrowError("Object",(string)"Function '"+__FUNCTION__+"' is only appled to Object with type "+Type2String(EvaType::t)+", while current object is a "+TypeString()+".");
#define __check_not(t) if ( objid == Index0 ) ThrowError("Object",(string)"Function '"+__FUNCTION__+"' is only applied to a none Null Object."); if ( type() == EvaType::t ) ThrowError("Object",(string)"Function '"+__FUNCTION__+"' can not be appled to Object with type "+Type2String(EvaType::t)+".");
#define __check_not_null if ( objid == Index0 ) ThrowError("Object",(string)"Function '"+__FUNCTION__+"' is only applied to a none Null Object.") 
#define __check_state_blocked if ( state() == Object_State_Block ) ThrowError("Object",(string)"Object is blocked with state code. Error to apply Function '"+__FUNCTION__+"'.")

/*
  this is a core function of the whole system. And it is complex too. Some part need spcial care.
  1. when Assignment of case List->Symbol or String->Symbol we must zero the value of _idx, otherwize it will become a referenced symbol by its remainning value. However, it is not.

 */
void Object::SetObject(const Object&other){ 
  if ( other.state() == Object_State_Uncomplete ){
    GlobalPool.Objects.Get(other.objid).set_state(0);
    ThrowError("SetObject","Recursive assignment in a Refrence-Style-Evaluation (such as Conjunct List) is forbiden.");
  }
  if ( objid == Index0 ){
    Malloc();
    _obj = GlobalPool.Objects.Get(other.objid);
    switch ( type() ){
    case EvaType::Number: case EvaType::Symbol:
      return;
    case EvaType::String:
      set_idx( GlobalPool.Strings.New() );
      _str = GlobalPool.Strings.Get( other.idx() );
      return;
    case EvaType::List:
      _obj.set_state(Object_State_Uncomplete);
      set_idx( GlobalPool.Lists.New() );
      GlobalPool.Lists.SetList( idx(), other.idx() );
      _obj.set_state(0);
      return;
    }
    return;
  }
  __check_state_blocked;
  switch ( other.type() ){
  case EvaType::Number: case EvaType::Symbol: //both simple type, no need to set type or other info
    Clear();
    _obj = GlobalPool.Objects.Get( other.objid );
    return;
  case EvaType::String://other is complex String, so need this to deal mem
    switch ( type() ){
    case EvaType::Number: case EvaType::Symbol: // this is simple, only need malloc string 
      _obj = GlobalPool.Objects.Get( other.objid );
      set_idx( GlobalPool.Strings.NewString( other.idx() ) ); 
      return;
    case EvaType::String: //both string, only need to update string value
      _str = GlobalPool.Strings.Get( other.idx() );
      return;
    case EvaType::List:// this is List,need 1. free ori 2. change type 3. update value
      GlobalPool.Lists.FreeList( idx() );
      _obj = GlobalPool.Objects.Get( other.objid );
      set_idx( GlobalPool.Strings.NewString( other.idx() ) );
      return;
    }
  case EvaType::List: // other is complex List,need more complex operation than string
    switch ( type() ){
    case EvaType::Number: case EvaType::Symbol: // this is simple, just malloc new
      _obj = GlobalPool.Objects.Get( other.objid );
      _obj.set_state(Object_State_Uncomplete);
      set_idx( GlobalPool.Lists.New() );
      GlobalPool.Lists.SetList( idx(), other.idx() );
      _obj.set_state(0);
      return;
    case EvaType::String: // this is string, free old, malloc new
      GlobalPool.Strings.Free(idx());
      _obj.set_state(Object_State_Uncomplete);
      set_idx( GlobalPool.Lists.New() );
      GlobalPool.Lists.SetList( idx(), other.idx() );
      _obj.set_state(0);
      return;
    case EvaType::List: // both list, update elements is enough
      _obj.set_state(Object_State_Uncomplete);
      set_idx( GlobalPool.Lists.New() );
      GlobalPool.Lists.SetList( idx(), other.idx() );
      _obj.set_state(0);
      return;
    }
  }
  return;
}

void Object::SetObjectRef(const Object &obj){
  if ( objid != Index0 ){
    Free();
  }
  objid = obj.objid ;
  cnt_incr();
}

void Object::SetNumber(const double num){
  if ( objid == Index0 ){
    Malloc();
    set_type( EvaType::Number );
    set_re( num);
    return;
  }
  __check_state_blocked;
  switch ( type() ){
  case EvaType::Symbol: set_ids( 0 ); break;
  case EvaType::Number: set_re( num); return;
  case EvaType::String: GlobalPool.Strings.Free(idx()); break;
  case EvaType::List: GlobalPool.Lists.FreeList(idx()); break;
  }
  set_type( EvaType::Number );
  set_re( num);
  return;
}

void Object::SetComplex(const complex num){
  SetList( SYMBOL_ID_OF_Complex );
  PushBackNumber( num.re );
  PushBackNumber( num.im );
  return;
}

void Object::SetNumber(const complex num){
  if ( num.realq() ){
    SetNumber( num.re );
    return;
  }
  SetList( SYMBOL_ID_OF_Complex );
  PushBackNumber( num.re );
  PushBackNumber( num.im );
  return;
}

void Object::SetString(const char*str){
  if ( objid == Index0 ){
    Malloc();
    set_type( EvaType::String );
    set_idx(GlobalPool.Strings.NewString(str) );
    return;
  }
  __check_state_blocked;
  switch ( type() ){
  case EvaType::Symbol: set_ids( 0); break;
  case EvaType::Number: break;
  case EvaType::String: _str = str; return;
  case EvaType::List: GlobalPool.Lists.FreeList(idx()); break;
  }
  set_type( EvaType::String );
  set_idx(GlobalPool.Strings.NewString(str) );
  return;
}

void Object::SetString(const string &str){
  return SetString( str.c_str() );
}

void Object::SetSymbol(const u_int id){
  if ( id >= GlobalPool.Symbols.TableSize() )
    ThrowError("Object","Try to SetSymbol with Symbol Id that does not exist.");
  if ( objid == Index0 ){
    Malloc();
    set_type( EvaType::Symbol );
    set_ids( id );
    return;
  }
  __check_state_blocked;
  switch ( type() ){
  case EvaType::Symbol:
    set_ids( id );
    set_idx( Index0 );
    return;
  case EvaType::Number:
    break;
  case EvaType::String:
    GlobalPool.Strings.Free(idx()); break;
  case EvaType::List:
    GlobalPool.Lists.FreeList(idx()); break;
  }
  set_type( EvaType::Symbol );
  set_ids( id );
  set_idx( Index0 );
  return;
}

void Object::SetSymbol(const char*str){
  u_int id =  GlobalPool.Symbols.GetOrNew( str );
  SetSymbol( id );
}

void Object::ReserveSize(const u_int len){
  __check(List);
  _list.reserve(len+1);
}

void Object::SetList(const u_int id){// id is the sym id of its symbol head
  if ( objid == Index0 ){
    Malloc();
    set_type( EvaType::List );
    set_idx( GlobalPool.Lists.New() );
    Object x;
    x.SetSymbol( id );
    _list.push_back( x );
    return;
  }
  __check_state_blocked;
  switch ( type() ){
  case EvaType::Symbol: set_ids( 0 ); break;
  case EvaType::Number: break;
  case EvaType::String: GlobalPool.Strings.Free(idx()); break;
  case EvaType::List:{
    vector< Object > &li = GlobalPool.Lists.Get( idx() );
    li.erase( li.begin() +1, li.end() );
    li[0].SetSymbol(id);
    return;
  }
  }
  set_type( EvaType::List );
  set_idx( GlobalPool.Lists.New() );
  Object x;
  x.SetSymbol( id );
  _list.push_back( x );
  return;
};

void Object::SetList(const char*str){
  u_int id = GlobalPool.Symbols.GetOrNew(str);
  SetList( id );
}

void Object::SetPair(Object&first,Object&second,u_int headSymId){
  SetList( headSymId );
  PushBack(first);
  PushBack(second);
}

void Object::SetPairByRef(Object&first,Object&second,u_int headSymId){
  SetList( headSymId );
  PushBackRef(first);
  PushBackRef(second);
}

void Object::SetTo(u_int p){
  __check(List);
  __check_state_blocked;
  if ( p > Size() )
    ThrowError("Object","Try to SetTo a Element does not exist.");
  Object tobj = ElementAt(p);
  *this = tobj;
  return;
}

string Object::ToRawString()const{
  string res;
  res= evawiz::ToString(objid);
  if ( objid != Index0 ){
    res += "{type="+TypeString()
      +",state="+evawiz::ToString( state() )
      +",code="+evawiz::ToString( code() )
      +",cnt="+evawiz::ToString( cnt() )
      +",ids="+evawiz::ToString( ids() )+
      +",v={"+evawiz::ToString(idx())+","
      +evawiz::ToString(re())+"}}";
  }
  return res;
}

string Object::TypeString()const{
  switch (type()){
  case EvaType::Number: return "Number";
  case EvaType::Symbol: return "Symbol";
  case EvaType::String: return "String";
  case EvaType::List: return "List"; 
  }ThrowError("Object","TypeString","switch");
}
///////////////////////////////////////////////
bool Object::NullQ()const{
  if ( objid == Index0 )
    return true;
  if ( type() == EvaType::Symbol and ids() == 0 )
    return true;
  if ( ( type() == EvaType::String or type() == EvaType::List ) and idx() == Index0 )
    return true;
  return false;
}

bool Object::NullSymbolQ()const{
  if ( objid == Index0 )
    return false;
  if ( type() == EvaType::Symbol and
       ids() == 0 ) return true;
  return false;
}

bool Object::NullObjectQ()const{
  if ( objid == Index0 )
    return true;
  return false;
}

bool Object::AtomQ()const{
  if ( objid == Index0 ) return true;
  if ( type() == EvaType::List )
    return false;
  return true;
}

bool Object::TypeQ(EvaType t)const{
  if ( objid == Index0 ) return false;
  if ( type() == t ) return true;
  return false;
}

///////////////////////////////////////////////
bool Object::SymbolQ()const{
  if ( objid == Index0 ) return false;
  if ( type() == EvaType::Symbol ) return true;
  return false;
}

bool Object::SymbolQ(const char*str)const{
  if ( objid == Index0 ) return false;
  if ( type() == EvaType::Symbol ){
    if ( compare( _sym, str) == 0 )
      return true;
  }
  return false;
}

bool Object::SymbolQ(const u_int id)const{
  if ( objid == Index0 ) return false;
  if ( type() == EvaType::Symbol ){
    if (  ids() == id )
      return true;
  }
  return false;
}

bool Object::ValuedSymbolQ()const{
  if ( objid == Index0 ) return false;
  if ( type() == EvaType::Symbol ){
    if (  ( idx() != Index0 ) and ids() != SYMBOL_ID_OF___Variable and ids() != SYMBOL_ID_OF_SerialCode )
      return true;
  }
  return false;
}

bool Object::NumberQ()const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number:
    return true;
  case EvaType::String: case EvaType::Symbol:
    return false;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) )
      return true;
    return false;
  }
  return false;
}

bool Object::NumberQ(const double nu)const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number:
    return re() == nu;
  case EvaType::String: case EvaType::Symbol:
    return false;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      return _list[1].NumberQ( nu) and _list[2].NumberQ(0);
    }
    return false;
  }
  return false;
}

bool Object::RealQ()const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number:
    return true;
  case EvaType::String: case EvaType::Symbol: case EvaType::List:
    return false;
  }
  return false;
}

bool Object::RealQ(const double nu)const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number:
    return re() == nu;
  case EvaType::String: case EvaType::Symbol: case EvaType::List:
    return false;
  }
  return false;
}

bool Object::NumberQ(const complex nu)const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number:
    return re() == nu.re and nu.im == 0;
  case EvaType::String: case EvaType::Symbol:
    return false;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      return _list[1].NumberQ( nu.re ) and _list[2].NumberQ( nu.im );
    }
    return false;
  }
  return false;
}

bool Object::ComplexQ()const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number: case EvaType::String: case EvaType::Symbol:
    return false;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      return true;
    }
    return false;
  }
  return false;
}

bool Object::ComplexQ(const complex nu)const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number:
    return re() == nu.re and nu.im == 0;
  case EvaType::String: case EvaType::Symbol:
    return false;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      return _list[1].NumberQ( nu.re ) and _list[2].NumberQ( nu.im );
    }
    return false;
  }
  return false;
}

bool Object::IntegerQ()const{
  if ( objid == Index0 ) return false;
  switch ( type() ){
  case EvaType::Number:
    return Math::IntegerQ( re() );
  case EvaType::String: case EvaType::Symbol:
    return false;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      return _list[1].IntegerQ() and _list[2].IntegerQ();
    }
    return false;
  }
  return false;
}

bool Object::StringQ()const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::String ) return false;
  return true;
}

bool Object::StringQ(const char*key)const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::String ) return false;
  if ( compare(_str, key) != 0 ) return false;
  return true;
}

bool Object::StringQ(const Index id)const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::String ) return false;
  if ( compare(_str, GlobalPool.Strings.Get(id) ) != 0 ) return false;
  return true;
}

bool Object::ListQ()const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::List ) return false;
  if ( _list.size() == 0 ) ThrowError("Object","Uncomplete List.");
  return true;
}

bool Object::ListQ(const char* head)const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::List ) return false;
  if ( _list.size() == 0 ) ThrowError("Object","Uncomplete List.");
  if ( not _list[0].SymbolQ( head ) ) return false;
  return true;
}

bool Object::ListQ(u_int headSymId)const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::List ) return false;
  if ( _list.size() == 0 ) ThrowError("Object","Uncomplete List.");
  if ( not _list[0].SymbolQ( headSymId ) ) return false;
  return true;
}

bool Object::ListQ(const Object&obj)const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::List ) return false;
  if ( _list.size() == 0 ) ThrowError("Object","Uncomplete List.");
  if ( not (_list[0] == obj) ) return false;
  return true;
}

bool Object::SimpleListQ()const{
  if ( objid == Index0 ) return false;
  if ( type() != EvaType::List ) return false;
  if ( _list.size() == 0 ) ThrowError("Object","Uncomplete List.");
  if ( not _list[0].SymbolQ() ) return false;
  return true;
}

bool Object::SimpleListQ(const char* head)const{
  return ListQ(head);
}

bool Object::SimpleListQ(const u_int id)const{
  return ListQ(id);
}

bool Object::HeadQ(const char*head)const{
  return compare( head, Head() ) == 0 ;
}

bool Object::HeadQ(const Object&obj)const{
  __check(List);
  return SimpleCompare( obj , _list[0] ) == 0 ;
}

//////////////////////////////////////
const char *Object::Head()const{
  switch ( type() ){
  case EvaType::Symbol: return "Symbol";
  case EvaType::Number: return "Number";
  case EvaType::String: return "String";
  case EvaType::List:
    //dout<< _ElementAt(0).ToString() <<endl;
    if ( _list.size() == 0 ) ThrowError("Object","Uncomplete List.");
    if ( _list[0].type() == EvaType::Symbol ){
      return GlobalPool.Symbols.GetKey( _list[0].ids() );
    }
    return "$ComplicateList$";
  }ThrowError("Object","Head","Switch error");
}

const char* Object::Key()const{
  __check_not_null;
  switch (type()){
  case EvaType::Symbol: return _sym;
  case EvaType::String: return _str.c_str();
  case EvaType::Number: ThrowError("Object","Try to get Key from Number");
  case EvaType::List: return _list[0].Key();
  }ThrowError("Object","Key","Switch error");
}

u_int Object::SymbolId()const{//return 0 if cannot get the id
  if ( objid == Index0 ) return 0;
  switch (type()){
  case EvaType::Symbol: return ids();
  case EvaType::List:{
    return _list[0].SymbolId();
  }
  case EvaType::String: case EvaType::Number:
    return 0;
  }
  return 0;
}

u_int Object::SymbolWeight()const{
  __check(Symbol);
  return GlobalPool.Symbols.GetWeight( ids() );
}

char Object::Boolean()const{
  if ( objid == Index0 ) return 0;
  switch ( type() ){
  case EvaType::Number: if ( re() == 0 ) return 0; else return 1;
  case EvaType::String:
    if ( idx() == Index0 )
      return 0;
    if ( _str == "" ) return 0;
    return 1;
  case EvaType::Symbol:
    if ( ids() == 0 ) return 0;
    if ( ids() == SYMBOL_ID_OF_True ) return 1;
    if ( ids() == SYMBOL_ID_OF_False ) return 0;
    return -1;
      
  case EvaType::List: 
    if ( ( idx() == Index0 ) ) return 0;
    if ( _list.size() == 0 ) ThrowError("Object","Uncomplete List.");
    if ( _list[0].type() == EvaType::Symbol ){
      if (  _list[0].ids() == SYMBOL_ID_OF_List  or 
            _list[0].ids() == evawiz::SYMBOL_ID_OF_Dict ){
        if ( _list.size() > 1 )
          return 1;
        else
          return 0;
      }
      return -1;
    }
    return -1;
  }
  ThrowError("Object","Boolean","Switch Error");
}

double Object::Number()const{
  __check_not_null;
  switch ( type() ){
  case EvaType::Number: return re();
  case EvaType::String: case EvaType::Symbol: break;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      return (double) _list[1];
    }
    break;
  }
  ThrowError("Object","Try to transfer a Non-Number Object to complex number.");
}

complex Object::Complex()const{
  __check_not_null;
  switch ( type() ){
  case EvaType::Number: return re();
  case EvaType::String: case EvaType::Symbol: break;
  case EvaType::List:
    if ( _list[0].SymbolQ( SYMBOL_ID_OF_Complex ) ){
      return complex( (double) _list[1], (double) _list[2] );
    }
    break;
  }
  ThrowError("Object","Try to transfer a Non-Number Object to complex number.");
}

const char* Object::String()const{
  __check(String);
  return _str.c_str();
}
////////////////////////////////////////

int Object::Dimensions(const Object&list,Object&dim,int n){//dim will be claimed by outer context
  if ( n == 0 ) return 1;
  if ( list.NullQ() ) return 1;
  if ( list.type() != EvaType::List ) return 1;
  if ( dim.NullQ() )
    dim.SetList_List();
  dim.PushBackNumber( list.Size() );
  Object dim1;
  if ( Dimensions( list(1),dim1, n-1 )< 0 ){
    dim.SetNull();
    return -1;
  }
  for( u_int i=2; i<= list.Size(); i++ ){
    Object dim2;
    if ( Dimensions( list(i), dim2,n-1)<0 or dim1 != dim2 ){
      dim.SetNull();
      return -1;
    }
  }
  if ( not dim1.NullQ() ){
    dim.InsertRef( dim.End(),dim1.Begin(),dim1.End());
  }
  return 1;
}

u_int Object::ElementsSize()const{
  __check(List);
  return _list.size();
}
u_int Object::Size()const{
  __check(List);
  return _list.size()-1;
}

Object &Object::First(){
  if ( _list.size() < 2 ) ThrowError("Object","Try to get First element form empty list.");
  return _list[1];
}

Object &Object::Last(){
  if ( _list.size() < 2 ) ThrowError("Object","Try to get Last element form an empty list.");
  return _list.back();
}

Object &Object::Back(){
  if ( _list.size() < 2 ) ThrowError("Object","Try to get Last element form an empty list.");
  return _list.back();
}

Object Object::RefObject(){
  if ( type() != EvaType::Symbol ) ThrowError("Object","Try to get RefObject for none Symbol Object.");
  if ( idx() == Index0 ) ThrowError("Object","Try to get RefObject for Symbol without Ref Value.");
  return Object( idx() ) ;
}
  
Object::iterator Object::Begin()const{
  __check(List);
  return _list.begin()+1;
}

Object::iterator Object::End()const{
  __check(List);
  return _list.end();
}

Object::iterator Object::ElementsBegin()const{
  __check(List);
  return _list.begin();
}

Object::iterator Object::ElementsEnd()const{
  __check(List);
  return _list.end();
}

u_int Object::PositionOfIterator(const iterator iter)const{
  __check(List);
  return iter - _list.begin();
}

void Object::Swap(Object&obj){
  _Object temp;
  temp = _obj;
  _obj = GlobalPool.Objects.Get(obj.objid);
  GlobalPool.Objects.Get(obj.objid) = temp;
}

void Object::SwapRef(Object&obj){
  Index tid = obj.objid;
  obj.objid = objid;
  objid = tid;
}

void Object::Flatten(){
  __check(List);
  __check_state_blocked;
  u_int i = 1;
  while ( i<= Size() ){
    if ( ElementAt(i).ListQ( _list[0] ) ){
      InsertRef( Begin()+i, ElementAt(i).Begin(), ElementAt(i).End() );
      //ElementAt(i).DeleteNoFree();
      Delete(i);
    }else{
      i++;
    }
  }
}

void Object::FlattenSequence(){
  __check(List);
  __check_state_blocked;
  u_int i = 1;
  while ( i<= Size() ){
    if ( ElementAt(i).ListQ( evawiz::SYMBOL_ID_OF_Sequence  ) ){
      InsertRef( Begin()+i, ElementAt(i).Begin(), ElementAt(i).End() );
      //ElementAt(i).DeleteNoFree();
      Delete(i);
    }else{
      i++;
    }
  }
}
  
void Object::FlattenListAndSequence(){
  __check(List);
  __check_state_blocked;
  u_int i = 1;
  while ( i<= Size() ){
    if ( ElementAt(i).ListQ( evawiz::SYMBOL_ID_OF_Sequence ) or 
         ElementAt(i).ListQ( _list[0] ) ){
      InsertRef( Begin()+i, ElementAt(i).Begin(), ElementAt(i).End() );
      //ElementAt(i).DeleteNoFree();
      Delete(i);
    }else{
      i++;
    }
  }
}

inline void Object::ForceDeepen(){
  __check_not_null;
  __check_state_blocked;
  Object temp;
  temp.SetObjectRef(*this);
  //dout<<"Before Free: "<<temp<<"  "<<*this<<endl;
  Free();
  //dout<<"After Free: "<<temp<<"  "<<*this<<endl;
  Malloc();
  set_type( EvaType::List );
  set_idx( GlobalPool.Lists.New() );
  _list.push_back( temp );
  //dout<<"push_finished."<<endl;
  //dout<<"After all: "<<*this<<endl;
}

void Object::Deepen(){
  if ( type() == EvaType::Number or type() == EvaType::String )
    ThrowError("Object","Try to Deepen an Object of type "+TypeString()+".");
  ForceDeepen();
}

void Object::ToList(){
  if ( NullObjectQ() ){
    Malloc();
    ForceDeepen();
    return;
  }
  if ( type() == EvaType::Number or type() == EvaType::String )
    ThrowError("Object","Try to make Object of type "+TypeString()+" to List.");
  if ( type() == EvaType::List ) return;
  ForceDeepen();
}

void	Object::PushBackRef(const Object&obj){
  //dout<<"Check type"<<endl;
  __check(List);
  //dout<<"push obj "<<obj<<endl;
  _list.push_back( obj );
  //dout<<"push finished"<<endl;
  //dout<<"size = "<<Size()<<endl;
  //dout<<"head = "<<ElementAt(0)<<endl;
  //dout<<"after push :"<<(*this)<<endl;
}

void Object::PushBack(const Object&obj){
  __check(List)
  Object tobj;
  tobj.SetObject(obj);
  _list.push_back( tobj );
}

void Object::PushBackNumber(const double num){
  __check(List)
  Object obj;
  obj.SetNumber(num);
  _list.push_back( obj );
}

void Object::PushBackComplex(const complex num){
  __check(List)
  Object obj;
  obj.SetComplex(num);
  _list.push_back( obj );
}

void Object::PushBackNumber(const complex num){
  __check(List)
  Object obj;
  obj.SetNumber(num);
  _list.push_back( obj );
}

void Object::PushBackSymbol(const char*key){
  __check(List)
  Object obj;
  obj.SetSymbol(key);
  _list.push_back( obj );
}

void Object::PushBackSymbol(const u_int keyId){
  __check(List)
  Object obj; obj.SetSymbol(keyId);
  _list.push_back( obj );
}

void Object::PushBackString(const char*key){
  __check(List)
  Object obj;
  obj.SetString(key);
  _list.push_back( obj );
}

void Object::PushBackString(const string&key){
  PushBackString( key.c_str() );
}


void Object::PushBackNull(){
  __check(List);
  Object obj ;
  _list.push_back(obj);
}

void Object::Insert(u_int pos,Object&obj){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object temp; temp.SetObject( obj );
  _list.insert(_list.begin()+pos,temp);
}

void Object::Insert(iterator pos,Object&obj){
  __check(List);
  Object temp; temp.SetObject( obj );
  _list.insert(pos,temp);
}

void Object::InsertRef(u_int pos, Object&obj){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  _list.insert(_list.begin()+pos, obj);
}

void Object::InsertRef(iterator pos, Object&obj){
  __check(List);
  _list.insert(pos, obj);
}

void Object::InsertRef(iterator pos, iterator spos, iterator epos){
  __check(List);
  _list.insert(pos,spos,epos);
}

void Object::InsertNumber(u_int pos,double val){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object obj; obj.SetNumber( val );
  _list.insert(_list.begin()+pos, obj);
}

void Object::InsertComplex(u_int pos,complex val){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object obj; obj.SetComplex( val );
  _list.insert(_list.begin()+pos, obj);
}

void Object::InsertNumber(u_int pos,complex val){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object obj; obj.SetNumber( val );
  _list.insert(_list.begin()+pos, obj);
}

void Object::InsertSymbol(u_int pos, char*key){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object obj; obj.SetSymbol( key );
  _list.insert(_list.begin()+pos, obj);
}

void Object::InsertSymbol(u_int pos, u_int key_id){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object obj; obj.SetSymbol( key_id );
  _list.insert(_list.begin()+pos, obj);
}

void Object::InsertString(u_int pos,char *key){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object obj; obj.SetString(key);
  _list.insert(_list.begin()+pos,obj);
}

void Object::InsertNull(u_int pos){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Insert at position out of list range.");
  Object obj;
  _list.insert(_list.begin()+pos,obj);
}

void Object::PopBack(){
  __check(List);
  if ( ElementsSize() ==  1 ) ThrowError("Object","Try to PopBack a empty List Object.");
  _list.pop_back();
}

void Object::Delete(u_int pos){
  __check(List);
  if ( pos > ElementsSize() ) ThrowError("Object","Try to Delete element out of range.");
  _list.erase( _list.begin() + pos);
}

Object::iterator Object::Delete(iterator pos){
  __check(List);
  return _list.erase( pos );
}

void Object::Delete(iterator spos, iterator epos){
  __check(List);
  _list.erase(spos,epos);
}

void Object::DeleteElements(){
  __check(List);
  vector< Object > &li = GlobalPool.Lists.Get( idx() );
  li.erase( li.begin() +1, li.end() );
  return;
}
//////////////////////////////////////////////////////

string Object::ToFullFormString()const{
  if ( objid == Index0 ) return "Null";
  switch ( type() ){
  case EvaType::Number: return evawiz::ToString( re() );
  case EvaType::String: return (string)"\""+_str+"\"";
  case EvaType::Symbol:
    if ( ids() == 0 )
      return "Null";
    if ( ids() == SYMBOL_ID_OF___Variable ){
      return "$_"+evawiz::ToString( (int)re() );
    }else if ( ids() == SYMBOL_ID_OF_SerialCode ){
      return "$$_"+evawiz::ToString( idx().first )+"_"+evawiz::ToString( idx().second );
    }
    return _sym;
  case EvaType::List:{
    string str;
    str = _list[0].ToFullFormString();
    auto iter = _list.begin()+1;
    str +="[";
    if ( iter != _list.end() ){
      str+= (*iter).ToFullFormString(); iter++;
    }
    while ( iter != _list.end() ){
      str+=","+(*iter).ToFullFormString(); iter++;
    }
    return str+"]";
  }//end of case List
  }//end of switch
  ThrowError("Object","Switch","ToFullFormString");
}

string Object::ToString(const Object&parentList)const{
  int levp,levl;
  if ( parentList.NullQ() )
    levp = 1000;
  else{
    levp = min( GlobalPool.Symbols.GetPriorityLeft( parentList.SymbolId() ),
                GlobalPool.Symbols.GetPriorityRight( parentList.SymbolId() ) );
  }
  levl = max( GlobalPool.Symbols.GetPriorityLeft( SymbolId() ),
              GlobalPool.Symbols.GetPriorityRight( SymbolId() ) );
  if ( levp < levl ){
    return "("+ToString()+")";
  }
  return ToString();
}

#define should_be(val) if ( Size()!=(val) ) goto default_label_for_goto;
#define should_gt(val) if ( Size()<=(val) ) goto default_label_for_goto;
#define should_lt(val) if ( Size()>=(val) ) goto default_label_for_goto;
#define CT(key) case evawiz::RSimpleHashCodeOfString(#key)
//refed
string Object::ToString()const{
  //dout<<"Try to print "<<ToRawString()<<endl;
  if ( objid == Index0 ) return "Null";
  string res;
  /*
#ifdef DEBUG
  if ( state() != 0  or code() != 0 ){
    res += "<"+evawiz::ToString( state() )+","+evawiz::ToString( code() )+">";
  }
#endif
  */
  switch ( type() ){
  case EvaType::Number:
    if ( re() < 0 )
      return "("+evawiz::ToString(re())+")";
    else
      return evawiz::ToString(re());
  case EvaType::String:
    return res+"\""+_str+"\"";
  case EvaType::Symbol:{
    if ( ids() == SYMBOL_ID_OF___Variable ){
      return "$_"+evawiz::ToString( (int)re() );
    }else if ( ids() == SYMBOL_ID_OF_SerialCode ){
      return "$$_"+evawiz::ToString( idx().first )+"_"+evawiz::ToString( idx().second );
    }
    /*
#ifdef DEBUG
    if ( ( idx() != Index0 ) ){
      return res+_sym+"(->"+Object(idx()).ToString()+")";
    }
#endif
    */
    return res+ _sym;
  }
  case EvaType::List:{
    if ( _list.size() == 0 )
      ThrowError("Object","Try to transform a List Object to string without head.");
    //dout<<"Try Print List with head '"<<_list[0]<<"'"<<endl;
    //head is not a symbol or is null
    if ( not  _list[0].SymbolQ() or _list[0].NullQ() ){
      //dout<<"List head is not a Symbol"<<endl;
      auto iter = _list.begin()+1;
      if ( _list[0].ListQ() ){
        res += "("+_list[0].ToString()+")"+"[" ;
      }else{
        res += _list[0].ToString()+"[" ;
      }
      if ( iter!=_list.end() ){ res+=(*iter).ToString(); iter++; }
      while (iter!=_list.end()){res+=","+(*iter).ToString(); iter++;}
      return res+"]";
    }
    //dout<<"List head is a Symbol with headId = "<<_list[0].SymbolId()<<endl;
    //head is a symbol
    /*
#ifdef DEBUG
    if ( ids() != 0 ) res+="<"+evawiz::ToString( ids() )+">";
    //res += "<"+evawiz::ToString( Size() )+">";
#endif
    */
    iterator iter;
    switch( SimpleHashCodeOfString( GlobalPool.Symbols.GetKey(_list[0].ids() ) ) ){
      CT(List): { res += "{"; iter=_list.begin()+1; if ( iter!=_list.end() ){res+=(*iter).ToString(); iter++;} while (iter!=_list.end()){res+=","+(*iter).ToString(); iter++;} return res+"}";}
      CT(Dict): { res += "Dict["; iter=_list.begin()+1;
        if ( iter!=_list.end() ){
          while ( iter!= _list.end() and ( not (*iter).ListQ()  or (*iter).Size()< 2 ) ) iter++; 
          if ( iter == _list.end() ){
            return res+"]";
          }
          res+=(*iter)[1].ToString()+"->"+(*iter)[2].ToString();
          iter++;
        }
        while (iter!=_list.end()){
          if ( not (*iter).ListQ()  or (*iter).Size()< 2 ) continue;
          res+=","+(*iter)[1].ToString()+"->"+(*iter)[2].ToString();
          //res+=","+(*iter).ToString();
          iter++;
        }
        return res+"]";
      }
      CT(Part):{ should_gt(1);  res += _list[1].ToString()+"[["+_list[2].ToString(); iter=_list.begin()+3; while (iter!=_list.end()) {res+=","+(*iter).ToString(); iter++;} return res+"]]";}//
      CT(Complex):{ should_be(2); return evawiz::ToString( Complex() ); }
      //<-To_String_Define
      CT(AddTo): { should_be(2); return _list[1].ToString(*this)+"+="+_list[2].ToString(*this); } 
      CT(And): { should_gt(0);  iter=_list.begin()+1; if ( iter!=_list.end() ){res+=(*iter).ToString(*this); iter++;} while (iter!=_list.end()){res+="&&"+(*iter).ToString(*this); iter++;} return res;}
      CT(Black): { should_lt(2); if ( Empty() ) return "_"; return "_"+_list[1].ToString(*this);} 
      CT(BlackNullSequence): { should_lt(2); if ( Empty() ) return "___"; return "___"+_list[1].ToString(*this); } 
      CT(BlackSequence): { should_lt(2); if ( Empty() ) return "__"; return "__"+_list[1].ToString(*this); } 
      CT(Conjunct): {should_be(2);return _list[1].ToString(*this)+"."+(_list)[2].ToString(*this);} 
      CT(CompoundExpression): {if (Empty()) return "";  res+=_list[1].ToString(*this)+";"; for ( u_int i =2; i<Size(); i++) res+=_list[i].ToString(*this)+";"; if ( _list.size() >2 ) res+=_list.back().ToString(*this); return res;} 
      CT(Decrement): { should_gt(1); return _list[1].ToString(*this)+"--"; } 
      CT(DivideBy): {should_be(2);return _list[1].ToString(*this)+"/="+_list[2].ToString(*this);} 
      //CT(Dot): {return _list[1].ToString(*this)+"."+(_list)[2].ToString(*this);}
      CT(Equal): {should_be(2); return _list[1].ToString(*this)+"=="+_list[2].ToString(*this);}
      CT(Factorial):{should_be(1); return _list[1].ToString(*this)+"!";} 
      CT(Greater): {should_be(2); return _list[1].ToString(*this)+">"+_list[2].ToString(*this);} 
      CT(GreaterEqual): {should_be(2); return _list[1].ToString(*this)+">="+_list[2].ToString(*this);} 
      CT(Increment): {should_be(1); return _list[1].ToString(*this)+"++"; } 
      CT(Less): {should_be(2); return _list[1].ToString(*this)+"<"+_list[2].ToString(*this);} 
      CT(LessEqual): {should_be(2); return _list[1].ToString(*this)+"<="+_list[2].ToString(*this);} 
      CT(Mod): {should_be(2); return _list[1].ToString(*this)+"%"+_list[2].ToString(*this);} 
      CT(ModBy): {should_be(2); return _list[1].ToString(*this)+"%="+_list[2].ToString(*this);} 
      CT(NoPrintExpression): {return "";} 
      CT(Not): {should_gt(1); return "!"+_list[1].ToString(*this);} 
      CT(Or): { should_be(2); return _list[1].ToString(*this)+"||"+_list[2].ToString(*this);} 
      CT(Parenthesis): {if (this->ElementsSize() == 0 ) return "()"; res += "("; iter=_list.begin()+1; if ( iter!=_list.end() ){res+=(*iter).ToString(); iter++;} while (iter!=_list.end()){res+=","+(*iter).ToString(); iter++;} return res+")";}
      CT(Pattern): {should_gt(2);res += _list[1].ToString(); if ( not( _list[2].ListQ(SYMBOL_ID_OF_Black)||_list[2].ListQ(SYMBOL_ID_OF_BlackSequence)||_list[2].ListQ(SYMBOL_ID_OF_BlackNullSequence) ) ) return res+=":"+_list[2].ToString(*this); else return res+=_list[2].ToString(*this); } 
      CT(Plus): {should_gt(1); res += _list[1].ToString(*this); for (iter = _list.begin()+2; iter!= _list.end(); iter++) res+="+"+(*iter).ToString(*this); return res;} 
      CT(Power): {should_be(2);  res += _list[1].ToString(*this)+"^"+_list[2].ToString(*this);; return res;} 
      CT(PreDecrement): {should_be(1); return "--"+_list[1].ToString(*this);} 
      CT(PreIncrement): {should_be(1); return "++"+_list[1].ToString(*this);} 
      CT(PureFunction): {should_be(1); return _list[1].ToString(*this)+"&";} 
      CT(ReplaceAll): {should_be(2); return _list[1].ToString(*this)+"/."+_list[2].ToString(*this);} 
      CT(ReplaceRepeated): {should_be(2); return _list[1].ToString(*this)+"//."+_list[2].ToString(*this);} 
      CT(Rule): {should_be(2); return _list[1].ToString(*this)+"->"+_list[2].ToString(*this);} 
      CT(RuleDelayed): {should_be(2); return _list[1].ToString(*this)+":>"+_list[2].ToString(*this);} 
      CT(Set): {should_be(2); return _list[1].ToString(*this)+"="+_list[2].ToString(*this);} 
      CT(SetDelayed): {should_be(2); return _list[1].ToString(*this)+":="+_list[2].ToString(*this);;} 
      CT(Slot): { should_be(1); return "$_"+_list[1].ToString(*this); }
      CT(SubtractFrom): {return "";} 
      CT(Times): {should_gt(1);  if ( Size()<=0 ) return ""; res += _list[1].ToString(*this); for ( u_int i= 2; i<=Size(); i++ ) res+="*"+_list[i].ToString(*this); return res;} 
      CT(TimesBy): {should_be(2); return _list[1].ToString(*this)+"*="+_list[2].ToString(*this);} 
      CT(UnEqual): {should_be(2); return _list[1].ToString(*this)+"!="+_list[2].ToString(*this);} 
    default:
    default_label_for_goto:
      {
        auto iter=_list.begin();
        res += (*iter).ToString(); iter++;
        res += "[";
        if ( iter!=_list.end() ){
          res +=(*iter).ToString(); iter++;
        }
        while (iter!=_list.end()){
          res +=","+(*iter).ToString(); iter++;
        }
        return res+"]";
      }
    }
  }//end of case List
  }//end of switch
  ThrowError("Object","ToString","Switch Error");
};

int Object::SimpleCompare(const Object&l1,const Object&l2){
  if ( l1.objid == l2.objid ) return 0;
  if ( l1.NullQ() ){ if ( l2.NullQ() ) return 0; else return -1;}else if ( l2.NullQ() ) return 1;
  switch ( l1.type() ){
  case EvaType::Number:{
    switch ( l2.type() ){
    case EvaType::Number: return compare( (double)l1, double(l2) );
    case EvaType::String: case EvaType::Symbol: case EvaType::List: return -1;
    }
  }//end of Number
  case EvaType::String:{
    switch ( l2.type() ){
    case EvaType::Number:  return 1;
    case EvaType::String: return compare( string(l1) ,string(l2) );
    case EvaType::Symbol: case EvaType::List: return -1;
    }
  }//end of String
  case EvaType::Symbol:{
    switch ( l2.type() ){
    case EvaType::Number: case EvaType::String: return 1;
    case EvaType::Symbol: return compare( GlobalPool.Symbols.GetWeight(l1.ids()),GlobalPool.Symbols.GetWeight(l2.ids()) );
    case EvaType::List: return -1;
    }
  }//end of Symbol
  case EvaType::List:{
    switch ( l2.type() ){
    case EvaType::Number: case EvaType::String: case EvaType::Symbol: return 1;
    case EvaType::List:{
      auto iter1 = l1.ElementsBegin();
      auto iter2 = l2.ElementsBegin();
      while ( iter1 != l1.End() && iter2 != l2.End() ){
        if ((*iter1).NullQ() ){
          if ( not (*iter2).NullQ() )
            return -1;
        }else{
          if ( (*iter2).NullQ() ) return 1;
          else{
            return_if_not(SimpleCompare(*iter1,*iter2),0);
          }
        }
        iter1++; iter2++;
      }
      if ( iter1 != l1.End() ) return 1;
      if ( iter2 != l2.End() ) return -1;
      return 0;
    }//end of ..List
    }
  }//end of Symbol
  }//end of switch
  ThrowError("Object","SimpleCompare","switch error");
}

#define exp_return(res) ({dprintf("%d, %s ?<> %s: %d",__LINE__,l1.ToString().c_str(),l2.ToString().c_str(), res); return res;})
int Object::ExpressionCompare(const Object&l1,const Object&l2){
  if ( l1.objid == l2.objid ) return(0);
  if ( l1.NullQ() ){ if ( l2.NullQ() ) return 0; else return -1;}else if ( l2.NullQ() ) return 1;

  //dprintf("%s ?<> %s",l1.ToString().c_str(), l2.ToString().c_str() );
  switch ( l1.type() ){
  case EvaType::Number:{
    switch ( l2.type() ){
    case EvaType::Number: return(compare( (double)l1, double(l2) ));
    case EvaType::String: case EvaType::Symbol: return(-1);
    case EvaType::List:{
      ///////////////////////
#define l2_LIST_SITUATION_DEALING                                       \
      if ( l2.ListQ( evawiz::SYMBOL_ID_OF_Times  ) and l2.Size() > 0 ){ \
        u_int p = 1;                                                    \
        while ( p< l2.Size() and l2(p).NumberQ() ) p++;                 \
        int res = ExpressionCompare( l1, l2(p) );                       \
        if ( res != 0 ) return( res );                              \
      }else if ( l2.ListQ(evawiz::SYMBOL_ID_OF_Power ) ){               \
        if ( l2.Size()>=1 ){                                            \
          int res = ExpressionCompare( l1, l2(1) );                     \
          if ( res != 0 ) return( res );                            \
        }                                                               \
      }                                                                 
      ///////////////////////
      l2_LIST_SITUATION_DEALING;
      return(-1);
    }
    }
  }//end of Number
  case EvaType::String:{
    switch ( l2.type() ){
    case EvaType::Number:  return(1);
    case EvaType::String: return(compare( string(l1) ,string(l2) ));
    case EvaType::Symbol: return(-1);
    case EvaType::List:{
      l2_LIST_SITUATION_DEALING;
      return(-1);
    }
    }
  }//end of String
  case EvaType::Symbol:{
    switch ( l2.type() ){
    case EvaType::Number: case EvaType::String: return(1);
    case EvaType::Symbol: return(compare( GlobalPool.Symbols.GetWeight(l1.ids()),GlobalPool.Symbols.GetWeight(l2.ids()) ));
    case EvaType::List:{
      l2_LIST_SITUATION_DEALING;
      return(-1);
    }
    }
  }//end of Symbol
#undef l2_LIST_SITUATION_DEALING               
  case EvaType::List:{
    switch ( l2.type() ){
    case EvaType::Number: case EvaType::String: return(1);
    case EvaType::Symbol:{
      // Even if l1 is Times or Power, here will surely bigger than a single symbol
      // no no ,,, previous line is not right, for example: a?<>a^2   a?<>b^2   b?<>a^2
      if ( l1(0).SymbolQ( SYMBOL_ID_OF_Times ) and l1.Size() > 0 ){
        u_int p = 1;
        while ( p < l1.Size() and l1(p).NumberQ() ) p++;
        int res = ExpressionCompare( l1(p), l2 );
        if ( res != 0 ) return( res );
      }else if ( l1(0).SymbolQ( SYMBOL_ID_OF_Power ) ){
        if ( l1.Size() == 2 ){
          int res = ExpressionCompare( l1(1), l2 );
          if ( res != 0 ) return( res );
          return(1);
        }
      }
      return(1);
    }
    case EvaType::List:{
      //dprintf("List Part %s ?<> %s",l1.ToString().c_str(), l2.ToString().c_str() );
      bool isOP1 = false,isOP2=false;
      if ( l1(0).SymbolQ( SYMBOL_ID_OF_Times ) ){
        isOP1 = true;
        if ( l1.Size() < 1 ) goto l1_is_normal_expr_compr_label;
        if ( l2(0).SymbolQ( SYMBOL_ID_OF_Times ) ){// 1: both are times
          isOP2 = true;
          goto oper_expression_compare_label;
        }else if ( l2(0).SymbolQ( SYMBOL_ID_OF_Power) ){ // l1 times  l2 Power
          if ( l2.Size() != 2 ) goto oper_expression_compare_label;
          isOP2 = true;
          u_int p1=1; if ( l1.Size()>=2 and l1(1).NumberQ() ) p1 ++;
          return_if_not( ExpressionCompare( l1(p1), l2(1) ), 0 );
          if ( l1.Size() == p1 )
            return(-1);
          return(1); 
        }
        // l1 is times , l2 are normal
        u_int p1=1; if ( l1.Size()>=2 and l1(1).NumberQ() ) p1 ++;
        return_if_not( ExpressionCompare( l1(p1), l2), 0);
        return(1);
      }else if ( l1(0).SymbolQ( SYMBOL_ID_OF_Power ) ){
        //dprintf("l1 p, l2 ?::: %s ?<> %s",l1.ToString().c_str(), l2.ToString().c_str() );
        isOP1 = true;
        if ( l1.Size() != 2 ) goto l1_is_normal_expr_compr_label;
        if ( l2(0).SymbolQ( SYMBOL_ID_OF_Times ) ){// l1 is times l2 times
          if ( l2.Size() > 0 ){  
            isOP2 = true;
            u_int p2 = 1; if ( l2.Size() >= 2 and  l2(1).NumberQ() ) p2 ++;
            return_if_not( ExpressionCompare( l1(1), l2(p2) ),0 );
            if ( l2.Size() == p2 )
              return(1);
            return(-1); 
          }
        }else if ( l2(0).SymbolQ( SYMBOL_ID_OF_Power) ){ // l1 power l2 Power
          isOP2 = true;
          goto oper_expression_compare_label;
        }
        // l1 is power, l2 are normal
        return_if_not( ExpressionCompare( l1(1), l2),0 );
        //dprintf("l1 p, l2 n::: %s ?== %s",l1.ToString().c_str(), l2.ToString().c_str() );
        return(1); 
      }else{//l1 is normal, test l2
      l1_is_normal_expr_compr_label:
        //dprintf("no diff %s ?<> %s",l1.ToString().c_str(), l2.ToString().c_str() );
        if ( l2(0).SymbolQ( evawiz::SYMBOL_ID_OF_Times  ) ){//l1 normal, l2 is times
          if ( l2.Size() < 1 ) goto normal_expression_compare_label;
          isOP2 = true;
          int p = 1; if ( l2.Size()>1 and l2(1).NumberQ() ) p++;
          return_if_not( ExpressionCompare(l1, l2(p)), 0);
          return( -1 );
        }else if ( l2(0).SymbolQ(evawiz::SYMBOL_ID_OF_Power ) ){
          if ( l2.Size() != 2 ) goto normal_expression_compare_label;
          isOP2 = true;
          return_if_not( ExpressionCompare(l1,l2(1)),0);
          return( -1 );
        }
        //both are normal
        goto normal_expression_compare_label;
      }
      normal_expression_compare_label:
      return_if_not( ExpressionCompare( l1(0), l2(0) ), 0 );
      //ExpressionCompare elements after same head
      //first non_number then numbers but numbers will be smaller
      oper_expression_compare_label:
      //dprintf("normal %s ?<> %s",l1.ToString().c_str(), l2.ToString().c_str() );
      auto iter1 = l1.Begin();
      auto iter2 = l2.Begin();
      // non numbers
      while ( iter1 != l1.End() and (*iter1).NumberQ() ) iter1++;
      while ( iter2 != l2.End() and (*iter2).NumberQ() ) iter2++;
      auto niter1 = iter1; auto niter2 = iter2;
      //dprintf("niter td");
      while ( iter1 != l1.End() and iter2 != l2.End() ){
        if ( (*iter1).NullQ() ){
          if ( (*iter2).NullQ() ){
            continue;
          }else{
            return( -1 );
          }
        }else{
          if ( (*iter2).NullQ() )
            return( 1 );
          //both are not empty
        }
        //dprintf("%s?<>%s",iter1->ToString().c_str(), iter2->ToString().c_str() );
        return_if_not(ExpressionCompare(*iter1,*iter2),0);
        iter1++; iter2++;
      }
      //dprintf("eles td");
      if ( iter1 != l1.End() ) return(1);
      if ( iter2 != l2.End() ) return(-1);
      if ( niter1 != l1.Begin() ){
        if ( niter2 != l2.Begin() ){
          return_if_not(compare( l1(1).Number(),l2(1).Number() ),0 ) ;
        }else{
          return( 1 );// return(1);
        }
      }else{
        if ( niter2 != l2.Begin() ){
          return( -1 );
        }
      }
      //dprintf("%s ?== %s",l1.ToString().c_str(), l2.ToString().c_str() );
      if ( isOP1 or isOP2 )
        return(ExpressionCompare( l1(0),l2(0) ));
      else
        return(0);
    }//end of ..List
    }
  }//end of Symbol
  }//end of switch
  ThrowError("Object","ExpressionCompare","switch error");
}

#undef exp_return

void Object::Sort(const ObjectCompareFunction lcf){
  if ( ElementsSize() > 1 ){
    sort( Begin(),End(), ObjectCompareFunctionLess(lcf) );
  }
}

void Object::SimpleSort(){
  static ObjectCompareFunctionLess lessFunc(SimpleCompare);
  if ( ElementsSize() > 1 ){
    sort(Begin(),End() , lessFunc);
  }
}

void Object::ExpressionSort(){
  static ObjectCompareFunctionLess lessFunc(ExpressionCompare);
  if ( ElementsSize() > 1 ){
    sort(Begin(),End() , lessFunc);
  }
}

bool Object::operator<(const Object&other)const{
  return SimpleCompare( *this, other)< 0 ;
}

bool Object::operator==(const Object&other)const{
  return SimpleCompare(*this,other)==0;
}

bool Object::operator!=(const Object&other)const{
  return SimpleCompare(*this,other)!=0;
}

////////////////////////////////////////////////// 
////////////////////////////////////////////////// 
////////////////////////////////////////////////// 
// no need to deal reference in following functions for all of them are using Referenced Functions
int Object::DictGetPosition(Object&left_value,iterator &iter_pos){
  //dout<<"DictGetPosition: get "<<left_value<<" from table "<<(*this)<<endl;
  int first=1,final = Size(), result=-999;
  u_int pos= final+1;
  while ( first <= final ){
    pos = first+(final-first)/2;
    if ( not ElementAt(pos).ListQ() or ElementAt(pos).Size() < 2 ){
      first = pos+1;
      result = 1;
      continue;
      ThrowError("Object","DictGetPosition encountered none-pair Element.");
    }
    result = SimpleCompare( left_value, ElementAt(pos)(1) );
    if ( result == 0 ){
      iter_pos = ElementsBegin()+pos;
      //  dout<<"find object at "<<pos<<endl;
      return 0;
    }else if ( result < 0 ){
      final = pos-1;
    }else{// result > 0
      first = pos+1;
    }
  }
  //dout<<"find object near "<<pos<<endl;
  if ( pos > Size() ){
    iter_pos = End();
    return -1;
  }
  iter_pos = ElementsBegin()+pos;
  if ( result > 0 ){
    iter_pos ++;
  }
  return -1;
}

Object  Object::DictGet(Object&left_value){
  iterator iter;
  if ( DictGetPosition(left_value,iter) == 0 ){
    return  (*iter)[2];
  }
  return NullObject;
}

Object Object::DictGetOrNew(Object&left_value){
  iterator iter;
  int result;
  if ( (result = DictGetPosition( left_value,iter) ) == 0 ){
    return (*iter)[2];
  }else{
    Object newList; newList.SetList_List();
    newList.PushBack( left_value );
    newList.PushBackNull();
    InsertRef(iter, newList);
    return newList[2];
  }
}

Object Object::DictValueAt(Object&left_value){
  iterator iter;
  if ( DictGetPosition(left_value, iter) == 0 ){
    return (*iter)[2];
  }
  return NullObject;
}

pair<bool,Object>  Object::DictInsert(Object&left_value,Object&right_value ){
  iterator iter;
  int result;
  if ( (result = DictGetPosition( left_value,iter) )== 0 ){
    return pair<bool,Object>(false,NullObject);
  }else{
    Object newList; newList.SetList_List();
    newList.PushBack(left_value);
    newList.PushBack(right_value);
    InsertRef( iter, newList);
    return pair<bool,Object>(true,newList[2]);
  }
}


Object Object::DictInsertOrUpdate(Object&left_value,Object&right_value){
  iterator iter;
  int result;
  if ( ( result = DictGetPosition( left_value,iter) ) == 0 ){
    (*iter)[2] = right_value;
    return (*iter)(2);
  }else{
    Object newList; newList.SetList_List();
    newList.PushBack(left_value);
    newList.PushBack(right_value);
    InsertRef( iter, newList);
    return newList[2];
  }
}

Object Object::DictPop(Object&left_value){
  iterator iter;
  if ( DictGetPosition( left_value,iter) == 0 ){
    Object res = (*iter)[2];
    //DeleteNoFree(iter);
    return res;
  }else{
    return NullObject;
  }
}

bool Object::DictDelete(Object&left_value){
  iterator iter;
  if ( DictGetPosition( left_value,iter) == 0 ){
    Delete(iter);
    return true;
  }else{
    return false;
  }
}

void Object::DictSort(){
  sort( Begin(),End());
}

#undef _obj
#undef _sym
#undef _str
#undef _list

