#!/usr/bin/env python
#coding:utf-8
################################################
## author: Xiao, Hu                           
## version: 6.23.2019
## copyright: ECD                             
################################################
## basic functions may be called from anywhere##
################################################
import time
import hashlib
import getpass
import re
import sys
import os
import pwd
import socket
import crypt
import random
import string
import subprocess
import traceback
import inspect
DEBUG = False

evawiz_root = os.getenv("EVAWIZ_ROOT");
if ( not evawiz_root ):
    evawiz_root = '/opt/evawiz'
user_home = os.getenv('HOME');
user_evawiz_dir = os.getenv('HOME_EVAWIZ')
if ( not user_evawiz_dir ):
    user_evawiz_dir = os.path.join(user_home,"evawiz");
    pass

BLK_SIZE = 1020

operation_auth_free =  {'register_user','check_user_info','connect_user','find_back', 'mod_register_user','mod_check_user_info','mod_connect_user','mod_find_back','mod_ping',}
if DEBUG: print("Debug Mode")
class CONSTANTS:
    RUNNING_SIDE = "server"
    pass

random.seed()

def dtraceback():
    if DEBUG:
        print('debug traceback:')
        print('-'*60)
        traceback.print_exc()
        print('-'*60)

def line_no():
    return inspect.currentframe().f_back.f_lineno;

def file_name():
    return inspect.currentframe().f_back.f_code.co_filename

def args_get(sn,args,default_value=None):
    if len(args) > sn: return args[sn]
    else: return default_value

def dprint(*params):
    if DEBUG:
        print(inspect.currentframe().f_back.f_code.co_filename,inspect.currentframe().f_back.f_lineno,":",end='',flush=True)
        for i in params:
            print(i,end='',flush=True)
        print()
logfile = evawiz_root+"/log"
def log(content,item='debug'):
    if not content: return
    file_content_append(logfile+"/"+item,content+"\n")

class switch(object):  
    def __init__(self, value):  
        self.value = value  
        self.fall = False  
  
    def __iter__(self):  
        """Return the match method once, then stop"""  
        yield self.match  
        raise StopIteration  
  
    def match(self, *args):  
        """Indicate whether or not to enter a case suite"""  
        if self.fall or not args:  
            return True  
        elif self.value in args: # changed for v1.5, see below  
            self.fall = True  
            return True  
        else:  
            return False 

        pass
    pass

class TerminalSizeType:
    row = 0
    col = 0
    def __init__(self,row = 0, col = 0):
        self.row  = row
        self.col = col
        pass
    pass

def shell_status_output(cmd):
    try:
        output = subprocess.check_output(cmd,shell=True)
        return (0,output)
    except subprocess.CalledProcessError as e:
        status = e.returncode
        output = e.output
        return (e.returncode, str(e) )
    except Exception as e:
        status = -1
        output = e
        return ( -1 ,str(e) )
    pass

def shell_output(cmd):
    return subprocess.check_output(cmd,shell=True)

def terminal_size():
    if sys.stdin.isatty():
        return TerminalSizeType(
            int( shell_output('stty size').split()[0] ),
            int( shell_output('stty size').split()[1] )
        )
    else:
        return TerminalSizeType(10,10);
    pass

COL_SIZE = 40
def print_progress(progress = 0, total_size = 0, volocity = 0, col_size =  COL_SIZE ):
    if col_size == 0 : col_size = (terminal_size().col)
    sys.stdout.write('\r'+' '*(col_size+20)+'\r')
    sys.stdout.write("\r    [")
    psize = int( col_size ) 
    vol = 0.0
    for i in range( psize ):
        if ( i <= psize*progress ):
            sys.stdout.write( '#' )
        else:
            sys.stdout.write(' ')
            pass
        pass
    sys.stdout.write("]    ")
    sys.stdout.write( "%.0f/%s KB   %.1f KB/s"%(total_size*progress,total_size, volocity) )
    sys.stdout.flush()
    #time.sleep(1)

def get_user_name():
    return getpass.getuser()

def get_mac_address():
    import uuid
    node = uuid.getnode()
    mac = uuid.UUID(int = node).hex[-12:]
    return mac

def is_mac_address(mac=''):
    if re.match('^[0-9a-f]{6}',mac) and len(mac) == 12:
        return True
    return False

def calculate_file_md5(filepath): #fp is opened file
    if not os.path.exists(filepath):
        return None
    local_md5 = hashlib.md5()
    with open(filepath,'r') as fp:
        while True:
            blk = fp.read(32768) # 32kb per block
            if not blk: break
            local_md5.update( blk.encode() )
    return local_md5.hexdigest()

def file_md5(filepath):
    return calculate_file_md5(filepath)

def text_md5_new(text):
    return hashlib.md5( text[0] ).hexdigest()

def text_md5(text):
    return hashlib.md5( text ).hexdigest()


def get_file_info(filepath):
    stat = os.stat(filepath)
    uid = stat.st_uid
    pw = pwd.getpwuid(uid)
    return (pw.pw_name,"%d"%uid)

def file_content_get(filepath):
    filecontent = ''
    if not os.path.exists(filepath): return "";
    with open(filepath,'r') as fp:
        while True:
            blk = fp.read(32768) # 32kb per block
            if not blk: break
            filecontent += blk
    return filecontent

def file_content_set(filepath,filecontent=''):
    with open(filepath,'w') as fp:
        fp.write(filecontent)

def file_content_append(filepath,filecontent=b''):
    if not filecontent: return None
    with open(filepath,'a') as fp:
        fp.write(filecontent)
    
def str_start_with(line,word):
    if line[0:len(word)]==word:
        return True
    return False

def str_end_with(line,word):
    if line[-len(word):]==word:
        return True
    return False

def str_after(line,word):
    wlen = len(word)
    if line[0:wlen]==word:
        return line[wlen:]
    return None 
    
def line_trim_comment(line):
    pos = line.find('#')
    if pos<0:
        return line
    else:
        return line[:pos]

def mcrypt(passstr):
    chars = string.ascii_letters + string.digits
    salt = random.choice(chars)+random.choice(chars)
    return crypt.crypt(passstr,salt)

def ctimestr():
    return time.strftime('%Y-%m-%d %H:%M:%S')

def unpackdict(dstr):
    if type(dstr) != type('a'):return {}
    dic = {}
    for pair in dstr.replace('\n',';').split(';'):
        if pair != '':
            (key,value) = pair.split(':')
            dic[key]=value
    return dic

def packdict(dic):
    if type(dic) != type({}): return ''
    dstr = ''
    for (key,value) in dic.items():
        dstr+='%s:%s;'%(key,value)
    return dstr
def dictQ(dic,key,value=None):
    if not dic: return False
    if not type(dic) == dict: return False
    if not key in dic: return False
    if value:
        return dic[key] == value;
    else:
        return True

def strstrip(string):
    return string.strip()

def get_modulename_from_path(path=''):
    if not path: path = os.getcwd()
    evawiz_dir = user_evawiz_dir
    modules_dir = os.path.join(evawiz_dir,'modules')
    #dprint(evawiz_dir,modules_dir)
    while True:
        dirname = os.path.dirname(path)
        #dprint('path = %s dirname = %s'%(path,dirname) )
        if os.path.exists(modules_dir) and os.path.samefile(dirname,modules_dir):break
        if os.path.samefile(dirname,evawiz_dir):break
        path = os.path.dirname(path)
        if path=='/': return None
    return os.path.basename(path)

def check_version_string(string=''):
    if not string: return False
    vers = string.split('.')
    if not len(vers) in (1,2,3): return False
    if False in list(map( lambda x: x.isdigit(), vers)): return False
    return True

def get_version_from_string(string=''):
    if not string: return None 
    vers = string.split('.')
    if not len(vers) == 3: return None
    if False in list(map( lambda x: x.isdigit(), vers)): return None 
    return list(map(lambda x: int(x) ,vers))

######
# single:options   transform: short option value to full option
######
def argv2argsoptions(args,single=[],transform={}):
    argsleft = []
    options = {}
    key = None
    for i in range(len(args)):
        arg = args[i]
        if arg[0] == '-':
            while arg and arg[0] == '-': arg = arg[1:]
            if not arg : raise Exception('Invalid argument specified "%s"'%args[i])
            key = arg
            #transform
            tokey = transform.get(key)
            if tokey: key = tokey
            if key in single: 
                options[key] = 'True'
                key = None
            continue
        if key:
            options[key] = arg
            key = None
        else:
            argsleft.append(arg)
    return (argsleft,options)

def options_transform(options,transform={}):
    for key in transform:
        value = options.get(key)
        if value:
            del options[key]
            options[ transform[key] ] = value
    return options

def get_option(options={},key='',default_value=''):
    if not options: return default_value
    value = options.get(key)
    if value: return value
    return default_value

def get_language(): #en-US, zh_CN and so on
    lang = os.getenv("EVAWIZ_LANG")
    if not lang: lang = os.getenv("LANG").split('.')[0].replace('-','_')
    if not lang: lang = 'en_US'
    return lang
def init_evawiz_language():
    pass
init_evawiz_language()

#################################################################
#module version management
#get module versions and repair incorrect information
class EvaVersion:
    version_file = '__eva_version_list'
    ignore_list = []
    runfile_list = []

    def __init__(self,module_dir,eva_branch='eva',branch_dir='',absolute_branch=False):
        module_dir = to_str( module_dir )
        eva_branch = to_str( eva_branch )
        branch_dir = to_str( branch_dir )

        cwd = os.getcwd()
        if not os.path.exists(module_dir):
            raise Exception('%s does not exists'%module_dir)
        os.chdir(module_dir)
        module_full_path = os.getcwd()
        self.module_name = os.path.basename(module_full_path)
        self.module_dir = module_full_path
        self.eva_branch = eva_branch
        if not branch_dir:
            self.branch_dir = os.path.join(module_full_path,'.%s'%eva_branch)
        else:
            if absolute_branch:
                self.branch_dir = branch_dir
            else:
                self.branch_dir = os.path.join(module_full_path,branch_dir)
        #create branch_dir if not exists
        if not os.path.exists(self.branch_dir):
            os.mkdir(self.branch_dir)
        else:
            if not os.path.isdir(self.branch_dir):
                os.rename(self.branch_dir,'%s.bak'%(self.branch_dir) )
                os.mkdir(self.branch_dir)
        self.hfile = self.module_name+'Module.h'
        self.eva_hfile = self.to_eva_file(self.hfile)
        self.version_file = self.branch_dir+"/"+self.version_file;
        os.chdir(cwd)

    def to_eva_file(self,filename=''):
        if not filename:return ''
        return '%'+filename+'%'

    def rto_eva_file(self,filename=''):
        if not filename: return '' 
        if filename == '': return ''
        if filename[0] != '%' : return ''
        try:
            if ( len( filename.split('%') ) < 2 ): return ''
            return filename.split('%')[1]
        except Exception as e:
            raise Exception('try to eva form rtransformation on non-eva filename')

    def check_hfile_change(self,old_file,new_file):
        if not os.path.isfile(old_file): return 'major'
        if not os.path.isfile(new_file): return 'major'
        #output1 is lines old files own but new files not
        (status1,output1) = shell_status_output(" diff -B --unchanged-line-format= --old-line-format= --new-line-format='%%L' %s %s "%(new_file,old_file))
        #output2 is lines new files own but new files not
        (status2,output2) = shell_status_output(" diff -B --unchanged-line-format= --old-line-format= --new-line-format='%%L' %s %s "%(old_file,new_file) )
        if not output1 and not output2: #hfile nochange revision change
            return 'revision'
        elif not output1 and output2: #only add something minor change
            return 'minor'
        else: #both have lines major change
            return 'major'

    def update_version(self,version,change,update_type):
        cur_vers = version
        if change == 'major' or update_type == 'major':
            return (cur_vers[0]+1,1,1)
        if change == 'minor' or update_type == 'major':
            return (cur_vers[0],cur_vers[1]+1,1)
        if change == 'revision' or update_type == 'revision':
            return (cur_vers[0],cur_vers[1],cur_vers[2]+1)
        ####
        raise Exception('try to update a version list with invalid discriptor')

    def get_pattern_list(self,pattern_type='ignore',ver_str=""):
        pattern_type = to_str( pattern_type )
        ver_str = to_str( ver_str )

        
        patterns = []
        if ( ver_str ):
            filename = os.path.join(self.branch_dir,'%'+'.eva'+pattern_type+'%'+ver_str)
        else:
            filename = os.path.join(self.module_dir,'.eva'+pattern_type+ver_str)
            pass
        dprint('filename of pattern list is ',filename);
        if pattern_type == 'ignore':
            patterns.append('!.evaignore')
            patterns.append('!.evarunfile')
            patterns.append('!lib%sModule.so'%self.module_name)
            patterns.append('!input.ev')
            patterns.append('/.eva*/')
            patterns.append('*.dat')
            patterns.append('*.tmp')
            pass
        if pattern_type == 'runfile':
            patterns.append('*.so')
            patterns.append('*.h')
            patterns.append('*.ev')
            pass
        if pattern_type == 'introfile':
            patterns.append('/doc/intro.art')
            pass
        if os.path.isfile( filename  ): 
            for pattern in file_content_get( filename ).split('\n'):
                pos = pattern.find('#')
                if pos >=0: pattern = pattern[0:pattern.find('#')]
                pattren = pattern.strip()
                if not pattern: continue
                if pattern[0] == '!':
                    patterns.insert(0,pattern)
                else:
                    patterns.append(pattern)
        #patterns.sort()
        dprint('patterns = ',patterns)
        #transform pattern to python re rules used in python re.match
        for i in range( 0, len(patterns) ):
            dprint(i,':',patterns[i])
            if patterns[i][0] == '!':
                is_pattern_match = True
                patterns[i] = patterns[i][1:]
            else:
                is_pattern_match = False
            patterns[i] = re.sub(r'\.','\.',patterns[i])
            patterns[i] = re.sub(r'\*','.*',patterns[i])
            patterns[i] = re.sub(r'\+','.+',patterns[i])
            if patterns[i][0] != '/':
                patterns[i] = '\A.*/'+patterns[i]
            else:
                patterns[i] = '\A'+patterns[i]
            patterns[i] = patterns[i]+'\Z'

            if is_pattern_match:
                patterns[i] = '!'+patterns[i]

            dprint(i,patterns[i])

        dprint('patterns = ',patterns)
        return patterns

    def is_pattern_matched(self,filename,patterns):
        for pa in patterns:
            dprint('test : %s <---M---> %s'%(pa,filename))
            is_pattern_match = False
            if pa[0] == '!':
                is_pattern_match = True
                pa = pa[1:]
                pa.strip()
            if pa[-3:] != '/\\Z' and filename[-1] == '/': continue 
            if pa[-3:] == '/\\Z' and filename[-1] != '/': continue 
            #check if matched
            dprint("try match",pa,filename);
            if re.match(pa,filename):
                dprint('allowed: %s    %s <---M---> %s'%(is_pattern_match,pa,filename))
                if is_pattern_match: return False
                dprint('matched: %s <---M---> %s'%(pa,filename))
                return True
        dprint(pa,filename,"not matched");
        return False

    def __get_push_file_tree(self,cu_dir,result_list=[]):
        dprint('self.module_dir= %s,cu_dir= %s,result= %s'%(self.module_dir,cu_dir,result_list))
        parent_dir=cu_dir
        dir_list = []
        file_list = []
        dprint('list files in %s'%(self.module_dir+'/'+cu_dir))
        dprint('self ignore list: %s'%(self.ignore_list))
        real_parent = self.module_dir + '/'+cu_dir
        for f in os.listdir( real_parent ):
            #dprint( 'item = %s'%os.path.join(real_parent,f) )
            if os.path.isdir( os.path.join(real_parent,f) ):
                dprint('dir = %s'%f)
                if self.is_pattern_matched( cu_dir+'/'+f+'/',self.ignore_list ):
                    dprint('dir %s is ignored'%( cu_dir+'/'+f+'/' ))
                    continue
                dir_list.append(f)
            else:
                dprint('file = %s'%f)
                if self.is_pattern_matched( cu_dir+'/'+f,self.ignore_list):
                    dprint('file %s is ignored'%( cu_dir+'/'+f) )
                    continue
                file_list.append(f)
                pass
            pass
        
        dir_list.sort();
        file_list.sort();
        result_list.append( (parent_dir,dir_list,file_list) )
        for sub_dir in dir_list:
            self.__get_push_file_tree( os.path.join(cu_dir,sub_dir),result_list)
        #dprint('cu_dir= %s,result= %s'%(cu_dir,result_list) )
        return

    def get_push_file_tree(self):
        if not self.ignore_list:
            self.ignore_list = self.get_pattern_list('ignore')
        file_tree= []
        self.__get_push_file_tree('',file_tree)
        return file_tree

    def put_module_version_list(self,version_list):
        with open(self.version_file,'w') as fp:
            for version in version_list:
                fp.write("%s %s %s\n"%(version[0],version[1],version[2]) )
        return True

    def get_module_version_list(self):
        cwd = os.getcwd()
        os.chdir(self.branch_dir)

        if False: # os.path.exists(self.version_file):
            versstr = file_content_get(self.version_file).split('\n')
            vers = []
            for verstr in versstr:
                verstr = verstr.split()
                if len(verstr) != 3: continue
                #check ver
                if not os.path.isfile("%s.%s.%s.%s"%( self.to_eva_file(self.module_name+'Module.h'),verstr[0],verstr[1],verstr[2]) ):
                    continue
                vers.append(list(map(int,verstr)))
            vers.sort()
            self.put_module_version_list(vers)
            os.chdir(cwd)
            return vers

        #no files exists,create one
        vers=[]
        eva_hfile_len = len(self.eva_hfile)
        for f in os.listdir( '.' ):
            if ( not  re.match('\A\%.*\%\.\d+\.\d+\.\d+\Z',f) ):
                continue;
            verstr = f.split("%")[2][1:];
            #dprint(verstr)
            vers.append(verstr)
            pass
        ####
        vers = set( vers );
        vers = list( vers );
        res = []
        for ver in vers:
            res.append( list(map(int,ver.split('.') ) ) );
            pass
        res.sort(key=lambda l:(l[0],l[1],l[2]))
        self.put_module_version_list(res)
        os.chdir(cwd)
        return res

    def get_current_version(self): 
        version_list = self.get_module_version_list()
        #dprint('current version list:',version_list)
        if not version_list:
            return [0,0,0]
        else:
            return version_list[-1]

    def check_version_string(self,string=''):
        if not string: return False
        vers = string.split('.')
        if not len(vers) in (1,2,3): return False
        if False in list(map( lambda x: x.isdigit(), vers)): return False
        return True

    def get_actual_version_from_string(self,string='',allowed_vers='0;'):
        # get the version or the version bigger than specified
        if string == 'newest' or string == '0': return self.get_current_version()
        if not self.check_version_string(string): return None
        version = string.split('.')
        version_list = self.get_module_version_list()
        dprint( 'version = ',version)
        dprint( 'version_list = ',version_list )
        for i in range(len(version_list)-1,-1,-1):
            vv = version_list[i]
            match = True
            for j in range( len(version) ):
                if vv[j] != int(version[j]):
                    match = False
                    break
            if match: return vv 
        return None

    def get_old_new_version(self,update_type,hold_version=None):
        cwd = os.getcwd()
        os.chdir(self.branch_dir)
        version_list = self.get_module_version_list()
        #dprint(version_list)
        if not version_list:
            os.chdir(cwd)
            if not hold_version:
                return [ [0,0,0],[1,1,1] ]
            old_ver = hold_version
            check_result = 'revision'
        else:
            old_ver = version_list[-1]
            #dprint(old_ver)
            old_hfile = '%s.%s.%s.%s'%(self.eva_hfile,old_ver[0],old_ver[1],old_ver[2])
            #dprint(old_hfile)
            new_hfile = '../%s'%self.hfile
            check_result = self.check_hfile_change(old_hfile,new_hfile)
        #dprint('get_old_new_version:check result = %s'%check_result)
        new_ver = self.update_version(old_ver,check_result,update_type)
        #dprint('get_old_new_version result:',old_ver,new_ver)
        os.chdir(cwd)
        return [old_ver,new_ver]

    def local_push_module(self,hold_version=None,update_type='revision'):
        cwd = os.getcwd()
        os.chdir(self.module_dir)
        if not os.path.exists(self.branch_dir):
            os.mkdir(self.branch_dir)
        else:
            if not os.path.isdir(self.branch_dir):
                os.rename(self.branch_dir,'%s.bak'%self.branch_dir)
                os.mkdir(self.branch_dir)
        os.chdir(self.branch_dir)
        (old_ver,new_ver) = self.get_old_new_version(update_type,hold_version)
        old_suffix = '.%s.%s.%s'%(old_ver[0],old_ver[1],old_ver[2])
        new_suffix = '.%s.%s.%s'%(new_ver[0],new_ver[1],new_ver[2])
        #dprint('suffix:',old_suffix,new_suffix);
        #cp files for files which changes and make links for files that do not change,
        #only save the space when previous version file are the same
        file_tree = self.get_push_file_tree()
        #dprint('file to push',file_tree)
        for (parent,dir_list,file_list) in file_tree:
            #dprint("\n\n\ndealing a parent>>>%s<<<"%parent)
            eva_parent = os.path.join( self.branch_dir,parent)
            cur_parent = os.path.join( self.module_dir,parent)
            #dprint('eva_parent = %s, cur_parent= %s'%(eva_parent,cur_parent))
            os.chdir(eva_parent)
            #dir_list dealing
            for dir_name in dir_list:
                #dprint('dealing dir :',dir_name);
                if not os.path.exists(dir_name):
                    os.makedirs(dir_name)
                os.system('ln -fs %s %s'%( dir_name, dir_name+new_suffix) )
            #file_list file
            for file_name in file_list:
                #dprint("dealing file:",file_name);
                eva_file = self.to_eva_file(file_name)
                cur_file = os.path.join(cur_parent,file_name)
                old_eva_file = eva_file+old_suffix
                new_eva_file = eva_file+new_suffix
                new_md5 = file_md5(cur_file)
                if os.path.exists(old_eva_file): 
                    #check whether are same
                    old_md5 = file_content_get(old_eva_file+'.md5')
                else:
                    old_md5 = None;
                #############
                if old_md5 == new_md5:
                    #dprint('make link of %s pointing to %s'%(new_eva_file,old_eva_file) )
                    if os.path.islink(old_eva_file):
                        os.system('ln -fs %s %s'%(os.readlink(old_eva_file),new_eva_file) )
                        os.system('ln -fs %s %s'%(os.readlink(old_eva_file+'.md5'),new_eva_file+'.md5') )
                    else:
                        os.system('ln -fs %s %s'%(old_eva_file,new_eva_file) )
                        os.system('ln -fs %s %s'%(old_eva_file+'.md5',new_eva_file+'.md5') )
                else:
                    #dprint("cp new file to %s"%new_eva_file);
                    os.system('cp %s %s'%(cur_file,new_eva_file))
                    file_content_set(new_eva_file+'.md5',new_md5)
        #### push sucess
        print('push operation success, update file_version to:',new_ver)
        os.chdir(self.branch_dir)
        file_content_append(self.version_file,'%s %s %s\n'%(new_ver[0],new_ver[1],new_ver[2]))
        #dprint('update version file finished')
        os.chdir(cwd)

    def local_push_module_to_dir(self,to_path =''):
        if ( not to_path  or not os.path.exists(to_path) ):
            return False
        cwd = os.getcwd()
        file_tree = self.get_push_file_tree()
        #dprint('file to push',file_tree)
        for (parent,dir_list,file_list) in file_tree:
            #dprint("\n\n\ndealing a parent>>>%s<<<"%parent)
            eva_parent = os.path.join( to_path,parent)
            cur_parent = os.path.join( self.module_dir,parent)
            #dprint('eva_parent = %s, cur_parent= %s'%(eva_parent,cur_parent))
            os.chdir(eva_parent)
            #dir_list dealing
            for dir_name in dir_list:
                #dprint('dealing dir :',dir_name);
                if not os.path.exists(dir_name):
                    os.makedirs(dir_name)
                #os.system('ln -fs %s %s'%( dir_name, dir_name+new_suffix) )
            #file_list file
            for file_name in file_list:
                cur_file = os.path.join(cur_parent,file_name)
                os.system('cp -f %s %s'%(cur_file,file_name))
        #### push sucess
        os.chdir(cwd)

    def __get_file_tree_of_version(self,cu_dir,result_list=[],suffix=None,suffix_len=0):
        #dprint('cu_dir= %s,result= %s,suffix=%s,suffix_len=%s'%(cu_dir,result_list,suffix,suffix_len))
        parent_dir=cu_dir
        dir_list = []
        file_list = []
        real_parent = self.branch_dir + '/'+cu_dir
        #dprint('real parent %s'%real_parent)
        for f in os.listdir( real_parent ):
            #dprint( 'item = %s'%os.path.join(real_parent,f) )
            if os.path.isdir( os.path.join(real_parent,f) ):
                #dprint('dir = %s'%f)
                if os.path.islink( os.path.join(real_parent,f)+suffix):
                    dir_list.append(f)
            else:
                if len(f)<suffix_len:continue
                #dprint('%s <-> %s'%(f[-suffix_len:],suffix))
                if f[-suffix_len:]!= suffix: continue
                file_list.append(f)
                pass
            pass
        dir_list.sort();
        file_list.sort();
        result_list.append( (parent_dir,dir_list,file_list) )
        for sub_dir in dir_list:
            self.__get_file_tree_of_version( os.path.join(cu_dir,sub_dir),result_list,suffix,suffix_len)
        #dprint('cu_dir= %s,result= %s'%(cu_dir,result_list) )
        return

    def get_file_tree_of_version(self,version=None):
        versions = self.get_module_version_list()
        #dprint('versions %s'%versions)
        #print(os.getcwd(),versions)
        #print(version)
        
        if not version: #6.23.2019
            if not versions: return None
            cur_vers = versions[-1]
        else:
            version = list(version)
            if not version in versions: return None
            cur_vers = version
        #dprint('try to find version %s'%cur_vers)
        #print(cur_vers)
        suffix = '.%s.%s.%s'%(cur_vers[0],cur_vers[1],cur_vers[2])
        suffix_len = len(suffix)
        file_tree= []
        self.__get_file_tree_of_version('',file_tree,suffix,suffix_len)
        return file_tree

    def __get_runfile_of_version(self,cu_dir,result_list=[],suffix=None,suffix_len=0):
        #dprint('cu_dir= %s,result= %s,suffix=%s,suffix_len=%s'%(cu_dir,result_list,suffix,suffix_len))
        parent_dir=cu_dir
        dir_list = []
        file_list = []
        real_parent = self.branch_dir + '/'+cu_dir
        #dprint('real parent %s'%real_parent)
        for f in os.listdir( real_parent ):
            if os.path.isdir( os.path.join(real_parent,f) ):
                if not os.path.islink( os.path.join(real_parent,f)+suffix): continue
                dir_list.append(f) #erase list empty list when operation finished
            else:
                if len(f)<suffix_len:continue
                if f[-suffix_len:] != suffix: continue
                if ( f[0] != '%' ): continue
                #dprint( 'test file = %s'%os.path.join(real_parent,f) )
                pure_f = self.rto_eva_file(f);  
                if ( pure_f == "" ): continue;
                full_pat = cu_dir+'/'+pure_f;
                if ( cu_dir != "" ):
                    full_pat = '/'+full_pat;
                if self.is_pattern_matched( full_pat,self.runfile_list):
                    file_list.append(f)
                    #dprint(" matched and appended to")
                else:
                    #dprint(" not matched")
                    pass
                pass
            pass
        dir_list.sort();
        file_list.sort();
        if ( len(dir_list)!=0 or len(file_list)!=0 ):
            result_list.append( (parent_dir,dir_list,file_list) )
        res_len = len(result_list );
        for sub_dir in dir_list:
            self.__get_runfile_of_version( os.path.join(cu_dir,sub_dir),result_list,suffix,suffix_len)
            pass
        if ( ( len(file_list) == 0 and
               len(dir_list) !=0 ) and
             len( result_list ) == res_len and
             ( cu_dir != '' )  ):
            result_list.pop();
            pass
        #dprint('cu_dir= %s,result= %s'%(cu_dir,result_list) )
        return

    def get_runfile_of_version(self,version=None,runfile_type='runfile'):
        versions = self.get_module_version_list()
        #dprint('get runfile of version %s from %s'%(version,versions))
        #print(os.getcwd(),versions)
        #print(version)
        if not version: #6.23.2019
            if not versions: return None
            cur_vers = versions[-1]
        else:
            version = list(version)
            if not version in versions: return None
            cur_vers = version
        #dprint('try to find version %s'%cur_vers)
        #print(cur_vers)
        suffix = '.%s.%s.%s'%(cur_vers[0],cur_vers[1],cur_vers[2])
        suffix_len = len(suffix)
        file_tree= []
        ###get runfile list
        #if not self.runfile_list:
        self.runfile_list = self.get_pattern_list(runfile_type,suffix)
        #dprint("runfile_list:",self.runfile_list)
        self.__get_runfile_of_version('',file_tree,suffix,suffix_len)
        return file_tree

    def send_file(self,filepath,fsend,frecv):
        dprint( 'step into send_file' )
        if not os.path.exists(filepath):
            dprint( 'step into send_file, but no file exist' )
            return None
        dprint( 'step into send_file' )
        filecontent=[]
        file_md5 = hashlib.md5()
        with open(filepath,'rb') as fp:
            while True:
                blk = fp.read(BLK_SIZE)
                if not blk:break
                file_md5.update( blk )
                filecontent.append( blk )
                pass
            pass
        file_md5 = file_md5.hexdigest()
        dprint("file_md5 = ",file_md5)
        fsend(file_md5)
        reply_info = frecv()
        dprint("reply info after", reply_info)
        if reply_info != 'need to send':
            dprint("no need to send")
            return "OK" 
        size = len(filecontent)
        dprint('size=',size)
        psize =   int( size / COL_SIZE )
        if psize == 0 : psize = 1
        dprint("send size = ",size,'try send size')
        fsend( "%s"%(size,) )
        tt = 0
        while True:
            try:
                i = 0
                t0 = time.time()
                for blk in filecontent:
                    #print('send blk ',i+1,' ',len(blk) )
                    i+=1
                    fsend( blk )
                    if ( CONSTANTS.RUNNING_SIDE == "user" and  i%psize == 0 ):
                        print_progress( 1.0*i/size ,size, psize/( time.time() - t0 ) )
                        t0 = time.time()
                        pass
                    pass
                break
            except Exception as e:
                tt += 1
                if tt == 10:
                    print('failed abort')
                    break
                print('send failed, try again:',e)
                pass
            pass
        sys.stdout.write("\r"+" "*(COL_SIZE+20)+"\r")
        dprint('try recv recv info')
        reply_info = frecv()
        dprint("send status = ",reply_info )
        if ( reply_info == "recv ok" ): return "OK" 
        return "NOT OK"

    def recv_file(self,filepath,old_file_md5,fsend,frecv):
        dprint( 'step into recv_file' )
        file_md5 = frecv()
        if file_md5 == old_file_md5: #same content
            fsend('no need to send')
            dprint("same file, no need to recv")
            return ('same file',file_md5)
        fsend('need to send')
        dprint('try recv size')
        size = frecv()
        dprint("size = ",size)
        size = int( size )
        psize =   int( size / COL_SIZE )
        #print(size, psize, COL_SIZE, CONSTANTS.RUNNING_SIDE)
        if psize == 0 : psize = 1
        full_size = size
        tt = 0
        while True:
            try:
                t0 = time.time()
                size = full_size
                real_md5 = hashlib.md5()
                filecontent = []
                while size > 0 :
                    #print('recv blk ',size,' ',end=' ' )
                    blk = frecv(is_bytes=True)
                    real_md5.update( blk )
                    filecontent.append( blk );
                    size -= 1
                    if CONSTANTS.RUNNING_SIDE =='user' and size % psize == 0:
                        print_progress( 1.0-1.0*size/full_size ,full_size,psize/(time.time() - t0 ) )
                        t0 = time.time()
                        pass
                    pass
                break
            except Exception as e:
                tt+=1
                if tt == 10:
                    print('failed abort')
                    break
                print('send failed, try again:',e)
                pass
            pass
        sys.stdout.write("\r"+" "*(terminal_size().col)+"\r")
        real_md5 = real_md5.hexdigest()
        dprint( 'recv md5 ', real_md5," , ", file_md5 )
        if False and file_md5 != real_md5:
            dprint('md5 inconsistent')
            fsend('md5 inconsistent')
            return ('recv fail',file_md5)
        #recved and content is right, write to filepath
        dprint('send recv ok')
        fsend('recv ok')

        dprint("try put content to >>>", filepath )
        with open(filepath,'wb')as fp:
            for blk in filecontent:
                fp.write(blk)
                pass
            pass
        dprint('new file write done')
        return ('recv ok',file_md5)

    def send_module(self,full_parent='/',file_tree=[],renametype='eva2normal',fsend=None,frecv=None,fsend_eof=None):
        #dprint('send the module')
        #send files
        #send md5 let server check whether need to transfer
        for parent,dirnames,filenames in file_tree:
            dprint(parent,dirnames,filenames)
            for dirname in dirnames:
                dprint('send dir $%s %s'%(parent,dirname) )
                fsend('dir $%s %s'%(parent,dirname) )
                if frecv()!= 'ok':
                    if fsend_eof: fsend_eof()
                    raise Exception('error while sending files to server,fail to send dir')
                #dprint(dirname,'over')
            #send info  (type,parent,name,md5)
            for filename in filenames:
                if renametype == 'eva2normal': send_filename = self.rto_eva_file(filename)
                elif renametype == 'unchange': send_filename = filename
                else : send_filename = filename
                if ( send_filename == "" ): continue;
                if parent:
                    print("   sending %s \t\tin %s"%(send_filename,parent));
                else:
                    print("   sending %s"%send_filename);
                dprint('send file $%s %s'%(parent,send_filename) )
                fsend('file $%s %s'%(parent,send_filename) )
                reply_info = frecv()
                if reply_info != 'ready to recv file':
                    print('can not send files when send module')
                    if fsend_eof: fsend_eof()
                    raise Exception('can not send files when send module, fail to send file')
                filefullpath = os.path.join(full_parent,parent,filename)
                dprint('file path = ',filefullpath)
                recvstat = self.send_file( filefullpath ,fsend, frecv )
                dprint('recvstat = ',recvstat)
                if recvstat != 'OK':
                    print('send files failed when send module')
                    if fsend_eof: fsend_eof()
                    raise Exception('send files failed when send module')
                #dprint(filename,'over')
            #end for
        #recv final info
        fsend('module_send_over')
        reply_info = frecv()
        if reply_info == 'module_send finished':
            return True
        return False

    def recv_module(self,full_parent='/',renametype='unchange',suffix='',old_suffix='',fsend=None,frecv=None):
        #normal files and directory
        #dprint('recv the module')
        while True:
            info = frecv()
            info = info.split()
            dprint('\ninfo =>>>%s<<<'%(info,))
            if len(info) == 1: break
            if len(info) != 3: raise Exception('file description invalid.' )
            (filetype,parent,filename) = (info[0],info[1][1:],info[2] )
            if filetype == 'module_send_over': break
            os.chdir( os.path.join(full_parent,parent) )
            # dir
            if filetype == 'dir':
                #new dir name
                if renametype == 'normal2eva': new_file = filename + suffix
                if os.path.exists(filename):
                    if not os.path.isdir(filename): os.rename(filename,'%s.bak'%filename)
                    shell_status_output('mkdir %s'%filename)
                    if suffix: shell_status_output('ln -fs %s %s'%(filename,new_file))
                else:
                    shell_status_output('mkdir %s'%filename)
                    if suffix: shell_status_output('ln -fs %s %s'%(filename,new_file))
                fsend('ok')
                #dprint(filename,'over')
                continue
            #file
            if filetype == 'file':
                ###new_file_name
                if renametype == 'unchange': #directly overwrite if exist
                    new_file = filename
                    if parent:
                        print("   recving %s \t\tin %s"%(new_file,parent));
                    else:
                        print("   recving %s"%new_file);
                    fsend('ready to recv file')
                    (recv_result,new_md5) = self.recv_file(new_file,'NO_OLD_FILE',fsend,frecv)
                    #dprint(filename,'over')
                    continue

                if renametype == 'normal2eva': #keep 
                    new_file = self.to_eva_file(filename) + suffix
                    if parent:
                        print("   recving %s \t\tin %s"%(new_file,parent));
                    else:
                        print("   recving %s"%new_file);
                    old_file = self.to_eva_file( filename ) + old_suffix
                    if os.path.exists( old_file ):
                        old_md5 = file_content_get(old_file+'.md5')
                    else:
                        old_md5 = 'NO_OLD_FILE'
                    #dprint('old_file',old_file,'with md5 ',old_md5)
                    fsend('ready to recv file')
                    (recv_result,new_md5) = self.recv_file(new_file,old_md5,fsend,frecv)
                    if recv_result == 'same file':
                        if os.path.islink(old_file):
                            shell_status_output('ln -fs %s %s'%(os.readlink(old_file),new_file))
                            shell_status_output('ln -fs %s %s'%(os.readlink(old_file+'.md5'),new_file+'.md5'))
                        else:
                            shell_status_output('ln -fs %s %s'%(old_file,new_file))
                            shell_status_output('ln -fs %s %s'%(old_file+'.md5',new_file+'.md5'))
                    elif recv_result == 'recv ok':
                        file_content_set(new_file+'.md5',new_md5)
                    else:
                        raise Exception('file recv error '+recv_result)
                    #dprint(filename,'over')
                    continue

            #otherwize
            raise Exception('file description invalid, may from attackers')
        #dprint('recv module over')
        fsend('module_send finished')
        return True

##########################################
#uncoding and encoding class

from binascii import b2a_hex, a2b_hex
import base64

def to_bytes(s):
    if isinstance(s,str): return s.encode()
    return s

def to_str(s):
    if not isinstance(s,str): return s.decode()
    return s
    
def encoding(s): #encode a string
    if isinstance(s,str): s = s.encode()
    return base64.encodestring(s) #creq1

def decoding(s): #decode a encoded string	
    if isinstance(s,str): s = s.encode()
    missing_padding = 4 - len(s)%4
    if missing_padding:
        s+=b'='*missing_padding
    return base64.decodestring(s) #creq1


def random_key(bytelen = 32,base= None):
    if not base: base = list(string.ascii_letters)+list(string.digits)+list('!@#$%^&*()<>,./?')
    else: base = list(base)
    key = ''
    for i in range(0,bytelen):
        key = key + random.choice(base)
    return key

def random_simple_key(bytelen = 32,base= None):
    if not base: base = list(string.ascii_letters)+list(string.digits)
    else: base = list(base)
    key = ''
    for i in range(0,bytelen):
        key = key + random.choice(base)
    return key

def passcrypt(passwd):
    if isinstance(passwd,str): passwd = passwd.encode()
    return hashlib.sha1( passwd ).hexdigest()

from Crypto.Hash import MD5
from Crypto.PublicKey import RSA
from Crypto.Util import randpool
import pickle

class RSAEncryption:
    blah = None
    RSAKey = None
    RSAPubKey = None
    RSAPubKeyStr = None
    Type = None

    def __init__(self,Type='host',pubkeystr='',privkeystr=''):
        #position = host || client
        self.Type = Type
        if (Type == 'host' or pubkeystr == ''):
            self.blah = randpool.RandomPool()
            self.RSAKey = RSA.generate(1024,self.blah.get_bytes)
            self.RSAPubKey = self.RSAKey.publickey()
            self.RSAPubKeyStr = pickle.dumps(self.RSAPubKey)
        elif (Type== 'client'):
            self.RSAPubKeyStr = decoding(pubkeystr)
            self.RSAPubKey = pickle.loads(self.RSAPubKeyStr)
        else:
            raise Exception('RSAEncryption Type',Type,'is not allowed!')
    def get_pub_key_str(self):
        return encoding(self.RSAPubKeyStr)

    def encrypt(self,text):
        if isinstance(text,str): text = text.encode()
        return encoding( self.RSAPubKey.encrypt(text,32)[0] )

    def decrypt(self,text):
        if isinstance(text,str): text = text.encode()
        return self.RSAKey.decrypt( decoding(text) )
        
    pass


from Crypto.Cipher import AES
class AESEncryption:
    key = None
    mode = None
    cryptor = None
    name = None
    def __init__(self,key=''):
        if key == '' : key = self.generate_random_key(32)
        self.key = key
        self.mode = AES.MODE_CBC
        #self.cryptor = AES.new(self.key,self.mode,b'0000000000000000')
    def generate_random_key(self,bytelen):
        return random_key(bytelen)

    def get_key(self):
        return self.key

    def encrypt(self,text=''):
        #dprint('type text = ',type(text) )
        if not isinstance(text,str): text = text.decode()
        cryptor = AES.new(self.key,self.mode,b'0000000000000000')
        unitlen = 16
        count = len(text)+2
        if count < unitlen:
            text = 'X'+text +'X'+ ('\0'*(unitlen - count) )
        elif count > unitlen:
            text = 'X'+text +'X'+ ('\0'*(unitlen - (count%unitlen)))
        else:
            text = 'X'+text +'X'
        ciphertext = cryptor.encrypt(text)
        #dprint('aes enrypt %s -> %s'%(text,b2a_hex(ciphertext) ) )
        return b2a_hex(ciphertext)

    def decrypt(self,text=''):
        if isinstance(text,str): text = text.encode()
        cryptor = AES.new(self.key,self.mode,b'0000000000000000')
        if len(text)%2 != 0: raise Exception('Text Decryption Error.')
        plaintext = cryptor.decrypt(a2b_hex(text)).rstrip(b'\0')[1:-1]
        #dprint('aes decrypt %s -> %s'%(text,plaintext))
        return plaintext

## python email interface
import smtplib  
from email.mime.text import MIMEText  
mailto_list=["einsxiao@hotmail.com"] 

def send_mail(mail_name,mail_host,mail_user,mail_pass,mail_postfix,to_list,subject,content): 
    me=mail_name+"<"+mail_user+"@"+mail_postfix+">"
    msg = MIMEText(content,_subtype='html',_charset='utf-8')
    msg['Subject'] = subject
    msg['From'] = me  
    msg['To'] = ";".join(to_list)  
    try:  
        s = smtplib.SMTP()  
        s.connect(mail_host) 
        s.login(mail_user,mail_pass) 
        s.sendmail(me, to_list, msg.as_string()) 
        s.close()  
        return True  
    except Exception as e:  
        print(str(e)  )
        return False  

#linux daemon program enwrap interface
import atexit
from signal import SIGTERM
class ServerDaemon:
    #pid_dir = os.getenv("EVAWIZ_ROOT")+"/pids"
    pid_dir = evawiz_root+"/pids"
    def __init__(self,run_func,pidfile='evawiz.pid',name='evawiz',stdin='/dev/null',stdout='/dev/null',stderr='/dev/null'):
        self.stdin = stdin
        self.stdout  = stdout
        self.stderr = stderr

        self.run_func = run_func
        if not os.path.exists(self.pid_dir): os.makedirs(self.pid_dir)
        self.pidfile = os.path.join(self.pid_dir,pidfile)
        self.name = name 

    def daemonize(self):
        """
        do the UNIX double-fork magic
        """
        try:
            pid = os.fork()
            if pid > 0:
                # exit first parent
                sys.exit(0)
        except OSError as e:
            sys.stderr.write("fork #1 failed: %d (%s)\n" % (e.errno, e.strerror))
            sys.exit(1)

        # decouple from parent environment
        os.chdir('/')
        os.setsid()
        os.umask(0)

        # do second fork
        try:
            pid = os.fork()
            if pid > 0:
                # exit from second parent
                sys.exit(0)
        except OSError as e:
            sys.stderr.write("fork #2 failed: %d (%s)\n" % (e.errno, e.strerror))
            sys.exit(1)

        # redirect standard file descriptors
        sys.stdout.flush()
        sys.stderr.flush()
        si = file(self.stdin, 'r')
        so = file(self.stdout, 'a+')
        se = file(self.stderr, 'a+', 0)
        os.dup2(si.fileno(), sys.stdin.fileno())
        os.dup2(so.fileno(), sys.stdout.fileno())
        os.dup2(se.fileno(), sys.stderr.fileno())

        # write pidfile
        atexit.register(self.delpid)
        pid = str(os.getpid())
        file(self.pidfile,'w+').write("%s\n" % pid)

    def delpid(self):
        os.remove(self.pidfile)

    def run(self):
        self.run_func()

    #------------------------------
    def start(self):
        """
        Start the daemon
        """
        # Check for a pidfile to see if the daemon already runs
        try:
            pf = file(self.pidfile,'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None

        if pid:
            sys.stderr.write("pidfile %s already exist. %s already running?\n" %(self.pidfile,self.name) )
            sys.exit(1)

        #start the daemon
        sys.stdout.write("Starting %s ...\n"%(self.name))
        sys.stdout.flush()
        self.daemonize()
        self.run()

    def stop(self):
        """
        Stop the daemon
        """
        #get pid of the daemon 
        try:
            pf = file(self.pidfile,'r')
            pid = int(pf.read().strip())
            pf.close()
        except IOError:
            pid = None

        if not pid:
            sys.stderr.write("Pidfile %s does not exist. %s not running?\n"% (self.pidfile,self.name) )
            return # not an error in a restart

        #kill the daemon
        try:
            sys.stdout.write("Stoping %s of pid %s ...\n"%(self.name,pid))
            sys.stdout.flush()
            while 1:
                os.kill(pid, SIGTERM)
                time.sleep(0.1)
        except OSError as err:
            err = str(err)
            if err.find("No such process") > 0:
                if os.path.exists(self.pidfile): os.remove(self.pidfile)
                sys.stdout.write("                                 [ OK ]\n")
            else:
                print(str(err))
                sys.stdout.write("                               [ FAILED ]\n")
                sys.exit(1)

    def restart(self):
        """
        Restart the daemon
        """
        self.stop()
        self.start()

    def status(self):
        """
        show the status of the daemon
        """
        try:
            if not os.path.exists( self.pidfile ): raise Exception("pid not exist")
            pid = int( file_content_get(self.pidfile).strip() )
        except Exception as e:
            pid = None

        if not pid:
            sys.stdout.write("%s is not running...\n"%self.name)
        else:
            sys.stdout.write("%s is running with pid %s...\n"%(self.name,pid) )

            
        return # not an error in a restart


        
