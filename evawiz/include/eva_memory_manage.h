#ifndef EvaStatic_Evawiz_H
#define EvaStatic_Evawiz_H

namespace evawiz{

  const int __EvaTableNewWaitTime = 50;
  class __EvaSymbolTable{//will not free
    struct positionType{
      u_int pos;
      char* key;
      positionType(char*_k,u_int _p){key = _k; pos = _p;};
    };
    struct recordType{
      unsigned short pL,pR;
      u_int weight;
      char* key;
      recordType(char*_k,u_int _w,u_int _l,u_int _r){key = _k; weight = _w; pL = _l; pR = _r;};
    }; //key -> pos_key
    bool locked;
    vector< positionType > positions;// 
    vector< recordType > records; //position to symbol weight and content 
    u_int ptr;//total amount of symbols in SymbolTable
    void waitAndLock(){while (locked){ usleep(__EvaTableNewWaitTime);} locked = true;};
    void unLock(){ locked =false; };
  public:
    __EvaSymbolTable();
    ~__EvaSymbolTable();
    u_int GetOrNew(const char*,u_int priLeft=0,u_int priRight=0);
    inline u_int GetId(const char*key){ return GetOrNew(key); };
    inline char* GetKey(u_int id){return records[id].key; };
    inline u_int GetWeight(u_int id){return records[id].weight; };
    inline u_int GetPriorityLeft(u_int id){return records[id].pL; };
    inline u_int GetPriorityRight(u_int id){return records[id].pR; };
    inline u_int TableSize(){ return records.size(); };
  };

  ////////////////////////////////////////////
#ifndef __MEMORY_POOL_ROW_SIZE
#  define __MEMORY_POOL_ROW_SIZE 65530
#endif
  template<class type>
  class __EvaTable{
  public:
    string tableName;
    bool locked;
    u_int RowSize;
    inline void waitAndLock(){
      while (locked){
        usleep(__EvaTableNewWaitTime);
      }
      locked = true;
    };
    inline void unLock(){
      locked =false;
    };
    Index idx;
    u_int lineNumber;
    vector< Index > freeObjs; //v_second row_idx
    vector< type* > objs; //first one is refed counter
    __EvaTable(int inSize = __MEMORY_POOL_ROW_SIZE){
      RowSize = __MEMORY_POOL_ROW_SIZE;
      if ( inSize >= 5 )
        RowSize = inSize; 
      else
        RowSize = 5; 
      type* row = new type[ RowSize +1 ];
      objs.push_back(row);
      lineNumber = 1;
      idx.first = 0;
      idx.second = 0;
    };
    virtual  ~__EvaTable(){
      //dout<<"    finalize Table "<<tableName<<endl;
      for ( auto iter = objs.begin(); iter!=objs.end(); iter++){
        delete [](*iter);
      }
      objs.clear();
      //dout<<"    finalize Table "<<tableName<<"finished"<<endl;
    };
    type &Get(const Index &v){
      if ( v > idx )
        ThrowError("Pool","Try get elements out of range.");
      return objs[ v.second ][ v.first ];
    }
    Index New(){
      waitAndLock();
      if ( freeObjs.size() > 0 ){
        Index res = freeObjs.back() ;
        freeObjs.pop_back();
        unLock();
        return res;
      }else{
        if ( idx.first == RowSize ){
          type* row = new type[ RowSize+1 ];
          objs.push_back(row);
          lineNumber++;
          idx.second ++; 
          idx.first = 0;
          unLock();
          return idx;
        }else{
          idx.first ++;
          unLock();
          return idx;
        }
      };
    };
    inline void Free(Index id){
      waitAndLock(); //should check 
      if ( id.first != 0 or id.second != 0 ){
        freeObjs.push_back(id);
      }
      unLock();
    };
  };

  ///////////////////////////////////////////////////////////////////////////////
  class __EvaStringTable:public __EvaTable<string>{
  public:
    __EvaStringTable(){
      tableName = "Strings";
    };
    Index NewString(const char*);
    Index NewString(Index);
  };
  class __EvaObjectTable:public __EvaTable<_Object>{
  public:
    __EvaObjectTable(){
      tableName = "Objects";
    };
  };
  /////////////////////
  class __EvaListTable:public __EvaTable< vector<Object> >{
  public:
    __EvaListTable(){
      tableName = "Lists";
    };
    Index NewList(const Index);//new from other List by Value
    void  FreeList(Index);
    void  SetList(Index left, Index right);
  private:
    void Free(Index);
  };

  class MatchRecord{
  public:
    u_int n;
    u_int p;// when n >1 p is the position or first element
    Index idx;//idx of Object(n==1) or List(n>1)
  public:
    MatchRecord(){
      n = 0;
      p = 0;
      idx = Index0;
    }
    MatchRecord(Index i){
      n = 1;
      p=0;
      idx = i;
    }
    MatchRecord(Index i, int n,int p){
      this->n = n;
      this->p = p;
      this->idx = i;
    }
  };

  class __EvaMatchTable:public __EvaTable< vector<MatchRecord> >{
  public:
    __EvaMatchTable(){
      tableName = "PatternMatchPairs";
    };
    void  FreePairs(Index id);
  private:
    void Free();
  };

  class __EvaOptimizeRecord:public __EvaTable< pair<double,string> >{
  };

  template<class type>
  class __EvaMatrixTable
  {
    map<string,Matrix_T<type>*> matrixTable; 
  public:
    __EvaMatrixTable()
    {
    };
    ~__EvaMatrixTable()
    {
      typename map<string,Matrix_T<type>* >::iterator iter;
      for ( iter = matrixTable.begin(); iter != matrixTable.end(); iter++ ){
        delete iter->second;
        iter->second = NULL;
      }
    };
    Matrix_T<type>   *GetMatrix(string name)
    {
      typename map<string,Matrix_T<type>* >::iterator iter;
      iter = matrixTable.find(name);
      if ( iter != matrixTable.end() ){
        return iter->second;
      }
      return NULL;
    };
    Matrix_T<type>   *GetOrNewMatrix(string name)
    {
      Matrix_T<type> **matPP = &matrixTable[name];
      if ( *matPP == NULL )
        *matPP = new Matrix_T<type>();
      return *matPP;
    };
    int RemoveMatrix(string name)
    {
      typename map<string,Matrix_T<type>* >::iterator iter;
      iter = matrixTable.find(name);
      if ( iter != matrixTable.end() ){
        delete iter->second;
        matrixTable.erase(iter);
        return 1;
      }
      return 0;
    };
    Matrix_T<type>   &operator()(string id)
    {
      return *GetMatrix(id);
    };
    Matrix_T<type>   &operator[](string id)
    {
      return *GetOrNewMatrix(id);
    };
  };

  class EvaMemoryPool{
  public:
    bool                is_finalizing;
    __EvaSymbolTable    Symbols;
    __EvaStringTable    Strings;
    __EvaListTable      Lists; 
    __EvaObjectTable    Objects;
    __EvaMatchTable     Matches;
    __EvaOptimizeRecord OpRec;
    __EvaMatrixTable<double>       Matrices;
    __EvaMatrixTable<complex>      ComplexMatrices;
    __EvaMatrixTable<float>        FloatMatrices;
    __EvaMatrixTable<floatcomplex> FloatComplexMatrices;
  public:
    EvaMemoryPool();
    ~EvaMemoryPool();
  public:
    Evaluation         *Eva;
  };
  template<int id> class __GlobalPool{
  public:
    static EvaMemoryPool pool;
  };
  template<int id> EvaMemoryPool __GlobalPool<id>::pool;
#define GlobalPool (__GlobalPool<0>::pool)
#define EvaKernel  (__GlobalPool<0>::pool.Eva)

  inline u_int SymbolIdOf(char*key){
    return GlobalPool.Symbols.GetId( key );
  };

    
#ifdef DEBUG
  inline int __Init_Clock_(Index &__clock_ID,string file, string function){
    return 0;
  }
#  define clockStart static Index __clock_ID;                           \
  if ( __clock_ID == Index0 ){                                          \
    __clock_ID = GlobalPool.OpRec.New();                                \
    GlobalPool.OpRec.Get( __clock_ID ).first = 0;                       \
    GlobalPool.OpRec.Get( __clock_ID ).second = (string)__FILE__+":\t"+__FUNCTION__; \
  }                                                                     \
  clock_t __start_T = clock()
#  define clockEnd GlobalPool.OpRec.Get( __clock_ID ).first+=(1.0*clock() - __start_T)/CLOCKS_PER_SEC;
#else
#  define __clockPrepare 
#  define clockStart 
#  define clockEnd 
#endif



  ////////////////////////////////////////////
#define ___DS_LIST _DS(Null,0);                   \
  _DS(List,                 1);                   \
  _DS(Dict,                 2);                   \
  _DS(Sequence,             3);                   \
  _DS(Slot,                 4);                   \
  _DS(Compound,             5);                   \
  _DS(ArgsFollow,           6);                   \
  _DS(PartArgsFollow,       7);                   \
  _DS(ParenthesisFollow,    8);                   \
  _DS(Prefix,               9);                   \
  _DS(PrefixIn,            10);                   \
  _DS(Black,               11);                   \
  _DS(BlackSequence,       12);                   \
  _DS(BlackNullSequence,   13);                   \
  _DS(Pattern,             14);                   \
  _DS(Get,                 15);                   \
  _DS(Increment,           16);                   \
  _DS(PreIncrement,        17);                   \
  _DS(Decrement,           18);                   \
  _DS(PreDecrement,        19);                   \
  _DS(Factorial,           20);                   \
  _DS(Power,               21);                   \
  _DS(Reciprocal,          22);                   \
  _DS(Times,               23);                   \
  _DS(Divide,              24);                   \
  _DS(Mod,                 25);                   \
  _DS(Dot,                 26);                   \
  _DS(Plus,                27);                   \
  _DS(Subtract,            28);                   \
  _DS(Greater,             29);                   \
  _DS(Less,                30);                   \
  _DS(GreaterEqual,        31);                   \
  _DS(LessEqual,           32);                   \
  _DS(Equal,               33);                   \
  _DS(UnEqual,             34);                   \
  _DS(Not,                 35);                   \
  _DS(And,                 36);                   \
  _DS(Or,                  37);                   \
  _DS(Alternatives,        38);                   \
  _DS(PureFunction,        39);                   \
  _DS(Rule,                40);                   \
  _DS(RuleDelayed,         41);                   \
  _DS(ReplaceAll,          42);                   \
  _DS(ReplaceRepeated,     43);                   \
  _DS(Postfix,             44);                   \
  _DS(Set,                 45);                   \
  _DS(SetDelayed,          46);                   \
  _DS(AddTo,               47);                   \
  _DS(TimesBy,             48);                   \
  _DS(SubtractFrom,        49);                   \
  _DS(DivideBy,            50);                   \
  _DS(PowerWith,           51);                   \
  _DS(ModBy,               52);                   \
  _DS(Put,                 53);                   \
  _DS(CompoundExpression,  54);                   \
  _DS(Seperator,           55);                   \
  _DS(Module,              56);                   \
  _DS(Function,            57);                   \
  _DS(Evaluate,            58);                   \
  _DS(Exit,                59);                   \
  _DS(ToExpression,        60);                   \
  _DS(Parenthesis,         61);                   \
  _DS(Print,               62);                   \
  _DS(Replace,             63);                   \
  _DS(System,              64);                   \
  _DS(If,                  65);                   \
  _DS(While,               66);                   \
  _DS(Do,                  67);                   \
  _DS(For,                 68);                   \
  _DS(Log,                 69);                   \
  _DS(SubstractFrom,       70);                   \
  _DS(Random,              71);                   \
  _DS(Part,                72);                   \
  _DS(Complex,             73);                   \
  _DS(Span,                74);                   \
  _DS(Args,                75);                   \
  _DS(__Variable,          76);                   \
  _DS($START$,             77);                   \
  _DS($END$,               78);                   \
  _DS(FullForm,            79);                   \
  _DS(Take,                80);                   \
  _DS(Table,               81);                   \
  _DS(HoldOnce,            82);                   \
  _DS(HoldPattern,         83);                   \
  _DS(Matrix,              84);                   \
  _DS(Attributes,          85);                   \
  _DS(True,                86);                   \
  _DS(False,               87);                   \
  _DS(SimpleSimplify,      88);                   \
  _DS(Range,               89);                   \
  _DS(Foreach,             90);                   \
  _DS(Help,                91);                   \
  _DS(Conjunct,            92);                   \
  _DS(Class,               93);                   \
  _DS(SerialCode,          94);                   \
  _DS(Tensor,              95);                   \
  //////
#define _DS(sym,id)  const u_int SYMBOL_ID_OF_##sym = id
  ___DS_LIST;
#undef _DS
#undef ___DS_LIST

#define INIT_SYMBOL_ID_OF(sym) u_int SYMBOL_ID_OF_##sym = GlobalPool.Symbols.GetId(#sym)



};


#endif
