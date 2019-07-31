from evawiz_basic import *
def request(handler,*args):
#def request_download_module_to_user(self,module_name='',options={}):
    module_name = args_get(0,args)
    options = args_get(1,args)

    #step 1 check module information
    if not module_name: return None
    handler.contact_server('mod_download_module')
    handler.send(module_name)
    reply_info = handler.recv()
    module_status = unpackdict(reply_info)
    #user type host-user 1 #
    #case 0 public download to evawiz modules
    if module_status['type'] == 'notexist':
        print('module %s does not exist. Please check your input'%module_name)
        handler.send_eof();
        return
    module_dir = options.get('dir') 
    if dictQ(module_status,'owner','#self') :
        #case 1 src perm download to evawiz root
        if not module_dir: module_dir = os.path.join(os.getenv('HOME_EVAWIZ'),module_name)
        version = options.get('version')
        dprint("version:%s"%version)
        downtype = options.get('type')
        if downtype == "update":
            version_str = file_content_get(module_dir+"/.__version");
            if version_str:
                handler.send("update %s"%(version_str,))
            else:
                handler.send("newest");
        elif not version:
            version_str = file_content_get(module_dir+"/.__version");
            dprint(version_str)
            if version_str:
                handler.send("download %s"%(version_str,))
            else:
                handler.send("newest");
        else:
            if check_version_string(version):
                handler.send(version)
            else:
                handler.send('do nothing')
                raise Exception('Error: version string is not int the right form.')
        #############
        reply_info = handler.recv()
        dprint('reply_info:',reply_info)
        if reply_info == "module is newest already":
            handler.send_eof()
            print("update successfully.")
            return
        if not str_start_with(reply_info,'ready_to_download'):
            handler.send_eof()
            raise Exception('Download Error: %s'%reply_info)
        version_str = reply_info.split()[1]
        dprint('version_str:',reply_info)
        if not os.path.exists(module_dir):
            os.mkdir(module_dir);
        ev = EvaVersion(module_dir)
        ####start to download cause
        print('downloading module of version: %s'%version_str);
        handler.send('start to download')
        ev.recv_module(module_dir,'unchange','','',handler.send,handler.recv)
        file_content_set(module_dir+"/.__version",version_str)
        handler.recv()
        print('Download successfully.')
    elif dictQ(module_status,'perm_run','enable') or dictQ(module_status,'perm_src', "enable"):
        #case 2 perm_run download to root but invisable
        #if not module_dir: module_dir = os.path.join(os.getenv('HOME_EVAWIZ'),"."+module_name)
        if not module_dir: module_dir = os.path.join(os.getenv('HOME_EVAWIZ'),module_name)
        version = options.get('version')
        dprint("version:%s"%version)
        downtype = options.get('type')
        if downtype == "update":
            version_str = file_content_get(module_dir+"/.__version");
            if version_str:
                handler.send("update %s"%(version_str))
            else:
                handler.send("newest");
        elif not version:
            handler.send('newest')
        else:
            if check_version_string(version):
                handler.send(version)
            else:
                handler.send('do nothing')
                raise Exception('Error: version string is not int the right form.')
        #############
        reply_info = handler.recv()
        dprint('reply_info:',reply_info)
        if reply_info == "module is newest already":
            handler.send_eof()
            print("update successfully.")
            return
        if not str_start_with(reply_info,'ready_to_download'):
            handler.send_eof()
            raise Exception('Download Error: %s'%reply_info)
        version_str = reply_info.split()[1]
        dprint('version_str:',reply_info)
        if not os.path.exists(module_dir):
            os.mkdir(module_dir);
        else:
            os.system("chmod +w %s"%module_dir);
            pass
        ev = EvaVersion(module_dir)
        ####start to download cause
        print('downloading module of version: %s'%version_str);
        handler.send('start to download')
        ev.recv_module(module_dir,'unchange','','',handler.send,handler.recv)
        file_content_set(module_dir+"/.__version",version_str)
        handler.recv()
        #####change the previliges
        #os.system("chmod -w %s"%module_dir);
        print('Download successfully')
    else:
        print("You are not permited to download module %s"%module_name)

    #########################
    handler.send_eof()
    return

