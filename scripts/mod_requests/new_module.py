from evawiz_basic import *

#def request(self,module_name='',options={}):
def request(handler,*args):
    module_name = args_get(0,args)
    options=args_get(1,args)
    if not options: options = {}
    
    module_type = get_option(options,'type','private')
    lang_type = get_option(options,'lang','c++')
    from_module= get_option(options,'from',None)
    evawiz_root=os.getenv('EVAWIZ_ROOT')
    template_dir=os.getenv('EVAWIZ')+'/Template'
    evawiz_dir = os.getenv('HOME_EVAWIZ')
    module_dir = evawiz_dir +'/'+module_name
    if from_module:
        from_module_dir = evawiz_dir +'/'+from_module
        if not os.path.exists(from_module_dir):
            from_module_dir = evawiz_dir +'/.'+from_module
            if not os.path.exists(from_module_dir):
                print("Source code of %s should be downloaded before act as a template."%from_module)
                return
    else:
        from_module_dir = None
    dprint('template_dir= %s'%template_dir)
    dprint('evawiz_dir= %s'%evawiz_dir)
    dprint('module_dir= %s'%module_dir)

    if not module_name:
        print('module name is required')
        raise Exception('new module requires module name')
    dprint(module_name,module_type,lang_type)
    if not re.match('[a-z][a-zA-Z0-9]{3,120}',module_name):
        print("module name should start with 'a..z' and be consist of 'A-Z,0-9' with length 4 or more")
        raise Exception('module name not valid')

    if not module_type in ('public','private','secret'):
        print('module type should be one of "public", "private" and "secret"')
        raise Exception('new module with a wrong type')

    if not lang_type in ('c++','cuda','eva','fortran','python','matlab'):
        print('module language type should be one of "c++", "cuda","eva", "fortran", "python" and "matlab"')
        raise Exception('new module with a wrong language type')

    #try to new module from local evawiz by nfs
    #cp the Template dir to EVAWIZ DIR
    if ( lang_type != 'c++' ): template_dir+='.'+lang_type
    if not os.path.exists(template_dir):
        print('host evawiz setting is not ready. Please contact your admin.')
        return
    #check evawiz dir
    if not os.path.exists(evawiz_dir): os.makedirs(evawiz_dir)
    if os.path.exists(module_dir):
        print('Module dir %s is already there.'%module_dir)
        return
    ###########
    dprint('local check finished. try contact with server')
    handler.contact_server('mod_new_module')
    handler.send(module_name+' '+module_type+' '+lang_type)
    reply_info = handler.recv()
    if reply_info == 'module is not free':
        print('module with name %s exists, please try another name'%module_name)
        handler.send_eof()
        return
    if not reply_info == 'module is free':
        handler.send_eof()
        raise Exception('something wrong. Please report to service@eva.run')
    if from_module:
        dprint('create from module %s'%from_module)
        #################
        dprint('try cp files');
        os.makedirs(module_dir);
        ev =EvaVersion(from_module_dir,'tmp',module_dir,True);
        ev.local_push_module_to_dir(module_dir);
        shell_status_output('cp -rf %s/* %s/'%(from_module_dir,module_dir) )
        shell_status_output('cp %s/.evaignore %s/'%(from_module_dir,module_dir) )
        shell_status_output('cp %s/.evarunfile %s/'%(from_module_dir,module_dir) )
        dprint('try rename');
        shell_status_output("%s/bin/change_from_to %s %s %s"%
                                 (evawiz_root,module_dir,from_module,module_name) )
        dprint('local operation finished');
    else: 
        dprint('create from module Template')
        os.makedirs(module_dir)
        from_module_dir = template_dir;
        dprint('try cp files from %s to %s'%(from_module_dir,module_dir) );
        shell_status_output('cp -rf %s/* %s/'%(from_module_dir,module_dir) )
        shell_status_output('cp %s/.evaignore %s/'%(from_module_dir,module_dir) )
        shell_status_output('cp %s/.evarunfile %s/'%(from_module_dir,module_dir) )
        if lang_type == 'cu':
            shell_status_output("%s/bin/rename -f 's/.cpp$/.cu/g' %s/*"%(evawiz_root,module_dir))
        dprint('try rename');
        shell_status_output("%s/bin/change_from_to %s %s %s"%
                                 (evawiz_root,module_dir,'Template',module_name) )
        dprint('local operation finished');
    #rename
    #local module is ready
    handler.send('local ready')
    result = handler.recv()
    if result == 'server ready':
        print('Module %s in %s create successfully'%(module_name,module_dir))
        handler.send_eof()
        return True
    else:
        print('Failed to create module %s due to server side failure.'%module_name)
        (status,output)=shell_status_output( "cd %s; %s/bin/trash %s"%(evawiz_root,evawiz_dir,module_name) )
        print('Local files removed')
        handler.send_eof()
        return False

