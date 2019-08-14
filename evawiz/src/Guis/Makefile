#Created from stadard compile template of Evawiz
#Current Module:Guis
################################################################
DEPENDED_MODULES := Qt-4.8
INCLUDE :=
LIBS :=  -lQtGui
SUBDIRS :=
#MPI_ENABLE := True
MOCFILES := runmod.h notepad.h 
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
RawFLAGS := -std=c++11 -O2 -fopenmp
RawDFLAGS := -std=c++11 -D DEBUG -g   -Wall  -Wextra  -Wno-unused -Wno-unused-parameter -fno-inline-functions -fopenmp
FORFLAGS := -O2
DFORFLAGS := -g -D DEBUG -Wall -Wextra  -Wno-unused -Wno-unused-parameter 
################################################################
MODULE_NAME := $(notdir $(CURDIR))Module
EXCU := exe

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

MOCOBJECTS := $(patsubst %.h, %.moc.o, $(MOCFILES))

DOBJECTS := $(patsubst %.cpp, build/debug/%.tcpp.o, $(SRCS))
DALLOBJECTS := $(DOBJECTS) $(DCOBJECTS) $(DCUOBJECTS) $(DFOROBJECTS)

OBJECTS := $(patsubst %.cpp, build/release/%.tcpp.o, $(SRCS))
ALLOBJECTS := $(OBJECTS) $(COBJECTS) $(CUOBJECTS) $(FOROBJECTS)

#########
#########
in-all: debug

all: release debug 

install: release upload

debug: all-prepare DALL post-work

release: all-prepare ALL post-work


##########
DALL: debugsharelib dexe 

debugsharelib: $(DALLOBJECTS) $(MOCOBJECTS)
	$(CC) $(DFLAGS) -shared -o lib$(MODULE_NAME)-dev.so $(DALLOBJECTS) $(INCLUDE)  $(LIBS)
	$(shell if [ ! -d $(HOME_EVAWIZ)/.temp ]; then mkdir $(HOME_EVAWIZ)/.temp; fi; rmcp  lib$(MODULE_NAME)-dev.so $(HOME_EVAWIZ)/.temp/  )
build/debug/%.tcpp.o: %.cpp $(HEADERS)  $(MOCOBJECTS)
	$(CC) $(DFLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)

###########
ALL: sharelib

sharelib: $(ALLOBJECTS) $(MOCOBJECTS)
	$(CC) $(FLAGS) -shared -o lib$(MODULE_NAME).so $(ALLOBJECTS) $(INCLUDE)  $(LIBS) 
	$(shell if [ ! -d $(HOME_EVAWIZ)/.temp ]; then mkdir $(HOME_EVAWIZ)/.temp; fi; rmcp  lib$(MODULE_NAME).so $(HOME_EVAWIZ)/.temp/  )
build/release/%.tcpp.o: %.cpp $(HEADERS) $(MOCOBJECTS)
	$(CC) $(FLAGS) -Xcompiler -fPIC -c $< -o $@ -I. $(INCLUDE)

############
%.moc.o: %.h 
	moc $< > $<.moc

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
	$(CC) $(DFLAGS) -o $(EXCU) $(DALLOBJECTS) $(INCLUDE) $(LIBS) -leva 

all-prepare: 
	@rmcp *.h ~/evawiz/include/
	@echo "moc files: $(MOCFILES)"
	@if [ "x`eva-path evacc $(DEPENDED_MODULES)`" == "x" ]; \
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
