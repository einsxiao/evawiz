#include"eva_headers.h"

using namespace std;
using namespace evawiz;

void Regex::Init(const string pat_str,int n,int mode)
{
  groupNumber = n;
  regs.resize( groupNumber + 1 );
  if ( ( code = regcomp( &pat, pat_str.c_str() , mode) ) != 0  ) {
    switch ( code ){
    case REG_NOMATCH: ThrowError("Regex","pattern"," regexec() failed to match.");
    case REG_BADPAT: ThrowError("Regex","pattern"," Invalid regular expression.");
    case REG_ECOLLATE: ThrowError("Regex","pattern"," Invalid collating element referenced.");
    case REG_ECTYPE: ThrowError("Regex","pattern"," Invalid character class type referenced.");
    case REG_EESCAPE: ThrowError("Regex","pattern"," Trailing '\' in pattern.");
    case REG_ESUBREG : ThrowError("Regex","pattern"," Number in \"\\digit\" invalid or in error. ");
    case REG_EBRACK : ThrowError("Regex","pattern"," \"[]\" imbalance. ");
    case REG_EPAREN : ThrowError("Regex","pattern"," \"\\(\\)\" or \"()\" imbalance.");
    case REG_EBRACE : ThrowError("Regex","pattern"," \"\\{\\}\" imbalance. ");
    case REG_BADBR : ThrowError("Regex","pattern"," Content of \"\\{\\}\" invalid: not a number, number too large, more than two numbers, first larger than second. ");
    case REG_ERANGE : ThrowError("Regex","pattern"," Invalid endpoint in range expression. ");
    case REG_ESPACE : ThrowError("Regex","pattern"," Out of memory. ");
    case REG_BADRPT : ThrowError("Regex","pattern"," '?', '*', or '+' not preceded by valid regular expression. ");
    default: ThrowError("Regex","pattern","Internal error.");
    }
  }
}

Regex::Regex(const string pat_str,int n,int mode){
  Init(pat_str,n,mode);
}

Regex::~Regex()
{
}

bool Regex::Match(const char*pat_str){
  if ( regexec( &pat, pat_str, 0, NULL, 0 ) == 0 ){
    return true;
  }else{
    return false;
  }
}

bool Regex::Match(const string pat_str){
  return Match(pat_str.c_str());
}

bool Regex::Match(const char*pat_str,Matches&mats){
  if ( regexec( &pat, pat_str, groupNumber+1, &regs[0], 0 ) == 0 ){
    mats.reserve( groupNumber + 1 );
    mats.clear();
    for ( int i=0;i<=groupNumber; i++ ){
      if ( regs[i].rm_so  >= 0 )
        mats.push_back( string(pat_str, regs[i].rm_so, regs[i].rm_eo - regs[i].rm_so ) );
      else
        break;
    }
    return true;
  }else{
    mats.clear();
    return false;
  }
}

bool Regex::Match(const string pat_str,Matches&mats){
  regs.resize( groupNumber + 1 );
  if ( regexec( &pat, pat_str.c_str(), groupNumber+1, &regs[0], 0 ) == 0 ){
    mats.resize( groupNumber + 1 );
    for ( int i=0;i<=groupNumber; i++ ){
      mats[i] = pat_str.substr( regs[i].rm_so, regs[i].rm_eo - regs[i].rm_so );
    }
    return true;
  }else{
    return false;
  }
}
