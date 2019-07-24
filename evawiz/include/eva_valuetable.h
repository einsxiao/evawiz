#ifndef Gisp_ValueTable_H
#define Gisp_ValueTable_H

namespace evawiz{
  class ValueTable{
  public:
    friend class Evaluation;
  private:
    void                        VeryInit();
    //ids of valued Symbol is equal to  pairTable[i].ids
    // pairTable[i].ids() == RecordTable addr
    Object                      pairTable;//normal certain value table
    Object                      patternTable;//rule list of patterns
    //////////////////////////////////
    //ValueTable              	 *upValueTable;
    //map<string,ValueTable>     *subValueTables;
    /////////////////////////////////////////////////
  public:
    typedef vector<Object>::iterator iterator;
    ValueTable();
    ValueTable(const ValueTable &upValueTable);
    ~ValueTable();
    ValueTable &operator=(const ValueTable&); 
    /////////////////////////////////////////////////
    bool                        UpdateOrInsertPattern(Object&left,Object&right,bool onlyUpdate=false,bool tryDel=false);
    bool                        UpdateOrInsertValue(Object&left,Object&right,bool onlyUpdate=false);//Insert or update a value pair
    bool                        UpdateOrInsert(Object&left,Object&right,bool onlyUpdate=false,bool isCheckedPattern=false);//Insert or update a value pair
    /////////////////////////////////////////////////
    //get the pair Object
    Object                      GetOrNewValuePairRef(Object&left,bool onlyGet=false,bool isCheckedNoPattern=false);
    /////////////////////////////////////////////////
    // 0: nothing get 1: get self-equal value 2:get new value 
    int                         GetValueFromPattern(Object&left, Object&right);
    int                         GetValueRef(Object&left,Object&right); 
    int                         GetValue(Object&left,Object&right,bool onlyCheckPatternTable=false); 
    /////////////////////////////////////////////////
    int                         Clear(Object&list,Evaluation*Eva); // Clear the value
    int                         ClearAll(Evaluation*Eva); // Clear all values
    /////////////////////////////////////////////////
    /*
    ValueTable                 *GetSubValueTable(const char*);
    ValueTable                 *GetOrInsertSubValueTable(const char*);
    */
    /////////////////////////////////////////////////
  };


};
#endif
