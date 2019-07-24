#ifndef __EVAWIZ_REGREX_H__
#define __EVAWIZ_REGREX_H__

class Regex{
  regex_t pat;
  int code = -1;
  int groupNumber;
public:
  vector<regmatch_t> regs;
public:
  void Init(const string pat_str,int n=32,int mode = REG_EXTENDED );
  Regex(const string pat_str,int n=32,int mode = REG_EXTENDED ); // REG_ICASE REG_NOSUB REG_NEWLINE
  ~Regex();
  typedef vector<string> Matches;
  bool Match(const char*);
  bool Match(const string);
  bool Match(const char*,Matches&);
  bool Match(const string,Matches&);
};





#endif
