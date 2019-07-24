
class launch_kernel{
public:
  struct __iter_rec{
    int          istart, iend;
    int          Nstart, Nend;
  };
  struct __arg_rec{
    int          typestart, typeend;
    int          argstart,  argend;
    int          varstart,  varend;
  };
  Node                   *tree;
  Syntax::__pragma_rec   *pragma;
  int                     serial_number;
  string                  filePath;
  string                 *content; //the whole content
  ////////////////////////////////////////////
  NodeIndex               declare_insert_pos;
  NodeIndex               define_insert_pos;
  u_int                   iters_start_pos;
  u_int                   args_start_pos;
  // the content of its body block
  NodeIndex               content_start_pos;
  NodeIndex               content_end_pos;

  vector< __arg_rec >     args;
  vector< __iter_rec >    iters;
  string                  iter_i(int i);
  string                  iter_N(int i);
  string                  iter_Ni(int i);
  string                  arg_type(int i);
  string                  arg_arg(int i);
  string                  arg_var(int i);

  int Init();

  string  declare_cpu();
  string  declare_gpu();

  string  call_cpu();
  string  call_gpu();

  string define_cpu(char ch); //ch 'f':fisrt part of definition, 's':second part
  string define_gpu();
  
};





  
