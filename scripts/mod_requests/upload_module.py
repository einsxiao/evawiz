from evawiz_basic import *
def request(handler,*args):
#def request_upload_module(self,module_name='',options={}):
    module_name = args_get(0,args)
    options = args_get(1,args)

    evawiz_dir = os.getenv("HOME_EVAWIZ")
    evawiz_root= os.getenv('EVAWIZ_ROOT')
    if not module_name: 
        module_name = get_modulename_from_path()
        if not module_name:
            print('a module name is required or operation should be done in the root of the module')
            raise Exception('module name is required to upload a module, operation not in module root')
        module_dir = evawiz_dir +'/'+module_name
    else:
        module_dir = evawiz_dir +'/'+module_name
    branch = get_option(options,'branch','eva')
    upload_type = get_option(options,'type','revision')

    dprint(evawiz_root,evawiz_dir,module_dir)
    if not os.path.exists(module_dir):
        print('module %s is not present in local path %s'%(module_name,module_dir))
        raise Exception('local file not exist')
    if module_name == 'test':
        print('module test is not allowed to upload. Only for local testing.')
        raise Exception('test is not allowed to upload')
    #send file structure and send all files that need to send
    handler.contact_server('mod_upload_module')
    handler.send(module_name)
    reply_info = handler.recv()
    if reply_info == 'not permit to upload':
        print('upload operation is not permited')
        raise Exception('not permited to upload module')
    server_version  = list(map( int,reply_info.split() ))
    ev = EvaVersion(module_dir, branch)
    local_version = ev.get_current_version()
    dprint('local version',local_version)
    if ( list(server_version) > list(local_version) ):
        handler.send('local repos not ready')
        #raise Exception('local repos is too old to do upload operation')
        #adjust local version to server_version + 1
        pass
    #upload local branch first
    print("Pushing module to local repos...")
    ev.local_push_module()
    new_version = ev.get_current_version()
    print('Local repo version of module %s update to '%(module_name),new_version)
    handler.send( '%s %s %s'%tuple(new_version) )
    file_tree = ev.get_file_tree_of_version( new_version )
    dprint(file_tree)
    reply_info = handler.recv()
    if reply_info != 'server ready':
        handler.send_eof()
        raise Exception('server is not ready to upload branch')

    #send files
    dprint('send the branch')
    if ev.send_module(ev.branch_dir,file_tree,'eva2normal',handler.send,handler.recv,handler.send_eof):
        print('module %s is updated and is uploaded to server as well'%(module_name))
    else:
        print('upload failed')
        handler.send_eof()
    handler.send("upload success")
    ver_str = handler.recv()
    handler.send_eof()

