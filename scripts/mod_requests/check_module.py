from evawiz_basic import *
def request(handler,*args):
#def request_check_module(self,module_name=None,show_version_list=False):
    module_name = args_get(0,args)
    show_version_list = args_get(1,args)
    dprint("args get = ",module_name,show_version_list)
    if not module_name:
        return None
    handler.contact_server('mod_check_module')
    dprint("try check module ",module_name)
    handler.send(module_name)
    ans = handler.recv()
    status = unpackdict(ans)
    if status['type'] in ('error'):
        print("Module error.")
        return
    if status['type'] == 'notexist':
        print("Module %s is not created."%module_name)
        print("""  You can use
        eva new %s
to create this module and register it to evawiz system."""%module_name)
        return
    print("Module infos:")
    if dictQ(status,'owner',"#self"):
        print("   module is owned by yourhandler.")
        print("   current version: %s.%s.%s"%(status['major_version'],
                                              status['minor_version'],
                                              status['revision_version']) )
        print("   permission to public:")
        print("     run permission: %s"%(status['perm_run']))
        print("     source code access permission: %s"%(status['perm_src']))
        print("     co-developing permission: %s"%(status['perm_deve']))
    else:
        if status['owner'] == '#system': status['owner'] = 'System'
        print("   owner: %s"%status['owner'])
        print("   current version: %s.%s.%s"%(status['major_version'],
                                      status['minor_version'],
                                      status['revision_version']))
        print("   run permission: %s"%(status['perm_run']))
        print("   source code access permission: %s"%(status['perm_src']))
        print("   co-developing permission: %s"%(status['perm_deve']))
        pass
    if show_version_list:
        print("   history version(s):\n      %s"%(status['history_version_list'].replace(',','\n      '), ))
        pass
    handler.send_eof()
    return

