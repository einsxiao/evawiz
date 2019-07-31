#Created from stadard compile template of Evawiz
#Current Module:Template
################################################################
DEPENDED_MODULES := fftw3
INCLUDE := 
LIBS :=  -lMatrixModule -lMPIModule -lcufft -lfftw3f_mpi -lfftw3_mpi -lfftw3f_threads -lfftw3_threads -lfftw3f -lfftw3 -lmpi_cxx
SUBDIRS :=
MPI_ENABLE := True
################################################################
################################################################
CC := evacc
FORCC := gfortran
MPIFORCC := mpifort
################################################################
MAKE := make
################################################################
FLAGS := -std=c++11 -O2 --compiler-options -Wno-sign-compare
DFLAGS := -std=c++11 -g -G -D DEBUG --compiler-options -g3  --compiler-options -Wall --compiler-options -Wextra --compiler-options -Wno-unused --compiler-options -Wno-unused-parameter  --compiler-options -fno-inline-functions --compiler-options -Wno-sign-compare
RawFLAGS := -std=c++11 -O2 -fopenmp
RawDFLAGS := -std=c++11 -D DEBUG -g3   -Wall  -Wextra  -Wno-unused -Wno-unused-parameter -fno-inline-functions -fopenmp
FORFLAGS := -O2
DFORFLAGS := -g3 -D DEBUG -Wall -Wextra  -Wno-unused -Wno-unused-parameter 
################################################################
MODULE_NAME := $(notdir $(CURDIR))Module
EXCU := exe
SHELL := /bin/bash

ifeq ($(MPI_ENABLE),True)
	LIBS := $(LIBS) -lmpi
	FORCC := $(MPIFORCC)
endif

INCLUDE:=$(INCLUDE) `eva-path evacc $(DEPENDED_MODULES)` 

SRCS := $(wildcard *.cpp)
CSRCS := $(wildcard *.c)
CUSRCS := $(wildcard *.cu)
FORSRCS := $(wildcard *.f90)
HEADERS := $(wildcard *.h) $(EVAWIZ_ROOT)/evawiz/include/evawiz

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

#################################################################
################################################################
in-all: debug

all: release debug

install: release debug
	@cp *.h ~/evawiz/include
	@cp *.so ~/evawiz/lib

debug: all-prepare DALL post-work

release: all-prepare ALL post-work


########################################################################################################
DALL: dexe debugsharelib

debugsharelib: $(DALLOBJECTS)
	$(CC) $(DFLAGS) -shared -o lib$(MODULE_NAME)-dev.so $(DALLOBJECTS) $(INCLUDE)  $(LIBS)
	$(shell if [ ! -d $(HOME_EVAWIZ)/.temp ]; then mkdir $(HOME_EVAWIZ)/.temp; fi; rmcp  lib$(MODULE_NAME)-dev.so $(HOME_EVAWIZ)/.temp/  )
build/debug/%.tcu.o: %.cu $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tcpp.o: %.cpp $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tc.o: %.c $(HEADERS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)
build/debug/%.tf90.o: %.f90 $(HEADERS)
	$(FORCC) $(DFORFLAGS) -fPIC -c $< -o $@ -I. $(INCLUDE)

#########################################################################################################
ALL: sharelib

sharelib: $(ALLOBJECTS)
	$(CC) $(FLAGS) -shared -o lib$(MODULE_NAME).so $(ALLOBJECTS) $(INCLUDE)  $(LIBS) 
	$(shell if [ ! -d $(HOME_EVAWIZ)/.temp ]; then mkdir $(HOME_EVAWIZ)/.temp; fi; rmcp  lib$(MODULE_NAME).so $(HOME_EVAWIZ)/.temp/  )
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
dexe: $(DALLOBJECTS)
ifeq ($(MPI_ENABLE),True)
ifndef CUSRCS
	mpic++ $(RawDFLAGS) -o $(EXCU) $(DALLOBJECTS) $(INCLUDE) $(LIBS) -leva 
else
	$(CC) $(DFLAGS) -o $(EXCU) $(DALLOBJECTS) $(INCLUDE) $(LIBS) -leva 
endif
else
	$(CC) $(DFLAGS) -o $(EXCU) $(DALLOBJECTS) $(INCLUDE) $(LIBS) -leva 
endif

all-prepare: 
	@rmcp *.h ~/evawiz/include/
	@if [ "x`evapath $(DEPENDED_MODULES)`" == "x" ]; \
		then echo "Error: Dependency failed.";\
		exit 2;\
		else echo "$(DEPENDED_MODULES)">.evadepend;\
		fi 
	$(shell if [ -f build ]; then rm build; mkdir build; elif ! [ -d build ]; then mkdir build; fi; )
	$(shell if [ -f build/debug ]; then rm build/debug; mkdir build/debug; elif ! [ -d build/debug ]; then mkdir build/debug; fi; )
	$(shell if [ -f build/release ]; then rm build/release; mkdir build/release; elif ! [ -d build/release ]; then mkdir build/release; fi; )
post-work:
	@rmcp *.so ~/evawiz/lib/
	@rmcp *.dll ~/evawiz/lib/

upload:
	@echo "Upload to Evawiz server"
	$(shell eva upload)
