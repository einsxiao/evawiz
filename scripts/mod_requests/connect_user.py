from evawiz_basic import *
def request(handler,*args):
#def request_connect_user(self):
    handler.contact_server('mod_connect_user')
    dprint('step into request_connect_user')
    handler.send('eva! I want to connect')
    reply_info = handler.recv()
    dprint('reply_info = ',reply_info)
    if reply_info != 'allow to connect':
        print('You are not permited to connect to evawiz server. Please contact the admin of your host')
        handler.send_eof()

    #get evawiz_account and its password to connect
    #username
    sys.stdout.write("evawiz account username >")
    sys.stdout.flush()
    while True:
        user_name = input()
        user_name.strip()
        dprint('user_name = ',user_name)
        if re.match('^[A-Za-z][A-Za-z0-9_-]{3,63}$',user_name) :
            break
        sys.stdout.write("username should begin with a letter and followed with letters and numbers(4 bits at least).\n")
        sys.stdout.write("username formart invalid, retype >")
        sys.stdout.flush()

    #password
    sys.stdout.write("password >")
    sys.stdout.flush()
    while True:
        password = getpass.getpass("")
        password.strip()
        if re.match('^.{8,64}$',password) and not re.match('^[0-9]+$',password) and not re.match('^[a-zA-Z]+$',password):
            break
        else:
            os.system('sleep 5')
            sys.stdout.write("password not correct, retype >")
            sys.stdout.flush()
    #send username and password
    handler.encrypt_send("%s %s"%(user_name,password))
    ans = handler.recv()
    if ans != 'failed to connect':
        #save auth information 
        auth_info = ans
        handler.write_auth_info(auth_info)
        print('succeed to connect evawiz server.')
        print('you can now access modules of account "'+user_name+'"')
        print('you can also create and upload modules of account "'+user_name+'"')
    else:
        handler.send("why");
        res = handler.recv();
        #failed to connect
        print('connect failed: '+res)

    handler.send_eof()

