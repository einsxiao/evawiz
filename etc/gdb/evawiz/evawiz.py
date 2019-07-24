#!/opt/evawiz/python/bin/python

##############################################################################
class StdStringPrinter(object):
    "Print a std::string"
    def __init__(self,val):
        self.val = val
    def to_string(self):
        return self.val['_M_dataplus']['_M_p']
    def display_hint(self):
        return "string"

##############################################################################
class StdVectorPrinter(object):
    "Print a std::vector"
    class _iterator(object):
        def __init__(self,val):
            #print 'try to print vector iterator'
            #self.valid = True
            #self.too_long = False
            self.counter = 0;
            self.begin = val['_M_impl']["_M_start"]
            self.end = val['_M_impl']["_M_finish"]
            return
        def __iter__(self):
            return self
        def next(self):
            if self.begin == self.end:
                raise StopIteration
            ret = ("[%d]"%self.counter, self.begin.dereference() )
            self.begin+=1
            self.counter+=1
            return ret
    def __init__(self,val):
        #print 'try to print vector'
        self.val = val
        self.begin = val['_M_impl']["_M_start"]
        self.end = val['_M_impl']["_M_finish"]
        self.end_storage = val['_M_impl']["_M_end_of_storage"]
        self.capacity = self.end_storage -self.begin;
        self.size = self.end -self.begin;
        self.valid = True
        if ( self.capacity < 0 or self.size < 0 ):
            self.valid = False
        self.too_long = False
        if ( self.size > 1000000 ):
            self.too_long = True

    def to_string(self):
        if ( not self.valid ): return '<data field not valid>'
        if ( self.too_long ): return '<data too large>'
        return "(size="+str(self.size)+",capacity="+str(self.capacity)+")"

    def children(self):
        if ( not self.valid ): return ''
        if ( self.too_long ): return ''
        return self._iterator(self.val)
                                
##########################
class IndexPrinter(object):
    "Print a evawiz::Index"
    def __init__(self,val):
        self.val = val
    def to_string(self):
        return str(self.val['second'])+'&'+str(self.val['first'])
    def display_hint(self):
        return "Index"


##########################
class ObjectPrinter(object):
    """Print a evawiz::object."""
    max_child =  100
    row_size = -1;

    def __init__(self,val):
        self.val = val;
        return
        
    def to_string(self):
        if ( self.val.address == 0 ): return "<invalid Object>";
        #return str( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->ToString()' ) );
        return  gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->ToString()' )['_M_dataplus']['_M_p'].string();
    def display_hint(self):
        return "Object"

import gdb.printing
def build_pretty_printers():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("evawiz")
    pp.add_printer('string',"^std::basic_string<char,.*>$",StdStringPrinter)
    pp.add_printer('vector',"^std::_Vector_base<.*>$",StdVectorPrinter)
    pp.add_printer('Index',"^evawiz::Index<.*>$",IndexPrinter)
    #pp.add_printer('Object',"^evawiz::Object<.*>$",ObjectPrinter)
    return pp


gdb.printing.register_pretty_printer( gdb.current_objfile(), build_pretty_printers() )
##########################################################################
"""
def look_up_type_Object(val):
    if str( val.type ) == 'evawiz::Object':
        return ObjectPrinter(val)
    else:
        return None

gdb.pretty_printers.append( look_up_type_Object )
"""
