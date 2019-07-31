from evawiz_basic import *
def request(handler,*args):
    handler.contact_server('mod_register_user')
    dprint('request_register_user')
    handler.send('eva! I want to register')
    reply_info = handler.recv()
    dprint('reply_info=',reply_info)
    if not reply_info == 'allow to register': # not allowed to register
        if reply_info == 'already registered':
            print('You have already registered yet. If you want to register again, please kill that account first.')
            handler.send_eof()
            return
        print('You are not permitted to register.')
        handler.send_eof()
        return

    print("Please complete following infomation to complete the register process")
    # user_name
    while True:
        sys.stdout.write("user name >")
        sys.stdout.flush()
        while True:
            user_name = input()
            user_name.strip()
            dprint('user_name = ',user_name)
            if re.match('^[a-z][a-z0-9_-]{3,63}$',user_name) :
                break
            sys.stdout.write("user name should begin with a letter and followed with letters and numbers(4 bits at least).\n")
            sys.stdout.write("user name form invalid>")
            sys.stdout.flush()
        handler.send('%s'%user_name)
        ans = handler.recv()
        if ans == 'user name valid': break
        print(ans) # warn error

    #password
    sys.stdout.write("password >")
    sys.stdout.flush()
    while True:
        password = getpass.getpass("")
        password.strip()
        if re.match('^.{8,64}$',password) and not re.match('^[0-9]+$',password) and not re.match('^[a-zA-Z]+$',password):
            sys.stdout.write("repeat password >")
            sys.stdout.flush()
            password1 = getpass.getpass("")
            password1.strip()
            if ( password == password1 ) :
                break
            sys.stdout.write("password not the same, retype >")
            sys.stdout.flush()
        else:
            sys.stdout.write("password should be at least 8 bits with letters, numbers and symbols mixed\n retype >")
            sys.stdout.flush()
    handler.encrypt_send( password )
    ans = handler.recv()
    if ans != 'password ok': raise Exception('server error. Please try again.')

    # email
    sys.stdout.write("your alternative email(important for finding back password or even username).\n")
    sys.stdout.write("email >")
    sys.stdout.flush()
    while True:
        email = input()
        email.strip()
        if ( len(email) > 128 ):
            sys.stdout.write("email address too long.\n")
        elif ( re.match('^[a-zA-Z0-9_\.\-]+@[a-zA-Z0-9\-]+\.[a-zA-Z0-9\-\.]+$',email) ):
            break
        sys.stdout.write("email not valid, retype >")
        sys.stdout.flush()
    handler.send('%s' % email)
    ans = handler.recv()
    #if email error
    if ans == 'email taken':
        print('email is taken by another account as alternative account. Please Try again.')
        handler.send_eof()
        return
    ## check email address
    state = handler.recv()
    if ( state != 'check mail' ):
        print(state)
        return
        
    print("An verification code has been sent to your mailbox. Please check your mailbox to continue.")
    sys.stdout.write("Verification code >")
    sys.stdout.flush()
    code = ""
    while True:
        code = input()
        dprint("'%s'"%(code,))
        code.strip()
        if ( len(code) == 6 ):
            break
        sys.stdout.write("Verification code is in the wrong form.\n")
        sys.stdout.write("Verification code >")
        sys.stdout.flush()
        pass
    handler.send(code)
    ans = handler.recv()
    if ( ans == "verify time out" ):
        print("Operation time out. Please try again.")
        handler.send_eof()
        return
    if ( ans == "verify code wrong" ):
        print("Verification code is wrong. Please try again.")
        handler.send_eof()
        return

    if not ans == 'failed to register':
        #save auth information 
        auth_info = ans
        handler.write_auth_info(auth_info)
        print("EVAWIZ acount '"+user_name+"' with password '%s' registered successfully."%password)
        print('You can log on https://eva.run with this account.')
        print('Email account '+user_name+'@eva.run is created for you.')

    else:
        handler.send("why");
        ans = handler.recv();
        print('register user operation failed: ',ans)

    handler.send_eof()

    pass
