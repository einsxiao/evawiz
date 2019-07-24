/////////////////////////////
#include"eva_headers.h"
//#include"AlgebraicModule.h"
using namespace std;
using namespace evawiz;

Kernel::Kernel(istream *istr_in,bool interactively,int argc,char*argv[]){
  superList.SetList( SYMBOL_ID_OF_List );
  inputList.SetList( SYMBOL_ID_OF_List );
  importList = new ImportList(istr_in,interactively);
#ifdef DEBUG
  evaluation = new Evaluation(true);
#else
  evaluation = new Evaluation();
#endif
  evaluation->superList = this->superList;
  evaluation->inputList = this->inputList;
  this->interactively = interactively;
  evaluation->InteractivelyMode = interactively;
  evaluation->argc = argc;
  evaluation->argv = argv;
}

Kernel::~Kernel(){
#ifdef DEBUG
  //cout<<"Totoal Object applied: "<<GlobalPool.Objects.idx<<endl;
  if ( GlobalPool.OpRec.idx != Index0 ){ 
    cout<<"Time consuming of Functions in seconds:"<<endl;
    Index id = Index0;
    while ( id < GlobalPool.OpRec.idx ){
      id.first++;
      cout<< GlobalPool.OpRec.Get(id).second<<":\t"<<GlobalPool.OpRec.Get(id).first<<endl;
    }
  }
#endif
  delete importList;
  delete evaluation;
}

int Kernel::Phrasing( ){
  u_int printQuan=0;
  bool printed = false;
  while ( !importList->End() ){ // one expression per loop
    try{
      if ( interactively && not printed ){
        cout<<"In["<<printQuan+1<<"]:="; // print the head
        printed = true;
      }
      if ( importList->GetList( superList , 0) < 0 ){
        if ( interactively ) cout<<"\n";
        continue; //get Argv
      }
      if ( superList.Size() > printQuan ){
        printQuan++; printed = false;
        inputList.PushBack(superList.Last());
#ifdef DEBUG
        cout<<endl<<">>>  "<< superList.Last()<<"  <<<"<<endl;
#endif
        evaluation->Evaluate(superList.Last());
        static EvaRecord *rec= NULL;
        if ( rec == NULL ){
          rec = evaluation->GetEvaRecord( "SimpleSimplify" );
          if ( rec == NULL )
            ThrowError("Kernel","SimpleSimplify from 'Algebraic' is not loaded properly.");
        }
        ((rec->module)->*(rec->memberFunction))(superList.Last());
        //dout<<"   evaluate finished, printing"<<endl;
        if ( superList.Last().ListQ( SYMBOL_ID_OF_FullForm ) ){
          if ( superList.Last().Size()!=1 ){
            ThrowError("FullForm",(string)"FullForm called with "+ToString(superList.Last().Size())+" arguments; 1 argument is expected.");
          }
          if ( interactively )
            cout<<"Out["<<printQuan<<"]//FullForm= ";
          string strout =superList.Last()[1].ToFullFormString();
          if ( strout != "" ) cout<<strout<<"\n";
        }else{
          string strout =superList.Last().ToString();
          if ( superList.Last().NullQ() ) strout = "";
          if ( interactively && strout!="")
            cout<<"Out["<<printQuan<<"]= ";
          if ( strout != "" ) cout<<strout<<"\n";
        }
      }
      if ( interactively ) cout<<"\n";
    }catch ( const ExceptionQuit&err ){
      return err.code;
    }catch ( const Error&err){
      cerr<<err.what()<<endl;
      evaluation->EvaluateRecursionDepth = -1;
      evaluation->ClearValueTablesTo(1);
    }catch ( const exception &err){
      cerr<<"Sourcecode::Error: "<<err.what()<<endl;
      evaluation->EvaluateRecursionDepth = -1;
      evaluation->ClearValueTablesTo(1);
    }catch ( ... ){
      cerr<<"System::Error: Unexpected error occured."<<endl;
      evaluation->EvaluateRecursionDepth = -1;
      evaluation->ClearValueTablesTo(1);
    }
  }
  return 0;
}



