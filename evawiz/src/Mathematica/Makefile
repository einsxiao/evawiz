#Created from stadard compile template of Evawiz
#Current Module:Mathematica
################################################################
DEPENDED_MODULES := 
INCLUDE := 
LIBS :=  -L${CADDSDIR} -lML64i3 -lm -lrt
LIBS :=  -L${CADDSDIR} -lML64i3 
LIBS :=  -lML64i3 
SUBDIRS :=
#MPI_ENABLE := True
################################################################
################################################################
CC := evacc
FORCC := gfortran
MPIFORCC := mpifort
################################################################
MAKE := make
################################################################
FLAGS := -std=c++11 -O2
DFLAGS := -std=c++11 -g -G -D DEBUG --compiler-options -g  --compiler-options -Wall --compiler-options -Wextra --compiler-options -Wno-unused --compiler-options -Wno-unused-parameter  --compiler-options -fno-inline-functions
FORFLAGS := -O2
DFORFLAGS := -g -D DEBUG -Wall -Wextra  -Wno-unused -Wno-unused-parameter 
################################################################
MODULE_NAME := $(notdir $(CURDIR))Module
CUDAVERSION := $(wildcard *.cu)
ifdef CUDAVERSION
OBJFIX:=tcu
else
OBJFIX:=tcpp
endif
INCLUDE := $(INCLUDE) `eva-path evacc $(DEPENDED_MODULES)` 
RPATH := `evarpath tcu $(DEPENDED_MODULES)`
EXCU := exe

ifeq ($(MPI_ENABLE),True)
	LIBS := $(LIBS) -lmpi
	FORCC := $(MPIFORCC)
endif

SRCS := $(wildcard *.cpp)
CSRCS := $(wildcard *.c)
CUSRCS := $(wildcard *.cu)
FORSRCS := $(wildcard *.f90)
HEADERS := $(wildcard *.h) /opt/evawiz/evawiz/include/evawiz

DOBJECTS := $(patsubst %.cpp, build/debug/%.tcpp.o, $(SRCS))
DCOBJECTS := $(patsubst %.c, build/debug/%.tc.o, $(CSRCS))
DCUOBJECTS := $(patsubst %.cu, build/debug/%.tcu.o, $(CUSRCS))
DFOROBJECTS := $(patsubst %.f90, build/debug/%.tf90.o, $(FORSRCS))
DALLOBJECTS := $(DOBJECTS) $(DCOBJECTS) $(DCUOBJECTS) $(DFOROBJECTS)

OBJECTS := $(patsubst %.cpp, build/release/%.tcpp.o, $(SRCS))
COBJECTS := $(patsubst %.c, build/release/%.tc.o, $(CSRCS))
CUOBJECTS := $(patsubst %.cu, build/release/%.tcu.o, $(CUSRCS))
FOROBJECTS := $(patsubst %.f90, build/release/%.tf90.o, $(FORSRCS))
ALLOBJECTS := $(OBJECTS) $(COBJECTS) $(CUOBJECTS) $(FOROBJECTS)
COUNT := 0

#################################################################
################################################################
in-all: debug

all: release debug

install: release debug

debug: all-prepare DALL post-work

release: all-prepare ALL post-work


########################################################################################################
DALL: dexe$(OBJFIX) debugsharelib$(OBJFIX)

debugsharelibtcu: $(DALLOBJECTS)
	$(CC) $(DFLAGS) $(RPATH) -shared -o lib$(MODULE_NAME)-dev.so $(DALLOBJECTS) $(INCLUDE)  $(LIBS)
	@chmod -x lib$(MODULE_NAME)-dev.so
debugsharelibtcpp: $(DALLOBJECTS)
	$(CC) $(DFLAGS) $(RPATH) -shared -o lib$(MODULE_NAME)-dev.so $(DALLOBJECTS) $(INCLUDE)  $(LIBS)
	@chmod -x `pwd`/lib$(MODULE_NAME)-dev.so
build/debug/%.tcu.o: %.cu $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tcpp.o: %.cpp $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tc.o: %.c $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tf90.o: %.f90 $(HEADERS)
	$(FORCC) $(DFORFLAGS) -fPIC -c $< -o $@ -I. $(INCLUDE)

#########################################################################################################
ALL: sharelib$(OBJFIX)

sharelibtcu: $(ALLOBJECTS)
	$(CC) $(FLAGS) $(RPATH) -shared -o lib$(MODULE_NAME).so $(ALLOBJECTS) $(INCLUDE)  $(LIBS) 
	@chmod -x lib$(MODULE_NAME).so
sharelibtcpp: $(ALLOBJECTS)
	$(CC) $(FLAGS) $(RPATH) -shared -o lib$(MODULE_NAME).so $(ALLOBJECTS) $(INCLUDE)  $(LIBS) 
	@chmod -x lib$(MODULE_NAME).so
build/release/%.tcu.o: %.cu $(HEADERS)
	$(CC) $(FLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/release/%.tcpp.o: %.cpp $(HEADERS)
	$(CC) $(FLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/release/%.tc.o: %.c $(HEADERS)
	$(CC) $(FLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/release/%.tf90.o: %.f90 $(HEADERS)
	$(FORCC) $(FORFLAGS) -fPIC -c $< -o $@ -I. $(INCLUDE)

#########################################################################################################
clean:
	@make -i cleandebug
	@make -i cleanrelease

cleandebug:
	@rm -rf build/debug
	@rm exe;
	rm lib$(MODULE_NAME)-dev.so;

cleanrelease:
	rm -rf build/release;
	rm lib$(MODULE_NAME).so;

#####################################################################################
dexetcpp: $(DALLOBJECTS)
	$(CC) $(DFLAGS) -o $(EXCU) $(DALLOBJECTS) $(INCLUDE) $(LIBS) -leva  
dexetcu: $(DALLOBJECTS)
	$(CC) $(DFLAGS) -o $(EXCU) $(DALLOBJECTS) $(INCLUDE) $(LIBS) -leva  

all-prepare: 
	@rmcp *.h ~/evawiz/include
	@if [ "x`evapath $(DEPENDED_MODULES)`" == "x" ]; \
		then echo "Error: Dependency failed.";\
	exit 2;\
		else echo "$(DEPENDED_MODULES)">.evadepend;\
	fi 
	$(shell if [ -f build ]; then rm build; mkdir build; elif ! [ -d build ]; then mkdir build; fi; )
	$(shell if [ -f build/debug ]; then rm build/debug; mkdir build/debug; elif ! [ -d build/debug ]; then mkdir build/debug; fi; )
	$(shell if [ -f build/release ]; then rm build/release; mkdir build/release; elif ! [ -d build/release ]; then mkdir build/release; fi; )
#	@${TNUM}=${#ALLOBJECTS[@]}
	$(shell count=${#ALLOBJECTS[@]} )
	$(shell echo ${count})
post-work:
	@rmcp *.h ~/evawiz/include
	@rmcp *.so ~/evawiz/lib

upload:
	@echo "Upload to Evawiz server"
	$(shell eva upload)
