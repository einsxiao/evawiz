#include<evawiz.h>
#include"MPIModule.h"
#include"mpi.h"
//#include"cublas_v2.h"
//#include"cblas.h"
using namespace std;
using namespace evawiz;
#define ModuleName MPI
__DEBUG_MAIN__("input.ev");

ModuleInitialize
{
}

ModuleFinalize
{
  if ( initialized ) Finalize();
} 

void MPIModule::Init()
{
  if ( initialized ) return;
  MPI_Init( &(EvaKernel->argc), &(EvaKernel->argv) );
  MPI_Comm_rank( MPI_COMM_WORLD, &rankID );
  MPI_Comm_size( MPI_COMM_WORLD, &rankSize );
  int namelen;
  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Get_processor_name( processor_name, &namelen );
  processorName = processor_name;
  initialized = true;
}
void MPIModule::Finalize()
{
  MPI_Finalize();
  initialized = false;
}

static void index_init(int n,int*ind)
{
  for ( int i=0; i< n; i++ )
    ind[i] = 0;
}

static void index_copy(int n, int*ind1,int*ind2)
{
  memcpy( ind1, ind2, n*sizeof(int) );
}

static bool index_incr(int*dim,int*ind)
{
  int p = dim[0] - 1;
  ind[p] ++;
  while ( true ){
    if ( ind[p] == dim[ p+1 ] ){
      if ( p == 0 )
        break;
      ind[p] = 0;
      p--;
      ind[p]++;
    }else{
      return true;
    }
  }
  return false;
}

/*
#define mat_type double
#define mpi_type MPI_DOUBLE
void   MPIModule::Distribute(Matrix_T<mat_type>&global_matrix,Matrix_T<mat_type>&local_matrix, int ith )  
{                                                                      
  Init();                                                              
  int local_size = 0;                                                  
  MPI_Status status;                                                   
  if ( rankID == 0 ){                                                  
    if ( global_matrix.Dim(ith)%rankSize != 0 ){                       
      for (int i=1; i< rankSize; i++ ){                     
        MPI_Send( &local_size, 1 , MPI_INT, i, 1, MPI_COMM_WORLD);     
      }                                                                
      ThrowError("MPI","Distribute","Task dividing requires that size of "+Math::OrderForm(ith)+" matrix dimension should be multiple of the rank size.");  
    }                                                                  
    int *dim = global_matrix.NewDimArray();                            
    dim[ith] = dim[ith]/rankSize;                                      
    local_matrix.Init(dim);                                            
    local_size = local_matrix.Size();                                  
    int *lind = new int[ dim[0] ], *gind = new int[ dim[0] ];          
    for (int i=rankSize-1; i>= 0; i-- ){                               
      index_init(dim[0], lind );                                       
      while ( true ){                                                  
        index_copy(dim[0], gind, lind);                                
        gind[ith-1] += dim[ ith ]*i;                            
        local_matrix(lind) = global_matrix( gind );                    
        if (not index_incr(dim, lind ) ) break;                        
      }                                                                
      if ( i != 0 ){                                                   
        MPI_Send( dim, 1 , MPI_INT,i,1,MPI_COMM_WORLD);                
        MPI_Send( dim+1, global_matrix.ND, MPI_INT,i,1,MPI_COMM_WORLD);  
        MPI_Send( local_matrix.Data, local_size, mpi_type, i, 1, MPI_COMM_WORLD);  
      }                                                                
    }                                                                  
    delete []dim; delete []lind; delete []gind;                        
  }else{                                                               
    int n;                                                             
    MPI_Recv( &n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status );         
    if ( n == 0 )                                                      
      ThrowError("MPI","Distribute","Task terminated when distributing Matrix.");  
    int *dim = new int[n+1];                                           
    dim[0] = n;                                                        
    MPI_Recv( dim+1, n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status );      
    local_matrix.Init(dim);                                            
    local_size = local_matrix.Size();                                  
    MPI_Recv( local_matrix.Data, local_size, mpi_type, 0, 1, MPI_COMM_WORLD, &status);  
    delete []dim;                                                      
  }                                                                    
  return;                                                             
}                                                                     

#undef mat_type 
#undef mpi_type
*/
 

#define _def_dis(mat_type, mpi_type)                                    \
  void   MPIModule::Distribute(Matrix_T<mat_type>&global_matrix,Matrix_T<mat_type>&local_matrix, int ith ) \
  {                                                                     \
    Init();                                                             \
    int local_size = 0;                                                 \
    MPI_Status status;                                                  \
    if ( rankID == 0 ){                                                 \
      if ( global_matrix.Dim(ith)%rankSize != 0 ){                      \
        for (int i=1; i< rankSize; i++ ){                    \
          MPI_Send( &local_size, 1 , MPI_INT, i, 1, MPI_COMM_WORLD);    \
        }                                                               \
        ThrowError("MPI","Distribute","Task dividing requires that size of "+Math::OrderForm(ith)+" matrix dimension should be multiple of the rank size."); \
      }                                                                 \
      int *dim = global_matrix.NewDimArray();                           \
      dim[ith] = dim[ith]/rankSize;                                     \
      local_matrix.Init(dim);                                           \
      local_size = local_matrix.Size();                                 \
      int *lind = new int[ dim[0] ], *gind = new int[ dim[0] ];         \
      for (int i=rankSize-1; i>= 0; i-- ){                              \
        index_init(dim[0], lind );                                      \
        while ( true ){                                                 \
          index_copy(dim[0], gind, lind);                               \
          gind[ith-1] += dim[ ith ] * i;                                \
          local_matrix(lind) = global_matrix( gind );                   \
          if (not index_incr(dim, lind ) ) break;                       \
        }                                                               \
        if ( i != 0 ){                                                  \
          MPI_Send( dim, 1 , MPI_INT,i,1,MPI_COMM_WORLD);               \
          MPI_Send( dim+1, global_matrix.ND, MPI_INT,i,1,MPI_COMM_WORLD); \
          MPI_Send( local_matrix.Data, local_size, mpi_type, i, 1, MPI_COMM_WORLD); \
        }                                                               \
      }                                                                 \
      delete []dim; delete []lind; delete []gind;                       \
    }else{                                                              \
      int n;                                                            \
      MPI_Recv( &n, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status );        \
      if ( n == 0 )                                                     \
        ThrowError("MPI","Distribute","Task terminated when distributing Matrix."); \
      int *dim = new int[n+1];                                          \
      dim[0] = n;                                                       \
      MPI_Recv( dim+1, n, MPI_INT, 0, 1, MPI_COMM_WORLD, &status );     \
      local_matrix.Init(dim);                                           \
      local_size = local_matrix.Size();                                 \
      MPI_Recv( local_matrix.Data, local_size, mpi_type, 0, 1, MPI_COMM_WORLD, &status); \
      delete []dim;                                                     \
    }                                                                   \
    return;                                                             \
  }                                                                     \

_def_dis( float,          MPI_FLOAT)
_def_dis( double ,        MPI_DOUBLE )
_def_dis( floatcomplex,   MPI_COMPLEX)
_def_dis( complex,        MPI_DOUBLE_COMPLEX)
_def_dis( int    ,        MPI_INT )
#undef _def_dis

#define _def_col(mat_type, mpi_type )                                   \
  void MPIModule::Collect(Matrix_T<mat_type>&local_matrix, Matrix_T<mat_type>&global_matrix, int ith) \
  {                                                                     \
    Init();                                                             \
    int local_size = local_matrix.Size();                               \
    int size = local_size * rankSize;                                   \
    MPI_Status status;                                                  \
    if ( rankID == 0 ){                                                 \
      int *dim = local_matrix.NewDimArray();                            \
      dim[ith] *= rankSize;                                             \
      global_matrix.Init( dim );                                        \
      dim[ ith ] /= rankSize;                                           \
      int *lind=new int[ dim[0] ],*gind=new int[ dim[0] ];              \
      for ( int i=0; i< rankSize; i++ ){                                \
        if ( i != 0 ){                                                  \
          MPI_Recv( local_matrix.Data, local_size, mpi_type,i, 2, MPI_COMM_WORLD,&status); \
        }                                                               \
        index_init(dim[0], lind );                                      \
        while ( true ){                                                 \
          index_copy(dim[0], gind, lind);                               \
          gind[ith-1] += dim[ ith ] * i;                                \
          global_matrix( gind ) = local_matrix( lind );                 \
          if ( not index_incr(dim, lind ) ) break;                      \
        }                                                               \
      }                                                                 \
      delete []dim; delete []lind; delete []gind;                       \
    }else{                                                              \
      MPI_Send( local_matrix.Data, local_size, mpi_type, 0, 2, MPI_COMM_WORLD); \
    }                                                                   \
    return;                                                             \
  }                                                                     \

_def_col( float,          MPI_FLOAT)
_def_col( double ,        MPI_DOUBLE )
_def_col( floatcomplex,   MPI_COMPLEX)
_def_col( complex,        MPI_DOUBLE_COMPLEX)
_def_col( int    ,        MPI_INT )
#undef _def_col

void MPIModule::Barrier()
{
  Init();
  MPI_Barrier( MPI_COMM_WORLD );
}

bool MPIModule::IsRootRank()
{
  Init();
  return rankID == 0;
}


DefineFunction(MPITest)
{

  Init(); 
  
  Matrix in,data,out;
  //prepare data
  int size = 4;
  if ( rankID == 0 ){
    in.Init(2,size,size);
    for (int i=0;i<in.Size();i++){
      in[i] = i;
    }
    cout<< in<<endl;
  }

  printf("process %d of %d on %s:\n",rankID,rankSize,processorName.c_str() );

  Distribute( in, data );

  data+= 33;

  printf("temp result of process %d of %d on %s:\n",rankID,rankSize,processorName.c_str() );
  cout<<data<<endl;

  Collect( data, out );

  if ( rankID == 0 ){
    cout<<"Final result:"<<endl;
    cout<<out<<endl;
  }

  Finalize();

  ReturnNull;
}



