#ifndef Objectect_Evawiz_H
#define Objectect_Evawiz_H
namespace evawiz{
  enum struct EvaType:unsigned char{Symbol, Number, String, List}; //reference is v.idx not 0
  typedef int (*ObjectCompareFunction)(const Object&,const Object&);
  struct ObjectCompareFunctionLess;

  union _Combine_Type{
    Index _idx;
    double _re;
    _Combine_Type()=default;
  };

  const unsigned char Object_State_Block = 15;
  const unsigned char Object_State_Uncomplete= 14;

  class _Object{
  public:
    friend class Object;
    friend class __EvaObjectTable;
    friend class __EvaListTable;
    unsigned char  _type_state; // 1B  EvaType and System State, 4byte for Type 4byte, for system state 0-15
    unsigned char  _code;       // 1B  255  0: normal exist 1:freed already 2:in valuetable , no free, protected
    unsigned short _cnt;        // 2B  65535
    unsigned int   _ids;        // 4B  4294967295
    _Combine_Type  _val;
    //Index          _idx;        // 8B  18446744073709551616L(42944967295,42944967295)  or +-1.79769e+308 (15-16 valid number)
    _Object() = default;
    ~_Object() = default;
    _Object &operator=(const _Object&obj){
      _type_state = obj._type_state;
      _code = obj._code;
      _ids = obj._ids;
      _val._idx = obj._val._idx;
      return *this;
    }
    void           init(){
      _type_state = 0;
      _code = 0;
      _cnt = 0;
      _ids = 0;
      _val._idx= Index0;
    }
    void           clear(){
      _type_state = 0;
      _code = 0;
      _ids = 0;
      _val._idx= Index0;
    };
    EvaType        type(){
      return EvaType(_type_state&15);
    };
    void           set_type(EvaType v){
      _type_state = (unsigned char)((_type_state&240)^((unsigned char)v) );
    };
    unsigned char  state(){
      return _type_state>>4;
    };
    void           set_state(unsigned char v){
      _type_state = (_type_state&15)^(v<<4);
    };
    ///////////////////////////////////
  };
};
#include"eva_memory_manage.h"
namespace evawiz{
  ////////////////////////////////////////////////////////
  class Object{//need not to be the pod type, for the convenions we get
  public:  Index objid;
  private:
    inline void Malloc(){
      if ( objid == Index0 ){
        objid =  GlobalPool.Objects.New();
        GlobalPool.Objects.objs[ objid.second ][ objid.first ].init();
        //dout<<"Malloc Object "<<idx<<endl;
      }
    };
    inline void Clear(){
      if ( objid == Index0 ) return; 
      if ( state() == Object_State_Block )
        ThrowError("Object is blocked. Error to apply function 'Clear'.");
      switch ( type() ){                             
      case EvaType::Number: break;                                      
      case EvaType::Symbol: break;                                      
      case EvaType::String: GlobalPool.Strings.Free( idx() ); break;  
      case EvaType::List: GlobalPool.Lists.FreeList( idx() ); break;  
      }
      GlobalPool.Objects.objs[ objid.second ][ objid.first ].clear();
    };
    inline void Free(){
      if ( objid == Index0 ) return; 
      if ( GlobalPool.is_finalizing ) return;
      if ( cnt() == 0 ){
        //dout<<"Object "<<idx<<" Free"<<endl;
        switch ( type() ){                             
        case EvaType::Number: break;                                      
        case EvaType::Symbol: break;                                      
        case EvaType::String: GlobalPool.Strings.Free( idx() ); break;  
        case EvaType::List: GlobalPool.Lists.FreeList( idx() ); break;  
        }
        //GlobalPool.Objects.objs[ objid.second ][ objid.first ].clear();
        //dout<<"Object "<<idx<<" after Free:"<<ToRawString()<<endl;
        GlobalPool.Objects.Free( objid );
      }else{
        cnt_decr();
      }
      objid= Index0;
      return;
    };
    inline void cnt_incr(){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ]._cnt++;
      //dout<<"Object "<<idx<<" ++:"<<cnt()<<endl;
    };
    inline void cnt_decr(){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ]._cnt --;
      //dout<<"Object "<<idx<<" --:"<<cnt()<<endl;
    };
  public:
    ///////////////////////////////////////////////////////////////////////
    inline Object(){
      //dout<<"New "<<idx;
      objid = Index0;
      //dout<<"->"<<idx<<endl;
    };
    inline ~Object(){
      //dout<<"Free "<<idx; 
      Free();
      //dout<<"->"<<idx<<endl;
    };
    ///////////////////////////////////////////////////////////////////////
    inline Object(const Object&obj){
      objid = obj.objid;
      if ( objid != Index0 ){
        cnt_incr();
      }
    };
    inline explicit Object(const Index&id){
      objid = id;
      if ( objid != Index0 ){
        if ( objid > GlobalPool.Objects.idx )
          ThrowError("Object","Try to construct an Object with objid out of range.");
        cnt_incr();
      }
    }; 
    typedef vector<Object>::iterator iterator;
    ///////////////////////////////////////////////////////////////////////
    inline Object    &operator=(const Object&obj){
      if ( objid != Index0 ) Free();
      if ( obj.objid == Index0 ) return *this;
      objid = obj.objid ;
      cnt_incr();
      return *this;
    }; 
    ////////////////////////////////////////////////////////
    inline EvaType           type()const{
      return GlobalPool.Objects.objs[ objid.second ][ objid.first ].type();
    };
    inline void              set_type(EvaType v){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ].set_type(v);
    };
    inline unsigned char     state()const{
      return GlobalPool.Objects.objs[ objid.second ][ objid.first ].state();
    };
    inline void              set_state(unsigned char v){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ].set_state(v);
    };
    inline unsigned char     code()const{
      return GlobalPool.Objects.objs[ objid.second ][ objid.first ]._code;
    };
    inline void              set_code(unsigned char v){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ]._code=v;
    };
    inline unsigned short    cnt()const{
      return GlobalPool.Objects.objs[ objid.second ][ objid.first ]._cnt;
    };
    inline unsigned int     &ids()const{
      return GlobalPool.Objects.objs[ objid.second ][ objid.first ]._ids;
    };
    inline void              set_ids(unsigned int v){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ]._ids = v;
    };
    inline Index            &idx()const{
      return GlobalPool.Objects.objs[ objid.second ][ objid.first ]._val._idx;
    };
    inline void              set_idx(Index val){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ]._val._idx = val;
    };
    inline double            re()const{
      return GlobalPool.Objects.objs[ objid.second ][ objid.first ]._val._re;
    };
    inline void              set_re(double val){
      GlobalPool.Objects.objs[ objid.second ][ objid.first ]._val._re = val;
    };
    ////////////////////////////////////////////////////////
    inline void              SetNull(){
      Clear();
    };
    ////////////////////////////////////////////////////////
    void                     SetObject(const Object&);
    void                     SetObjectRef(const Object&);
    void                     SetNumber(const double);
    void                     SetComplex(const complex);
    void                     SetNumber(const complex);
    void                     SetString(const char*);
    void                     SetString(const string&);
    ////////////////
    void                     SetSymbol(const u_int);
    void                     SetSymbol(const char*);
    ////////////////
    void                     ReserveSize(const u_int);
    void                     SetList(const u_int);
    void                     SetList(const char*);
    void                     SetList_List(){
      SetList( SYMBOL_ID_OF_List );
    };
    void                     SetPair(Object&first, Object&second,u_int headSymId=SYMBOL_ID_OF_List );
    void                     SetPairByRef(Object&first,Object&second,u_int headSymId=SYMBOL_ID_OF_List );
    void                     SetTo(u_int);
    ////////////////////////////////////////////////////////
    string                   ToRawString()const;
    string                   TypeString()const;
    ////////////////////////////////////////////////////////
    bool                     NullQ()const;
    bool                     NullObjectQ()const;
    bool                     NullSymbolQ()const;
    bool                     AtomQ()const;
    bool                     TypeQ(const EvaType)const;
    bool                     SymbolQ()const;
    bool                     SymbolQ(const char*)const;
    bool                     SymbolQ(const u_int)const;
    bool                     ValuedSymbolQ()const;
    bool                     NumberQ()const;
    bool                     NumberQ(const double)const;
    bool                     NumberQ(const complex)const;
    bool                     RealQ()const;
    bool                     RealQ(const double)const;
    bool                     ComplexQ()const;
    bool                     ComplexQ(const complex)const;
    bool                     IntegerQ()const;
    bool                     StringQ()const;
    bool                     StringQ(const char*)const;
    bool                     StringQ(const Index)const;
    bool                     ListQ()const;
    bool                     ListQ(const char*)const;
    bool                     ListQ(const u_int)const;
    bool                     ListQ(const Object&)const;
    bool                     ListQ_List()const{return ListQ( SYMBOL_ID_OF_List );};
    bool                     SimpleListQ()const;
    bool                     SimpleListQ(const char*)const;
    bool                     SimpleListQ(const u_int)const;
    bool                     HeadQ(const char *)const;
    bool                     HeadQ(const Object&)const;
    ////////////////////////////////////////////////////////
    const char*              Head()const;
    ////////////////////////////////////////////////////////
    const char*              Key()const;//only String and Symbol has key
    u_int                    SymbolId()const;//return 0 if no Id
    u_int                    SymbolWeight()const;//only Symbol applies
    ////////////////////////////////////////////////////////
    char                     Boolean()const;
    double                   Number()const;
    complex                  Complex()const;
    const char*              String()const;//only apply to String
    explicit operator        bool()const{
      return Boolean();
    };
    explicit operator        int()const{
      return (int)Number();
    };
    explicit operator        float()const{
      return Number();
    };
    explicit operator        double()const{
      return Number();
    };
    explicit operator        complex()const{
      return Complex();
    };
    explicit operator        floatcomplex()const{
      return (floatcomplex)Complex();
    };
    explicit operator        string()const{
      return(type()==EvaType::String)?Key():ToString();
    };
    ////////////////////////////////////////////////////////
    static int               Dimensions(const Object&list,Object&dim,int n=-1);
    Object 	                &ElementAt(const u_int id){
      return GlobalPool.Lists.objs[ idx().second ][ idx().first][ id ];
    };
    Object 	                &_ElementAt(const u_int id)const{
      return GlobalPool.Lists.objs[ idx().second ][ idx().first][ id ];
    };
    inline Object 	        &operator[](const u_int id){
      return GlobalPool.Lists.objs[ idx().second ][ idx().first][ id ];
    };
    inline Object 	        &operator()(const u_int id)const{
      return GlobalPool.Lists.objs[ idx().second ][ idx().first][ id ];
    };
    u_int          	         ElementsSize()const;
    u_int               	   Size()const;
    Object 	            	  &First();
    Object 	                &Last();
    Object              	  &Back();
    Object                   RefObject();
    ///////////////////////////////////////////////////////////////
    iterator            	   Begin()const;
    iterator            	   End()const;
    iterator            	   ElementsBegin()const;
    iterator            	   ElementsEnd()const;
    u_int               	   PositionOfIterator(const iterator iter)const;
    inline bool         	   Empty()const{ if ( (not ListQ() )  or ( Size() == 0) ) return true; else return false; };
    /////////////////////////////////////////////////////////////////
    void                	   Swap(Object&);
    void                	   SwapRef(Object&);
    void                	   Flatten();
    void                	   FlattenSequence();
    void                	   FlattenListAndSequence();
    void                	   ForceDeepen();// make current list to be head 
    void                	   Deepen();// make current list to be head 
    void                	   ToList();// make current list to be head 
    void	              	   PushBack(const Object&);
    void	              	   PushBackRef(const Object&);
    void                	   PushBackNumber(const double);
    void                	   PushBackComplex(const complex);
    void                	   PushBackNumber(const complex);
    void	              	   PushBackSymbol(const char*key);
    void	              	   PushBackSymbol(u_int symbolId);
    void	              	   PushBackString(const char*key);
    void	              	   PushBackString(const string&key);
    void                	   PushBackNull();
    /////////////
    void                	   Insert(u_int pos,Object &obj);
    void                	   Insert(iterator iter,Object &obj);
    void                	   InsertRef(u_int pos,Object &obj);
    void                	   InsertRef(iterator iter,Object &obj);
    void                	   InsertRef(iterator pos,iterator spos, iterator epos);
    void                	   InsertNumber(u_int pos,double val);
    void                	   InsertComplex(u_int pos,complex val);
    void                	   InsertNumber(u_int pos,complex val);
    void                	   InsertSymbol(u_int,char*key);
    void                	   InsertSymbol(u_int,u_int keyid);
    void                	   InsertString(u_int,char*key);
    void                	   InsertNull(u_int);
    /////////////
    void                	   PopBack();
    void                	   Delete(u_int pos);
    iterator            	   Delete(iterator iter);
    void                	   Delete(iterator spos, iterator epos);
    void                	   DeleteElements();
    ////////////////////////////////////////////////////////////////
    string	            	   ToFullFormString()const;
    string              	   ToString(const Object&parentList)const;
    string              	   ToString()const;
    ////////////////////////////////////////////////////////////////
    static int 	        	   SimpleCompare(const Object&l1,const Object&l2) ; 
    static int 	        	   ExpressionCompare(const Object&l1,const Object&l2) ; 
    void                	   Sort(const ObjectCompareFunction lcf=Object::ExpressionCompare);
    void	              	   SimpleSort();
    void	              	   ExpressionSort();
    ////////////////////////////////////////////////////////////////
    bool		            	   operator<(const Object&other)const;
    bool		            	   operator==(const Object&other)const;
    bool		            	   operator!=(const Object&other)const;
    ////////////////////////////////////////////////////////////////
    int                 	   DictGetPosition(Object&left_value,iterator &iter_pos);
    Object              	   DictGet(Object&left_value);
    Object              	   DictGetOrNew(Object&left_value);
    Object              	   DictValueAt(Object&left_value);
    pair<bool,Object>   	   DictInsert(Object&left_value,Object&right_value);
    Object              	   DictInsertOrUpdate(Object&left_value,Object&right_value);
    Object              	   DictPop(Object&left_value);
    bool                	   DictDelete(Object&left_value);
    void                	   DictSort();
    //////////////////////////////////////////
    int                 	   OrderListGetPosition(Object&value,iterator&iter,ObjectCompareFunction lcf=Object::SimpleCompare);
    bool                	   OrderListGetEqualRange(Object&value,iterator&s_iter,iterator&e_iter, ObjectCompareFunction lcf = Object::SimpleCompare);
    bool                	   OrderListInsert(Object&,ObjectCompareFunction lcf=Object::SimpleCompare);//insert only the value
    bool                	   OrderListInsertNoDumplicate(Object&value,ObjectCompareFunction lcf = SimpleCompare);
    bool                	   OrderListDeleteValue(Object&,ObjectCompareFunction lcf = SimpleCompare);
    bool                	   OrderListDeleteAllValue(Object&,ObjectCompareFunction lcf = SimpleCompare);
    void                	   OrderListSort(ObjectCompareFunction lcf=SimpleCompare);
  };
  const Object NullObject;

  template<class ostype> ostype &operator<<(ostype &os,const Object&obj){
    os<<obj.ToString();
    return os;
  };

  struct ObjectCompareFunctionLess{
    ObjectCompareFunction LCF;
    inline ObjectCompareFunctionLess(ObjectCompareFunction lcf){LCF = lcf;}
    inline bool operator()(Object l1,Object l2){return LCF(l1,l2)<0;}
  };

};
#endif
