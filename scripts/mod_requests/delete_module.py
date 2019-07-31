from evawiz_basic import *
def request(handler,*args):
#def request_delete_module(self,module_name = '',options={}):
    module_name=args_get(0,args)
    options=args_get(1,args)
    if not options: options = {}
    if not module_name:
        print('A module name is required.')
        raise Exception('a module name is required')

    if not re.match('[a-z][a-zA-Z0-9]{3,120}',module_name):
        print('module name is invalid. Please try again')
        raise Exception('module name invalid')

    #if not only local, remove server info
    if ( not options.get('local') ):
        handler.contact_server('mod_delete_module')
        handler.send(module_name)
        reply_info = handler.recv()
        if not reply_info == 'module deleted':
            print('failed to delete module %s on server.'%(module_name))
        else:
            print('module %s is removed from evawiz system successfully'%module_name)
            pass
        pass
    #server info processed, remove local files
    evawiz_root= os.getenv('EVAWIZ_ROOT')
    evawiz_dir = os.getenv('HOME_EVAWIZ')
    module_dir = evawiz_dir +'/'+module_name
    (status,output) = shell_status_output("cd %s; chmod +w %s; %s/bin/trash %s"%(evawiz_dir,module_name,evawiz_root,module_name) )
    dprint(status,output)
    handler.send_eof()

