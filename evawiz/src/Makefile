#compile setting
DCC := mpic++
DCC := nvcc
FLAGS := -std=c++11 
DFLAGS := -std=c++11 -G -g

DNVFLAGS := $(FLAGS) -G -g --compiler-options -g3  --compiler-options -Wall --compiler-options -Wextra -D DEBUG --compiler-options -Wno-unused-parameter --compiler-options -fopenmp  -Wno-deprecated-gpu-targets
NVFLAGS := $(FLAGS) -O2 --compiler-options -fopenmp  -Wno-deprecated-gpu-targets

##################################################################################
# core should be compiled ahead, evacc is depeneded on core(libeva.so)
DIRS := 
DIRS := core evacc Algebraic System Arithmatics List Matrix OS Class MPI MatrixOperations String Python
#envDIRS := Guis Mathematica MatlabEngine
DIRS := $(DIRS) $(envDIRS)
SHELL := /bin/bash
##################################################################################
BUILDDIRS = $(DIRS:%=build-%)
INSTALLDIRS = $(DIRS:%=install-%)
CLEANDIRS = $(DIRS:%=clean-%)

SRCS = $(wildcard *.cpp) 
OBJECTS = $(patsubst %.cpp, %.o, $(SRCS))
DOBJECTS = $(patsubst %.cpp, %.dev.o, $(SRCS))

INCLUDE = `eva-path tcpp` 
INCLUDE = `eva-path tcu` 
SHELL := /bin/bash
MAKE = make
OUT = evawiz
DOUT = evawiz-dev

debug: debug-prepare DALL
debug-prepare:
	@echo "Debug Version"
	$(eval MODE = debug )
release: prepare ALL
prepare:
	@echo "Release Version"
	$(eval MODE = release )

ALL: $(OUT)
	rmcp -f $(OUT) ../bin/
	rmcp -f $(OUT) ../../bin/
	rmcp -f $(DOUT) ../bin/
	rmcp -f $(DOUT) ../../bin/
	rmcp -f ../Template/Makefile ../Template.cuda/
	rmcp -f ../Template/Makefile ../Template.fortran/
	#rm  ../lib/*-dev.so
$(OUT):	$(BUILDDIRS) $(OBJECTS) 
#	$(DCC) $(FLAGS) -o $@ $(OBJECTS) $(INCLUDE) -leva -lcudart -lmpi -lmpi_cxx
	$(DCC) $(NVFLAGS) -o $@ $(OBJECTS) $(INCLUDE) -leva
%.o: %.cpp
#	$(DCC) $(FLAGS) -c -fPIC $< -o $@ $(INCLUDE)
	$(DCC) $(NVFLAGS) -Xcompiler -fPIC -c  $< -o $@ $(INCLUDE)

DALL: $(DOUT)
$(DOUT):	$(BUILDDIRS) $(DOBJECTS) 
	#mpic++ $(FLAGS)  -o $@ $(DOBJECTS) $(INCLUDE) -leva-dev -lcudart -lmpi -lmpi_cxx
#	$(DCC) $(FLAGS)  -o $@ $(DOBJECTS) $(INCLUDE) -leva-dev -lcudart -lmpi -lmpi_cxx
	$(DCC) $(NVFLAGS)  -o $@ $(DOBJECTS) $(INCLUDE) -leva-dev 
%.dev.o: %.cpp
#	$(DCC) $(FLAGS) -c -fPIC $< -o $@ $(INCLUDE)
	$(DCC) $(NVFLAGS) -Xcompiler -fPIC -c $< -o $@ $(INCLUDE)

$(BUILDDIRS):
	@echo "Dealing dir $(@:build-%=%)"
	#@cp $(@:build-%=%)/*.h ../include/
	-$(MAKE) -C $(@:build-%=%) $(MODE)
	#@cp $(@:build-%=%)/*.so ../lib/
install: 
	rmcp -f $(OUT) ../bin/
	rmcp -f $(OUT) ../../bin/
clean: $(CLEANDIRS)
	rm *.o;rm $(OUT)
$(CLEANDIRS): 
	-$(MAKE) -C $(@:clean-%=%) clean
$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install
