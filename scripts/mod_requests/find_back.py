from evawiz_basic import *
def request(handler):
    handler.contact_server('mod_find_back')
    dprint('request_find_back')
    handler.send('eva! I want to find back')
    reply_info = handler.recv()
    dprint('reply_info=',reply_info)
    if not reply_info == 'allow to find back': # not allowed to register
        handler.send('why')
        ans = handler.recv();
        print("failed to find back:",ans);
        return;
    # email
    sys.stdout.write("your alternative email (provided when you register).\n")
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
        pass
    handler.encrypt_send(email)

    res = handler.recv();
    if ( res != "please check your mailbox" ):
        print("Operation failed for:",res);
        handler.send_eof();
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
    handler.encrypt_send(code)
    ans = handler.recv()
    if ( ans == "verify time out" ):
        print("Operation time out. Please try again.")
        handler.send_eof()
        return
    if ( ans == "verify code wrong" ):
        print("Verification code is wrong. Please try again.")
        handler.send_eof()
        return

    sys.stdout.write("New password >")
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
    handler.encrypt_send(password)
    ans = handler.recv()
    if ans == "password changing done":
        print("Your account password is changed. Operation done successfully.")
    else:
        print("Operation failed: %s"%(ans,))
        pass
    handler.send_eof()
    return

