#

##############################################################################
class StdStringPrinter(object):
    "Print a std::string"
    def __init__(self,val):
        self.val = val

    def to_string(self):
        return self.val['_M_dataplus']['_M_p']

    def display_hint(self):
        return "std::string"

##############################################################################
class StdVectorPrinter(object):
    "Print a std::vector"
    class _iterator(object):
        def __init__(self,val):
            #print('try to print vector iterator')
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

    def display_hint(self):
        return "std::vector"

                                
##########################
class IndexPrinter(object):
    "Print a evawiz::Index"
    def __init__(self,val):
        #print('try print index')
        self.val = val
        pass
    def to_string(self):
        return str(self.val['second'])+'&'+str(self.val['first'])
    def display_hint(self):
        return "evawiz::Index"


# ##########################
class ObjectPrinter(object):
    """Print a evawiz::Object."""
    max_child =  100

    class _iterator(object):
        def __init__(self,val):
            self.counter = 0;
            self.begin = val['_M_impl']["_M_start"]
            self.end = val['_M_impl']["_M_finish"]
        def __iter__(self):
            return self
        def next(self):
            if self.begin == self.end:
                raise StopIteration
            ret = ("{0}".format(self.counter),self.begin.dereference() )
            self.begin+=1
            self.counter+=1
            return ret

    def __init__(self,val):
        self.val = val;
        pass
        
    def to_string(self):
        if self.val:
            #self._str = str( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->ToString()' ) );
            #print(self._objid)
            self._type = str( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->type()' ) );
            self._cnt = str( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->cnt()' ) );

            if self._type == "evawiz::EvaType::Number":
                self._val = str( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->Number()' ) );
                return "Number({0})={1}".format(self._cnt, self._val )

            if self._type == "evawiz::EvaType::String":
                self._val = str( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->Key()' ) );
                return "String({0})={1}".format(self._cnt, self._val )

            if self._type == "evawiz::EvaType::Symbol":
                self._val = str(gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->Key()' ) );
                return "Symbol({0})={1}".format(self._cnt, self._val )

            if self._type == "evawiz::EvaType::List":
                self._size = gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->ElementsSize()' );
                self._idx_second = int( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->idx().second' ) );
                self._idx_first = int( gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->idx().first' ) );
                if self._size == 0: return "(List {0}&{1})".format(self._idx_second,self._idx_first)
                self._list = gdb.parse_and_eval( ' __GlobalPool<0>::pool.Lists.objs[{0}][{1}]'.format(self._idx_second,self._idx_first) )
                self._start = self._list['_M_impl']['_M_start']
                self._end = self._list['_M_impl']['_M_finish']
                return "List({0},len={1})".format(self._cnt,self._size ,self._start,self._end)

            return "evawiz::UnknownObject"
        #return  gdb.parse_and_eval( '((evawiz::Object*)'+str( self.val.address )+')->ToString()' )['_M_dataplus']['_M_p'].string();
        return "evawiz::NullObject"

    def children(self):
        if self._type != "evawiz::EvaType::List": return ''
        return self._iterator(self._list)

    def display_hint(self):
        return "evawiz::Object"

            

import gdb.printing
def build_pretty_printers():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("evawiz")
    #pp.add_printer('string',"^std::.*basic_string<char,.*>$",StdStringPrinter)
    #pp.add_printer('vector',"^std::_Vector_base<.*>$",StdVectorPrinter)
    pp.add_printer('Index',"^evawiz::Index$",IndexPrinter)
    pp.add_printer('Object',"^evawiz::Object$",ObjectPrinter)
    return pp


gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printers()
)
##########################################################################
"""
def look_up_type_Object(val):
    if str( val.type ) == 'evawiz::Object':
        return ObjectPrinter(val)
    else:
        return None

gdb.pretty_printers.append( look_up_type_Object )
"""
