#ifndef EvaError_Evawiz_H
#define EvaError_Evawiz_H

namespace evawiz{
  class Error:public exception{
  public:
    string module,function,message;
    Error(){}
    Error(string mes){ message =mes ; }
    Error(string func,string mes){ function=func; message = mes; }
    Error(string mod, string func,string mes){ module = mod; function = func ;message = mes; }
    virtual ~Error()throw(){};
    virtual const char *what()const throw();
    void trace()const;
    inline friend ofstream &operator<<(ofstream &os, Error e){ os<<e.what()<<endl; return os;}
    inline friend ostream &operator<<(ostream &os, Error e){ os<<e.what()<<endl; return os;}
    static string decode_undefined_sybmol_string(char*message);
  };
  class ExceptionQuit:public Error{
  public:
    int code;
    ExceptionQuit(){ code = 0; }
    ExceptionQuit(int c){ code = c; }
  };


  class DebugOutput{
  public:

    template<class type>
    DebugOutput&operator<<(type val){
#ifdef DEBUG
      cerr<<val;
#endif
      return *this;
    }

    typedef DebugOutput& (*manipulator)(DebugOutput&);

    DebugOutput&operator<<(manipulator manip){
      return manip(*this);
    }

    static DebugOutput&endl(DebugOutput&out){
#ifdef DEBUG
      cerr<<std::endl;
#endif
      return out;
    }

    // this is the type of std::cout
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    // this is the function signature of std::endl
    typedef CoutType& (*StandardEndLine)(CoutType&);

    // define an operator<< to take in std::endl
    DebugOutput& operator<<(StandardEndLine manip)
    {
      // call the function, but we cannot return it's value
#ifdef DEBUG
      manip(std::cerr);
#endif
      return *this;
    }
  };
  class __DebugOutput{public: static DebugOutput out;};

  

  

};


#endif
