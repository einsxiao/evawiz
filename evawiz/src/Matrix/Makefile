#Created from stadard compile template of Evawiz
#Current Module:Template
################################################################
DEPENDED_MODULES := 
INCLUDE :=
LIBS := 
CULIBS := -lcurand
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
FLAGS := -std=c++11 -O2 --compiler-options -Wno-unused-function --compiler-options -Wno-sign-compare
DFLAGS := -std=c++11 -g -G -D DEBUG --compiler-options -g3  --compiler-options -Wall --compiler-options -Wextra --compiler-options -Wno-unused --compiler-options -Wno-unused-parameter  --compiler-options -fno-inline-functions --compiler-options -Wno-unused-function --compiler-options -Wno-sign-compare
FORFLAGS := -O2
DFORFLAGS := -g3 -D DEBUG -Wall -Wextra  -Wno-unused -Wno-unused-parameter 
################################################################
MODULE_NAME := $(notdir $(CURDIR))Module
EXCU := exe
OS = $(EVAWIZ_OS_TYPE)
SHELL := /bin/bash

CUINCLUDE:=$(INCLUDE) `eva-path tcu $(DEPENDED_MODULES)` 
INCLUDE:=$(INCLUDE) `eva-path tcpp $(DEPENDED_MODULES)` 

SRCS := $(wildcard *.cpp)
CSRCS := $(wildcard *.c)
CUSRCS := $(wildcard *.cu)
FORSRCS := $(wildcard *.f90)
HEADERS := $(wildcard *.h) $(EVAWIZ_ROOT)/evawiz/include/evawiz.h

DOBJECTS := $(patsubst %.cpp, build/debug/%.tcpp.o, $(SRCS))
DCOBJECTS := $(patsubst %.c, build/debug/%.tc.o, $(CSRCS))
DCUOBJECTS := $(patsubst %.cu, build/debug/%.tcu.o, $(CUSRCS))
DCUCPPOBJECTS := $(patsubst %.cu, build/debug/%.tcu.o.tcpp.o, $(CUSRCS))
DFOROBJECTS := $(patsubst %.f90, build/debug/%.tf90.o, $(FORSRCS))
DALLCPPOBJECTS := $(DOBJECTS) $(DCOBJECTS) $(DCUCPPOBJECTS) $(DFOROBJECTS)
DALLCUOBJECTS := $(DOBJECTS) $(DCOBJECTS) $(DCUOBJECTS) $(DFOROBJECTS)

OBJECTS := $(patsubst %.cpp, build/release/%.tcpp.o, $(SRCS))
COBJECTS := $(patsubst %.c, build/release/%.tc.o, $(CSRCS))
CUOBJECTS := $(patsubst %.cu, build/release/%.tcu.o, $(CUSRCS))
CUCPPOBJECTS := $(patsubst %.cu, build/release/%.tcu.o.tcpp.o, $(CUSRCS))
FOROBJECTS := $(patsubst %.f90, build/release/%.tf90.o, $(FORSRCS))
ALLCPPOBJECTS := $(OBJECTS) $(COBJECTS) $(CUCPPOBJECTS) $(FOROBJECTS)
ALLCUOBJECTS := $(OBJECTS) $(COBJECTS) $(CUOBJECTS) $(FOROBJECTS)

DCUDAVERSION := 
CUDAVERSION := 
ifdef CUOBJECTS
	DCUDAVERSION := DCUALL
	CUDAVERSION := CUALL
endif

ifeq ($(MPI_ENABLE),True)
	LIBS := $(LIBS) -lmpi
	FORCC := $(MPIFORCC)
endif

OSFIX :=
ifeq ($(OS),GNU/Linux)
	FLAGS := $(FLAGS) -DOS_LINUX
	DFLAGS := $(DFLAGS) -DOS_LINUX
endif
ifeq ($(OS),Cygwin)
	OSFIX :=WIN
	FLAGS := $(FLAGS) -DOS_WINDOWS -U__STRICT_ANSI__
	DFLAGS := $(DFLAGS) -DOS_WINDOWS -U__STRICT_ANSI__
	DCUDAVERSION := 
	CUDAVERSION := 
endif

#################################################################
################################################################
normal: debug

all: debug release

install: $(RELEASE) upload

debug: all-prepare DCPPALL$(OSFIX) $(DCUDAVERSION) post-work

release: all-prepare CPPALL$(OSFIX) $(CUDAVERSION) post-work

########################################################################################################
DCUALL: $(DALLCUOBJECTS)
	$(CC) $(DFLAGS) -shared -o lib$(MODULE_NAME)_cuda-dev.so $(DALLCUOBJECTS) $(CUINCLUDE)  $(LIBS) $(CULIBS)
	$(CC) $(DFLAGS) -o $(EXCU)_cuda $(DALLCUOBJECTS) $(CUINCLUDE) $(LIBS) -leva
DCPPALL: $(DALLCPPOBJECTS)
	$(CC) $(DFLAGS) -fc++ -shared -o lib$(MODULE_NAME)-dev.so $(DALLCPPOBJECTS) $(INCLUDE)  $(LIBS) 
	$(CC) $(DFLAGS) -fc++ -o $(EXCU) $(DALLCPPOBJECTS) $(INCLUDE) $(LIBS) -leva
DCPPALLWIN: $(DALLCPPOBJECTS)
	$(CC) $(FLAGS) -fc++ -shared -o lib$(MODULE_NAME).dll $(DALLCPPOBJECTS) $(INCLUDE)  $(LIBS) -leva
	$(CC) $(DFLAGS) -fc++ -o $(EXCU) $(DALLCPPOBJECTS) $(INCLUDE) $(LIBS) -leva
build/debug/%.tcu.o: %.cu $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(CUINCLUDE)
build/debug/%.tcu.o.tcpp.o: %.cu $(HEADERS)
	$(CC) $(DFLAGS) -fc++ -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tcpp.o: %.cpp $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tc.o: %.c $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tf90.o: %.f90 $(HEADERS)
	$(FORCC) $(DFORFLAGS) -fPIC -c $< -o $@ -I. $(INCLUDE)

#########################################################################################################
CUALL: $(ALLCUOBJECTS)
	$(CC) $(FLAGS) -shared -o lib$(MODULE_NAME)_cuda.so $(ALLCUOBJECTS) $(CUINCLUDE)  $(LIBS) $(CULIBS)
	$(shell if [ ! -d $(HOME_EVAWIZ)/.temp ]; then mkdir $(HOME_EVAWIZ)/.temp; fi; rmcp  lib$(MODULE_NAME)_cuda.so $(HOME_EVAWIZ)/.temp/  )
CPPALL: $(ALLCPPOBJECTS)
	$(CC) $(FLAGS) -fc++ -shared -o lib$(MODULE_NAME).so $(ALLCPPOBJECTS) $(INCLUDE)  $(LIBS) 
	$(shell if [ ! -d $(HOME_EVAWIZ)/.temp ]; then mkdir $(HOME_EVAWIZ)/.temp; fi; rmcp  lib$(MODULE_NAME).so $(HOME_EVAWIZ)/.temp/  )
CPPALLWIN: $(ALLCPPOBJECTS)
	$(CC) $(FLAGS) -fc++ -shared -o lib$(MODULE_NAME).dll $(ALLCPPOBJECTS) $(INCLUDE)  $(LIBS)  -leva
	$(shell if [ ! -d $(HOME_EVAWIZ)/.temp ]; then mkdir $(HOME_EVAWIZ)/.temp; fi; rmcp  lib$(MODULE_NAME).dll $(HOME_EVAWIZ)/.temp/  )
build/release/%.tcu.o: %.cu $(HEADERS)
	$(CC) $(FLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(CUINCLUDE)
build/release/%.tcu.o.tcpp.o: %.cu $(HEADERS)
	$(CC) $(FLAGS) -fc++ -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/release/%.tcpp.o: %.cpp $(HEADERS)
	$(CC) $(FLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/release/%.tc.o: %.c $(HEADERS)
	$(CC) $(FLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/release/%.tf90.o: %.f90 $(HEADERS)
	$(FORCC) $(FORFLAGS) -fPIC -c $< -o $@ -I. $(INCLUDE)

#########################################################################################################
clean:
	@echo "clean up *.o, *.so, *.dll, exe ..."
	@rm -rf build/*;
	@if [ -f exe ]; then rm exe; fi
	@if [ -f lib$(MODULE_NAME).so ]; then rm lib$(MODULE_NAME).so; fi
	@if [ -f lib$(MODULE_NAME)-dev.so ]; then rm lib$(MODULE_NAME)-dev.so; fi
	@if [ -f lib$(MODULE_NAME).dll ]; then rm lib$(MODULE_NAME).dll; fi

#####################################################################################

all-prepare: 
	@echo "current os is " $(OS)
	@if [ "x`eva-path evacc $(DEPENDED_MODULES)`" == "x" ]; \
		then echo "Error: Dependency failed.";\
		exit 2;\
		else echo "$(DEPENDED_MODULES)">.evadepend;\
		fi 
	$(shell if [ -f build ]; then rm build; mkdir build; elif ! [ -d build ]; then mkdir build; fi; )
	$(shell if [ -f build/debug ]; then rm build/debug; mkdir build/debug; elif ! [ -d build/debug ]; then mkdir build/debug; fi; )
	$(shell if [ -f build/release ]; then rm build/release; mkdir build/release; elif ! [ -d build/release ]; then mkdir build/release; fi; )
	@rmcp *.h ../../include/
post-work:
	@rmcp *.so ../../lib/
	@rmcp *.dll ../../lib/

upload:
	@echo "Upload to Evawiz server"
	$(shell eva upload)
