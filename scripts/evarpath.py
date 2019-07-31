#!/usr/bin/env python3
from evawiz_basic import *

if ( len(sys.argv)<2 ):
    exit(0);

pre = " -Wl,"
if ( sys.argv[1] == "tcu" ):
    pre = " -Xlinker "
    pass
inc= "";

def add_path(path,absolute=False):
    global inc
    if absolute:
        inc += "%s-rpath='%s'"%(pre,path)
    else:
        inc += "%s-rpath='$ORIGIN/%s' "%(pre,path)
        pass
    pass
        
if ( len(sys.argv)== 3 and sys.argv[1] == 'tcu' and sys.argv[2] == 'MAIN_EVA' ):
    home_evawiz = os.getenv("HOME_EVAWIZ")
    if not home_evawiz: home_evawiz = os.getenv("HOME")+"/evawiz"
    evawiz_root = os.getenv("EVAWIZ_ROOT")
    if not evawiz_root: evawiz_root = '/opt/evawiz'
    evawiz = evawiz_root+'/evawiz'
    add_path('/opt/evawiz/base/staticlibs',True)
    add_path('/opt/evawiz/gcc/lib64',True)
    add_path('/opt/evawiz/cuda-7.0/lib64',True)
    add_path('/opt/evawiz/openmpi/lib',True)
    add_path('/opt/evawiz/evawiz/lib',True)
    add_path('/opt/evawiz/python/lib',True)
    add_path('/opt/evawiz/templibs',True)
    matlab_root = os.getenv("MATLAB_ROOT")
    if matlab_root:
        add_path("%s/bin/glnxa64"%matlab_root,True)
        add_path("%s/sys/os/glnxa64"%matlab_root,True)
    sys.stdout.write(inc);
    exit(0);
    pass

###for normal modules
add_path('lib');
add_path('');

home_evawiz = os.getenv("HOME_EVAWIZ")
if not home_evawiz: home_evawiz = os.getenv("HOME")+"/evawiz"
evawiz_root = os.getenv("EVAWIZ_ROOT")
if not evawiz_root: evawiz_root = '/opt/evawiz'
evawiz = evawiz_root+'/evawiz'
for i in range(2,len(sys.argv)):
    is_find = False;
    mod = sys.argv[i]
    path = os.path.join( home_evawiz,mod);
    if os.path.exists(path):
        lib_path = os.path.join( path, 'lib');
        if os.path.exists( lib_path ):
            add_path('../%s/lib'%mod);
            pass
        add_path('../'+mod);
        pass
    path = os.path.join(evawiz,"modules/"+mod);
    if os.path.exists(path):
        lib_path = os.path.join( path, 'lib');
        if os.path.exists( lib_path ):
            add_path(lib_path,True);
            pass
        add_path(path,True);
        pass
    pass
if os.path.exists( 'lib' ):
    add_path('lib');
    pass

sys.stdout.write(inc)
exit(0);

