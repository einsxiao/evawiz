#include"eva_headers.h"
#include<execinfo.h>
#include <cxxabi.h> 
using namespace std;
using namespace evawiz;

DebugOutput __DebugOutput::out;


void evawiz::Error::trace()const{
#ifdef DEBUG
  const int buf_size = 1000;
  static void *buffer[buf_size];
  cerr<<"-------------------------------"<<endl;
  cerr<<"Evawiz Backstrace:"<<endl;
  int size = backtrace( buffer, buf_size);
  char**messages = backtrace_symbols(buffer,size);
  // skip first stack frame (points here)
  for (int i = 1; i < size && messages != NULL; ++i){
    char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;
    // find parantheses and +address offset surrounding mangled name
    for (char *p = messages[i]; *p; ++p) {
      if (*p == '(') {
        mangled_name = p; 
      } else if (*p == '+') {
        offset_begin = p;
      } else if (*p == ')') {
        offset_end = p;
        break;
      }
    }

    // if the line could be processed, attempt to demangle the symbol
    if (mangled_name && offset_begin && offset_end && 
        mangled_name < offset_begin) {
      *mangled_name++ = '\0';
      *offset_begin++ = '\0';
      *offset_end++ = '\0';
      
      int status;
      char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

      // if demangling is successful, output the demangled function name
      if (status == 0) {    
        std::cerr <<"("<< i << ")" << messages[i] << ": " 
                  << real_name << "+" << offset_begin << offset_end 
                  << std::endl;

      }
      // otherwise, output the mangled function name
      else {
        std::cerr <<"("<< i << ")" << messages[i] << ": " 
                  << mangled_name << "+" << offset_begin << offset_end 
                  << std::endl;
      }
      free(real_name);
    }else{
      // otherwise, print the whole line
      std::cerr <<"("<< i << ")" << messages[i] << std::endl;
    }
  }
  std::cerr << std::endl;
  cerr<<"-------------------------------"<<endl;
  free(messages);
#endif
}

const char *evawiz::Error::what()const throw(){
  //print backtrace information
  if ( module != "" ){
    return (module+"::"+function+"::Error: "+message).c_str();
  }else{
    if ( function != "" ){
      return (function+"::Error: "+message).c_str();
    }else{
      return ("Error: "+message).c_str();
    }
  }
}

