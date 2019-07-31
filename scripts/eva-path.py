#!/usr/bin/env python3
from evawiz_basic import *
if ( len(sys.argv)<2 ):
    exit(0);

pre = " -Wl,"
if ( sys.argv[1] == "tcu" or sys.argv[1] == 'evacc' ):
    pre = " -Xlinker "
    pass
inc= "";

def add_include(path,absolute=False):
    if ( not os.path.exists( path ) ): return
    global inc
    inc += " -I%s "%(path,)
    pass

def add_lib(path,absolute=False):
    if ( not os.path.exists( path ) ): return
    global inc
    inc += " -L%s "%(path,);
    if absolute:
        inc += " %s-rpath=%s "%(pre,path)
    else:
        inc += " %s-rpath=$ORIGIN/%s "%(pre,path)
        pass
    pass

def add_path(path,absolute=False):
    if ( not os.path.exists( path ) ): return
    global inc
    inc += " -I%s "%(path,)
    inc += " -L%s "%(path,);
    if absolute:
        inc += " %s-rpath=%s "%(pre,path)
    else:
        inc += " %s-rpath=$ORIGIN/%s "%(pre,path)
        pass
    pass
        
home_evawiz = os.getenv("HOME_EVAWIZ")
if not home_evawiz: home_evawiz = os.getenv("HOME")+"/evawiz"
evawiz_root = os.getenv("EVAWIZ_ROOT")
if not evawiz_root: evawiz_root = '%s'%(evawiz_root,)
evawiz = evawiz_root+'/evawiz'

#current directory
add_lib('./lib');
add_include('./include');
add_lib('./');
add_include('./');

#EVAWIZ_INCLUDE_PATH
evawiz_include_path = os.getenv("EVAWIZ_INCLUDE_PATH");
for item in evawiz_include_path.split(':'):
    add_include(item, True);
    pass

#EVAWIZ_LIBRARY_PATH
evawiz_library_path = os.getenv("EVAWIZ_LIBRARY_PATH");
for item in evawiz_library_path.split(':'):
    add_lib(item,True);
    #print item
    pass

#depended modules
home_evawiz = os.getenv("HOME_EVAWIZ")
if not home_evawiz: home_evawiz = os.getenv("HOME")+"/evawiz"
evawiz_root = os.getenv("EVAWIZ_ROOT")
if not evawiz_root: evawiz_root = os.getenv("HOME")+"/.evawiz"
evawiz = evawiz_root+'/evawiz'
for i in range(2,len(sys.argv)):
    is_find = False;
    mod = sys.argv[i]
    path = os.path.join( home_evawiz,mod);
    if os.path.exists(path):
        add_lib('../%s/lib'%mod);
        add_include('../%s/include'%mod);
        add_path('../'+mod);
        pass
    path = os.path.join(evawiz,"modules/"+mod);
    if os.path.exists(path):
        add_lib(path+'/lib',True);
        add_lib(path+'/lib64',True);
        add_include(path+'/include',True);
        add_path(path,True);
        pass
    path = os.path.join(evawiz_root,"base/"+mod);
    if os.path.exists(path):
        add_lib(path+'/lib',True);
        add_lib(path+'/lib64',True);
        add_include(path+'/include',True);
        add_path(path,True);
        pass
    path = os.path.join(evawiz_root,"programs/"+mod);
    if os.path.exists(path):
        add_lib(path+'/lib',True);
        add_lib(path+'/lib64',True);
        add_include(path+'/include',True);
        add_path(path,True);
        pass


    pass


sys.stdout.write(inc)
exit(0);

