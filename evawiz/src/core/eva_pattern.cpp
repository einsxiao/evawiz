
#include"eva_headers.h"

using namespace std;
using namespace evawiz;

class __EvaPatternUnifyPairsTable:public __EvaTable< map<u_int,int> >{
public:
  __EvaPatternUnifyPairsTable(){
    tableName = "PatternUnifyPairs";
  };
  void  FreePairs(Index id){
    objs[id.second][id.first].clear();
    waitAndLock(); //should check 
    freeObjs.push_back(id);
    unLock();
  }

private:
  void Free();
};

static __EvaPatternUnifyPairsTable UnifyPairsPool;

Pattern::Pattern(){
};

Pattern::~Pattern(){
};

//the pairs is is   SymbolId to Variable ID, so a map can accerlerate the pair process
// Variable coded from 1, zero are left unused
void localUnifyLeft(Object&list,map<u_int,int>&pairs){
  if ( list.NullQ() ) return;
  if ( list.AtomQ() ) return;
  if ( list.ListQ( SYMBOL_ID_OF_Pattern ) && list.Size() >= 2 ){
    if ( not list[1].SymbolQ() or list[1].ids() == 0 ){
      return;
    }
    // pattern previously occured will auto treated for map will not accept dumplicate value of one key
    auto res = pairs.insert( pair<u_int,int>( list[1].ids(), pairs.size()+1 ) );
    list[1].SetSymbol( SYMBOL_ID_OF___Variable ); list[1].set_re( (*res.first).second );
    return;
  }
  for ( auto iter = list.ElementsBegin(); iter!= list.ElementsEnd(); iter++){
    localUnifyLeft(*iter,pairs);
  }
  return;
}

void localUnifyRight(Object&list,map<u_int,int>&pairs){
  if ( list.NullQ() ) return;
  if ( list.AtomQ() ){
    if ( list.SymbolQ() && list.ids() != 0 and not list.SymbolQ( SYMBOL_ID_OF___Variable ) ){
      auto iter = pairs.find(list.ids());
      if (iter != pairs.end() ){
        list.SetSymbol( SYMBOL_ID_OF___Variable ); list.set_re( (*iter).second );
      }
    }
    return;
  }
  for ( auto iter = list.ElementsBegin(); iter != list.ElementsEnd(); iter++){
    localUnifyRight(*iter,pairs);
  }
  return;
}

// unify a rule is kind of thing that transform expr[x_,y_] -> expr2[x,y] to expr[$_1_,$_2_] -> expr2[$_1,$_2]
int Pattern::UnifyRule(Object&left,Object&right){
  Index idx = UnifyPairsPool.New();
  map<u_int,int> &pairs = UnifyPairsPool.Get(idx);
  localUnifyLeft(left, pairs);
  localUnifyRight(right, pairs);
  int res = pairs.size();
  UnifyPairsPool.FreePairs(idx);
  left.set_code( res );
  return res;
}

int Pattern::UnifyRuleLeft(Object&left){
  Index idx = UnifyPairsPool.New();
  map<u_int,int> &pairs = UnifyPairsPool.Get(idx);
  localUnifyLeft(left, pairs);
  int res = pairs.size();
  UnifyPairsPool.FreePairs(idx);
  left.set_code( res );
  return res;
}

int Pattern::UnifyRule( Object & rule ){
  if ( rule.ListQ(SYMBOL_ID_OF_HoldPattern) && (rule).Size() == 1){
    return UnifyRule(rule[1]);
  }
  if ( not RuleQ(rule) ){
    ThrowError("UnifyRule","Try to unify a non-rule-form Expression.");
  }
  return UnifyRule( rule[1],rule[2] );
}

// the pattern is LEFT part of the unified rule
// Pattern    name_Head?Func/;Func2[name].default
//          or name:
// Pattern[name(could be Null),form(one of black blackSeq. , blackNullSeq.),test_function, test_expr, default]
//              ...                ...                                        ?e                /;e         .e
// pairs 0:nothin  1: VariableT1  2:VariableT2 ...
// Viariable 0: means unamed pattern
// in this function pair_size is the size of valid pairs not the data length of pairs. so acctually pair_size == pairs.size()-1
#define return_false ({pair_size = old_pair_size; return false;})
bool UNIFY_MATCHQ(Object&expr,Object &pat, vector<MatchRecord> &pairs,u_int&pair_size, u_int pos1,u_int pos2,bool isShortestMatch){
  int old_pair_size = pair_size;
  //sdprintf("try match %s by %s, pair_size = %d, pos1=%d, pos2=%d ",expr.ToString().c_str(), pat.ToString().c_str(),pair_size,pos1,pos2);
  //situation I: whole expression match
  if ( pos1 == 0 and pos2 == 0 ){
    /////situation I.1: expr is HoldOnce or HoldPattern
    if ( expr.ListQ() ){
      if ( expr.ListQ( SYMBOL_ID_OF_HoldOnce ) || expr.ListQ(SYMBOL_ID_OF_HoldPattern ) ){
        if ( expr.Size() != 1 ){
          ThrowError("Pattern",expr.ToString() + " is in the wrong form.");
        }
        if ( not UNIFY_MATCHQ( expr[1],pat,pairs,pair_size,0,0,isShortestMatch) )
          return_false;
        return true;
      }
    }
    ///// situation I.2: pat is a Pattern
    if ( pat.ListQ() ){
      if ( pat.ListQ( SYMBOL_ID_OF_HoldOnce ) || pat.ListQ(SYMBOL_ID_OF_HoldPattern ) ){
        if ( pat.Size() != 1 ){
          ThrowError("Pattern",pat.ToString() + " is in the wrong form.");
        }
        if ( not UNIFY_MATCHQ( expr,pat[1],pairs,pair_size,0,0,isShortestMatch))
          return_false;
        return true;
      }
      if ( pat.ListQ( SYMBOL_ID_OF_Pattern ) ){
        if ( pat.Size() < 2 ){
          ThrowError("Pattern",expr.ToString()+" is in the wrong form.");
        }
        unsigned int varI = pat[1].re();
        if ( varI != 0  ){
          if ( varI > pair_size + 1 ){
            ThrowError("Pattern","Unified Pattern is in the wrong form.");
          }
          if ( varI <= pair_size ){ // VariableT already recorded
            if ( pairs[varI].n == 1 && (Object(pairs[varI].idx) == expr ) )
              return true;
            return_false;
          }
          //Pattern not exist, Record new Single match
          //dprintf("record new var %d",varI);
          pair_size ++; while ( pair_size >= pairs.size() ) pairs.push_back( MatchRecord() );
          pairs[varI].n = 1;
          pairs[varI].p = 0;
          pairs[varI].idx = expr.objid;
        }//varI == 0 of unamed situation has the same condition check as named ones.
        //check requirements
        // Head requirement for Black, BlackSequence, BlackNullSequence will be checked
        if ( not UNIFY_MATCHQ(expr, pat[2], pairs,pair_size, 0, 0 ,isShortestMatch) ) 
          return_false;
        if ( pat.Size() > 2 ){//3rd pos is the condition test function
        }
        if ( pat.Size() > 3 ){//4rd pos is the condition test expression contains pattern name
        }
        return true;
      //// situation I.3: pat is one of Black, BlackSequence and BlackNullSequence
      }else if ( pat.ListQ(SYMBOL_ID_OF_Black ) ||
                 pat.ListQ(SYMBOL_ID_OF_BlackSequence ) ||
                 pat.ListQ(SYMBOL_ID_OF_BlackNullSequence ) ){
        if ( pat.Size()<1 ){ // no head requirement
          return true;
        }else{//judge the head in Black[String] whether equal "abc" or Black[Object ]  {a,b,c}
          if ( not pat[1].SymbolQ() ){
            ThrowError("Pattern","Pattern Head should specified in a Symbol.");
          }
          if ( pat[1].SymbolQ( expr.Head() ) )
            return true;
          return_false;
        }
        //end of situation I.3
      }else{//pat is a List, but than Pattern, Black, BlackSequence, BlackNullSequence
        if ( expr.AtomQ() ){//for sure will not match
          return_false;
        }// otherwize test match in situation II
      }
    }else{//pat.AtomQ()
    //// situation I.4: of at least one is atom
      if ( expr.AtomQ() ){
        return Object::SimpleCompare( expr, pat) == 0; 
      }else{
        return_false;
      }
    }
  }//end of situation I

  // situation II
  // both are list. 3 situations
  // 1: pos2 > pat.Size;
  // 2: pos1 > expr.Size expr could be Atom. so Size may not applied.
  // 3: both are not at the end of the list
  // situation II.1: dealing expr or pat comes to the end
  if ( pos2 > pat.Size() ){ 
    if ( pos1 > expr.Size() )
      return true;
    return_false;
  }else if ( pos1 > expr.Size() ){
    // situation II.1: expr is end but pat[pos2] can match null, proceed.
    if ( pat[pos2].ListQ( SYMBOL_ID_OF_BlackNullSequence ) )//last one can represent null and no name 
      return UNIFY_MATCHQ(expr,pat,pairs,pair_size,pos1,pos2+1,isShortestMatch);
    if ( (pat[pos2].ListQ( SYMBOL_ID_OF_Pattern) ) and
         (pat[pos2].Size() >=2 ) and
         (pat[pos2][2].ListQ( SYMBOL_ID_OF_BlackNullSequence ) ) ){//name BlackNullSequence
      u_int varI = pat[pos2][1].re();
      if ( varI != 0  ){ // pattern exist
        if ( varI > pair_size + 1 ){
          ThrowError("Pattern","Coding of unified pattern is not right.");
        }
        if ( varI <= pair_size ){ // VariableT already recorded
          if ( pairs[varI].n != 0 ) // should be null match, otherwize return false
            return_false;
        }else{ // pattern not exist, push a new record represent null match
          pair_size ++;
          while ( pair_size >= pairs.size() )
            pairs.push_back( MatchRecord() );
          pairs[pair_size].n = 0;
        }
      }//unamed pattern, just try next match
      if ( not UNIFY_MATCHQ(expr,pat,pairs,pair_size,pos1,pos2+1,isShortestMatch) ){
        return_false;
      }
      return true;
    }
    return false;
  }
  // situation 3
  // elements
  // BlackSequence and BlackNullSequence
  bool singleq = false;
  bool nullq = false;
  //situation II.2: pat[pos2] is pattern and its content is BlackSequence or BlackNullSequence
  if ( pat[pos2].ListQ( SYMBOL_ID_OF_Pattern )  ){
    if ( pat[pos2].Size() < 2 ){
      ThrowError("Pattern","Pattern is in the wrong form.");
    }
    if ( (singleq=pat[pos2][2].ListQ(SYMBOL_ID_OF_Black ))   ||
         (pat[pos2][2].ListQ( SYMBOL_ID_OF_BlackSequence ))  ||
         (nullq=pat[pos2][2].ListQ(SYMBOL_ID_OF_BlackNullSequence )) ) {
      //////pattern of form Black, BlackNullSequence or BlackSequence
      u_int varI = pat[pos2][1].re();
      if ( varI != 0  ){ // pattern is a named pattern try to record or compare to what already recorded
        if ( varI > pair_size + 1 ){
          //dprintf("error pat: %s",pat.ToString().c_str() );
          //dprintf("varI = %d, pair_size=%d, real pair size = %d",varI,pair_size,(int)pairs.size() );
          ThrowError("Pattern","Coding of unified pattern is not right.");
        }
        if ( varI <= pair_size ){ // VariableT already recorded // pattern Exist
          if ( expr.Size() - pos1 + 1 < pairs[varI].n ) //rest elements are not enough 
            return_false;
          if ( pairs[varI].n == 1 ){
            if ( Object(pairs[varI].idx) != expr[pos1] )
              return_false;
          }else{//null match for nullsequence will processed by for clause
            for ( u_int i = 0; i< pairs[varI].n; i++){
              vector<Object>&tlist=GlobalPool.Lists.Get( pairs[varI].idx );
              if ( pairs[varI].p + i >= tlist.size() ){
                //dprintf("\nexpr=%s,\npat=%s, \n pair_size=%d pos1=%d, pos2=%d \n n=%d,size=%d varI=%d ", expr.ToString().c_str(), pat.ToString().c_str(), pair_size, pos1, pos2, pairs[varI].n, (int)tlist.size(),(int)varI );
                ThrowError("MatchQ","recorded matched list index out of range.");
              }
              if ( tlist[ pairs[varI].p+i ] != expr[pos1+i] )
                return_false;
            }
          }
          if ( not UNIFY_MATCHQ( expr, pat, pairs,pair_size, pos1 + pairs[varI].n, pos2+1,isShortestMatch) )
            return_false;
          return true;
        }else{ // pattern not exist Record new match record // varI == pair_size + 1 
          pair_size ++; while ( pair_size >= pairs.size() ) pairs.push_back( MatchRecord() );
          pairs[varI].n = 0;
          if ( singleq ){//Form Black, only single match
            //check if Match at current position
            if ( not ({u_int new_pair_size = pair_size -1;UNIFY_MATCHQ( expr[pos1], pat[pos2],pairs,new_pair_size,0,0,isShortestMatch);}) ){
              return_false;
            }
            pairs[varI].n = 1;
            pairs[varI].p = 0;
            pairs[varI].idx = expr[pos1].objid;
            if ( not UNIFY_MATCHQ( expr, pat, pairs, pair_size, pos1+1, pos2+1,isShortestMatch) )
              return_false;
            return true;
          }else if ( isShortestMatch ){//shortest match
            // zero match of current pat
            if ( nullq and ({ u_int new_pair_size = pair_size -1;UNIFY_MATCHQ( expr, pat, pairs, new_pair_size, pos1,pos2+1 ,isShortestMatch);}) )
              return true;
            // 1, 2 or more
            for ( u_int i = pos1; i<= expr.Size(); i++ ){
              //check if match at expr[pi]
              if ( not ({ u_int new_pair_size = pair_size -1;UNIFY_MATCHQ( expr[i], pat[pos2], pairs,new_pair_size, 0,0, isShortestMatch);}) )
                return_false;
              // record current match in pairs and try following match
              if ( i == pos1 ){//1
                pairs[varI].n = 1;
                pairs[varI].p = 0;
                pairs[varI].idx = expr[pos1].objid;
              }else if ( i > pos1 ){// 2 or more 
                pairs[varI].n = i - pos1 + 1;
                pairs[varI].p = pos1;
                //pairs[varI].idx = expr.objid;
                pairs[varI].idx = expr.idx();
              }
              if ( UNIFY_MATCHQ( expr, pat, pairs,pair_size, i+1,pos2+1,isShortestMatch) )
                return true;
            }
            return_false;
          }else{ // longest match
            u_int pi = 0;
            for ( pi= pos1; pi<=expr.Size(); pi++){//test each if match at every position
              //check if match at expr[pi]
              if ( not ({u_int new_pair_size = pair_size -1; UNIFY_MATCHQ( expr[pi], pat[pos2],pairs,new_pair_size,0,0,isShortestMatch);}) )
                break;
            }
            for ( u_int i = pi-1; i>= pos1; i-- ){//record possible match, and try following match
              if ( i > pos1 ){ // 2 ore more
                pairs[varI].n = i - pos1 +1 ;
                pairs[varI].p = pos1;
                //pairs[varI].idx = expr.objid;
                pairs[varI].idx = expr.idx();
              }else{ // i == pos1  //1
                pairs[varI].n = 1;
                pairs[varI].p = 0;
                pairs[varI].idx = expr[pos1].objid;
              }
              if ( UNIFY_MATCHQ( expr, pat, pairs,pair_size, i+1,pos2+1,isShortestMatch) )
                return true;
            }
            pairs[varI].n = 0;
            if ( nullq and UNIFY_MATCHQ( expr, pat, pairs,pair_size, pos1,pos2+1 ,isShortestMatch) )
              return true;
            return false;
          }
        }
        //situation II.3: pat[pos2] is pattern and its content is BlackSequence or BlackNullSequence, but without a code
      }else{ // unamed pattern, no need to record new pair, different from  named match
        goto unamed_pattern_match_label;
      }//end of unamed pattern
      //situation II.4: pat[pos2] is BlackSequence or BlackNullSequence, but without a code
    }else{//not one of Black BlackSequence or BlackNullSequence
      ThrowError("Pattern","Content of pattern is required to be Black, BlackSequence, or BlackNullSequence.");
      //return false;
    }
  }else if ( (singleq = pat[pos2].ListQ( SYMBOL_ID_OF_Black ) ) or
             (pat[pos2].ListQ( SYMBOL_ID_OF_BlackSequence )) or
             (nullq=pat[pos2].ListQ( SYMBOL_ID_OF_BlackNullSequence )) ){
  unamed_pattern_match_label:
    // another kind of unamed pattern
    if ( singleq ){//single match
      //check if Match at current position
      if ( not UNIFY_MATCHQ( expr[pos1], pat[pos2],pairs,pair_size,0,0,isShortestMatch) ){
        return_false;
      }
      if ( UNIFY_MATCHQ( expr, pat, pairs, pair_size, pos1+1, pos2+1,isShortestMatch) )
        return true;
      return_false;
    }else if ( isShortestMatch ){//shortest match
      if ( nullq and UNIFY_MATCHQ( expr, pat, pairs,pair_size, pos1,pos2+1 ,isShortestMatch) )
        return true;
      for ( u_int i = pos1; i<= expr.Size(); i++ ){
        //check only head condition 
        if ( not UNIFY_MATCHQ( expr[i], pat[pos2], pairs,pair_size, 0, 0, isShortestMatch ) )
          return_false;
        if ( UNIFY_MATCHQ( expr, pat, pairs,pair_size, i+1,pos2+1,isShortestMatch) )
          return true;
      }
      return_false;
    }else{ // longest match
      u_int pi = 0;
      for ( pi= pos1; pi<=expr.Size(); pi++){
        //check head and other requirements
        if ( not UNIFY_MATCHQ( expr[pi], pat[pos2], pairs, pair_size, 0, 0, isShortestMatch) )
          break;
      }
      for ( u_int i = pi-1; i>= pos1; i-- ){
        if ( UNIFY_MATCHQ( expr, pat, pairs, pair_size, i+1,pos2+1,isShortestMatch) )
          return true;
      }
      if ( nullq and UNIFY_MATCHQ( expr, pat, pairs, pair_size, pos1,pos2+1 ,isShortestMatch) )
        return true;
      return_false;
    }
    // end of directly BlackSequence and BlackNullSequnce
  }
  //situation II.6: other situations
  if ( not UNIFY_MATCHQ( expr[pos1], pat[pos2], pairs,pair_size,0,0 ,isShortestMatch) )
    return_false;
  //process to next match position
  if ( UNIFY_MATCHQ( expr,pat, pairs,pair_size, pos1+1, pos2+1,isShortestMatch) )
    return true;
  return_false;
}
#undef return_false

int Pattern::UnifiedMatchQ(Object&expr,Object&pat){
  Index idx = GlobalPool.Matches.New();
  vector<MatchRecord>&pairs = GlobalPool.Matches.Get( idx );
  pairs.reserve( pat.code()+1 );
  pairs.clear();
  u_int pair_size = 0;
  bool res = UNIFY_MATCHQ(expr,pat,pairs,pair_size,0,0,false);
  GlobalPool.Matches.FreePairs( idx );
  return res;
}

int Pattern::UnifiedMatchQ(Object&expr,Object&pat,vector<MatchRecord>&pairs){
  pairs.reserve( pat.code() +1 );
  u_int pair_size = 0;
  return UNIFY_MATCHQ(expr,pat,pairs,pair_size,0,0,false);
}

static int __patternType(Object & expr){
  if ( expr.ListQ( SYMBOL_ID_OF_Pattern ) && expr.Size() >=2){
    if ( expr[2].ListQ(SYMBOL_ID_OF_BlackNullSequence ) ) return 5;
    if ( expr[2].ListQ(SYMBOL_ID_OF_BlackSequence ) ) return 4;
    if (expr[2].ListQ(SYMBOL_ID_OF_Black ) ) return 3;
  }
  if ( expr.ListQ(SYMBOL_ID_OF_BlackNullSequence) ) return 5;
  if ( expr.ListQ(SYMBOL_ID_OF_BlackSequence) ) return 4;
  if ( expr.ListQ(SYMBOL_ID_OF_Black) ) return 3;
  return 0;
}

// -5 -1(unknow but less) 0(complete same) 1(unknown expr more) 5
// -1 <
//  0 == 
//  1 >
int Pattern::PatternCompare(Object &pat1,Object &pat2){
  //cout<<pat1<<","<<pat2<<endl;
  switch ( pat1.type() ){
  case EvaType::Number: 
    switch ( pat2.type() ){
    case EvaType::Number: 
      return compare( pat1.Number(),pat2.Number() );
    default:
      return -1;
    }
  case EvaType::String:
    switch ( pat2.type()){
    case EvaType::Number: return 1;
    case EvaType::String: return compare(pat1.Key(),pat2.Key());
    default: return -1;
    }
  case EvaType::Symbol:
    switch ( pat2.type() ){
    case EvaType::Number: case EvaType::String: return 1;
    case EvaType::Symbol: return compare( pat1.SymbolWeight(), pat2.SymbolWeight() );
    default: return -1;
    }
  case EvaType::List:{
    switch ( pat2.type()){
    case EvaType::Number: case EvaType::String: return 1;
    case EvaType::Symbol: return 1;
    case EvaType::List:{
      // Pattern and kind of thins need to be considered
      int w1,w2;
      w1 = __patternType(pat1); w2 =  __patternType(pat2);
      //cout<<w1<<","<<w2<<endl;
      if ( w1 ){//pat1 is a pattern
        if ( w2 ){//both are pattern
          return_if_not( w1-w2, 0);
        }else{//pat2 is not a pattern
          return 1;
        }
      }else{// pat1 is not a pattern
        if ( w2 ){ // only pat2 is a pattern
          return -1;
        }else{//both are not pattern
          return_if_not( compare( pat1.Head(), pat2.Head() ), 0 );
        }
      }
      auto iter1 = pat1.ElementsBegin();
      auto iter2 = pat2.ElementsBegin();
      // non numbers
      while ( iter1 != pat1.End() && iter2 != pat2.End() ){
        if ( (*iter1).NullQ() ) return 1;
        if ( (*iter2).NullQ() ) return -1;
        return_if_not( PatternCompare(*iter1,*iter2),0 );
        iter1++; iter2++;
      }
      if ( iter1 != pat1.End() ) return 1;
      if ( iter2 != pat2.End() ) return -1;
      return 0;
      //end of default
    }
    default: return -1;
    }
  }
  }//end of switch outermost
  return 0;
}

void Pattern::UnifiedApplyPatternPairs(Object &Argv,vector<MatchRecord>&pairs){
  if ( pairs.size() <= 1 ) return;
  // replace from back to head can avoid create extra space to store pairs when
  //   apply on the original Obj;
  if ( Argv.SymbolQ( SYMBOL_ID_OF___Variable ) ){
    unsigned int p = Argv.re();
    if ( p > 0 && p < pairs.size() ){
      if ( pairs[p].n == 0 ){
        Argv.SetList( SYMBOL_ID_OF_Sequence );
        return;
      }else if ( pairs[p].n == 1 ){
        Argv.SetObject( Object(pairs[p].idx) );
        return;
      }else{
        Argv.SetList( SYMBOL_ID_OF_Sequence );
        for ( u_int i = 0; i< pairs[p].n; i++){
          vector<Object>&tlist = GlobalPool.Lists.Get( pairs[p].idx );
          Argv.PushBack( tlist[ pairs[p].p+i ] );
        }
        return;
      }
    }else{
      ThrowError("Pattern","Pattern unify is not right.");
      //return;
    }
  }
  if ( not Argv.ListQ() ) return;
  for (Object::iterator iter =Argv.ElementsBegin(); iter != Argv.ElementsEnd(); iter++){
    UnifiedApplyPatternPairs( *iter,pairs);
  }
  return;
}

int Pattern::UnifiedRawReplaceAll(Object &Argv, Object &rule,bool repeated){
  if ( rule.Size() < 2 )
    ThrowError("UnifiedRawReplaceAll","Rule is not in the required form.");
  Index idx = GlobalPool.Matches.New();
  vector<MatchRecord> &pairs = GlobalPool.Matches.Get(idx);
  //vector<MatchRecord> pairs;// matchqq will construct a pair Argv
  pairs.reserve(rule[1].code() + 1 );
  if ( ({u_int pair_size=0;UNIFY_MATCHQ( Argv, rule[1] ,pairs,pair_size,0,0,false);}) ){ // normal Matched
    Object nObj; nObj.SetObject( rule[2] );
    UnifiedApplyPatternPairs( nObj, pairs );
    GlobalPool.Matches.FreePairs(idx);
    Argv = nObj;
    return 1;
  }
  if ( Argv.AtomQ() ){
    GlobalPool.Matches.FreePairs(idx);
    return false;
  }
  bool replaced = false;
  for ( Object::iterator iter=Argv.ElementsBegin(); iter!= Argv.ElementsEnd(); iter++){
    if ( UnifiedRawReplaceAll(*iter,rule,repeated) ) {
      replaced = true;
      if ( !repeated )
        return true;
    }
  }
  return replaced;
}

int Pattern::UnifiedReplaceAll(Object &Argv, Object &rule,bool repeated){
  if ( rule.ListQ( SYMBOL_ID_OF_HoldPattern ) && (rule).Size() == 1){
    return UnifiedReplaceAll(Argv,(rule)[1],repeated);
  }
  if ( !RuleQ(rule) ){
    if ( repeated )
      ThrowError("UnifiedReplaceAll",rule.ToString()+" is not a rule.");
    else
      ThrowError("UnifiedReplace",rule.ToString()+" is not a rule.");
  }
  return UnifiedRawReplaceAll(Argv,rule,repeated);
}


bool Pattern::MatchQ(Object &Argv,Object &pattern){
  Object temp_pat;
  temp_pat.SetObject( pattern );
  UnifyRuleLeft( temp_pat );
  return UnifiedMatchQ(  Argv, temp_pat );
}

int ApplyPatternPairs(Object &Argv,map<Object ,Object >&pairs){
  if ( Argv.AtomQ() ){
    map<Object ,Object >::iterator iter = pairs.find(Argv);
    if ( iter!=pairs.end() )
      Argv=iter->second;
    return 0;
  }
  for (Object::iterator iter =Argv.ElementsBegin(); iter != Argv.ElementsEnd(); iter++){
    ApplyPatternPairs( *iter,pairs);
  }
  return 0;
}

int Pattern::ReplaceAll(Object &Argv, Object &rule,bool repeated){
  if ( rule.ListQ(SYMBOL_ID_OF_HoldPattern) && (rule).Size() == 1){
    return ReplaceAll(Argv,rule[1],repeated);
  }
  if ( !RuleQ(rule) ){
    if ( repeated )
      ThrowError("ReplaceAll",rule.ToString()+" is not a rule.");
    else
      ThrowError("Replace",rule.ToString()+" is not a rule.");
  }
  UnifyRule( rule[1],rule[2] );
  //dprintf("ReplaceAll with unified rule: %s", rule.ToString().c_str() );
  return UnifiedRawReplaceAll(Argv,rule,repeated);
}

bool Pattern::RuleQ(Object &Argv){
  if ( not Argv.ListQ( SYMBOL_ID_OF_Rule ) && not Argv.ListQ( SYMBOL_ID_OF_RuleDelayed ) )
    return false;
  if ( Argv.Size() != 2 )
    return false;
  return true;
}

bool Pattern::PatternListQ(Object &list){
  switch( list.type() ){
  case EvaType::Number: case EvaType::String: case EvaType::Symbol: return false;
  case EvaType::List:
    if ( list[0].SymbolQ() ){
      if ( list[0].ids() ==  SYMBOL_ID_OF_Pattern ||
           list[0].ids() == SYMBOL_ID_OF_Black ||
           list[0].ids() == SYMBOL_ID_OF_BlackSequence ||
           list[0].ids() == SYMBOL_ID_OF_BlackNullSequence ) 
        return true;
    }
    for (auto iter = list.ElementsBegin();iter!= list.ElementsEnd();iter++){
      if ( PatternListQ(*iter) )
        return true;
    }
    return false;
  }
  return false;
}

