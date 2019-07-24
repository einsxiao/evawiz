#include"evawiz.h"
#include"MatrixModule.h"
#include"eva_oldmacro.h"
using namespace std;
using namespace evawiz;

#undef ModuleNameStr
#define ModuleNameStr "Matrix"

__DEBUG_MAIN__("input.ev");

__CREATE_MODULE__(Matrix)
MatrixModule::MatrixModule(__CONSTRUCTION_PARA__){
  {
    RegisterFunction("Matrix",(MemberFunction)(&MatrixModule::Matrix_Eva));
    AddAttribute("Matrix",AttributeType::Conjunctable);
    AddAttribute("Matrix",AttributeType::Setable);
  }
  {
    RegisterFunction("MatrixExist",(MemberFunction)(&MatrixModule::MatrixExist_Eva));
  }
  {
    RegisterFunction("MatrixPosition",(MemberFunction)(&MatrixModule::MatrixPosition)); AddAttribute("MatrixPosition","Protected");
  }
  {
    RegisterFunction("MatrixSetPosition",(MemberFunction)(&MatrixModule::MatrixSetPosition)); AddAttribute("MatrixSetPosition","Protected");
  }
}

MatrixModule::~MatrixModule(){
};


///////////////

Matrix *MatrixModule::GetMatrix(string name){
  return GlobalPool.Matrices.GetMatrix(name);
}

ComplexMatrix *MatrixModule::GetComplexMatrix(string name){
  return GlobalPool.ComplexMatrices.GetMatrix(name);
}

Matrix *MatrixModule::GetOrNewMatrix(string name){
  return GlobalPool.Matrices.GetOrNewMatrix(name);
}

ComplexMatrix *MatrixModule::GetOrNewComplexMatrix(string name){
  return GlobalPool.ComplexMatrices.GetOrNewMatrix(name);
}

int MatrixModule::RemoveMatrix(string name){
  return GlobalPool.Matrices.RemoveMatrix(name);
}

int MatrixModule::RemoveComplexMatrix(string name){
  return GlobalPool.ComplexMatrices.RemoveMatrix(name);
}


//////////////////////

Matrix&MatrixModule::operator()(string id){
  return *EvaKernel->GetMatrix(id);
}

Matrix&MatrixModule::operator[](string id){
  return *EvaKernel->GetOrNewMatrix(id);
}

int MatrixModule::Matrix_Eva(Object &Argv){
  Conjunct_Context(Matrix){
    Object&matobj = Argv[1];
    Object&operobj = Argv[2];
    CheckArgsShouldEqual(matobj,1);
    CheckArgsShouldBeString(matobj,1);
    string matname = matobj[1].Key();

    Conjunct_Case( Set ){
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      if ( operobj[1].NumberQ() ){
        Matrix* matptr = GetMatrix( matname );
        if ( matptr == NULL )
          ThrowError("Matrix","Assign a number to an empty Matrix is not allowed.");
        *matptr = operobj[1].Number();
        Argv.SetTo(1);
        ReturnNormal;
      }
      CheckArgsShouldBeList(operobj,1);
      Matrix* matptr = GetOrNewMatrix( matname );
      MatrixSet( *matptr, operobj[1] );
      Argv.SetTo(1);
      ReturnNormal;
    }

    Conjunct_Case( ReadFile ){
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeString(operobj,1);
      string filename = operobj[1].Key();
      if ( not FileExistQ( filename ) )
        ThrowError(Argv.Key(),"data file "+filename+" deos not exist.");
      Matrix* matptr = GetOrNewMatrix( matname );
      if ( matptr->ReadFile(filename)<0 ){
        ThrowError(Argv.Key(),"data file "+filename+" deos not exist.");
      }
      Argv.SetTo(1);
      ReturnNormal;
    }

    Conjunct_Case( BinaryReadFile ){
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeString(operobj,1);
      string filename = operobj[1].Key();
      if ( not FileExistQ( filename ) )
        ThrowError(Argv.Key(),"data file "+filename+" deos not exist.");
      Matrix* matptr = GetOrNewMatrix( matname );
      if ( matptr->BinaryReadFile(filename)<0 ){
        ThrowError(Argv.Key(),"data file "+filename+" deos not exist.");
      }
      Argv.SetTo(1);
      ReturnNormal;
    }

    Conjunct_Case( ReadVTKFile ){
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeString(operobj,1);
      string filename = operobj[1].Key();
      if ( not FileExistQ( filename ) )
        ThrowError(Argv.Key(),"data file "+filename+" deos not exist.");
      Matrix* matptr = GetOrNewMatrix( matname );
      if ( matptr->ReadVTKFile(filename)<0 ){
        ThrowError(Argv.Key(),"data file "+filename+" deos not exist.");
      }
      Argv.SetTo(1);
      ReturnNormal;
    }

    Matrix* matptr = GetMatrix( matname );

    Conjunct_Case( ToList ){
      if ( matptr == NULL ) ThrowError("Matrix","ToList",matobj.ToString()+" does not exist yet." );
      CheckArgsShouldEqual(operobj,0);
      Object res;
      MatrixGet( *matptr, res );
      Argv = res;
      ReturnNormal;
    }

    Conjunct_Case( Size ){
      if ( matptr == NULL ) ThrowError("Matrix","Size",matobj.ToString()+" does not exist yet." );
      CheckArgsShouldEqual(operobj,0);
      Argv.SetList_List();
      Argv.PushBackNumber( matptr->Size() );
      if ( matptr->ND > 0 ){
        Object dim; dim.SetList_List();
        for (int i=1; i <= matptr->ND; i++ )
          dim.PushBackNumber( matptr->Dim(i) );
        Argv.PushBackRef( dim );
      }
      ReturnNormal;
    }

    Conjunct_Case( DeviceToHost ){
      if ( matptr == NULL ) ThrowError("Matrix","DeviceToHost",matobj.ToString()+" does not exist yet." );
      CheckArgsShouldEqual(operobj,0);
      if ( matptr->DataDevice == NULL )
        ThrowError("Matrix","DeviceToHost",matobj.ToString() + " has no data on Device side; operation cannot be completed.");
      matptr->DeviceToHost();
      ReturnNormal;
    }

    Conjunct_Case( HostToDevice ){
      if ( matptr == NULL ) ThrowError("Matrix","HostToDevice",matobj.ToString()+" does not exist yet." );
      CheckArgsShouldEqual(operobj,0);
      if ( matptr->Data == NULL )
        ThrowError("Matrix","HostToDevice",matobj.ToString() +" has no data on Host side; operation cannot be completed.");
      matptr->HostToDevice();
      ReturnNormal;
    }

    Conjunct_Case( DumpFile ){
      if ( matptr == NULL ) ThrowError("Matrix","DumpFile",matobj.ToString()+" does not exist yet." );
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeString(operobj,1);
      string filename = operobj[1].Key();
      if ( matptr->DumpFile(filename)<0 ){
        ThrowError(Argv.Key(),"data file "+filename+" can not be written.");
      }
      ReturnNull;
    }

    Conjunct_Case( BinaryDumpFile ){
      if ( matptr == NULL ) ThrowError("Matrix","DumpFile",matobj.ToString()+" does not exist yet." );
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeString(operobj,1);
      string filename = operobj[1].Key();
      if ( matptr->BinaryDumpFile(filename)<0 ){
        ThrowError(Argv.Key(),"data file "+filename+" can not be written.");
      }
      ReturnNull;
    }

    Conjunct_Case( DumpVTKFile ){
      if ( matptr == NULL ) ThrowError("Matrix","DumpFile",matobj.ToString()+" does not exist yet." );
      CheckArgsShouldEqual(operobj,1);
      EvaKernel->Evaluate( operobj[1] );
      CheckArgsShouldBeString(operobj,1);
      string filename = operobj[1].Key();
      if ( matptr->DumpVTKFile(filename)<0 ){
        ThrowError(Argv.Key(),"data file "+filename+" can not be written.");
      }
      ReturnNull;
    }

    Conjunct_Case( Clear ){
      if ( matptr == NULL ) ThrowError("Matrix","Clear",matobj.ToString()+" does not exist yet." );
      RemoveMatrix( matname );
    }

    ReturnNormal;
  }

  Set_Context(Matrix){
    CheckArgsShouldEqual(Argv[1],1);
    CheckArgsShouldBeString(Argv[1],1);
    EvaKernel->Evaluate( Argv[2] );
    MatrixSet( Argv[1][1].Key(), Argv[2] );
    Argv.SetTo(2);
    ReturnNormal;
  }
  
  CheckShouldNoLessThan(1);
  if ( Argv[1].StringQ() ){//A Single string, only to figure out the name of the matrix, without care about the existence
    if ( Argv.Size() > 1 )
      ThrowError("Matrix","When specify a Matrix with its name, one argument is required.");
    ReturnNormal;
  }

  if ( Argv[1].NumberQ() ){// form of Matrix[3,{5,9,4},"HostDevice"]
    int dimN=(double)Argv[1];
    if ( dimN < 1)
      ThrowError("Matrix","The number of Matrix Dimensions should be a positive integer.");
    if ( Argv.Size() < 2 ){
      ThrowError("Matrix","When Init Matrix with Dimensions, two or more arguments are required.");
    }
    int dim[dimN+2]; dim[0]=dimN;
    if ( (int)Argv[2].Size() != dimN )
      ThrowError("Matrix","The dimension specification is not consistent with previous argument.");


    for ( int i=1; i<=dimN; i++ ){
      if ( ! Argv[2][i].NumberQ() )ThrowError("Matrix","Matrix Dimension specification should be numbers.");
      dim[i] = (double)Argv[2][i];
      if ( dim[i] < 1) ThrowError("Matrix","Matrix Dimension should be a positive integer.");
    }
    int matPos= MatrixHost;
    if ( Argv.Size() > 2 ){
      CheckShouldBeString(3);
      if ( Argv[3].StringQ("Host") ) matPos = MatrixHost;
      else if ( Argv[3].StringQ("Device") ) matPos = MatrixDevice;
      else if ( Argv[3].StringQ("HostDevice") ) matPos = MatrixHostDevice;
      else {
        ThrowError("Matrix","When Init Matrix, position specification should be one of Host, Device, or HostDevice.");
      }
    }
    for ( ;;){
      string name = evawiz::RandomString( Len_Of_New_Matrix_Name );
      Matrix *mat = GetMatrix(name);
      if ( mat == NULL ){
        mat = GetOrNewMatrix(name);
        mat->Init(dim,matPos);
        Argv.DeleteElements();
        Argv.PushBackString( name );
        break;
      }
    }
    ReturnNormal;
  }
  if ( Argv[1].ListQ_List()  ){
    if ( Argv.Size() > 1 )
      ThrowError("Matrix","When Init Matrix with a List, only one argument is required.");
    for ( ;;){
      string name = evawiz::RandomString( Len_Of_New_Matrix_Name );
      Matrix *mat = GetMatrix(name);
      if ( mat == NULL ){
        mat = GetOrNewMatrix(name);
        MatrixSet( *mat, Argv[1] );
        Argv.DeleteElements();
        Argv.PushBackString( name );
        break;
      }
    }

      
  }
  ReturnHold;
}

int MatrixModule::MatrixGet(string name,Object &Argv){//mat of Object form
  Matrix * mat = GetMatrix(name);
  if ( mat == NULL ){
    Argv.SetNull();
    return 0;
  }else{
    MatrixModule::MatrixGet(*mat,Argv);
    return 1;
  }
}


int MatrixModule::MatrixSet(string name,Object&Argv){
  Matrix *mat = GetOrNewMatrix(name);
  return MatrixSet(*mat,Argv); 
} 

int MatrixModule::MatrixPosition(Object &Argv){
  CheckArgsShouldEqual(Argv,0)
  if ( EvaSettings::MatrixPosition() == MatrixDevice )
    Argv.SetString("Device");
  else if ( EvaSettings::MatrixPosition() == MatrixHostDevice )
    Argv.SetString("HostDevice");
  else Argv.SetString("Host");
  ReturnNormal;
}

int MatrixModule::MatrixSetPosition(Object &Argv){
  CheckArgsShouldEqual(Argv,1);
  CheckShouldBeString(1);
  if ( Argv[1].StringQ( "Host" ) )
    EvaSettings::SetMatrixPosition(MatrixHost);
  else if ( Argv[1].StringQ("Device") )
    EvaSettings::SetMatrixPosition(MatrixDevice);
  else if ( Argv[1].StringQ("HostDevice") )
    EvaSettings::SetMatrixPosition( MatrixHostDevice);
  else{
    ThrowError(Argv.Key(),"position specification should be one of {\"Host\",\"Device\",\"HostDevice\"}.");
  }
  ReturnNull;
}

int LocalMatrixQ(Object &Argv,Object &dim){
  if ( not Argv.ListQ_List() ){
    if ( !Argv.NumberQ() ) return -1;
    return 1;
  }
  if ( dim.NullQ() )
    dim.SetList_List();
  dim.PushBackNumber( Argv.Size() );
  Object dim1;
  if ( LocalMatrixQ( Argv[1], dim1 ) < 0 ){
    dim.SetNull();
    return -1;
  }
  for ( u_int i=2; i<=Argv.Size(); i++ ){
    Object dim2;
    LocalMatrixQ(Argv[i],dim2);
    if ( dim1 != dim2 ){
      dim.SetNull();
      return -1;
    }
  }
  if ( not dim1.NullQ() ){
    dim.InsertRef( dim.End(), dim1.Begin(), dim1.End() );
  }
  return 1;
}

bool MatrixModule::MatrixQ(Object & Argv,Object&dim){
  dim.SetList_List();
  if ( LocalMatrixQ( Argv,dim)>0 )
    return true;
  return false;
}


bool MatrixModule::MatrixQ(Object&Argv){
  Object dim;
  return MatrixQ( Argv, dim );
}

bool MatrixModule::MatrixExist(string name){
  if ( GetMatrix(name ) != NULL ) return true;
  return false;
}

int MatrixModule::MatrixExist_Eva(Object &Argv){
  CheckShouldEqual(1);
  CheckShouldBeString(1);
  if ( MatrixExist( Argv[1].Key() ) ){
    Argv.SetNumber( true );
    ReturnNormal;
  }
  Argv.SetNumber(false);
  ReturnNormal;
}

#define LOCAL_ARRAY_2_LIST(type,to_type)                                \
  static int LocalArray2List(Object &Argv,type*arr,int *dim,int pdim, int parr){ \
    if ( pdim>dim[0] ){                                                 \
      Argv.SetNumber( (to_type) arr[parr] );                            \
      return parr+1;                                                    \
    }                                                                   \
    Argv.SetList_List();                                                \
    for (int i=0; i<dim[pdim]; i++){                                    \
      Argv.PushBackNull();                                              \
      parr=LocalArray2List( Argv[i+1],arr,dim,pdim+1,parr);             \
    }                                                                   \
    return parr;                                                        \
  }

LOCAL_ARRAY_2_LIST(int,int)
LOCAL_ARRAY_2_LIST(long long,double)
LOCAL_ARRAY_2_LIST(float,double)
LOCAL_ARRAY_2_LIST(double,double)
LOCAL_ARRAY_2_LIST(complex,complex)
#undef LOCAL_ARRAY_2_LIST
#define ARRARY_2_LIST(type) \
int MatrixModule::Array2List(Object &Argv,type*arr,int *dim){ \
  LocalArray2List( Argv,arr,dim,1,0); \
  return 0; \
}
ARRARY_2_LIST(int)
ARRARY_2_LIST(long long)
ARRARY_2_LIST(float)
ARRARY_2_LIST(double)
ARRARY_2_LIST(complex)
#undef ARRARY_2_LIST

int MatrixModule::MatrixGet(Matrix&matrix,Object &Argv){
  Argv.SetList_List();
  if ( not matrix.Size() == 0 ){
    int *dim = matrix.NewDimArray();
    Array2List(Argv,matrix.Data,dim);
    delete []dim;
  }else{
    return -1;
  }
  return 1;
}

int MatrixModule::MatrixGet(ComplexMatrix&matrix,Object &Argv){
  if ( matrix.Size() != 0 ){
    int *dim = matrix.NewDimArray();
    Array2List(Argv,matrix.Data,dim);
    delete []dim;
  }else{
    return -1;
  }
  return 1;
}

static int LocalList2Array(Object &Argv,double*arr, int parr){
  if ( Argv.AtomQ() ){
    if ( Argv.NumberQ() ){
      arr[parr] = double(Argv);
      return parr+1;
    }else{
      Warning("List2Array","Non-number encontered in operation of Argv to array.");
      return -1;
    }
  }
  for (u_int i=0; i<Argv.Size(); i++){
    parr=LocalList2Array( Argv[i+1],arr,parr);
    if ( parr<0 ) return -1;
  }
  return parr;
}


static int LocalList2Array(Object &Argv,complex*arr, int parr){
  if ( Argv.AtomQ() ){
    if ( Argv.NumberQ() ){
      arr[parr] = Argv.Number();
      return parr+1;
    }else{
      Warning("List2Array","Non-number encontered in operation of Argv to array.");
      return -1;
    }
  }
  for (u_int i=0; i<Argv.Size(); i++){
    parr=LocalList2Array( Argv[i+1],arr,parr);
    if ( parr<0 ) return -1;
  }
  return parr;
}

int MatrixModule::List2Array(Object &Argv,double*arr){
  LocalList2Array(Argv,arr,0);
  return 0;
}
int MatrixModule::List2Array(Object &Argv,complex*arr){
  LocalList2Array(Argv,arr,0);
  return 0;
}

template<class type1>
void localMatrixSet(Matrix_T<type1>&mat,Object&Argv,u_long&ind){
  if ( Argv.ListQ() ){
    for(u_int i=1;i<=Argv.Size();i++){
      localMatrixSet(mat,Argv[i],ind);
    }
    return;
  }else if ( Argv.NumberQ() ){
    mat.Data[ind] = (type1)Argv;
    ind++;
    return;
  }
  ThrowError("MatrixSet","Object is not all numbers.");
}

#define _MATRIX_SET(type_basic)                                         \
  int MatrixModule::MatrixSet(Matrix_T<type_basic>&matrix,Object&Argv){ \
    if ( Argv.NumberQ() ){                                              \
      if ( matrix.Size() == 0  )                                        \
        ThrowError("MatrixSet","Assign a number to an empty Matrix is not allowd."); \
      matrix = Argv.Number();                                           \
    }                                                                   \
    Object dim; dim.SetList_List();                                     \
    if ( not MatrixQ( Argv, dim ) )                                     \
      ThrowError("MatrixSet","Matrix assignment from list should be of Matrix form."); \
    int n =dim.Size();                                                  \
    int *arr= new int[n+2];                                             \
    arr[0]=n;                                                           \
    for (int i=1; i<=n; i++){                                           \
      arr[i] =  (int)dim[i] ;                                           \
    }                                                                   \
    matrix.Init( arr ,MatrixHost);                                      \
    u_long ind=0;                                                       \
    localMatrixSet( matrix, Argv, ind);                                 \
    delete []arr;                                                       \
    return 1;                                                           \
  }; 
_MATRIX_SET(complex);
_MATRIX_SET(double);
_MATRIX_SET(float);
_MATRIX_SET(floatcomplex);
_MATRIX_SET(int);
#undef _MATRIX_SET


/*
Matrix &MatrixModule::GetTempMatrix(Matrix mat)
{
  int first = 0, final = tempMatrix.size() -1, pos = 0,result = 0;
  auto sIter = tempMatrix.begin();
  vector<Matrix*>::iterator iter;
  while ( first <= final ){
    pos = first + ( final - first )/2;
    result = mat.DimensionCompare( tempMatrix[pos].first );
    if ( result == 0 ){
      sIter = tempMatrix.begin() + pos;
      break;
    }else if ( result < 0 ){
      final = pos -1;
    }else{
      first = pos +1;
    }
  }
  if ( result != 0 ){
    if ( pos >= (int)tempMatrix.size() ){
      tempMatrix.push_back(  pair<int*,vector<Matrix*> >() );
      tempMatrix.rbegin()->first = mat.NewDimArray();
      sIter = tempMatrix.end() - 1;
    }else{
      if ( result > 0 ) pos++;
      tempMatrix.insert( tempMatrix.begin()+pos, pair<int*,vector<Matrix*> >() );
      sIter =  tempMatrix.begin()+pos;
    }
  }
  iter = sIter->second.begin();
  for(;;) {
    if ( iter == sIter->second.end() ){
      Matrix *tM = new Matrix;
      (*tM).Init( sIter->first ,MatrixHostDevice );
      sIter->second.push_back( tM );
      return *tM;
    }
    if ( (**iter).state == 2 ){
      (**iter).state = 3;
      return (**iter);
    }else
      iter++;
  }
}
*/

