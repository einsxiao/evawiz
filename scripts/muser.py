#!/usr/bin/env python
#coding:utf-8
################################################
## author: Xiao, Hu                           ##
## version: 1.0                               ##
## copyright: ECD                             ##
################################################
## define the many functions called by user   ##
################################################
from evawiz_basic import *
import socket 
#import ssl
import importlib


CONSTANTS.RUNNING_SIDE = "user"

server_port = file_content_get( os.getenv("EVAWIZ_ROOT")+"/port.info") 
class UserRequest:
    bufsize = 1024
    mac_addr = get_mac_address()
    user_name = get_user_name()
    # host_name = socket.gethostname()
    conn = None
    connection = None
    server_addr = 'eva.run'
    server_port = None
    proxy = None
    aes = None
    closed = False

    def __init__(self, host='eva.run', port=5555, proxy=None ):
        self.server_addr = host
        self.server_port = port
        self.proxy = proxy
        if self.proxy:
            self.proxy = self.proxy.split(':')
            dprint( "proxy = ",self.proxy )
            if len( self.proxy ) != 2: raise Exception("EVAWIZ_PROXY is not specified in the right form.");
            pass
        pass

    def send_eof(self):
        self.conn.close()
        self.closed = True
        pass

    def send_bytes(self,arg):
        try:
            self.conn.send(b"%4d"%len(arg))
            return self.conn.send(arg)
        except Exception as e:
            raise Exception("Send error occured:"+str(e) )
        pass

    def recv_bytes(self):
        try:
            mes_size = self.conn.recv(4)
            while len(mes_size)<4:
                mes_size += self.conn.recv( 4 - len(mes_size) )
            #print('plan size=',mes_size,end=' ')
            size = int( mes_size )
            res = b""
            while size > 0:
                res += self.conn.recv(size)
                #print('real size=',len(res), end = ' ' )
                size -= len(res)
            #print('recv bytes done')
            #print()
            return res

        except Exception as e:
            raise Exception("Recv error occured:"+str(e) )
        pass

    def send(self,arg):
        if isinstance(arg,str): arg = arg.encode()
        return self.send_bytes(arg)

    def recv(self,is_bytes=False):
        res = self.recv_bytes()
        if not is_bytes: res = res.decode()
        return res

    def encrypt_send(self,arg):
        arg = self.aes.encrypt(arg)
        return self.send_bytes(arg)

    def encrypt_recv(self,is_bytes=False):
        res = self.recv_bytes()
        res = self.aes.decrypt( res ) 
        if not is_bytes: res = res.decode()
        return res

    """
    response_type:encrypted(enforward and enbackward) noncrypt(forward and backward)
    """
    def contact_server(self,operation='say_hello',response_type='encrypted'):
        self.operation = operation
        self.response_type = response_type

        auth_info = ''
        if not self.operation in operation_auth_free:
            dprint("not a auth-free operation")
            auth_info = self.get_auth_info()
            dprint("auth_info = ",auth_info )
            if not auth_info:
                print("You are not connected to server. Operation %s is not allowed. Please connect or register."%(operation))
                exit(0)
                pass
            pass

        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM )
        # dealing with specified proxy
        dprint("start all proxy settings")
        if self.proxy:
            dprint("connect via proxy")
            sock.connect( (self.proxy[0], int(self.proxy[1]) ) )
            con_info = 'CONNECT %s:%s HTTP/1.0\r\n\r\n' % (self.server_addr, self.server_port)
            dprint("send connect info = ",con_info)
            sock.send( con_info )
            dprint("try recv")
            ans = sock.recv(4096)
            dprint("recv = ",ans)
        else:
            dprint("try connect %s %s"%(self.server_addr,self.server_port) )
            sock.connect( (self.server_addr, self.server_port) )
            pass
        dprint("done all proxy settings")

        self.conn = sock

        dprint("send hello")
        self.send("Hello EVAWIZ! You are great!")
        reply_info = self.recv() #recv 1
        dprint("first response = ",reply_info)

        #send aes key key by rsa key
        self.rsa_pub_key = self.recv()
        dprint("self.rsa_pub_key = ",self.rsa_pub_key)
        self.rsa = RSAEncryption( "client", self.rsa_pub_key )
        self.aes = AESEncryption()
        dprint("aes key = ",self.aes.get_key() )
        self.send(  self.rsa.encrypt( self.aes.get_key() ) )

        # send operation
        dprint('send infos')
        self.send(self.operation)
        self.encrypt_send(self.mac_addr)
        self.send(self.response_type)
        self.send(self.user_name)

        #send auth_info
        dprint("check operation if not need to auth operation = %s"%self.operation)
        if self.operation in operation_auth_free:
            return
        auth_info = self.get_auth_info()
        self.encrypt_send(auth_info)

        dprint(reply_info)

        #check authorization result
        ans = self.recv()
        dprint('auth returned',ans)
        if ans == 'auth passed':
            return True
        else:
            print('You are not connected to server. Operation',self.operation,'is not allowed.')
            raise Exception('trying operation not permited')

    def discontact_server(self):
        self.send_eof()
        
    # get local connection information
    def get_auth_info(self):
        evawiz_dir = os.getenv('HOME')+'/.evawiz'
        if not os.path.exists(evawiz_dir): os.system("mkdir "+evawiz_dir)
        auth_file = evawiz_dir+'/.user.auth'
        if os.path.exists(auth_file):
            os.system('chmod 600 '+auth_file)
        else:
            return ''
        hfile = open(auth_file)
        info = hfile.readline()
        hfile.close()
        os.system('chmod 000 '+auth_file)
        return info

    # write connection information
    def write_auth_info(self,auth_info=None):
        if len(auth_info)!=96: return None
        evawiz_dir= os.getenv('HOME')+'/.evawiz/'
        if not os.path.exists(evawiz_dir): os.system("mkdir "+evawiz_dir)
        auth_file = evawiz_dir + '/.user.auth'
        if os.path.exists(auth_file): os.system("chmod 600 "+auth_file)
        else: os.system("touch "+auth_file)
        file = open(auth_file,'w')
        file.write(auth_info)
        file.close()
        os.system("chmod 000 "+auth_file)
        return True

    # clear connection information
    def clear_auth_info(self):
        evawiz_dir= os.getenv('HOME')+'/.evawiz/'
        if not os.path.exists(evawiz_dir): return
        auth_file = evawiz_dir + '/.user.auth'
        if os.path.exists(auth_file): os.system("chmod 600 "+auth_file)
        os.system("rm "+auth_file)
        return True



    #get configurations
    def get_settings(self,setting_name='eva'):
        evawiz_dir= os.getenv('HOME')+'/.evawiz/'
        if not os.path.exists(evawiz_dir): return None
        setting_file = evawiz_dir+item+'.conf'
        hfile = open(setting_file)
        try:
            setting_txt = hfile.read()
        finally:
            hfile.close()
        return unpackdict(setting_txt)

    #write configurations
    def write_settings(self,set_dic={},setting_name='eva'):
        evawiz_dir= os.getenv('HOME')+'/.evawiz/'
        if not os.path.exists(evawiz_dir): os.system("mkdir "+evawiz_dir)
        setting_file = evawiz_dir+item+'.conf'
        hfile = open(setting_file,'w')
        try:
            hfile.write(packdict(set_dic))
        finally:
            hfile.close()
        return True

    def mod_request(self,req,*args):
        mod = importlib.import_module("mod_requests."+req)
        return mod.request(self,*args)
    
    pass


            
