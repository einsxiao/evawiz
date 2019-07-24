#ifndef Gisp_Pattern_H
#define Gisp_Pattern_H

namespace evawiz{
  const int MatchLess = -2;
  const int MatchLeftByRight= -1;
  const int MatchEqual = 0;
  const int MatchRightByLeft = 1;
  const int MatchGreater = 2;
  

  class Pattern{
  public:
    Pattern();
    ~Pattern();
    // replace pattern.second to get the replaced pattern.second
    // pair is from matchqq function
    /////////////////////////////////////////////////
    ////////////////////////////////////////
    //used between unified patterns
    //static int        UnifiedMatchCompare(Object &pat1,Object &pat2); 
    static int        PatternCompare(Object&pat1,Object&pat2); 
    static int        UnifyRuleLeft(Object&left);
    static int        UnifyRule(Object&left,Object&right);
    static int        UnifyRule(Object&rule);
    static int        UnifiedMatchQ(Object&expr, Object&pattern);
    static int        UnifiedMatchQ(Object&expr, Object&pattern,vector<MatchRecord>&pairs);
    static void       UnifiedApplyPatternPairs(Object&Argv,vector<MatchRecord>&pairs);
    static int        UnifiedRawReplaceAll(Object&Argv,Object&rule,bool repeated);
    static int        UnifiedReplaceAll(Object&expr, Object&rule,bool repeated);
    ////////////////////////////////////////////////////////////////////
    static int        Compare(Object&pat1,Object&pat2,bool matched = false);
    static bool       MatchQ(Object&Argv,Object&pattern); //well done
    static int        ReplaceAll(Object&Argv,Object&rule,bool repeated = true);
    ////////////////////////////////////////
    static bool       RuleQ(Object&Argv);
    static bool       PatternListQ(Object&list);
  };
};
#endif
