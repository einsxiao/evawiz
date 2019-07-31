from evawiz_basic import *

def request(handler,*args):
    handler.contact_server('mod_ping')
    handler.send("eva! are you there!")
    res = handler.recv()
    dprint('res = ',res)
    pass
    
