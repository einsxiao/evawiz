from evawiz_basic import *

def request(handler,*args):
    #check ~/evawiz/.user.auth and verify its content
    dprint('step into request_check_user_info')
    if not os.getenv('HOME_EVAWIZ'):
        raise Exception("Evawiz environment does not set properly. Please source $EVAIZ_ROOT/evawiz.source or 'evawiz.source' file where the Evawiz installed.")
    #check local user state 
    info = handler.get_auth_info()
    if not info:
        return 'not connected to server'

    #check info
    if len(info) != 96:
        return 'not connected to server'

    #####################################    
    handler.contact_server('mod_check_user_info')
    handler.send(info)
    check_result = handler.recv()
    dprint("check_result = ",check_result)
    handler.send_eof()
    con_user_name = str_after(check_result,'connected to server user ')
    if con_user_name:
        return "connected to server"
    print()
    if check_result == 'user connect info not right':
        return """connection info outdated"""

    return "not connected to server"

