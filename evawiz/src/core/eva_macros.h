#ifndef EVA_MACROS_EVAWIZ_H
#define EVA_MACROS_EVAWIZ_H

#define InitVar(variable,default_value) if ( not (EvaKernel->GetValue(#variable,variable)) ) { variable = default_value; }
#define InitVar2(variable,default_value) if ( not (EvaKernel->GetValue(#variable,variable)) ) { variable = default_value; }
#define InitVar3(eva_variable_name,variable,default_value) if ( not (EvaKernel->GetValue(eva_variable_name,variable)) ) { variable = default_value; }

#define InitNameVariable(name,variable,default_value)({ \
      if ( not (EvaKernel->GetValue(name,variable)) ) { \
        variable = default_value;                       \
      }                                                 \
    })
#define InitNameVariablePrint(name,variable,default_value)({      \
      if ( not (EvaKernel->GetValue(name,variable)) ) {           \
        variable = default_value;                                 \
        cout<<name<<" is set to default value "<< variable<<endl; \
      }else{                                                      \
        cout<<name<<" is set to "<< variable<<endl;               \
      }                                                           \
    })

#define InitVariable(variable,default_value) InitNameVariable(#variable,variable,default_value)
#define InitVariablePrint(variable,default_value) InitNameVariablePrint(#variable,variable,default_value); 


#define InitNameArray(name,array) ({                    \
      int err=0;                                        \
      Object tempObj;                                   \
      tempObj.SetSymbol(name);                          \
      EvaKernel->Evaluate(tempObj);                     \
      if(not tempObj.ListQ() or tempObj.Size() == 0 ){  \
        err=-1;                                         \
      }else{                                            \
        for (u_int i=0;i<tempObj.Size();i++){           \
          if ( not tempObj[i+1].NumberQ() ){            \
            err = -1;                                   \
            break;                                      \
          }else{                                        \
            array[i] = tempObj[i+1].Number();           \
          }                                             \
        }                                               \
      }                                                 \
      err;                                              \
    })

#define InitNameArrayPrint(name,array) ({               \
      int err=0;                                        \
      Object tempObj;                                   \
      tempObj.SetSymbol(name);                          \
      EvaKernel->Evaluate(tempObj);                     \
      if(not tempObj.ListQ() or tempObj.Size() == 0 ){  \
        err=-1;                                         \
      }else{                                            \
        cout<< name <<" is set to {";                   \
        for (u_int i=0;i<tempObj.Size();i++){           \
          if ( not tempObj[i+1].NumberQ() ){            \
            err = -1;                                   \
            break;                                      \
          }else{                                        \
            array[i] = tempObj[i+1].Number();           \
            if ( i > 0 ) cout<<",";                     \
            cout<<array[i];                             \
          }                                             \
        }                                               \
        cout<<"}"<<endl;                                \
      }                                                 \
      err;                                              \
    })

#define InitArray(array) ({InitNameArray(#array,array);})
#define InitArrayPrint(array) ({InitNameArrayPrint(#array,array);})

#define InitNameMatrix(name,matrix) ({int err=0;  \
      Object tempObj;                             \
      tempObj.SetSymbol(name);                    \
      EvaKernel->Evaluate(tempObj);               \
      if( not MatrixModule::MatrixQ(tempObj) ){   \
        err=-1;                                   \
      }else{                                      \
        MatrixModule::MatrixSet(matrix,tempObj);  \
      }                                           \
      err;                                        \
    })
#define InitNameMatrixPrint(name,matrix) ({int err=0;           \
      Object tempObj;                                           \
      tempObj.SetSymbol(name);                                  \
      EvaKernel->Evaluate(tempObj);                             \
      if( not MatrixModule::MatrixQ(tempObj) ){                 \
        err=-1;                                                 \
      }else{                                                    \
        MatrixModule::MatrixSet(matrix,tempObj);                \
      }                                                         \
      if( err >= 0 ){                                           \
        cout<< name <<" is set to "<<tempObj.ToString()<<endl;; \
      }                                                         \
      err;                                                      \
    })
#define InitMatrix(matrix) InitNameMatrix(#matrix, matrix)
#define InitMatrixPrint(matrix) InitNameMatrixPrint(#matrix, matrix)


#define InitNameObject(name,obj) ({int err=0;         \
      obj.SetSymbol(name);                            \
      EvaKernel->Evaluate(obj);                       \
      if ( obj.SymbolQ( name ) )                \
        err = -1;                               \
      err;                                      \
    })


#define InitNameObjectPrint(name,obj) ({int err=0;          \
      obj.SetSymbol(name);                                  \
      EvaKernel->Evaluate(obj);                             \
      if ( obj.SymbolQ( name ) )                            \
        err = -1;                                           \
      if( err >= 0 ){                                       \
        cout<< name <<" is set to "<<obj.ToString()<<endl;; \
      }                                                     \
      err;                                                  \
    })

#define InitObject(obj) InitNameObject(#obj, obj)
#define InitObjectPrint(obj) InitNameObjectPrint(#obj, obj)

#if defined(__CUDACC__)
#  define __cond_host_device__ __host__ __device__ 
#  define __cond_host__ __host__ 
#  define __cond_device__ __device__ 
#  define __cond_cuda__ 
#else
#  define __cond_host_device__ 
#  define __cond_host__ 
#  define __cond_device__ 
#  define __cond_cuda__  //
#endif

#define return_if_not(value,cvalue) { register auto __temp_value__ = (value); if ( __temp_value__ != (cvalue) ) return __temp_value__; }
#define return_if(value,cvalue) { register auto __temp_value__ = (value); if ( __temp_value__ == (cvalue) ) return __temp_value__; }

#ifdef DEBUG
#  define ThrowError(...)  ({                                 \
      cerr<<endl<<"Error at: "<< __FILE__ <<"("<<__LINE__<<") "<<string(#__VA_ARGS__)<<endl; \
      evawiz::Error err(__VA_ARGS__); err.trace(); throw err; \
    })
#else
#  define ThrowError(...)  ({                         \
      cerr<<endl<<"Error at: "<< __FILE__ <<"("<<__LINE__<<") "<<string(#__VA_ARGS__)<<endl; \
      evawiz::Error err(__VA_ARGS__); throw err;      \
    })
#endif

#define dout __DebugOutput::out

#ifdef  DEBUG
#define dprintf(fmt,...) printf("File: %s, Line: %05d: " fmt "\n",__FILE__,__LINE__,##__VA_ARGS__)
#define sdprintf(fmt,...) printf(fmt "\n",##__VA_ARGS__)
#else
#define dprintf(fmt,...) 
#define sdprintf(fmt,...) 
#endif

#ifdef DEBUG
#  define OUT(str,var) ({ cerr<<__FILE__<<"("<<__LINE__<<#var<<"):"<<str<<" = "<<(var)<<endl; })
#  define VOUT(var) ({ cerr<<__FILE__<<"("<<__LINE__<<"):"<<#var<<" = "<<(var)<<endl; })
#  ifdef __CUDACC__
#    define DMOUT(mat) ({ (mat).DeviceToHost(); (mat).DumpFile(#mat ".dat"); })
#    define MOUT(mat)  ({ (mat).DumpFile(#mat ".dat"); })
#  else
#    define DMOUT(mat) (mat).DumpFile(#mat ".dat");
#    define MOUT(mat) (mat).DumpFile(#mat ".dat");
#  endif
#else
#  define OUT(str,var)
#  define VOUT(var) 
#  define DMOUT(mat)  
#  define MOUT(mat)  
#endif

#define ReturnObject(arg)  ({ Argv = arg; return 1; })
#define ReturnString(arg)  ({ Argv.SetString(arg); return 1; })
#define ReturnSymbol(arg)  ({ Argv.SetSymbol(arg); return 1; })
#define ReturnNumber(arg)  ({ Argv.SetNumber(arg); return 1; })
#define ReturnHold ({return 0;})
#define ReturnNormal ({return 1;})
#define ReturnNull ({Argv.SetNull(); return 1;})
#define ReturnError ({return -1;})

#define CheckArgsShouldNotEqual(Argv,n){if((Argv).Size()==(n)){Warning(ModuleNameStr,(Argv).Key(),(Argv).ToString()+" is forbidden to call with "+evawiz::ToString((Argv).Size())+((Argv).Size()>1?" arguments.":" argument."));ReturnError;}}
#define CheckArgsShouldEqual(Argv,n) { if ( (Argv).Size() != (n) ){ Warning(ModuleNameStr, (Argv).Key(),(Argv).ToString()+" called with "+evawiz::ToString((Argv).Size())+((Argv).Size()>1?" arguments; while ":" argument; while ")+evawiz::ToString(n)+" argument"+((n)>1?"s are":" is")+" required.");ReturnError;}}
#define CheckArgsShouldNoLessThan(Argv,n) { if ( (Argv).Size() < (n) ){ Warning(ModuleNameStr, (Argv).Key(),(Argv).ToString()+" called with "+evawiz::ToString((Argv).Size())+" argument"+((Argv).Size()>1?"s":"")+"; while no less than "+evawiz::ToString(n)+" argument"+((n)>1?"s are":" is")+" required.");ReturnError;}}
#define CheckArgsShouldNoMoreThan(Argv,n) { if ( (Argv).Size() > (n) ){ Warning(ModuleNameStr, (Argv).Key(),(Argv).ToString()+" called with "+evawiz::ToString((Argv).Size())+" argument"+((Argv).Size()>1?"s":"")+"; while no more than "+evawiz::ToString(n)+" argument"+((n)>1?"s are":" is")+" required.");ReturnError;}}
#define CheckArgsShouldBeWithin(Argv,n1,n2) { if ( (Argv).Size()<(n1) || (Argv).Size()>(n2) ){ Warning(ModuleNameStr, (Argv).Key(),(Argv).ToString()+" called with "+evawiz::ToString((Argv).Size())+" argument"+((Argv).Size()>1?"s":"")+"; while the required number of arguments should be in the range of "+ToString(n1)+", "+ToString(n2)+".");ReturnError;}}
#define CheckArgsShouldNotBeWithin(Argv,n1,n2) { if ( (Argv).Size()>=(n1) && (Argv).Size()<=(n2) ){ Warning(ModuleNameStr, (Argv).Key(),(Argv).ToString()+" called with "+evawiz::ToString((Argv).Size())+" argument"+((Argv).Size()>1?"s":"")+"; while the required number of arguments should not be in the range of "+ToString(n1)+", "+ToString(n2)+".");ReturnError;}}
#define CheckArgsShouldInForm(Argv,form) { if ( !Pattern::CheckArgs( (Argv),(form) ) ) { Warning(ModuleNameStr, (Argv).Key(),(Argv).Key()+" should be called in the form of "+(Argv).Key()+"["+form+"]");ReturnError;}}
#define CheckArgsShouldNotBeEmpty(Argv) { if ( (Argv).Size() < 1){ Warning(ModuleNameStr, (Argv).Key(),"Argument of "+(Argv).Key()+" should not be empty.");ReturnError;}}
#define CheckArgsShouldBeEmpty(Argv) { if ( (Argv).Size() < 1){ Warning(ModuleNameStr, (Argv).Key(),"Argument of "+(Argv).Key()+" should be empty.");ReturnError;}}
#define CheckArgsShouldBeNumber(Argv,i) { if ( !(Argv)[i].NumberQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should be a Number.");ReturnError;}}
#define CheckArgsShouldBeString(Argv,i) { if ( !(Argv)[i].StringQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should be a String.");ReturnError;}}
#define CheckArgsShouldBeSymbol(Argv,i) { if ( !(Argv)[i].SymbolQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should be a Symbol.");ReturnError;}}
#define CheckArgsShouldBeList(Argv,i) { if ( (Argv)[i].AtomQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should be an List Object.");ReturnError;}}
#define CheckArgsShouldBeListWithHead(Argv,i,head) { if ( not (Argv)[i].ListQ( #head ) ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should be an Object with head of "+#head+".");ReturnError;}}
#define CheckArgsShouldNotBeNumber(Argv,i) { if ( (Argv)[i].NumberQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should not be a Number.");ReturnError;}}
#define CheckArgsShouldNotBeString(Argv,i) { if ( (Argv)[i].StringQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should not be a String.");ReturnError;}}
#define CheckArgsShouldNotBeSymbol(Argv,i) { if ( (Argv)[i].SymbolQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should not be a Symbol.");ReturnError;}}
#define CheckArgsShouldNotBeList(Argv,i) { if ( (Argv)[i].ListQ() ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should not be an List.");ReturnError;}}
#define CheckArgsShouldNotBeListWithHead(Argv,i,head) { if ( (Argv)[i].ListQ(#head) ){ Warning(ModuleNameStr, (Argv).Key(),Math::OrderForm(i)+" argument should not be an List with head of "+#head+".");ReturnError;}}

#define CheckShouldNotEqual(n) CheckArgsShouldNotEqual(Argv,n)
#define CheckShouldEqual(n) CheckArgsShouldEqual(Argv,n)
#define CheckShouldNoLessThan(n) CheckArgsShouldNoLessThan(Argv,n)
#define CheckShouldNoMoreThan(n) CheckArgsShouldNoMoreThan(Argv,n)
#define CheckShouldBeWithin(n1,n2) CheckArgsShouldBeWithin(Argv,n1,n2)
#define CheckShouldNotBeWithin(n1,n2) CheckArgsShouldNotBeWithin(Argv,n1,n2)
#define CheckShouldInForm(form) CheckArgsShouldInForm(Argv,form)
#define CheckShouldNotBeEmpty() CheckArgsShouldNotBeEmpty(Argv,)
#define CheckShouldBeEmpty() CheckArgsShouldBeEmpty(Argv,)
#define CheckShouldBeNumber(i) CheckArgsShouldBeNumber(Argv,i)
#define CheckShouldBeString(i) CheckArgsShouldBeString(Argv,i)
#define CheckShouldBeSymbol(i) CheckArgsShouldBeSymbol(Argv,i)
#define CheckShouldBeList(i) CheckArgsShouldBeList(Argv,i)
#define CheckShouldBeListWithHead(i,head) CheckArgsShouldBeListWithHead(Argv,i,head)
#define CheckShouldNotBeNumber(i) CheckArgsShouldNotBeNumber(Argv,i)
#define CheckShouldNotBeString(i) CheckArgsShouldNotBeString(Argv,i)
#define CheckShouldNotBeSymbol(i) CheckArgsShouldNotBeSymbol(Argv,i)
#define CheckShouldNotBeList(i) CheckArgsShouldNotBeList(Argv,i)
#define CheckShouldNotBeListWithHead(i,head) CheckArgsShouldNotBeListWithHead(Argv,i,head)

#define ReturnIfNotEqual(n){if( (Argv).Size()!=(n)){ ReturnHold; } }
#define ReturnIfEqual(n) { if ( (Argv).Size() == (n) ){  ReturnHold; } }
#define ReturnIfLessThan(n) { if ( (Argv).Size() < (n) ){  ReturnHold; } }
#define ReturnIfMoreThan(n) { if ( (Argv).Size() > (n) ){  ReturnHold; } }
#define ReturnIfNotWithin(n1,n2) { if ( (Argv).Size()<(n1) || (Argv).Size()>(n2) ){  ReturnHold; } }
#define ReturnIfWithin(n1,n2) { if ( (Argv).Size()>=(n1) && (Argv).Size()<=(n2) ){  ReturnHold; } }
#define ReturnIfNotInForm(form) { if ( !Pattern::CheckArgs( (Argv),(form) ) ) {  ReturnHold; } }
#define ReturnIfNotEmpty() { if ( (Argv).Size() >= 1){  ReturnHold; } }
#define ReturnIfEmpty() { if ( (Argv).Size() < 1){  ReturnHold; } }
#define ReturnIfNotNumber(i) { if ( !(Argv)[i].NumberQ() ){  ReturnHold; } }
#define ReturnIfNotString(i) { if ( !(Argv)[i].StringQ() ){  ReturnHold; } }
#define ReturnIfNotSymbol(i) { if ( !(Argv)[i].SymbolQ() ){  ReturnHold; } }
#define ReturnIfNotAtom(i) { if ( !(Argv)[i].AtomQ() ){  ReturnHold; } }
#define ReturnIfNotList(i) { if ( (Argv)[i].AtomQ() ){  ReturnHold; } }
#define ReturnIfNotListWithHead(i,head) { if ( (Argv)[i].AtomQ() || ( not (Argv).KeyQ(#head) ) ){  ReturnHold; } }

#define Conjunct_Context(func) if ( Argv.ListQ( SYMBOL_ID_OF_Conjunct ) and Argv[1].ListQ( SYMBOL_ID_OF_##func) )
#define Conjunct_Case( sym ) if ( Argv[2].ListQ( SYMBOL_ID_OF_##sym ) )
#define Conjunct_Case_Condition( sym ) ( Argv[2].ListQ( SYMBOL_ID_OF_##sym ) )
#define Conjunct_Case_Variable( sym ) if ( Argv[2].SymbolQ( SYMBOL_ID_OF_##sym ) )

#define Set_Context(func) if ( Argv.ListQ( SYMBOL_ID_OF_Set ) and Argv[1].ListQ( SYMBOL_ID_OF_##func) )

#define DelaySet_Context(func) if ( Argv.ListQ( SYMBOL_ID_OF_Set) and Argv[1].ListQ() and Argv[1][0].ListQ(SYMBOL_ID_OF_##func) )

#define DelayFunction_Context(func) if ( Argv[0].ListQ( SYMBOL_ID_OF_##func ) )

#define Declare_ValueTablePair_Id_Of(sym) Index ValueTablePair_Id_Of_##sym;
#define Init_ValueTablePair_Id_Of(sym) ({ Object obj; obj.SetSymbol(#sym); ValueTablePair_Id_Of_##sym = EvaKernel->GetOrNewValuePairRefAtTop( obj ).objid ; })


#if defined(__CUDACC__)
  inline void HandleError( cudaError_t cu_err,const char *file,int line ) {
    if (cu_err != cudaSuccess) {
      printf( "%s in %s at line %d\n", cudaGetErrorString( cu_err ), file, line );
      exit( EXIT_FAILURE );
    }
  }
#  define CUDA_HANDLE_ERROR( cu_err ) (HandleError( cu_err, __FILE__, __LINE__ ))
#  define CUDA_HANDLE_NULL( a ) {                                     \
    if (a == NULL) { printf( "Host memory failed in %s at line %d\n",	\
                             __FILE__, __LINE__ );                    \
                     exit( EXIT_FAILURE );}}
#  define CUDA_LAST_ERROR() ({ cudaError_t cu_err = cudaGetLastError(); if ( cu_err!=cudaSuccess ) ThrowError("Cuda", (string) cudaGetErrorString(cu_err)); })
#else
#  define CUDA_HANDLE_ERROR( cu_err ) 
#  define CUDA_HANDLE_NULL( a )
#  define CUDA_LAST_ERROR() 
#endif

#define _blockDim3(ni,nj,nk) dim3(ni,nj,nk)
#define _blockDim2(ni,nj) dim3(ni,nj)
#define _blockDim1(ni) dim3(ni)
#define _blockIdx3(ii,jj,kk) const int ii = blockIdx.x,jj = blockIdx.y,kk = blockIdx.z
#define _blockIdx2(ii,jj) const int ii = blockIdx.x, jj = blockIdx.y
#define _blockIdx1(ii) const int ii = blockIdx.x

#define _threadDim3(ni,nj,nk) dim3(ni,nj,nk)
#define _threadDim2(ni,nj) dim3(ni,nj)
#define _threadDim1(ni) dim3(ni)
#define _threadIdx3(ii,jj,kk) const int ii = threadIdx.x, jj = threadIdx.y, kk = threadIdx.z
#define _threadIdx2(ii,jj) const int ii = threadIdx.x, jj = threadIdx.y;
#define _threadIdx1(ii) const int ii = threadIdx.x

#define __CudaThreadNumberPerBlock evawiz::EvaSettings::cudaThreadsNumberPerBlock
#define _cuDim(N) dim3(__CudaBlockNumber(N)),dim3(__CudaThreadNumberPerBlock)




#endif
