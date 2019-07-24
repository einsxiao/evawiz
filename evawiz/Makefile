#compile setting
##################################################################################
DIRS = src
BUILDDIRS = $(DIRS:%=build-%)
INSTALLDIRS = $(DIRS:%=install-%)
CLEANDIRS = $(DIRS:%=clean-%)
SRCS = $(wildcard *.cpp) 
debug: debug-prepare ALL
debug-prepare:
	@echo "Debug Version"
	$(eval MODE = debug )
release: prepare ALL
prepare:
	@echo "Release Version"
	$(eval MODE = release )
ALL: $(BUILDDIRS) 

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%) $(MODE)
clean: $(CLEANDIRS)

$(CLEANDIRS): 
	$(MAKE) -C $(@:clean-%=%) clean
install: $(INSTALLDIRS)
	@cp bin/evawiz ../bin/evawiz
$(INSTALLDIRS):
	$(MAKE) -C $(@:install-%=%) install

