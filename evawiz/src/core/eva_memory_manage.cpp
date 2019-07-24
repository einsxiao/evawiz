#include<eva_headers.h>

using namespace evawiz;


EvaMemoryPool::EvaMemoryPool(){
#define _Init_Sym(sym,pL,pR) if ( SYMBOL_ID_OF_##sym != Symbols.GetOrNew(#sym,pL,pR) ) ThrowError("EvaMemoryPool",(string)"Error occured while initialize Symbol '"+#sym+"'. Macro id and in time id is different.");
  _Init_Sym(Null,0,0);                      
  _Init_Sym(List,0,0);                      
  _Init_Sym(Dict,0,0);                      
  _Init_Sym(Sequence,0,0);                  
  //////////////////////////////////////
  _Init_Sym(Slot,35,34);                   
  _Init_Sym(Compound,0,0);               
  _Init_Sym(ArgsFollow,37,0);              
  _Init_Sym(PartArgsFollow,37,0);          
  _Init_Sym(ParenthesisFollow,37,0);       
  /////////////////// 30 is the smallest level
  _Init_Sym(Prefix,48,44);                 
  _Init_Sym(PrefixIn,48,44);               
  ///////////////////        
  _Init_Sym(Black,45,46);                   
  _Init_Sym(BlackSequence,45,46);           
  _Init_Sym(BlackNullSequence,45,46);       
  _Init_Sym(Pattern,47,48);                 
  ///////////////////
  ///////////////////
  _Init_Sym(Get,50,51);                     
  ///////////////////
  _Init_Sym(Increment,55,0);                
  _Init_Sym(PreIncrement,0,55);             
  _Init_Sym(Decrement,55,0);                
  _Init_Sym(PreDecrement,0,55);             
  ///////////////////
  _Init_Sym(Factorial,60,0);                
  _Init_Sym(Power,70,69);                   
  ///////////////////
  _Init_Sym(Reciprocal,0,80);              
  ///////////////////
  _Init_Sym(Times,100,100);                 
  _Init_Sym(Divide,100,99);                 
  _Init_Sym(Mod,100,99);                    
  _Init_Sym(Dot,100,99);                    
  ///////////////////
  _Init_Sym(Plus,110,110);                  //
  _Init_Sym(Subtract,110,109);              //
  /////////////////// differ from c++
  _Init_Sym(Greater,130,129);               //
  _Init_Sym(Less,130,129);                  //
  _Init_Sym(GreaterEqual,130,129);          //
  _Init_Sym(LessEqual,130,129);             //
  _Init_Sym(Equal,130,129);                 //
  _Init_Sym(UnEqual,130,130);               //
  ///////////////////
  _Init_Sym(Not,0,168);                     //
  _Init_Sym(And,170,170);                   //
  _Init_Sym(Or,175,175);                    //
  ///////////////////
  _Init_Sym(Alternatives,181,181);          //
  ///////////////////
  ///////////////////
  _Init_Sym(PureFunction,190,0);//          //
  _Init_Sym(Rule,200,199);                  //
  _Init_Sym(RuleDelayed,200,199);           //
  ////////////////////////////
  _Init_Sym(ReplaceAll,205,204);            //
  _Init_Sym(ReplaceRepeated,205,204);       //
  ///////////////////
  _Init_Sym(Postfix,207,48);                //
  ///////////////////
  _Init_Sym(Set,48,210);                    //
  _Init_Sym(SetDelayed,48,210);             //
  _Init_Sym(AddTo,48,210);                  //
  _Init_Sym(TimesBy,48,210);                //
  _Init_Sym(SubtractFrom,48,210);           //
  _Init_Sym(DivideBy,48,210);               //
  _Init_Sym(PowerWith,48,210);              //
  _Init_Sym(ModBy,48,210);                  //
  ///////////////////
  _Init_Sym(Put,220,219);                   //
  ///////////////////
  _Init_Sym(CompoundExpression,230,230);    //
  ///////////////////
  _Init_Sym(Seperator,240,240);             //
  /////////////No symbols but used frequently
  _Init_Sym(Module,0,0);                    //
  _Init_Sym(Function,0,0);                  //
  _Init_Sym(Evaluate,0,0);                  //
  _Init_Sym(Exit,0,0);                      //
  _Init_Sym(ToExpression,0,0);              //
  _Init_Sym(Parenthesis,0,0);               //
  _Init_Sym(Print,0,0);                     //
  _Init_Sym(Replace,0,0);                   //
  _Init_Sym(System,0,0);                    //
  _Init_Sym(If,0,0);                        //
  _Init_Sym(While,0,0);                     //
  _Init_Sym(Do,0,0);                        //
  _Init_Sym(For,0,0);                       //
  _Init_Sym(Log,0,0);                       //
  _Init_Sym(SubstractFrom,0,0);             //
  _Init_Sym(Random,0,0);                    //
  _Init_Sym(Part,0,0);                      //
  _Init_Sym(Complex,0,0);                   //
  _Init_Sym(Span,176,176);                  //
  _Init_Sym(Args,0,0);                      //
  _Init_Sym(__Variable,0,0);                //
  _Init_Sym($START$,0,0);                   //
  _Init_Sym($END$,0,0);                     //
  _Init_Sym(FullForm,0,0);                  //
  _Init_Sym(Take,0,0);                      //
  _Init_Sym(Table,0,0);                     //
  _Init_Sym(HoldOnce,0,0);                  //
  _Init_Sym(HoldPattern,0,0);               //
  _Init_Sym(Matrix,0,0);                    //
  _Init_Sym(Attributes,0,0);                //
  _Init_Sym(True,0,0);                      //
  _Init_Sym(False,0,0);                     //
  _Init_Sym(SimpleSimplify,0,0);            //
  _Init_Sym(Range,0,0);                     //
  _Init_Sym(Foreach,0,0);                   //
  _Init_Sym(Help,50,0);                     //
  _Init_Sym(Conjunct,42,43);                //
  _Init_Sym(Class,0,0);                     //
  _Init_Sym(SerialCode,0,0);                //
#undef _Init_Sym
  is_finalizing = false;


  srand( (int)time(0) );
}

EvaMemoryPool::~EvaMemoryPool(){
  is_finalizing = true;
  //dout<<endl;
  //dout<<"Finalilze EvaMemoryPool"<<endl;
}


__EvaSymbolTable::__EvaSymbolTable(){
  //GetOrNew("Null",0,0);
  ptr = 0;
}

__EvaSymbolTable::~__EvaSymbolTable(){
  //dout<<"   finalize SymbolTable"<<endl;
  for ( auto iter = records.begin(); iter != records.end(); iter++)
    delete (*iter).key;
  //dout<<"   finalize SymbolTable finished"<<endl;
}

u_int __EvaSymbolTable::GetOrNew(const char*key,u_int priLeft,u_int priRight){
  waitAndLock();
  int first=0,final = positions.size()-1, result=-999;
  u_int pos= final+1;
  while ( first <= final ){
    pos = first+(final-first)/2;
    result = compare( key, positions[pos].key );
    if ( result == 0 ){
      if ( priLeft != 0 or priRight != 0 ){
        records[ positions[pos].pos ].pL = priLeft;
        records[ positions[pos].pos ].pR = priRight;
      }
      unLock();
      return positions[pos].pos;
    }else if ( result < 0 ){
      final = pos-1;
    }else{// result > 0
      first = pos+1;
    }
  }
  auto iter_pos = positions.begin() + pos;
  if ( pos < positions.size() and result > 0 ){
    iter_pos ++;
    pos ++;
  }
  recordType rec( strdup( key), pos, priLeft, priRight );
  iter_pos = positions.insert( iter_pos, positionType( rec.key, ptr) );
  records.push_back(rec);
  //update weight
  iter_pos++;
  while ( iter_pos != positions.end() ){
    records[ (*iter_pos).pos ].weight++;
    iter_pos ++;
  }
  unLock();
  return (ptr++);
}

#define declare_ref(obj,id) auto &obj= objs[id.second][id.first]

Index __EvaStringTable::NewString(const char*str){
  Index id;
  id = __EvaTable::New();
  objs[id.second][id.first ] = str;
  return id;
}

Index __EvaStringTable::NewString(Index id){
  Index newid;
  newid = __EvaTable::New();
  objs[newid.second ][newid.first ] = objs[id.second ][id.first]; 
  return newid;
}

Index __EvaListTable::NewList(Index id){
  Index newid = New();
  declare_ref(newlist,newid);
  declare_ref(list,id);
  newlist.resize( list.size() );
  for ( auto iter = newlist.begin(),riter = list.begin() ; iter != newlist.end(); iter++ ,riter++){
    (*iter).SetObject(*riter);
  }
  return newid;
}

void __EvaListTable::FreeList(Index id){
  declare_ref(obj,id);
  obj.clear();
  //dout<<"    free list "<<id<<" size after"<<obj.size()<<endl;
  if ( obj.capacity() > 128 )
    obj.shrink_to_fit();
  waitAndLock(); //should check 
  freeObjs.push_back(id);
  unLock();
}

void __EvaListTable::SetList(Index left, Index right){
  declare_ref(lobj,left);
  declare_ref(robj,right);
  if ( lobj.size() > robj.size() ){
    lobj.erase( lobj.begin()+robj.size(),lobj.end() );
  }else{
    lobj.resize( robj.size() );
  }
  for ( auto iter = lobj.begin(),riter = robj.begin(); iter!= lobj.end(); iter++,riter++){
    (*iter).SetObject( *riter );
  }
  return;
}

void __EvaMatchTable::FreePairs(Index id){
  declare_ref(obj,id);
  obj.clear();
  if ( obj.capacity() > 64 ) obj.resize(16);
  waitAndLock(); //should check 
  freeObjs.push_back(id);
  unLock();
}

#undef declare_ref

