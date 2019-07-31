from evawiz_basic import *
def request(handler,*args):
#def request_perm_module(self,module_name='',options={}):
    module_name = args_get(0,args)
    options = args_get(1,args)
    if not module_name: return None

    # version 
    to_user = get_option(options,'user','#none')
    to_group = get_option(options,'group','#none')
    perm_str = get_option(options,'perm','r')
    print('perm_str = ',perm_str)
    version = get_option(options,'version','0')
    #check perm and version form
    if not check_version_string(version):
        print("Version string is not in the right form.")
        #handler.send_eof();
        return 
    ###check perm_str
    for ch in perm_str:
        if not ch in ('i','r','s','d'):
            print("Perm string is not in the right form.")
            #handler.send_eof();
            return 
    ###check to_user and to_group
    if ( to_user == "#none" and to_group == "#none" ):
        print("at least a username or a groupname is required. Specify '-u all' to all users.")
        #handler.send_eof();
        return 
    ###############################################################
    handler.contact_server('mod_perm_module')
    ###############################################################
    ####check_module part
    handler.send(module_name)
    reply_info = handler.recv()
    module_status = unpackdict(reply_info)
    #user type host-user 1 #
    #case 0 public download to evawiz modules
    if module_status['type'] == 'notexist':
        print('module %s does not exist. Please check.'%module_name)
        handler.send_eof();
        return
    if module_status['owner'] != '#self':
        print("no permission to permit module %s"%module_name)
        handler.send_eof();
        return
    ###############################################################
    ###################
    handler.send("%s %s %s %s"%(to_user,to_group,perm_str,version))
    ###
    reply_info = handler.recv()
    if ( reply_info == 'perm ok'):
        print("Perm operation to module '%s' have done successfully."%module_name)
        handler.send_eof();
        return ;
    if ( reply_info == 'perm string false' ):
        print("Perm operation to module '%s' failed for wrong perm string."%module_name)
        handler.send_eof();
        return ;
    if ( reply_info == 'version not ok'):
        print("Perm operation to module '%s' failed for version specification is not in the right form."%module_name)
        handler.send_eof();
        return ;
    if ( reply_info == 'perm to self'):
        print("No need to perm to owner.")
        handler.send_eof();
        return ;
    if ( reply_info == 'user not exist'):
        print("Try to permit to non-exist user '%s'."%to_user)
        handler.send_eof();
        return ;
    if ( reply_info == 'group not exist'):
        print("Try to permit to non-exist group '%s'."%to_group)
        handler.send_eof();
        return ;
    if ( reply_info == 'no user or group exist'):
        print("user or group specified is not exist.")
        handler.send_eof();
        return ;
    print("perm operation to module '%s' failed."%module_name)
    handler.send_eof();
    return 

