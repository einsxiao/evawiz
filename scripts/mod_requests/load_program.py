from evawiz_basic import *
def request(handler,*args):
#def request_load_program(self,program_name = '', mode = 'silent'):
    program_name = args_get(0,args)
    mode = args_get(0,args,'silent')

    handler.contact_server('load_program');
    handler.send( program_name )
    ans =  handler.recv()
    handler.send_eof()
    dprint("response: ",ans)

    #writable to $(EVAWIZ_ROOT)/programs
    if os.access( os.getenv("EVAWIZ_ROOT")+"/programs", os.R_OK ):
        if ans == 'program not exist':
            if mode != 'silent':
                print("Program %s is not availiable. Please check avaliable programs at https://eva.run or report your request to service.eva.run."%(program_name,))
                pass
            return None
        #try load
        cmd = os.getenv('EVAWIZ_ROOT') + "/bin/update-eva "+program_name;
        dprint("try to update with cmd:",cmd);
        p = subprocess.Popen(cmd,stdout=subprocess.PIPE,stderr=subprocess.PIPE,bufsize=1,shell=True);
        for line in iter(p.stdout.readline,b''):
            if mode != 'silent':
                print(line)
                handler.send(line);
                pass
            pass
        p.stdout.close();
        p.wait();
        return True
    #not writable 
    handler.contact_server('load_program');
    handler.send( program_name )
    ans =  handler.recv()
    dprint("response: ",ans)
    if ans == 'program not exist':
        handler.send_eof();
        print("Program %s is not availiable. Please check avaliable programs at https://eva.run or report your request to service.eva.run."%(program_name,))
        return None

    #check if evawizd is running
    if os.path.exists( os.getenv('EVAWIZ_ROOT')+"/pids/evawiz_host_server.pid" ):
        print("Evawiz Deamon is running. Loading request submited to Host server. Please wait for some moment to check whether the program is ready.")
        request_file = os.getenv("EVAWIZ_ROOT")+"/programs.request"
        file_content_append( request_file, program_name )
        return
    else:
        print("You have no access to the evawiz directory and the host server is neither running. Please contact your admin to deal with this." )
        return
    pass
