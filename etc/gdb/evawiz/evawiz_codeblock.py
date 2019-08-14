#!/opt/evawiz/python/bin/python

def dprint(*params):
    print "debug_info: "
    for i in params:
        print i,
        print

class StdStringPrinter(object):
    "Print a std::string"

    def __init__(self,val):
        self.val = val

    def to_string(self):
        return self.val['_M_dataplus']['_M_p']

    def display_hint(self):
        return "string"

                                
##############################################################################
        
class EvaObjPrinter:
    """Print a EvaObj object."""
    max_child =  100

    def __init__(self,val,is_detail = True):
        self.is_detail = is_detail
        self.val = val;
        self.this = val.address
        self.rtype = str(val['typ'])
        self.rstate= int(val['state'])
        self.rv= int(val['v'])
        self.rv1= val['v1']
        self.rv2= val['v2']
        self.rre=self.rv1['re']
        self.rim=self.rv2['im']
        #self.rkey="" rkey set afterwords key may be null
        self.relements=self.rv2['elements']
        ### for elements
    def prepare_content(self,count=0):
        self.elements = self.relements.referenced_value();
        self.begin = self.elements['_M_impl']["_M_start"]
        self.end = self.elements['_M_impl']["_M_finish"]
        self.end_storage = self.elements['_M_impl']["_M_end_of_storage"]
        self.count = self.end - self.begin
        self.capacity = self.end_storage - self.begin
        if self.count == 0:
            return self.rkey
        obj = EvaObjPrinter( self.begin.dereference(), False )
        res = ""
        if self.is_detail:
            res =  self.rkey+"{\n  "+obj.to_string()
        else:
            res = "["+str(count)+"]"+self.rkey+"{"+obj.to_string()
        counter = 1
        self.begin+=1
        while self.begin != self.end:
            obj = EvaObjPrinter( self.begin.dereference(), False )
            if self.is_detail:
                # if counter != 1:
                #     res+=",\n"
                # else:pass
                res += "    "+str(counter)+":"+obj.to_string()
                #res +=",\n    "+str(counter)+":"+obj.to_string(counter)
                #res +="    "+obj.to_string(counter)
            else:
                # if counter != 1: res+=","
                # else:pass
                # res += obj.to_string()
                res+=","+obj.to_string(counter)
            counter+=1
            if counter > self.max_child: break
            self.begin+=1
        if counter < self.count:
            res+=",\n...with extra "+str(self.count - counter)+" elements"
        if self.is_detail:
            res+="\n}"
        else:
            res+="}"
        return res


    def to_string(self,count=0):
        s="";
        if ( self.rstate !=0 or self.rv != 0 ):
            s=" ,{state="+str(self.rstate)+", v="+str(self.rv)+"}";

        if ( self.rtype == "evawiz::NumberT" ):
            return str(self.rre)

        if ( self.rtype == "evawiz::ComplexT" ):
            return "("+str( self.rre )+","+str( self.rim)+")"

        if ( self.rtype == "evawiz::StringT" ):
            if ( self.rre ): self.rkey=self.rv1['key'].string();
            return '"'+self.rkey+'"'

        if ( self.rtype == "evawiz::SymbolT" ):
            if ( self.rre ): self.rkey=self.rv1['key'].string();
            res = ""
            #if self.this and self.is_detail :
            #    res = "EvaObj@"+str(self.this)+":";
            if ( self.relements ):
                res = res+" "+self.prepare_content(count)
            else:
                res = " "+self.rkey
            res+=s;

            return res

        if ( self.rtype == "evawiz::ReferenceT" ):
            print ">>>deal ref<<<",
            self.rlref=self.rv1['lref']
            self.rrref=self.rv2['rref']
            refobj = EvaObjPrinter( self.rrref.referenced_value() );
            res = "-->" + refobj.to_string();
            return res

        if ( self.rtype == "evawiz::VariableT" ):
            return "$_"+int(self.rre)+s

        return "{ v1={re="+str(self.rre)+", key="+self.rkey+"},"+"v2={im="+ \
            str(self.rim)+", ptr="+str(self.rrref)+"}}"

    def display_hint(self):
        return "EvaObj"


##########################################################################
##########################################################################
# def lookup_function(val):
#     lookup_tag = val.type.tag
#     if lookup_tag == None:
#         return None
#     regexs = (
#         (re.compile("^std::basic_string<char,.*>$"),StdStringPrinter),
#     )
#     for (regex,printer) in regexs:
#         print ( regex, printer)
#         if ( regex.match(lookup_tag) ):
#             return printer(val)
#    return None

import gdb.printing
def build_pretty_printers():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("evawiz")
    pp.add_printer('string',"^std::basic_string<char,.*>$",StdStringPrinter)
    pp.add_printer('EvaObj',"^evawiz::EvaObj$",EvaObjPrinter)
    return pp

gdb.printing.register_pretty_printer( gdb.current_objfile(), build_pretty_printers() )
##########################################################################
