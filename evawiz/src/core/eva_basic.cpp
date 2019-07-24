#include<eva_headers.h>
#include<unistd.h>
using namespace evawiz;

void evawiz::Warning(string mod,string func,string mes){
  cerr<<mod<<"::"<<func<<"::"<<"Warning"<<": "<<mes<<endl;
}

void evawiz::Warning(string func,string mes){
  cerr<<func<<"::"<<"Warning"<<": "<<mes<<endl;
}

void evawiz::Warning(string mes){
  cerr<<"Warning"<<": "<<mes<<endl;
}


//////////////////////////////////////////////////////
int evawiz::EvaSettings::matrixPosition = MatrixHost;
int evawiz::EvaSettings::calculatePosition = CalculatePositionHost;
int evawiz::EvaSettings::runningMode = RunningModeGpu;
int evawiz::EvaSettings::cudaThreadsNumberPerBlock = 256;
int evawiz::EvaSettings::evawizCpuKernelThreadsNumber = 4;
int evawiz::EvaSettings::precision = 7;
bool evawiz::EvaSettings::scientificFormat = false;
double evawiz::EvaSettings::epsilon = 0.000000000000005;

void evawiz::EvaSettings::SetFormat(int ps,bool sci){
  precision = ps;
  if ( ps<=0 )  precision = 1;
  if ( ps>=16 ) precision = 16;
  scientificFormat = sci;
}

void   evawiz::EvaSettings::SetCudaThreadsNumberPerBlock(int num)
{
  if ( num <= 0 and num%32 != 0 and num > 1024 )
    ThrowError("SetCudaThreadsNumberPerBlock","Thread number per block is suggested to be multiple of 32 and a number with 0 and 1024.");
  cudaThreadsNumberPerBlock = num;
}

void   evawiz::EvaSettings::SetEvawizCpuKernelThreadsNumber(int num)
{
  if ( num <= 0 )
    ThrowError("SetEvawizCpuKernelThreadsNumber","Cpu threads number should be greater than one.");
  evawizCpuKernelThreadsNumber = num;
}

string evawiz::ToString(int num){
  char chs[32];
  sprintf(chs,"%d",num);
  return string(chs);
}

string evawiz::ToString(u_int num){
  char chs[32];
  sprintf(chs,"%d",num);
  return string(chs);
}

string evawiz::ToString(u_long num){
  char chs[32];
  sprintf(chs,"%ld",num);
  return string(chs);
}

string evawiz::ToString(Index num){
  char chs[64];
  sprintf(chs,"%d&%d",num.second,num.first );
  return string(chs);
}

string evawiz::ToString(void*num){
  char chs[10];
  sprintf(chs,"0x%lx",(u_long)num);
  return string(chs);
}

class OstringStreamTable:public __EvaTable< ostringstream >{
public:
  OstringStreamTable():__EvaTable( 5 ){};
};

OstringStreamTable ostringsPool;


string evawiz::ToString(double num){
  Index idx = ostringsPool.New();
  ostringstream&ss = ostringsPool.Get(idx);
  ss.str("");
  ss.precision(EvaSettings::precision);
  ss.setf( ios::uppercase);
  if ( EvaSettings::scientificFormat ){
    ss<<std::scientific<<num;
  }else{
    ss<<num;
  }
  ostringsPool.Free(idx);
  return ss.str();
}

string evawiz::ToString(complex num){
  Index idx = ostringsPool.New();
  ostringstream&ss = ostringsPool.Get(idx);
  ss.str("");
  ss.precision(EvaSettings::precision);
  if ( EvaSettings::scientificFormat ){
    ss<<std::scientific<<"("<<num.re<<","<<num.im<<")";
  }else{
    ss<<"("<<num.re<<","<<num.im<<")";
  }
  ostringsPool.Free(idx);
  return ss.str();
}

string evawiz::ToString(float num){
  Index idx = ostringsPool.New();
  ostringstream&ss = ostringsPool.Get(idx);
  ss.str("");
  ss.precision(EvaSettings::precision);
  ss.setf( ios::uppercase);
  if ( EvaSettings::scientificFormat ){
    ss<<std::scientific<<num;
  }else{
    ss<<num;
  }
  ostringsPool.Free(idx);
  return ss.str();
}

string evawiz::ToString(floatcomplex num){
  Index idx = ostringsPool.New();
  ostringstream&ss = ostringsPool.Get(idx);
  ss.str("");
  ss.precision(EvaSettings::precision);
  if ( EvaSettings::scientificFormat ){
    ss<<std::scientific<<"("<<num.re<<","<<num.im<<")";
  }else{
    ss<<"("<<num.re<<","<<num.im<<")";
  }
  ostringsPool.Free(idx);
  return ss.str();
}

double evawiz::ToNumber(const char*str){
  double num;
  sscanf(str,"%lf",&num);
  return num;
}
  
evawiz::complex evawiz::ToComplex(const char*str){
  complex num;
  sscanf(str,"(%lf,%lf)",&num.re,&num.im);
  return num;
}

/*
int evawiz::compare(EvaType type1,EvaType type2){
  if ( type1< type2 ) return -1;
  else if (type1> type2) return 1;
  else return 0;
}
*/

bool approximatelyEqual(double a, double b, double epsilon) {
  return fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyGreaterThan(double a, double b, double epsilon) {
  return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool definitelyLessThan(double a, double b, double epsilon) {
  return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

int evawiz::compare(double num1,double num2){
  register double cut =  ( (fabs(num1) < fabs(num2) ? fabs(num2) : fabs(num1)) * EvaSettings::epsilon);
  register double diff= num1-num2;
  if ( diff > cut ) return 1;
  if ( diff < -cut ) return -1;
  return 0;
}

int evawiz::compare(complex num1,complex num2){
  register int c1 = compare(num1.re,num2.re);
  return_if_not(c1,0);
  c1 = compare(num1.im,num2.im);
  return c1;
}

int evawiz::compare(const char*str1,const char*str2){
  if ( str1 == NULL ) {
    if ( str2 == NULL )
      return 0;
    else
      return -1;
  }else{
    if ( str2 == NULL )
      return 1;
    else
      return sign(strcmp(str1,str2));
  }
}

int evawiz::compare(string str1,string str2){
  return compare(str1.c_str(), str2.c_str() );
}

template<class type>  void local_swap(type&v1, type&v2){
  std::swap(v1,v2);
}

#define LOCAL_SWAP(type) void evawiz::swap(type&l1,type&l2){local_swap(l1,l2);}
//LOCAL_SWAP(EvaType)
LOCAL_SWAP(short unsigned int)
LOCAL_SWAP(int)
LOCAL_SWAP(u_int)
LOCAL_SWAP(char*)
LOCAL_SWAP(double)
LOCAL_SWAP(float)
LOCAL_SWAP(complex)
LOCAL_SWAP(long)
LOCAL_SWAP(u_long)

#undef LOCAL_SWAP

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// System Operations
int evawiz::StringSplit(string str, vector<string>&strs,string split){
  strs.clear();
  int p,p1,p2;
  for (unsigned i = 1; i < split.size(); i++){
    if (split[i] == split[0]) continue;
    p = -1;
    while ( ( p=str.find( split[i] ,p+1 ) ) >=0 ){
      str[p]=split[0];
      if (p == (int)str.size() -1 ) break;
    }
    
  }
  str += split[0];
  //while ( (p=str.find(""+split[0]+split[0])) >=0 ) str.erase(p,1);
  p2=-1;
  while ( true ){
    p1 = p2;
    if ( (p2 = str.find(split[0],p2+1) )<0 ) break;
    strs.push_back(str.substr(p1+1,p2-p1-1));
    while ( p2<(int)str.size() && str[ p2+1 ] == split[0] ) p2++;
  }
  return 0;
}
int evawiz::StringSplit(string str,vector<string>&strs){
  return StringSplit(str,strs," \n\t,");
}

string evawiz::RandomString(int len){
  if ( len < 1 ) ThrowError("RandomString","Length specified should bigger than zero.");
  string res = "";
  for ( int i = 0 ; i< len; i++ ){
    char ch =  35+56*evawiz::Rand();
    res+=ch;
  }
  return res;
}

string evawiz::RandomNumberString(int len){
  if ( len < 1 ) ThrowError("RandomString","Length specified should bigger than zero.");
  string res = "";
  for ( int i = 0 ; i< len; i++ ){
    char ch =  '0'+(evawiz::Rand()*9);
    res+=ch;
  }
  return res;
}



string evawiz::IthWord(string str,int n){
  int p,p1=-1,p2=-1,c=0;
  if ( str[ str.size() ] == '\n' ){
    str.erase(str.size(),1);
  }
  str=str+" ";
  while ( (p=str.find(',') ) >=0 )
    str[p]=' ';
  while ( (p=str.find("  ")) >=0 )
    str.erase(p,1);
  p1=-1;p2=-1;c=0;
  while ( true ){
    c++; p1 = p2;
    p2 = str.find(" ",p2+1);
    if ( p2<0 ){
      return "";
    }
    if ( c==n ){
      return str.substr(p1+1,p2-p1-1);
    }
    while ( p2<(int)str.size() && str[ p2+1 ] == ' ') p2++;
  }
}

string evawiz::RemoveFirstWord(string str,string &leftword){
  string word = IthWord(str,1);
  leftword = str.substr( word.size() );
  while ( leftword[0] == ' ' ) leftword.erase(0,1);
  return word;
}

string evawiz::GetEnv(string name){
  string content = "";
  char *p;
  if ( !(p = getenv(name.c_str())) ){
    return "";
  }
  content = p;
  return content;
}

int evawiz::SetEnv(string var,string expr,bool overwrite){
  if ( setenv(var.c_str(),expr.c_str(),overwrite )  ){
    Warning("SetEnv","Failed to change environment varible by Expression: "+ expr);
    return -1;
  }
  return 0;
}

string evawiz::GetDirectory(){
  string pwd="";
  char *buffer;  
  if( (buffer = getcwd(NULL, 0)) == NULL) {  
    ThrowError("GetDirectory","Can not get Directory information.");
  }  
  pwd = buffer;
  free( buffer );
  return pwd;
}

string evawiz::GetParentDirectory(){
  char *buffer;  
  if( (buffer = getcwd(NULL, 0)) == NULL) {  
    ThrowError("GetDirectory","Can not get Directory information.");
  }  
  int ind = strrchr(buffer,'/') - buffer;
  if ( ind <= 0 ) return "";
  buffer[ind]='\0';
  string pwd = buffer;
  free( buffer );
  return pwd;
}

string evawiz::GetBaseName(string path){
  const char *base = strrchr(path.c_str(),'/');
  if ( base - path.c_str() <= 0 )
    return path;
  return (base+1);
}

string evawiz::GetDirName(string path){
  int ind = strrchr(path.c_str(),'/') - path.c_str();
  if ( ind <= 0 ) return "";
  return path.substr(0,ind);
}

bool evawiz::SetDirectory(string dirname){
  if ( chdir( dirname.c_str() ) != 0 )
    return false;
  return true;
}

static int LocalSystem(string cmd, string&result){
  result="";
  FILE *fp;
  char line[1024];
  fp = popen(cmd.c_str(),"r");
  if ( fp == NULL ){
    evawiz::Warning("System",cmd+" cannot be executed.");
    return -1;
  }
  if ( fgets(line,sizeof(line),fp) != NULL ){
    if ( line[ strlen(line)-1 ] == '\n' ){
      line[ strlen(line)-1 ] = '\0';
    }
    result=line;
  }
  int rc=pclose(fp); // the return status
  if ( rc == -1 ) return -1;
  if ( WEXITSTATUS(rc) != 0 ) return -1;
  return 0;
}

string evawiz::System(string cmd){
  string result;
  LocalSystem(cmd,result);
  return result;
}


int evawiz::System(string cmd, vector<string>&result){
  result.clear();
  FILE *fp;
  char line[1024];
  fp = popen(cmd.c_str(),"r");
  if ( fp == NULL ){
    cerr<<"  Error: "<<cmd<<" cannot be excuted"<<endl;
    return -1;
  }
  while ( fgets(line,sizeof(line),fp) != NULL ){
    if ( line[ strlen(line)-1 ] == '\n' ){
      line[ strlen(line)-1 ] = '\0';
    }
    result.push_back(line);
  }
  return pclose(fp)/256;
}

int evawiz::Execute(string cmd){
  int status=system(cmd.c_str());
  if ( status == -1 ){
    cerr<<"  Error: "<<cmd<<" cannot be excuted"<<endl;
    return -1;
  }else if ( WEXITSTATUS(status) != 0 ){
    return -1;
  }
  return 0;
}

bool evawiz::FileExistQ(string filename){
  struct stat buf;
  if ( lstat(filename.c_str(),&buf) < 0)
    return false;
  return true;
}

bool evawiz::CheckFileType(string filename,string type){
  struct stat buf;
  if ( lstat(filename.c_str(),&buf) < 0) return false;
  if ( type == "DIR" && S_ISDIR(buf.st_mode) ) return true;
  if ( type == "REG" && S_ISREG(buf.st_mode) ) return true;
  if ( type == "LINK" && S_ISLNK(buf.st_mode) ) return true;
  if ( type == "FIFO" && S_ISFIFO(buf.st_mode) ) return true;
  return false;
}

bool evawiz::CommandExist(string cmd){
  string str;
  if (LocalSystem("type "+cmd+" 2>&1",str)< 0 )
    return false;
  else
    return true;
}

int evawiz::Showing(string str)
{
  cout<<str<<"    "<<flush; System("sleep 0.5");cout<<"\r"<<flush;
  cout<<str<<".   "<<flush; System("sleep 0.5");cout<<"\r"<<flush;
  cout<<str<<"..  "<<flush; System("sleep 0.5");cout<<"\r"<<flush;
  cout<<str<<"... "<<flush; System("sleep 0.5");cout<<"\r"<<flush;
  return 0;
}

void evawiz::sleep_micro(int sec)
{
  usleep(1000*sec);
}

void evawiz::sleep_sec(int sec)
{
  usleep( sec*1000000);
}

string evawiz::GetFileContent(string filename)
{
  string content;
  ifstream ifs;
  ifs.open(filename.c_str() );
  if (ifs){
    string str;
    while ( getline(ifs,str) )
      content +=str+"\n";
    ifs.close();
    return content;
  }  
  return "";
}

int evawiz::SetFileContent(string filename,string content){
  ofstream ofs;
  ofs.open(filename.c_str() );
  if (ofs){
    ofs<<content;
    ofs.close();
    return 0;
  }
  return -1;
}

string evawiz::GetFileMD5(string filename){
  string md5info = IthWord(System("md5sum "+filename),1);
  if (md5info == "md5sum:") return "FILE_NOT_EXIST";
  return md5info;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
unsigned int evawiz::SimpleHashCodeOfString(const char*str){
  register unsigned int code = 5381;
  register const char *po=str;
  if ( str == NULL ) return 0;
  for (;(*po)!='\0';po++){ code= (code*33) ^ (*po); }
  return code;
}

const char* evawiz::Type2String(EvaType type){
  switch(type){
  case EvaType::Number: return "Number";
  case EvaType::String: return "String";
  case EvaType::Symbol: return "Symbol";
  case EvaType::List: return "List";
  }
  ThrowError("Type2String","switch error.");
}


#define CT(type) case RSimpleHashCodeOfString(#type): return AttributeType::type;
AttributeType evawiz::String2AttributeType(const char*str){
  switch ( SimpleHashCodeOfString( str ) ){
  CT(Listable);CT(Flat);CT(Orderless);CT(OneIdentity);
  CT(HoldFirst);CT(HoldRest);CT(HoldAll);CT(HoldAllCompleted);
  CT(SequenceHold); CT(GrammarUnit); 
  CT(Constant); CT(DelayFunction); CT(Conjunctable);
  CT(Setable); CT(DelaySetable); CT(Destroyable);
  CT(Protected);CT(Locked); CT(MAXENUM);
  CT(Null);
  }
  ThrowError("Attribute",(string)"Unknown Attribute type "+str+".");
}
#undef CT

#define CT(type) case AttributeType::type: return #type;
const char* evawiz::AttributeType2String(AttributeType type){
  switch ( type ){
  CT(Listable);CT(Flat);CT(Orderless);CT(OneIdentity);
  CT(HoldFirst);CT(HoldRest);CT(HoldAll);CT(HoldAllCompleted);
  CT(SequenceHold); CT(GrammarUnit);
  CT(Constant); CT(DelayFunction); CT(Conjunctable);
  CT(Setable); CT(DelaySetable); CT(Destroyable);
  CT(Protected);CT(Locked); CT(MAXENUM);
  CT(Null);
  }
  ThrowError("Attribute","Unkown Attribute type.");
}
#undef CT

bool evawiz::AttributeQ(bool*attri,AttributeType type){
  return attri[(u_char)type];
}

bool evawiz::AttributeQ(bool*attri,const char *str){
  return attri[ (u_char)String2AttributeType(str) ];
}

void evawiz::SetAttribute(bool*attri,AttributeType type,bool val){
  attri[(u_char)type] = val;
}

void evawiz::SetAttribute(bool*attri,const char*str,bool val){
  attri[ (u_char)String2AttributeType(str) ] = val;
}

//const string   specialCharecterSet=" \n\t\"\'\\[](){},.;+-*/^~!@#$%&|.:=_<>"; 
bool evawiz::SpecialCharecterQ(char ch){
  switch ( ch ){
  case (' '): case ('\n'): case ('\t'): case ('\"'): case ('\''): case ('\\'): case ('['): case (']'): case ('('): case (')'): case ('{'): case ('}'): case (','): case ('.'): case (';'): case ('+'): case ('-'): case ('*'): case ('/'): case ('^'): case ('~'): case ('!'): case ('@'): case ('#'): case ('$'): case ('%'): case ('&'): case ('|'): case (':'): case ('='): case ('_'): case ('<'): case ('>'): case ('?'): return true;
  default: return false;
  }
}

pair<double,double> evawiz::GetProcessMemUsage()
{
  using std::ios_base;
  using std::ifstream;
  using std::string;

  double vm_usage     = 0.0;
  double resident_set = 0.0;

  // 'file' stat seems to give the most reliable results
  //
  ifstream stat_stream("/proc/self/stat",ios_base::in);

  // dummy vars for leading entries in stat that we don't care about
  //
  string pid, comm, state, ppid, pgrp, session, tty_nr;
  string tpgid, flags, minflt, cminflt, majflt, cmajflt;
  string utime, stime, cutime, cstime, priority, nice;
  string O, itrealvalue, starttime;

  // the two fields we want
  //
  unsigned long vsize;
  long rss;

  stat_stream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr
              >> tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt
              >> utime >> stime >> cutime >> cstime >> priority >> nice
              >> O >> itrealvalue >> starttime >> vsize >> rss; // don't care about the rest

  stat_stream.close();

  long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
  return pair<double,double>(vm_usage     = vsize / 1024.0, resident_set = rss * page_size_kb);
}

bool evawiz::GPUDeviceQ(){
  if ( Execute("nvidia-smi -L >/dev/null 2>&1") != 0 )
    return false;
  return true;
}

double evawiz::GaussRand(double mean,double variance){
  static double gau_var1,gau_var2,gau_s;
  static int gau_phase = 0;
  double x;
  if ( gau_phase == 0 ){
    do{
      double u1 = evawiz::Rand();
      double u2 = evawiz::Rand();
      gau_var1=2*u1-1;
      gau_var2=2*u2-1;
      gau_s=gau_var1*gau_var1+gau_var2*gau_var2;
    }while(gau_s>=1||gau_s==0);
    x=gau_var1*sqrt(-2*log(gau_s)/gau_s);
  }else{ 
    x=gau_var2*sqrt(-2*log(gau_s)/gau_s);
  }
  gau_phase=1-gau_phase;
  return x*variance+mean;
}


