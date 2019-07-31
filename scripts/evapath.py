#!/usr/bin/env python3
from evawiz_basic import *

inc = " -I. -L. "
home_evawiz = os.getenv("HOME_EVAWIZ")
if not home_evawiz: home_evawiz = os.getenv("HOME")+"/evawiz"
evawiz_root = os.getenv("EVAWIZ_ROOT")
if not evawiz_root: evawiz_root = '/opt/evawiz'
evawiz = evawiz_root+'/evawiz'
for i in range(1,len(sys.argv)):
    is_find = False;
    mod = sys.argv[i]
    path = os.path.join( home_evawiz,mod);
    if os.path.exists(path):
        is_find = True;
        inc_path = os.path.join( path, 'include');
        if os.path.exists( inc_path ): inc += " -I%s "%(inc_path)
        lib_path = os.path.join( path, 'lib');
        if os.path.exists( lib_path ): inc += " -L%s "%(lib_path)
        inc += " -I%s -L%s "%(path,path)
        pass
    path = os.path.join(evawiz,"modules/"+mod);
    if os.path.exists(path):
        is_find = True;
        inc_path = os.path.join( path, 'include');
        if os.path.exists( inc_path ): inc += " -I%s "%(inc_path)
        lib_path = os.path.join( path, 'lib');
        if os.path.exists( lib_path ): inc += " -L%s "%(lib_path)
        inc += " -I%s -L%s "%(path,path)
        pass
    path = os.path.join(evawiz_root,"base/"+mod);
    if os.path.exists(path):
        is_find = True;
        inc_path = os.path.join( path, 'include');
        if os.path.exists( inc_path ): inc += " -I%s "%(inc_path)
        lib_path = os.path.join( path, 'lib');
        if os.path.exists( lib_path ): inc += " -L%s "%(lib_path)
        inc += " -I%s -L%s "%(path,path)
        pass

    if not is_find:
        sys.stderr.write(""" Error: Dependency failed.
        Cannot find Depending Module '%s'.
        Please execute 'eva get %s' ahead of current operation.
        """%(mod,mod) );
        exit(1);

inc+=" -I%s/include -L%s/lib"%(evawiz,evawiz,)
inc+=" -L%s/base/staticlibs "%(evawiz_root,)
inc+=" -I%s/gcc/include"%(evawiz_root,)
#inc+=" -L%s/gcc/lib"%(evawiz_root,)
inc+=" -L%s/gcc/lib64"%(evawiz_root,)
inc+=" -I%s/cuda-7.0/include"%(evawiz_root,)
inc+=" -L%s/cuda-7.0/lib64"%(evawiz_root,)
inc+=" -I%s/openmpi/include/ "%(evawiz_root,)
inc+=" -L%s/openmpi/lib/ "%(evawiz_root,)
inc+=" -I%s/python/include/python2.7/ -L%s/python/lib "%(evawiz_root,evawiz_root,)

evawiz_include = os.getenv("EVAWIZ_INCLUDE_PATH");

matlab_root = os.getenv("MATLAB_ROOT")
if matlab_root:
    inc+=" -I%s/extern/include "%matlab_root
    inc+=" -L%s/bin/glnxa64 "%matlab_root
    inc+=" -L%s/sys/os/glnxa64 "%matlab_root

sys.stdout.write(inc)
exit(0);

