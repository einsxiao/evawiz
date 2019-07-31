from evawiz_basic import *
def request(handler,*args):
#def request_list_module(self,options={}):
    options = args_get(0,args)

    user = get_option(options,'user','#none');
    group = get_option(options,'group','#none');
    dprint("user = %s, group = %s"%(user,group));
    handler.contact_server('mod_list_module')
    handler.send(user+' '+group)
    if ( user == '#none' ): user = 'yourself'

    reply_info = handler.recv();
    if reply_info == 'no user exist':
        print("No user %s exist."%(user,))
        return
    if reply_info == 'no modules':
        print("No modules is created by %s."%(user,))
        return
    if ( user == "#none" ):
        print("Module(s) owned:")
    else:
        print("Module(s) owned by %s:"%user)
    dprint(reply_info)
    modlist = unpackdict(reply_info);
    dprint("unpack dict :",modlist)
    for (mod,version) in modlist.items():
        print("\t  %s : %s"%(mod,version))
    handler.send_eof()
    return

