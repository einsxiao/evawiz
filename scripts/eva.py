#!/usr/bin/env python3
#coding:utf-8
################################################
## author: Xiao, Hu
## version: 6.23.2019
################################################
import sys
import os
evawiz_root = os.getenv("EVAWIZ_ROOT")
sys.path.append(evawiz_root+"/pymods")
from evawiz_basic import *
import muser
 
evawiz_server=os.getenv("EVAWIZ_SERVER");
evawiz_server_port=os.getenv("EVAWIZ_SERVER_PORT");
evawiz_proxy=os.getenv("EVAWIZ_PROXY");
if not evawiz_server: evawiz_server = 'ovo.ltd'
if not evawiz_server_port: evawiz_server_port = '5555'
#evawiz_server = "localhost"

## no old kind of bridge host working
# hostport = file_content_get( os.getenv("EVAWIZ_ROOT")+"/port.info")
# if ( hostport == "" ):
#   print('warning: evawizd is not running.')
#   hostport=5556
#   pass
  
#dprint( evawiz_server, evawiz_server_port)
user = muser.UserRequest(evawiz_server,int(evawiz_server_port) ,evawiz_proxy)

def help_info():
  print("""Usage: eva [operation] [operants]
  Valid operations: 
    edit|-e [file_name]
          start emacs to edit file with name file_name

    run|-r [script]
  	  run script in evawiz kernel

    new|create [options] [module_name] 
          create module with a unique name and register into Evawiz system. 
         options:
             -t|type [private|public|secret]
                  default value: private
             -l|lan [c++|cuda|fortran|matlab|python|eva]
                  default value: c++
             -f|from [module_name]
                  created new module based on existed module [module_name]

    delete [module_name] 
          try to delete a module 
         options:
             -local
                  only remove localfiles 

    push|upload [module_name]
          upload module, push new code to server side

    get|pull|download [module_name] 
          try to download a module, pull code from server side to local 

    load [program_name]
          try to request local host to load a program pre-compiled by EVAWIZ
            the program will be located in $EVAWIZ_ROOT/programs/

    perm [options] [module_name]
          try to give permision to otherrs(user,group)
          options:
             -u [user]
                   give permission to a user
                   if user takes the value of 'All' or your own evawiz account name,
                        the permission will valid for all users.
             -g [group_name]
                   give permission to a group
             -p [perm_string]
                   specify the permission content by a string
                   default value: r
          perm_string is consisted of 'r', 's', 'd', in which
               r: permission to run the module
               s: permission to view and use source code, plus r
               d: permission to push self-modified code to the author, plus r and s

    check |[options1] |[module_name] |[options2]
          check the module status, or user status, or local account status
          options1:
             -u [user]
                   check info. of user, ignoring following arguments.
             -g [group]
                   check info of group, ignoring following arguments.
          options2:
             -v
                   show hitory version list, using together with 'check module'

    register 
          register an account in Evawiz system. 

    connect 
          setup a trusted connection to an evawiz account. 

    findback|changepassword
          request to reset your account password via your registered email.

    clear
          clear connect evawiz information.

    -nw
          open with no windows mode. You can also use evanw to start the no windows mode.

    -h|-?|--help|help
          show this help. 
  """)

try:
  #according content of sys.argv decide what to do

  #no argv run something
  script_path = os.path.split(os.path.realpath(__file__))[0]
  emacs_path = script_path+'/../emacs/bin/emacs'
  if not os.path.exists( emacs_path ):
    emacs_path = script_path+'/../bin/emacs'
    pass
  if not os.path.exists( emacs_path ):
    emacs_path = 'emacs'
    pass
  emacs_path = 'emacs'
  emacs_path +=" -l $EVAWIZ_ROOT/etc/emacs/site-lisp/default.el "
  sysargv,options=argv2argsoptions(sys.argv,['version','help','edit','run','nowindow','local'],{'?':'help','h':'help','v':'version','r':'run','e':'edit','u':'user','g':'group','p':'perm','t':'type','b':'branch','nw':'nowindow'})
  #no argument
  if (len(sys.argv) == 1):
    if sys.stdin.isatty(): #Console
      os.system(emacs_path+' & 1>/dev/null 2>&1 ')
    else: #Redirected
      for istr in os.popen(script_path+'/../evawiz/bin/evawiz'): print(istr,)
    exit(0)

  if len(sysargv)>1:
    operation = sysargv[1]
  else:
    operation = 'NOT_DEFINED'
  #edit
  no_window_opt_str = '  '
  if options.get('nowindow'):
    no_window_opt_str = ' -nw ';

  if operation == 'edit' or options.get('edit'):
    if len(sysargv)<3:
      print('Error: a filename is required')
      exit(1)
    files = ''
    for i in range(2,len(sysargv)):
      files+=" "+sysargv[i]
    os.system(emacs_path+' '+no_window_opt_str+files+' & 1>/dev/null 2>&1')
    exit(0)

  #help
  if operation == 'help' or options.get('help'):
    help_info()
    exit(0)

  #checkmod check module state
  if operation == 'ping':
    user.mod_request('ping')
    exit(0)
    pass
  if operation == 'check':
    if len(sysargv)<3:
      if ( get_option(options,'user') ):
        #user.request_list_module(options);
        user.mod_request("list_module",options)
        exit(0)
      print("check evawiz settings")
      #if user.request_check_user_info():
      if user.mod_request("check_user_info"):
        user.mod_request("list_module",options)
        #user.request_list_module();
        exit(0)
      exit(1)
    module_name = sysargv[2]
    print("check module '%s'... "%module_name)
    if get_option(options,'version'):
      #user.request_check_module(module_name,True);
      user.mod_request('check_module',module_name,True)
    else:
      #user.request_check_module(module_name);
      user.mod_request('check_module',module_name )
      pass
    exit(0)

  #new module
  if operation == 'new' or operation == 'create':
    if len(sysargv)<3:
      print('Arguement error: a module name is required.')
      exit(1)
    module_name = sysargv[2]
    options_transform(options,{'t':'type','n':'name','l':'lang','f':'from'})
    print('try to register module %s'%module_name)
    #user.request_new_module(module_name,options)
    user.mod_request('new_module',module_name,options)
    exit(0)
    
  #delete module
  if operation == 'delete':
    if len(sysargv) < 3:
      print('Argument error. A module name is required.')
      exit(1)
    module_name = sysargv[2]
    #user.request_delete_module(module_name)
    user.mod_request('delete_module',module_name)
    exit(0)
    
 #upload module
  if operation == 'upload' or operation == 'push':
    if len(sysargv) < 3:
      module_name = ''
    else:
      module_name = sysargv[2]
    ####
    options_transform(options,{'b':'branch','t':'type','v':'version'})
    #user.request_upload_module(module_name,options)
    user.mod_request("upload_module",module_name,options)
    exit(0)

  #download module
  if operation == 'download' or operation == 'pull' or operation == 'get':
    sysargv,options=argv2argsoptions(sys.argv,[],{'v':'version','b':'branch'})
    if len(sysargv) < 3:
      module_name = get_modulename_from_path()
      if not module_name:
        print('Argument Error:A module name is required. Otherwize, operation need to be done at the root of the module.')
    else:
      module_name = sysargv[2]
      pass
    #########
    dprint('module_name = %s'%module_name)
    options_transform(options,{'b':'branch','v':'version'})
    #user.request_download_module_to_user(module_name,options)
    user.mod_request("download_module",module_name,options)
    exit(0)

  #load program
  if operation == 'load' :
    if len(sysargv) < 3:
      print('Argument Error:A program name is required.')
      pass
    program_name = sysargv[2]
    #user.request_load_program(program_name,'normal');
    user.mod_request("load_program",program_name,'normal');
    exit(0);

  #perm_module
  if operation == "perm":
    if len(sysargv) < 3:
      module_name = get_modulename_from_path()
      if not module_name:
        print('Argument Error:A module name is required. Otherwize, operation need to be done at the root of the module.')
    else:
      module_name = sysargv[2]
    ############
    dprint('module_name = %s'%module_name)
    options_transform(options,{'g':'group','u':'user','p':'perm'})
    #user.request_perm_module(module_name,options)
    user.mod_request("perm_module",module_name,options)
    exit(0)

  #say_hello test
  if operation == 'say_hello':
    print('say hello test')
    user.contact_host('say_hello','noncrypt')
    user.request_say_hello()
    exit(0)

  if operation == 'register':
    print('checking your account infomation')
    #local connection info
    #check_result = user.request_check_user_info_only()
    check_result = user.mod_request("check_user_info_only")
    if check_result == 'connected to server':
      print('You are already connected to server. No need to register a new evawiz account.')
      exit(0)
    print("checking finished. Permitted to register.")
    if check_result == "connection info outdated": # delete connection info
        evawiz_dir= os.getenv('HOME')+'/.evawiz/'
        auth_file = evawiz_dir + '/.user.auth'
        os.system('rm -f '+auth_file)
    ###
    #user.request_register_user()
    user.mod_request('register_user')
    exit(0)

  if operation == 'connect':
    #user.request_connect_user()
    user.mod_request("connect_user")
    exit(0)

  if operation == 'disconnect':
    #user.request_connect_user()
    user.mod_request("disconnect_user")
    exit(0)

  if operation == 'run' or options.get('run'): 
    if len(sysargv)<3:
      print('Error: a script is required.')
      exit(1)
    os.system(script_path+'/../evawiz/bin/evawiz <'+sysargv[2])
    exit(0)

  if operation == 'clear':
    evawiz_dir = os.getenv('HOME')+'/.evawiz'
    if not os.path.exists(evawiz_dir): os.system("mkdir "+evawiz_dir)
    auth_file = evawiz_dir+'/.user.auth'
    dprint("auth_file = ",auth_file);
    if os.path.exists(auth_file):
      os.system('chmod 600 '+auth_file)
      os.system('rm '+auth_file)
      print("clear local auth information.")
      pass
    exit(0);
    pass

  if operation == 'findback' or operation == 'changepassword':
    #user.request_find_back()
    user.mod_request("find_back")
    exit(0);
    pass
   
  #show local version
  if operation == 'version' or get_option(options,'version'):
    print("""evawiz version: 6.23.2019
    All rights reserved by EVAWIZ.
    """)
    exit(0)

  #unkown option or operator
  if operation[0] == '-':
    print("Error: unknown option \'"+operation+"\'")
    exit(1)
  #treated as edit, operation as script file
  files = ''
  for i in range(1,len(sysargv)):
    files+=""" \""""+sysargv[i]+"""\""""
  os.system(emacs_path+no_window_opt_str+files+" & 1>/dev/null 2>&1")
except KeyboardInterrupt as e:
  print("\nprogram killed by keyboard interrupt")
  dtraceback()
except EOFError as e:
  print("\nprogram killed for EOF Error")
  dtraceback()
except Exception as e:
  print("program exist with error:",e)
  dtraceback()

