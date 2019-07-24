#include"evawiz.h"
#include"ListModule.h"
#include"eva_oldmacro.h"
using namespace evawiz;
#undef ModuleNameStr
#define ModuleNameStr "List"
__CREATE_MODULE__(List);

__DEBUG_MAIN__("input.ev");
ListModule::ListModule(__CONSTRUCTION_PARA__){
  ///////////////////////////////////////////////////////////////////
  {
    RegisterFunction("List",List);
    AddAttribute("List",AttributeType::DelayFunction);
    AddAttribute("List",AttributeType::DelaySetable);
    AddAttribute("List",AttributeType::Conjunctable);
  }
  {
    RegisterFunction("Range",Range);
    AddAttribute("Range",AttributeType::Protected);
  }
  ///////////////////////////////////////////////////////////////////
  {
    RegisterFunction("Take",Take); //Take[expr,{i,10}]
    AddAttribute("Take",AttributeType::GrammarUnit);
  }
  {
    RegisterFunction("Drop",Drop); 
    AddAttribute("Drop",AttributeType::GrammarUnit);
  }
  {
    RegisterFunction("Part",Part); 
    AddAttribute("Part",AttributeType::HoldFirst);
    RegisterFunction("SetPart",SetPart);
    AddAttribute("SetPart",AttributeType::HoldFirst);
    AddAttribute("Part",AttributeType::Setable );
  }
  ///////////////////////////////////////////////////////////////////
  {
    RegisterFunction("Table",Table);
    AddAttribute("Table",AttributeType::HoldAll);
    AddAttribute("Table",AttributeType::GrammarUnit);
  }
  ///////////////////////////////////////////////////////////////////
  RegisterFunction("Head",Head); 
  RegisterFunction("Size",Size); 
  RegisterFunction("First",First); 
  ///////////////////////////////////////////////////////////////////
  RegisterFunction("Sort",Sort); 
  RegisterFunction("Flatten",Flatten); 
  RegisterFunction("Dimensions",ListModule::Dimensions); 
  ///////////////////////////////////////////////////////////////////
  {
    RegisterFunction("Dict",Dict);
    AddAttribute("Dict",AttributeType::DelayFunction);
    AddAttribute("Dict",AttributeType::DelaySetable);
    AddAttribute("Dict",AttributeType::Conjunctable);
  }
  {
    RegisterFunction("Append",Append);
    RegisterFunction("AppendTo",AppendTo);
    AddAttribute("AppendTo",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("Insert",Insert);
    RegisterFunction("InsertTo",InsertTo);
    AddAttribute("InsertTo",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("Delete",Delete);
    RegisterFunction("DeleteFrom",DeleteFrom);
    AddAttribute("DeleteFrom",AttributeType::HoldFirst);
  }
  {
    RegisterFunction("PopBack",PopBack);
    AddAttribute("PopBack",AttributeType::HoldFirst);
  }

}
ListModule::~ListModule(){};

inline bool toNormalIndex(int index,u_int len,int&normalindex){
  if ( index > (int)len or index < -(int)len )
    return false;
  if ( index < 0 )
    normalindex = len +index+1;
  return true;
}

//#define d_return(x) ({dprintf("");return x; })
bool SpanToIndexes(int len,Object &index, int &start, int &end, int &step ){
  if ( not (index.Size() == 3 or index.Size() == 2 ) )
    return(false);
  //step 
  bool givenStep = false;
  if ( index.Size() == 2 ){
    step = 1;
  }else if ( index[3].NumberQ( ) ){
    step = (int)index[3];
    givenStep = true;
    if ( step == 0 )
      return(false);
  }else if ( index[3].NullQ() ){
    step = 1;
  }else
    return(false);
  //start
  if ( index[1].NullQ() or index[1].SymbolQ( SYMBOL_ID_OF_$START$ ) ){
    if ( step > 0 )
      start = 1;
    else
      start = len;
  }else if ( index[1].NumberQ( ) ){
    start = (int)index[1];
    if ( start > len or start < - len )
      return(false);
    if ( start < 0 ) start += len +1;
  }else
    return(false);
  //end
  if ( index[2].NullQ() or index[2].SymbolQ( SYMBOL_ID_OF_$END$ ) ){
    if ( step > 0 )
      end = len;
    else
      end = 1;
  }else if ( index[2].NumberQ( ) ){
    end = (int)index[2];
    if ( end > len or end < -len ) return(false);
    if ( end < 0 ) end += len +1;
  }else return(false);

  if ( givenStep ) return true;
  if ( start > end and step > 0 ){
    step = -step;
  }
  return true;
}
//#undef return

int GetRealPartList(Object&valueList,Object&partList,u_int pos,Object&resultList,bool isReal);
void localSetPart(Object&valueList,Object&partList,u_int pos,Object&rightValue);

int ListModule::List(Object &Argv){
  static INIT_SYMBOL_ID_OF(Exists);
  static INIT_SYMBOL_ID_OF(Insert);
  static INIT_SYMBOL_ID_OF(Delete);
  static INIT_SYMBOL_ID_OF(Append);
  static INIT_SYMBOL_ID_OF(PushBack);
  static INIT_SYMBOL_ID_OF(Clean);
  static INIT_SYMBOL_ID_OF(Length);
  static INIT_SYMBOL_ID_OF(Size);

  DelaySet_Context(List){
    //List[...][...] = 
    // |  1,0 |  1,1 |   2
    Object res;
    localSetPart(Argv[1][0], Argv[1], 1, Argv[2] );
    Argv.SetTo(2);
    ReturnNormal;
  }
  DelayFunction_Context(List){
    Object res;
    GetRealPartList( Argv[0], Argv, 1, res, true );
    Argv = res;
    ReturnNormal;
  }
  Conjunct_Context(List){
    Object &listobj = Argv[1];
    Object &operobj = Argv[2];

    if ( Conjunct_Case_Condition( Append ) or Conjunct_Case_Condition( PushBack ) ){
      CheckArgsShouldEqual( operobj, 1 );
      EvaKernel->Evaluate( operobj[1] );
      
      listobj.PushBack( operobj[1] );
      Argv.SetTo(1);
      ReturnNormal;
    }

    Conjunct_Case( Insert ){
      CheckArgsShouldEqual( operobj, 2 );
      EvaKernel->Evaluate( operobj[1] );
      EvaKernel->Evaluate( operobj[2] );

      CheckArgsShouldBeNumber( operobj, 1 );
      listobj.Insert( int(operobj[1]), operobj[2] );
      Argv.SetTo(1);
      ReturnNormal;
    }

    Conjunct_Case( Length ){
      CheckArgsShouldEqual( operobj, 0 );
      ReturnNumber( listobj.Size() ); 
    }

    Conjunct_Case( Size ){
      CheckArgsShouldEqual( operobj, 0 );
      ReturnNumber( listobj.Size() ); 
    }

    Conjunct_Case( Delete ){
      CheckArgsShouldEqual( operobj, 1 );
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeNumber( operobj, 1 );
      int ind = int( operobj[1] );
      if ( ind < 1 or ind > (int)listobj.Size() )
        ThrowError("List","Delete","Index "+ToString(ind)+" is out of range.");
      listobj.Delete(ind);
      Argv.SetTo(1);
      ReturnNormal;
    }

    Conjunct_Case( Clean ){
      CheckArgsShouldEqual( operobj, 0 );
      listobj.DeleteElements();
      Argv.SetTo(1);
    }

    //If no case matched just let it go
    //ReturnNormal;
  }

  //dealing Sequence
  u_int i = 1;
  while ( i<= Argv.Size() ){
    if ( Argv.ElementAt(i).ListQ( evawiz::SYMBOL_ID_OF_Sequence  ) ){
      Argv.InsertRef( Argv.Begin()+i, Argv.ElementAt(i).Begin(), Argv.ElementAt(i).End() );
      Argv.Delete(i);
    }else{
      EvaKernel->Evaluate( Argv[i] );
      i++;
    }
  }
  return 1;
}

int ListModule::Range(Object &Argv){
  CheckShouldBeWithin(1,3);
  if ( Argv.Size() == 1 ){
    EvaKernel->Evaluate( Argv[1] );
    if ( Argv[1].NumberQ() ){
      int N =(int) Argv[1];
      Argv.SetList_List();
      Argv.ReserveSize( N );
      for ( int i = 1; i<=N; i++ )
        Argv.PushBackNumber(i);
    }
    ReturnNormal;
  }

  if ( Argv.Size() == 2 ){
    EvaKernel->Evaluate(Argv[1]);
    EvaKernel->Evaluate(Argv[2]);
    if ( Argv[1].NumberQ() && Argv[2].NumberQ() ){
      int Nbegin =(int) Argv[1];
      int Nend =(int) Argv[2];
      Argv.SetList_List();
      if ( Nend < Nbegin ) ReturnNormal;
      Argv.ReserveSize( (Nend-Nbegin)+1 );
      for ( int i = Nbegin; i<=Nend; i++ ){
        Argv.PushBackNumber(i );
      }
    }
    ReturnNormal;
  }

  if ( Argv.Size() == 3 ){
    EvaKernel->Evaluate(Argv[1]);
    EvaKernel->Evaluate(Argv[2]);
    EvaKernel->Evaluate(Argv[3]);
    if ( Argv[1].NumberQ() && Argv[2].NumberQ() && Argv[3].NumberQ() ){
      double Nbegin =(double) Argv[1] ;
      double Nend =(double) Argv[2] ;
      double Nincr =(double) Argv[3];
      if ( Nincr == 0 ) ThrowError("Range","Increment should not be a zero.");
      int steps = (Nend-Nbegin)/Nincr;
      Argv.SetList_List();
      if ( steps < 0 ) ReturnNormal;
      Argv.ReserveSize( steps+1 );
      for ( int i = 0; i<=steps; i++ ){
        Argv.PushBackNumber(Nbegin+i*Nincr);
      }
    }
    ReturnNormal;
  }
  ReturnHold;
}

static int take(Object&expr,Object&takelist,u_int p,Object&resultList){//expr should make sure to be a nonatom
  Object & iter = takelist[p];
  resultList.SetList_List();
  if ( iter.NumberQ() ){
    int N =(double) iter;
    if ( N >= 0 ){
      if ( N > (int)expr.Size() ){
        ThrowError("Take","Cannot take position 1 through "+ToString(N)+".");
      }
      for ( int i = 1; i<= N; i++ ){
#define address_ele_take(p_i)                                   \
        if ( p == takelist.Size() ){                            \
          resultList.PushBackRef( expr[p_i] );                  \
        }else{                                                  \
          resultList.PushBackNull();                            \
          take( expr[p_i], takelist, p+1, resultList.Back() );  \
        }                                                       \
        /////
        address_ele_take(i);
      }
    }else{
      if ( -N> (int)expr.Size() ){
        ThrowError("Take","Cannot take position "+ToString(N)+"through -1.");
      }
      for ( int i = expr.Size() + N+1; i<= (int)expr.Size(); i++ ){
        address_ele_take(i);
      }
    }
    return 1;
  }
  if ( (iter).ListQ_List() ){//AtomQ must be not true
    if ( iter.Size() > 3 || iter.Size()< 1 ){
      ThrowError("Take","Sequence specification "+iter.ToString()+" is not valid.");
    }
    Object &num1 = (iter)[1];
    int ind1,bak;
    if ( num1.NumberQ() ){
      bak = ind1 =(double) num1;
      if ( ind1<0 ) ind1 += expr.Size() +1 ; 
      if ( iter.Size() == 1){//only one number...
        if ( ind1 > (int)expr.Size() or ind1 < 1 ){
          ThrowError("Take","Cannot take position "+ToString(bak) );
        }
        address_ele_take( ind1 );
        return 1;
      }
    }else{
      ThrowError("Take","Sequence specification "+iter.ToString()+" is not valid.");
    }
    Object&num2 = (iter)(2);
    int ind2;
    if ( num2.NumberQ() ){
      bak = ind2 =(double) num2;
      if ( ind2<0 ) ind2 += expr.Size()+1;
      if ( iter.Size() == 2){
        if ( ind2 > (int)expr.Size()  or ind2 < 1 ){
          ThrowError("Take","Cannot take position "+ToString(bak)+" through "+ToString(bak)+".");
        }
        for ( int i=ind1; i<= ind2; i++ ){
          address_ele_take( i );
        }
        return 1;
      }
    }else{
      ThrowError("Take","Sequence specification "+iter.ToString()+" is not valid.");
    }
    Object &inter = (iter)(3);
    int ninter;
    if ( inter.NumberQ() ){
      ninter =(double)inter;
      if ( iter.Size() == 3){
        if ( ninter == 0 ){
          ThrowError("Take","Sequence specification "+iter.ToString()+" is not valid.");
        }
        if ( ninter> 0 ){
          for ( int i = ind1; i <= ind2; i+=ninter ){
            address_ele_take(i);
          }
        }else if ( ninter < 0 ){
          for ( int i = ind1; i>= ind2; i+=ninter ){
            address_ele_take(i);
          }
        }
        return 1;
      }
    }else{
      ThrowError("Take","Sequence specification "+iter.ToString()+" is not valid.");
    }
  }
  ThrowError("Take","Sequence specification "+iter.ToString()+" is not valid.");
}

int ListModule::Take(Object &Argv){
  CheckArgsShouldNoLessThan(Argv,2);
  CheckShouldBeList(1);
  
  Object resultList;
  take( Argv[1], Argv, 2, resultList ); 
  Argv = resultList;
  ReturnNormal;
}

static int drop(Object&expr, Object&droplist,u_int p){
  Object &iter = droplist[p];
  if ( iter.AtomQ() ){
    if ( iter.NumberQ() ){
      int N = iter.Number();
      if ( N>=0 ){
        if ( N> (int)expr.Size() )ThrowError("Drop","Cannot drop position 1 through "+ToString(N)+" in "+expr.ToString()+".");
        expr.Delete( expr.Begin(),expr.Begin()+N );
#define address_next_level_of_drop_if_exist                             \
        if ( p < droplist.Size() ){                                     \
          for (auto iter = expr.Begin(); iter != expr.End(); iter ++ ){ \
            drop ( *iter, droplist, p+ 1 );                             \
          }                                                             \
        }
        address_next_level_of_drop_if_exist;
      }else{
        if ( -N> (int)expr.Size() ){ThrowError("Drop","Cannot drop position "+ToString(N)+" through -1 in "+expr.ToString()+".");}
        expr.Delete( expr.End() + N, expr.End() );
        address_next_level_of_drop_if_exist;
      }
      return 1;
    }else{
      ThrowError("Drop","Sequence specification "+iter.ToString()+" is not valid.");
    }
  }
  if ( (iter).ListQ_List() ){//AtomQ must be not true
    if ( iter.Size() > 3 || iter.Size()< 1 ){ThrowError("Drop","Sequence specification "+iter.ToString()+" is not valid.");}
    Object &num1 = (iter)(1);
    int ind1,indb1;
    if ( num1.NumberQ() ){
      indb1 = ind1 =  num1.Number() ;
      if ( ind1<0 ) ind1 += expr.Size()+1;
      if ( iter.Size() == 1){
        if ( abs(ind1) > (int)expr.Size() ){ThrowError("Drop","Cannot drop position "+ToString(indb1)+" in "+expr.ToString()+"."); }
        expr.Delete(ind1);
        address_next_level_of_drop_if_exist;
        return 1;
      }
    }else{
      ThrowError("Drop","Sequence specification "+iter.ToString()+" is not valid.");
    }
    Object&num2 = (iter)(2);
    int ind2,indb2;
    if ( num2.NumberQ()  ){
      indb2 = ind2 =  num2.Number();
      if ( ind2<0 ) ind2 += expr.Size()+1;
      if ( iter.Size() == 2){
        if ( abs(ind2) > (int)expr.Size() ){ThrowError("Drop","Cannot drop position "+ToString(indb1)+" through "+ToString(indb2)+" in "+expr.ToString()+".");}
        for (int i = ind1; i<= ind2; i++ )
          expr.Delete(ind1);
        address_next_level_of_drop_if_exist;
        return 1;
      }
    }else{
      ThrowError("Drop","Sequence specification "+iter.ToString()+" is not valid.");
    }
    Object &inter = (iter)(3);
    int ninter;
    if ( inter.IntegerQ() ){
      ninter =  inter.Number();
      if ( iter.Size() == 3){
        if ( (int)inter == 0 )ThrowError("Drop","Sequence specification "+iter.ToString()+" is not valid.");
        if ( ninter> 0 ){
          int adj = ( ind2 - ind1 ) % ninter ;
          for ( int i = ind2-adj; i >= ind1; i-=ninter ){
            expr.Delete( i );
          }
          address_next_level_of_drop_if_exist;
        }else if ( ninter < 0 ){
          for ( int i = ind1; i>= ind2; i+=ninter ){
            expr.Delete( i );
          }
          address_next_level_of_drop_if_exist;
        }
        return 1;
      }
    }
    ThrowError("Drop","Sequence specification "+iter.ToString()+" is not valid.");
  }
  ThrowError("Drop","Sequence specification "+iter.ToString()+" is not valid.");
  return 0;
}

int ListModule::Drop(Object &Argv){
  CheckArgsShouldNoLessThan(Argv,2);
  CheckShouldBeList(1);
  Object expr = Argv(1);
  if ( expr.AtomQ() )ThrowError("Drop","Nonatomic experssion expected at position 1.");
  drop( Argv[1], Argv, 2 );
  Argv.SetTo(1);
  ReturnNormal;
}

int GetRealPartList(Object&valueList,Object&partList,u_int pos,Object&resultList,bool isReal){
  if ( valueList.NullQ() or partList.NullQ() ) return -1;
  if ( valueList.AtomQ() )
    ThrowError("Part","Part specification for atom "+valueList.ToString()+" is not allowed.");
  bool LastLevel = ( partList.Size() == pos );
  Object &index = partList[pos]; 
  if ( index.ListQ() ){
    if ( index.ListQ_List() ){
      resultList.SetList_List();
      for ( u_int i =1; i<= index.Size(); i++ ){
        if ( not (index)[i].NumberQ() ){ ThrowError("Part","Part specification should be a number");}
        int ind = index[i].Number();
        if ( not toNormalIndex(ind,valueList.Size(),ind) ){
          Warning("Part","Part specification "+ToString(ind)+" for "+valueList.ToString()+" is out of range.");
          return -1;
        }
        if ( LastLevel ){
          if ( isReal )
            resultList.PushBackRef( valueList[ind] ); 
          else
            resultList.PushBack( valueList[ind] ); 
        }else{
          resultList.PushBackNull();
          GetRealPartList(valueList(ind),partList,pos+1,resultList[i],isReal);
        }
      }
      return 1;
    }else if ( index.ListQ( SYMBOL_ID_OF_Span ) ){
      int start, end, step;
      if ( not  SpanToIndexes( valueList.Size(), index, start, end, step ) )
        ThrowError("Span","Span "+index.ToString()+" is in the wrong form.");
      //dprintf("%s -> start = %d, end = %d, step = %d",index.ToString().c_str(),start,end,step);
      resultList.SetList_List();
      for ( int i = start; (step>0?i<=end:i>=end); i+=step ){
        if (LastLevel ){
          if ( isReal )
            resultList.PushBackRef( valueList[ i ] );
          else
            resultList.PushBack( valueList[ i ] );
        }else{
          resultList.PushBackNull();
          GetRealPartList( valueList[ i ], partList, pos+1, resultList[i], isReal);
        }
      }
      return 1;
    }
    ThrowError("Part","Part index "+index.ToString()+" is not in the right form.");
  }
  if ( not partList[pos].NumberQ() )
    return -1;
  int ind = partList[pos].Number();
  if ( not toNormalIndex(ind,valueList.Size(),ind) ){
    Warning("Part","Part specification "+ToString(ind)+" for "+valueList.ToString()+" is out of range.");
    return -1;
  }
  if ( LastLevel ){
    resultList.SetObjectRef( valueList[ind] );
  }else{
    GetRealPartList( valueList[ind], partList, pos+1, resultList, isReal);
  }
  return 1;
}

int ListModule::Part(Object&Argv){
  Set_Context(Part){
    return SetPart( Argv );
  }
  CheckShouldNoLessThan(2);
  // rewrite the Part[Part[a_,b__],c__] :> Part[a,b,c]
  while ( Argv[1].ListQ( SYMBOL_ID_OF_Part ) ){
    CheckArgsShouldNoLessThan(Argv[1],2);
    Argv.InsertRef(Argv.Begin()+1,Argv[1].Begin(),Argv[1].End());
    Argv.Delete(1);
  }
  // the value of part should be evaluated till last
  EvaKernel->Evaluation::Evaluate( Argv[1], false, true );
  Object resultList;
  GetRealPartList(Argv[1],Argv,2,resultList,true);
  Argv = resultList;
  ReturnNormal;
  
}

void localSetPart(Object&valueList,Object&partList,u_int pos,Object&rightValue){
  if ( valueList.NullQ() or partList.NullQ() ) return;
  if ( valueList.AtomQ() )
    ThrowError("Part","Part specification for atom "+valueList.ToString()+" is not allowed.");
  bool LastLevel = ( partList.Size() == pos );
  Object&index = partList[pos];
  if ( index.ListQ() ){
    if ( index.ListQ_List() ){
      if ( rightValue.ListQ_List() ){
        if ( index.Size() != rightValue.Size() ) ThrowError("SetPart","Left Part List should have the size of the right value List.");
        for ( u_int i=1; i<= index.Size(); i++ ){
          if ( not (index)[i].NumberQ() ){ ThrowError("Part","Part specification should be a number");}
          int ind = (index)[i].Number();
          if ( not toNormalIndex(ind,valueList.Size(),ind) ){ ThrowError("Part","Part "+ToString(ind)+" of "+valueList.ToString()+" does not exist.");}
          if ( LastLevel ){
            valueList[ind].SetObject( rightValue[i] );
          }else{
            localSetPart( valueList[ind], partList,pos+1,rightValue[i] );
          }
        }
        return;
      }
      //only an normal Object
      for ( u_int i =1; i<= index.Size(); i++ ){
        if ( not (index)[i].NumberQ() ){ ThrowError("Part","Part specification should be a number");}
        int ind = (index)[i].Number();
        if ( not toNormalIndex(ind,valueList.Size(),ind) ){ ThrowError("Part","Part "+ToString(ind)+" of "+valueList.ToString()+" does not exist.");}
        if ( LastLevel ){
          valueList[ind].SetObject( rightValue );
        }else{
          localSetPart( valueList[ind],partList,pos+1,rightValue);
        }
      }
      return;
    }else if ( index.ListQ( SYMBOL_ID_OF_Span ) ){
      int start, end, step;
      if ( not  SpanToIndexes( valueList.Size(), index, start, end, step ) )
        ThrowError("Span","Span "+index.ToString()+" is in the wrong form.");
      for( int i = start; (step>0?i<= end:i>=end); i+=step ){
        if ( LastLevel )
          valueList[i].SetObject( rightValue );
        else
          localSetPart( valueList[i], partList, pos+1, rightValue );
      }
      return;
    }
    ThrowError("SetPart","Part index "+index.ToString()+" is not in the right form.");
  }
  if ( not index.NumberQ() )
    ThrowError("SetPart","Part specification should be a Number.");
  int ind = index.Number();
  if ( not toNormalIndex(ind,valueList.Size(),ind) ){ ThrowError("Part","Part "+ToString(ind)+" of "+valueList.ToString()+" does not exist.");}
  if ( LastLevel ){
    valueList[ind].SetObject( rightValue );
  }else{
    localSetPart( valueList[ind],partList,pos+1,rightValue);
  }
  return;
}

int ListModule::SetPart(Object&part,Object &rightValue){
  if ( not part.ListQ( SYMBOL_ID_OF_Part ) )
    ThrowError("SetPart","Invalid calling for SetPart.");
  CheckArgsShouldNoLessThan(part, 2);
  while ( part[1].ListQ( SYMBOL_ID_OF_Part ) ){
    CheckArgsShouldNoLessThan( part[1],2 );
    part.InsertRef( part.Begin()+1, part[1].Begin(),part[1].End() );
    part.Delete(1);
  }
  Object value = EvaKernel->GetValuePairRef( part[1] );
  if ( value.NullQ() ) ThrowError("SetPart",part[1].ToString()+" is not an Symbol with a value.");
  localSetPart( value[2], part, 2, rightValue );
  return 0;
}

int ListModule::SetPart( Object&Argv){
  CheckShouldEqual( 2 );
  SetPart( Argv[1], Argv[2]);
  Argv.SetTo(2);
  ReturnNormal;
}

///////////////////////////////////
int ListModule::Head(Object &Argv){
  CheckShouldEqual(1);
  if ( Argv[1].ListQ() ){
    Object head = Argv[1][0];
    Argv = head;
    ReturnNormal;
  }else{
    const char * head = Argv[1].Head();
    Argv.SetSymbol( head );
    ReturnNormal;
  }
}

int ListModule::Size(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  Argv.SetNumber(Argv[1].Size());
  ReturnNormal;
}

int ListModule::First(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  if ( Argv[1].Size()<1 ){
    Warning("First","Try to get first element of a atom List");
    ReturnNull;
  }
  Object first = Argv[1](1);
  Argv.SetObjectRef( first );
  ReturnNormal;
}
///////////////////////////////////

int ListModule::Table(Object &Argv){
  CheckArgsShouldNoLessThan(Argv,2);
  while ( Argv.Size() >2 ){//change Table to recursively form
    Object newtable; newtable.SetList( SYMBOL_ID_OF_Table );
    newtable.PushBackRef( Argv(1) );
    newtable.PushBackRef( Argv(2) );
    Argv.Delete(2);
    Argv(1)=newtable;
  }
  Object&expr = Argv(1);
  Object&iter = Argv(2);
  if ( not (iter).ListQ( SYMBOL_ID_OF_List ) || iter.Size()<1 || iter.Size()>4) ThrowError(Argv.Key(),"Argument "+iter.ToString()+" is not the proper form of an iterator.\n");
  if ( iter.Size() == 1 ){
    Object&num = (iter)(1);
    EvaKernel->Evaluate(num);
    if ( num.NumberQ() ){
      Object newlist; newlist.SetList_List();
      int N =(double)num;
      for ( int i = 1; i<=N; i++ ){
        Object newexpr; newexpr.SetObject( expr);
        EvaKernel->Evaluate(newexpr);
        newlist.PushBackRef( newexpr );
      }
      Argv = newlist;
      ReturnNormal;
    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  if ( iter.Size() == 2 ){
    Object&var = (iter)(1);
    if ( !var.SymbolQ() ) ThrowError(Argv.Key(),var.ToString()+" cannot be used as an iterator.");
    Object&num = (iter)(2);
    EvaKernel->Evaluate(num);
    if ( num.NumberQ() ){
      EvaKernel->newGrammarModule();
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(var,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Table","Can not create innner variable.");
      Object&vobj = pairobj[2];
      Object newlist; newlist.SetList_List();
      int N =(double)num;
      for ( int i = 1; i<=N; i++ ){
        Object newexpr; newexpr.SetObject( expr );
        vobj.SetNumber( i );
        EvaKernel->Evaluate(newexpr);
        //dprintf("newexpr %s ",newexpr.ToString().c_str() );
        newlist.PushBack( newexpr );
      }
      Argv.SetObjectRef(newlist);
      EvaKernel->deleteGrammarModule();
      ReturnNormal;
    }else if ( (num).ListQ( SYMBOL_ID_OF_List )){
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(var,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Table","Can not create innner variable.");
      Object&vobj = pairobj[2];
      Object newlist; newlist.SetList_List();
      for ( u_int i = 1; i<=num.Size(); i++ ){
        Object newexpr; newexpr.SetObject( expr );
        vobj.SetNumber( i );
        EvaKernel->Evaluate(newexpr);
        newlist.PushBack( newexpr );
      }
      Argv.SetObjectRef( newlist );
      EvaKernel->deleteGrammarModule();
      ReturnNormal;
    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  if ( iter.Size() == 3 ){
    Object&var = (iter)(1);
    if ( !var.SymbolQ() ) ThrowError(Argv.Key(),var.ToString()+" cannot be used as an iterator.");
    Object&num1 = (iter)(2);
    Object&num2 = (iter)(3);
    EvaKernel->Evaluate(num1);
    EvaKernel->Evaluate(num2);
    if ( num1.NumberQ() && num2.NumberQ() ){
      EvaKernel->newGrammarModule();
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(var,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Table","Can not create innner variable.");
      Object&vobj = pairobj[2];
      Object newlist; newlist.SetList_List();
      int Nbegin = num1.Number() ;
      int Nend =  num2.Number();
      for ( int i = Nbegin; i<=Nend; i++ ){
        Object newexpr; newexpr.SetObject(expr);
        vobj.SetNumber(i);
        EvaKernel->Evaluate(newexpr);
        newlist.PushBack( newexpr );
      }
      Argv.SetObjectRef(newlist);
      EvaKernel->deleteGrammarModule();
      ReturnNormal;
    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  if ( iter.Size() == 4 ){
    Object&var = (iter)(1);
    if ( !var.SymbolQ() ) ThrowError(Argv.Key(),var.ToString()+" cannot be used as an iterator.");
    Object&num1 = (iter)(2);
    Object&num2 = (iter)(3);
    Object&incr = (iter)(4);
    EvaKernel->Evaluate(num1);
    EvaKernel->Evaluate(num2);
    EvaKernel->Evaluate(incr);
    if ( num1.NumberQ() && num2.NumberQ() && incr.NumberQ() ){
      EvaKernel->newGrammarModule();
      Object pairobj = EvaKernel->GetOrNewValuePairRefAtTop(var,false,false,true);
      if ( pairobj.NullQ() ) ThrowError("Table","Can not create innner variable.");
      Object&vobj = pairobj[2];
      Object newlist; newlist.SetList_List();
      double Nbegin = num1.Number();
      double Nend = num2.Number();
      double Nincr = incr.Number();
      if ( Nincr == 0 ) ThrowError("Table","Increment should not be a zero.");
      int steps = (Nend-Nbegin)/Nincr;
      for ( int i = 0; i<= steps; i++ ){
        Object newexpr; newexpr.SetObject(expr);
        vobj.SetNumber(Nbegin+i*Nincr);
        EvaKernel->Evaluate(newexpr);
        newlist.PushBack( newexpr );
      }
      Argv.SetObjectRef( newlist );
      EvaKernel->deleteGrammarModule();
      ReturnNormal;

    }
    ThrowError(Argv.Key(),"Iterator "+iter.ToString()+" does not have appropriate bounds.");
  }
  ReturnHold;
}

int ListModule::Sort(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  if ( not Argv[1].ListQ_List() ) ThrowError(Argv.Key(),"1st argument of "+Argv.ToString()+" should be a Argv.");
  Argv[1].ExpressionSort();
  Argv.SetTo(1);
  ReturnNormal;
}

int ListModule::Flatten(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  Argv.SetTo(1);
  Argv.Flatten();
  ReturnNormal;
}

int ListModule::Dimensions(Object&Argv){
  CheckShouldBeWithin(1,2);
  Object  dim; dim.SetList_List();
  if ( Argv.Size() == 1 ){
    if ( Argv[1].ListQ( SYMBOL_ID_OF_Matrix ) ){
      if ( Argv[1].Size() != 1 or not Argv[1][1].StringQ() ) ThrowError("Matrix","Matrix should have a string to specify its name.");
      Matrix*mat = EvaKernel->GetMatrix( Argv[1][1].Key() );
      if ( mat == NULL ){
        ThrowError("Dimensions",Argv(1).ToString()+" does not exist.");
      }
      int nd = mat->DimN();
      for (int i=1; i<=nd; i++)
        dim.PushBackNumber( mat->Dim(i) );
      Argv = dim;
      ReturnNormal;
    }else{
      Object::Dimensions(Argv[1],dim,-1);
      Argv = dim;
      ReturnNormal;
    }
  }else{ 
    CheckShouldBeNumber(2);
    int depth = Argv[2].Number();
    Object::Dimensions(Argv[1],dim,depth);
    Argv = dim;
    ReturnNormal;
  }
}

int ListModule::Dict(Object&Argv){
  static INIT_SYMBOL_ID_OF(Exists);
  static INIT_SYMBOL_ID_OF(Insert);
  static INIT_SYMBOL_ID_OF(Delete);
  static INIT_SYMBOL_ID_OF(Append);
  static INIT_SYMBOL_ID_OF(PushBack);
  static INIT_SYMBOL_ID_OF(Clean);
  static INIT_SYMBOL_ID_OF(Length);
  static INIT_SYMBOL_ID_OF(Size);

  Conjunct_Context(Dict){//Conjunct calling
    Object&dict = Argv[1];
    Object&oper = Argv[2];
    Conjunct_Case( Exists ){
      CheckArgsShouldEqual(oper,1);
      Object::iterator iter;
      if ( dict.DictGetPosition( oper[1] ,iter) == 0  )
        ReturnNumber(true);
      ReturnNumber(false);
    }
    Conjunct_Case( Delete ){
      CheckArgsShouldEqual(oper,1);
      dict.DictDelete( oper[1] ) ;
      ReturnNull;
    }
    Conjunct_Case( Insert ){
      CheckArgsShouldEqual(oper,2);
      dict.DictInsertOrUpdate(oper[1],oper[2]);
      ReturnNull;
    }
    Conjunct_Case( Size ){
      CheckArgsShouldEqual(oper,0);
      ReturnNumber( dict.Size() );
    }
    ReturnNormal;
  }
  DelaySet_Context(Dict){//DealySet Calling  Dict[...] = 
    Object&dict = Argv[1][0];
    Object&key = Argv[1][1];
    Object&right = Argv[2];
    dict.DictInsertOrUpdate( key, right );
    Argv.SetTo(2);
    ReturnNormal;
  }
  DelayFunction_Context(Dict){
    CheckShouldEqual(1);
    Object&dict = Argv[0];
    Object&key = Argv[1];
    Object res = dict.DictGetOrNew( key );
    ReturnObject( res );
  }
  //normal calling doing nothing
  ReturnNormal;
}


int ListModule::Append(Object&Argv)
{
  CheckShouldEqual(2);
  Object &var = Argv[1];
  Object &ele = Argv[2];

  if ( not var.ListQ() ){
    ThrowError("Append","Cannot append element to an Non-List Object.");
  }

  var.PushBack( ele );
  Argv.SetTo(1);
  ReturnNormal;
}

int ListModule::AppendTo(Object&Argv)
{
  CheckShouldEqual(2);
  Object &var = Argv[1];
  Object &ele = Argv[2];

  if ( var.NumberQ() or var.StringQ() ){
    ThrowError("Append","Cannot append element to an Object of Number or String.");
  }
  // if not an Symbol with value assigned to 
  if ( var.ListQ() ){ 
    EvaKernel->Evaluate( var );
    if ( not var.ListQ() ){
      ThrowError("Append","Cannot append element to Non-List Object.");
    }
    var.PushBack( ele );
    Argv.SetTo(1);
    ReturnNormal;
  }
  Object res = EvaKernel->GetValuePairRef(var);
  if ( res.NullQ() )
    ThrowError("Append","Cannot append element to an symbol without a value.");
 
  if ( res.ids() != 0 ){
    if (  EvaKernel->AttributeQ(res.ids() ,AttributeType::Protected) ){
      ThrowError("Append",(var).ToString() +(string)" is Protected.");
    }
  }
  EvaKernel->Evaluate( res[2] );

  if ( not res[2].ListQ() ){
    ThrowError("Append","Cannot append element to Non-List Object.");
  }

  res[2].PushBack( ele );
  Argv.SetObject( res[2] );
  ReturnNormal;
}

int ListModule::Insert(Object&Argv)
{
  CheckShouldEqual(3);
  CheckShouldBeNumber(3);
  Object &var = Argv[1];
  Object &ele = Argv[2];
  int pos = Argv[3].Number();

  if ( not var.ListQ() ){
    ThrowError("Insert","Cannot insert element to Non-List Object.");
  }

  if ( pos < 0 ){
    pos = var.Size() - pos + 1;
  }
  if ( pos < 1 or pos > ( (int) var.Size() + 1) )
    ThrowError("Insert","Insert position is out of List Range.");
  
  var.Insert(pos, ele);
  Argv.SetTo(1);
  ReturnNormal;
}

int ListModule::InsertTo(Object&Argv)
{
  CheckShouldEqual(3);
  CheckShouldBeNumber(3);
  Object &var = Argv[1];
  Object &ele = Argv[2];
  int pos = Argv[3].Number();

  if ( var.NumberQ() or var.StringQ() ){
    ThrowError("Insert","Cannot insert element to an Object of Number or String.");
  }
  // if not an Symbol with value assigned to 
  if ( var.ListQ() ){ 
    EvaKernel->Evaluate( var );
    if ( not var.ListQ() ){
      ThrowError("Insert","Cannot insert element to Non-List Object.");
    }
    var.Insert(pos, ele );
    Argv.SetTo(1);
    ReturnNormal;
  }
  Object res = EvaKernel->GetValuePairRef(var);
  if ( res.NullQ() )
    ThrowError("Insert","Cannot insert element to an symbol without a value.");
 
  if ( res.ids() != 0 ){
    if (  EvaKernel->AttributeQ(res.ids() ,AttributeType::Protected) ){
      ThrowError("Insert",(var).ToString() +(string)" is Protected.");
    }
  }
  EvaKernel->Evaluate( res[2] );

  if ( not res[2].ListQ() ){
    ThrowError("Insert","Cannot insert element to Non-List Object.");
  }

  res[2].Insert(pos, ele );
  Argv.SetObject( res[2] );
  ReturnNormal;
}


int ListModule::Delete(Object&Argv)
{
  CheckShouldEqual(2);
  CheckShouldBeNumber(2);
  Object &var = Argv[1];
  int pos = Argv[2].Number();
  
  if ( not var.ListQ() ){
    ThrowError("Delete","Cannot delete element to Non-List Object.");
  }
  
  if ( pos < 0 ){
    pos = var.Size() - pos + 1;
  }
  if ( pos < 1 or pos > (int) var.Size()  )
    ThrowError("Delete","Delete position is out of List Range.");
  
  var.Delete(pos);
  Argv.SetTo(1);
  ReturnNormal;
}

int ListModule::DeleteFrom(Object&Argv)
{
  CheckShouldEqual(2);
  CheckShouldBeNumber(2);
  Object &var = Argv[1];
  int pos = Argv[2].Number();

  if ( var.NumberQ() or var.StringQ() ){
    ThrowError("Delete","Cannot delete element to an Object of Number or String.");
  }
  // if not an Symbol with value assigned to 
  if ( var.ListQ() ){ 
    EvaKernel->Evaluate( var );
    if ( not var.ListQ() ){
      ThrowError("Delete","Cannot delete element to Non-List Object.");
    }
    if ( pos < 1 or pos > (int) var.Size()  )
      ThrowError("Delete","Delete position is out of List Range.");
    var.Delete(pos);
    Argv.SetTo(1);
    ReturnNormal;
  }

  Object res = EvaKernel->GetValuePairRef(var);
  if ( res.NullQ() )
    ThrowError("DeleteFrom","Cannot delete element to an symbol without a value.");
 
  if ( res.ids() != 0 ){
    if (  EvaKernel->AttributeQ(res.ids() ,AttributeType::Protected) ){
      ThrowError("DeleteFrom",(var).ToString() +(string)" is Protected.");
    }
  }

  EvaKernel->Evaluate( res[2] );

  if ( not res[2].ListQ() ){
    ThrowError("DeleteFrom","Cannot delete element to Non-List Object.");
  }
  
  if ( pos < 1 or pos > (int) var.Size() )
    ThrowError("Delete","Delete position is out of List Range.");
  res[2].Delete( pos );
  Argv.SetObject( res[2] );
  ReturnNormal;
}

int ListModule::PopBack(Object&Argv)
{
  CheckShouldEqual(1);
  Object &var = Argv[1];

  if ( var.NumberQ() or var.StringQ() ){
    ThrowError("PopBack","Cannot delete element to an Object of Number or String.");
  }
  // if not an Symbol with value assigned to 
  if ( var.ListQ() ){ 
    EvaKernel->Evaluate( var );
    if ( not var.ListQ() ){
      ThrowError("PopBack","Cannot delete element to Non-List Object.");
    }
    Object t = var.Last();
    var.PopBack();
    Argv = t;
    ReturnNormal;
  }
  Object res = EvaKernel->GetValuePairRef(var);
  if ( res.NullQ() )
    ThrowError("PopBack","Cannot delete element to an symbol without a value.");
 
  if ( res.ids() != 0 ){
    if (  EvaKernel->AttributeQ(res.ids() ,AttributeType::Protected) ){
      ThrowError("PopBack",(var).ToString() +(string)" is Protected.");
    }
  }
  EvaKernel->Evaluate( res[2] );

  if ( not res[2].ListQ() ){
    ThrowError("PopBack","Cannot delete element to Non-List Object.");
  }
  
  Object t = res[2].Last();
  res[2].PopBack();
  Argv = t;
  ReturnNormal;
}
