from evawiz_basic import *
def t(a,*args):
    dprint( args_get(0,args))
    dprint( args_get(1,args))
    dprint( args_get(2,args))
    dprint( args_get(3,args))
    pass
t(1,3)
t(1,3,4,5)
t(1,3)
t(1,"bcdef",True)
