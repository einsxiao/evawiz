#include"evacc.h"
#include "syntax_launch_kernel.h"

int launch_kernel::Init(){
  //check self
  if ( pragma->self->eles.size() != 6 ){
    warnErrorAtPos( pragma->self->start,
                    "a kernel profile and a kernel function arguments and input variable list is expected. No other things allowed.");
    if ( pragma->self->eles.size() > 6 )
      throwCCErrorAtPos( pragma->self->eles[6]->start,"extra expression find here.");
    if ( pragma->self->eles.size() < 6 )
      throwCCErrorAtPos( pragma->self->end-1,"argument list is missed.");
  }
  // check args list format
  if ( pragma->self->eles[4]->type != "<<<>>>" )
    throwCCErrorAtPos( pragma->self->eles[4]->start, "a kernel dimension profile enclosed by '<<<' and  '>>>' is expected here.");
  if ( pragma->self->eles[5]->type != "bracket" )
    throwCCErrorAtPos( pragma->self->eles[4]->start, "arguments list and input variables of form '(type1 arg1:var1, ...)' is expected here.");
  //check context
  // get declaration position and defination position
  if ( pragma->parent->type != "block" ){
    throwCCErrorAtPos( pragma->self->start,"pragma evawiz launch is not properly used within current context.");
  }

  int p_pos = pragma->pos; //NodeIndex insert_pos;
  if ( tree->eles[p_pos]->type != "block" )
    throwCCErrorAtPos( pragma->self->start,"pragma evawiz launch is not properly used within current context.");
  define_insert_pos = tree->eles[p_pos]->end;
  
  for(;;){
    p_pos --;
    if ( p_pos < 0 ) break;
    if ( tree->eles[p_pos]->type == "semicolon" or
         tree->eles[p_pos]->type == "block"     or
         tree->eles[p_pos]->type == "pragma" ) 
      break;
  }
  declare_insert_pos = tree->eles[p_pos+1]->start;
  //get content position
  if ( pragma->level_pos+1 >= pragma->parent->eles.size() or
       pragma->parent->eles[pragma->level_pos+1]->type != "block"){
    throwCCErrorAtPos( pragma->self->end,"pragma evawiz launch should followed by a block enclosed by '{' and '}'.");
  }
  content_start_pos = pragma->parent->eles[pragma->level_pos+1]->start ;
  content_end_pos   = pragma->parent->eles[pragma->level_pos+1]->end ;
  //iter profile
  Node * inode = pragma->self->eles[4];
  iters_start_pos = inode->start.column;
  if ( inode->eles.size() == 0 )
    throwCCErrorAtPos( inode->start, "loop or thread profile is not allowed to be empty.");
  u_int pos = 0;
  while ( pos <  inode->eles.size() ){ // i: Ni
    //i
    if ( inode->eles[pos]->type != "word" )
      throwCCErrorAtPos( inode->eles[pos]->start, "a loop or thread index variable is expected here." );

    iters.push_back( launch_kernel::__iter_rec() );

    iters.back().istart = inode->eles[pos]->start.pos ;
    iters.back().iend   = inode->eles[pos]->end.pos ;

    //:
    pos++;
    if ( pos >= inode->eles.size() )
      throwCCErrorAtPos( inode->eles[pos-1]->end+1, "a ':' is expected here.");
    if ( inode->eles[pos]->type != "colon" )
      throwCCErrorAtPos( inode->eles[pos]->start, "a ':' is expected here.");

    //:Ni[,]
    pos++;
    if ( pos >= inode->eles.size() )
      throwCCErrorAtPos( inode->eles[pos-1]->end+1, "loop or thread dimension is expected here.");
    iters.back().Nstart = inode->eles[pos]->start.pos;
    while ( pos < inode->eles.size() and inode->eles[ pos ]->type != "comma" )
      pos ++;
    iters.back().Nend   = inode->eles[pos-1]->end.pos;
    pos++;
  }
    
  //args list and input list
  inode = pragma->self->eles[5];
  args_start_pos = inode->start.column;
  if ( inode->eles.size() == 0 )
    throwCCErrorAtPos( inode->start, "arguments and input variables list  is not allowed to be empty.");
  pos = 0;
  while ( pos < inode->eles.size() ){// type arg: var,...
    //type arg :
    args.push_back( launch_kernel::__arg_rec() );
    args.back().typestart = inode->eles[pos]->start.pos;
    int t_pos = pos;
    while ( pos < inode->eles.size() and inode->eles[pos]->type != "colon" )
      pos ++;
    if ( pos >= inode->eles.size() or inode->eles[pos]->type != "colon" )
      throwCCErrorAtPos( inode->eles[t_pos]->start, "arguments and variable list of form 'type arg: var' is expected here.");
    if ( pos - t_pos < 2 ) // int(0) a(1):(2) ina(3),
      throwCCErrorAtPos( inode->eles[t_pos]->start, "type or arg of form 'type arg: var' is expected here.");
    if ( inode->eles[pos-1]->type != "word" )
      throwCCErrorAtPos( inode->eles[pos-1]->start, "arg of form 'type arg: var' is expected to be a valid c++ variable name.");
    args.back().typeend  = inode->eles[pos-2]->end.pos;
    args.back().argstart = inode->eles[pos-1]->start.pos;
    args.back().argend   = inode->eles[pos-1]->end.pos;

    pos++; // to pass through the ':'

    //var
    if ( pos == inode->eles.size() )
      throwCCErrorAtPos( inode->eles[pos-1]->end, "an input variable is expected here.");
    args.back().varstart = inode->eles[pos]->start.pos;
    while ( pos < inode->eles.size() and inode->eles[pos]->type != "comma" )
      pos++;
    args.back().varend   = inode->eles[pos-1]->end.pos;
    pos++;
  }
  return 0;
}

string launch_kernel::iter_i(int i){
  return content->substr( iters[i].istart, iters[i].iend -  iters[i].istart );
}
string launch_kernel::iter_N(int i){
  return content->substr( iters[i].Nstart, iters[i].Nend -  iters[i].Nstart );
}
string launch_kernel::iter_Ni(int i){
  return "_N"+iter_i(i);
}
string launch_kernel::arg_type(int i){
  return content->substr( args[i].typestart, args[i].typeend -  args[i].typestart );
}
string launch_kernel::arg_arg(int i){
  return content->substr( args[i].argstart, args[i].argend -  args[i].argstart );
}
string launch_kernel::arg_var(int i){
  return content->substr( args[i].varstart, args[i].varend -  args[i].varstart );
}


string  launch_kernel::declare_cpu(){
  string out = "static            void";
  out += string(pragma->self->eles[4]->start.column - 22,' ');
  out += " cpu_kernel_"+ToString(1000+serial_number);
  //out += string(pragma->self->eles[5]->start.column - pragma->self->eles[4]->start.column - 5,' ');
  out += "(";
  out += string(args[0].typestart -pragma->self->eles[5]->start.pos-1, ' ');
  out += arg_type(0); 
  out += string( args[0].argstart - args[0].typeend ,' ');
  out += arg_arg(0);
  for ( u_int i = 1; i< args.size(); i++ ){
    out += string( args[i].typestart - args[i-1].argend -1 ,' ')+",";
    out += arg_type(i); 
    out += string( args[i].argstart - args[i].typeend ,' ');
    out += arg_arg(i); 
  }
  out += string ( pragma->self->eles[5]->end.pos - args.back().argend -1, ' ');
  for ( u_int i=0; i< iters.size(); i++ ){
    out += ",const int _N"+iter_i(i);
  }
  out += ")";
  return out;
}

string  launch_kernel::declare_gpu(){
  string out = "static __global__ void";
  out += string(pragma->self->eles[4]->start.column - 22,' ');
  out += "cuda_kernel_"+ToString(1000+serial_number);
  //out += string(pragma->self->eles[5]->start.column - pragma->self->eles[4]->start.column - 5,' ');
  out += "(";
  out += string(args[0].typestart -pragma->self->eles[5]->start.pos-1, ' ');
  out += arg_type(0); 
  out += string( args[0].argstart - args[0].typeend ,' ');
  out += arg_arg(0); 
  for ( u_int i = 1; i< args.size(); i++ ){
    out += string( args[i].typestart - args[i-1].argend -1 ,' ')+",";
    out += arg_type(i); 
    out += string( args[i].argstart - args[i].typeend ,' ');
    out += arg_arg(i); 
  }
  out += string ( pragma->self->eles[5]->end.pos - args.back().argend -1, ' ');
  // the Ni,Nj,Nk
  for ( u_int i=0; i< iters.size(); i++ ){
    out += ",const int _N"+iter_i(i); 
  }
  string N_strs[9];
  N_strs[ iters.size() ] = "";
  for ( int i = iters.size()-1; i>=0; i-- ){
    N_strs[i] = iter_i(i)+N_strs[i+1];
    if ( i < (int)iters.size() - 1 ) {
      out += ",const int _N"+N_strs[i];
    }
  }
  out += ")";
  return out;
}

string  launch_kernel::call_cpu(){
  string out;
  out += " cpu_kernel_"+ToString(1000+serial_number);
  out += string(pragma->self->eles[5]->start.column - pragma->self->start.column - 16,' ');
  out += "(";
  out += string(args[0].varstart -pragma->self->eles[5]->start.pos-1, ' ');
  out += arg_var(0); 
  for ( u_int i = 1; i< args.size(); i++ ){
    out += string( args[i].varstart - args[i-1].varend -1 ,' ')+",";
    out += arg_var(i); 
  }
  out += string ( pragma->self->eles[5]->end.pos - args.back().varend -1, ' ');
  for ( u_int i = 0; i< iters.size(); i++ ){
    out += ","+ iter_N(i); 
  }
  out += ");";
  return out;
}

string  launch_kernel::call_gpu(){
  string out;
  out += "cuda_kernel_"+ToString(1000+serial_number);
  out += string(pragma->self->eles[4]->start.column - pragma->self->start.column - 24,' ');
  out += "<<<_cuDim(";
  out += string(iters[0].Nstart - pragma->self->eles[4]->start.pos -3 , ' ');

  out +=  iter_N(0); 
  for ( u_int i = 1; i < iters.size(); i++ ){
    out += string( iters[i].Nstart - iters[i-1].Nend -1, ' ')+"*";
    out += iter_N(i); 
  }
  out += ")>>>";
  //out+="<<< _cuDim(9*10) >>>";
  out += "(";
  out += string(args[0].varstart -pragma->self->eles[5]->start.pos-1, ' ');
  out += arg_var(0); 
  for ( u_int i = 1; i< args.size(); i++ ){
    out += string( args[i].varstart - args[i-1].varend -1 ,' ')+",";
    out += arg_var(i); 
  }
  out += string ( pragma->self->eles[5]->end.pos - args.back().varend -1, ' ');
  for ( u_int i = 0; i< iters.size(); i++ ){
    out += ","+ iter_N(i); 
  }

  string N_strs[9];
  N_strs[ iters.size() ] = "";
  for ( int i = iters.size()-2; i>=0; i-- ){
    out += "," + iter_N( iters.size() -1 );
    for ( int j= iters.size() -2; j >= i; j-- )
      out += "*"+iter_N( j );
  }

  out += "); CUDA_LAST_ERROR(); ";
  return out;
}

string launch_kernel::define_cpu(char part)
{ //part: 's'(second part) 'f'(first part)
  int size = iters.size();
  string out;
  if ( part == 'f' ){
    out = declare_cpu() + " {\n#pragma omp parallel for num_threads( EvaSettings::evawizCpuKernelThreadsNumber )";
    return out;
  }
  out = string(declare_cpu().size(),' ') + "  ";
  for ( int i=0;i<size; i++ ){
    out += "for (int " + iter_i(i) + " = 0; "+ iter_i(i) + " < "+ iter_Ni(i) + "; "+ iter_i(i) + "++ )";
  }
  out += "\n"+(*content).substr( pragma->self->end.pos, content_start_pos.pos - pragma->self->end.pos );
  out += (*content).substr( content_start_pos.pos, content_end_pos.pos - content_start_pos.pos )+"}";
  return out;
}

string launch_kernel::define_gpu(){
  //int size = iters.size();
  string out = declare_gpu() + " {";
  string N_strs[9];
  N_strs[ iters.size() ] = "";
  for ( int i = iters.size()-1; i>=0; i-- ){
    N_strs[i] = iter_i(i)+N_strs[i+1];
  }
  out += "int _tid = blockIdx.x*blockDim.x + threadIdx.x; ";
  string temp_var = iter_i( iters.size()-1 );
  if ( iters.size() > 2 ){
    out += "int "+temp_var+";";
  }
  out += "while ( _tid < _N"+N_strs[0]+" ){";
  string iterI;
  if ( iters.size() == 1 ){
    out += "const int "+iter_i(0) +" = _tid;";
  }else if ( iters.size() == 2 ){
    out += "const int "+iter_i(0) + " = _tid/" + iter_Ni(1) + ";";
    out += "const int "+iter_i(1) + " = _tid - " + iter_i(0) + "*" + iter_Ni(1) + ";";
  }else if ( iters.size() >= 3 ){
    out += "const int "+iter_i(0) + " = ("+temp_var+" = _tid)/_N"+N_strs[1] + ";"; 
    for ( int i= 1; i < (int)iters.size() -1 ; i++ ){
      out += "const int "+iter_i(i) + " = ("+temp_var+" -= "+ iter_i(i-1) +"*_N"+ N_strs[i]  +")/_N"+N_strs[i+1] + ";"; 
    }
    out += iter_i( iters.size() -1 )+" = ("+temp_var+"-"+ iter_i(iters.size()-2)+"*_N"+N_strs[iters.size()-1] +");"; 
  }
  //append content
  out += "\n"+(*content).substr( pragma->self->end.pos, content_start_pos.pos - pragma->self->end.pos );
  out += (*content).substr( content_start_pos.pos, content_end_pos.pos - content_start_pos.pos );
  out += " _tid += blockDim.x* gridDim.x;}}";
  return out;
}
