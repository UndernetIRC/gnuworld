# Generated automatically from Makefile.in by configure.
# $Id: Makefile,v 1.3 2000/07/05 19:15:06 dan_karrels Exp $
# all		-- gnuworld
# clean		-- cleanup objects and executable
# distclean	-- everything =P

# Standard defines
CXX = g++
DEFS = -DHAVE_CONFIG_H
LIBS = -ldl -lnsl 

# Set this to -g if you want to be able to debug the client, otherwise
# use -O to have the compiler do some optimization instead.
CXXFLAGS = -g -Wall -O0 -I/home/dan/gnuworld/include -I/home/dan/gnuworld/lib

SHLIB_CXX = g++
SHLIB_LD = g++ -shared -nostartfiles

MOD_CXX = g++
MOD_LD = g++
MOD_DIRS =  ccontrol.mod cservice.mod

BIN_PATH = /home/dan/gnuworld/bin
SRC_PATH = /home/dan/gnuworld/src

RM = rm
CP = cp
MV = mv
LN = ln

############ You shouldn't change anything below this line ###################

VERSION = @VERSION@

SHELL = /bin/sh
MAKE2 = $(MAKE) $(MFLAGS)
MFLAGS ='CXX=$(CXX)'					\
	'ANSIFLAGS=$(ANSIFLAGS)'			\
	'CXXFLAGS=$(CXXFLAGS)'				\
	'DEFS=$(DEFS)'					\
	'LDFLAGS=$(LDFLAGS)'				\
	'RM=$(RM)'

all: libgnuworld gnuworld modules
everything: all

libgnuworld: lib/Makefile
	@cd lib; $(MAKE2) all

gnuworld: src/Makefile Makefile libgnuworld
	@cd src; $(MAKE2) all

modules:
	@echo "Building modules..."
	@cd src; $(MAKE2) modules
	@for i in $(MOD_DIRS); do \
		whichMod=`echo $$i | cut -d"." -f1`; \
		whichMod="$$whichMod.so"; \
		if [ -f $(BIN_PATH)/$$whichMod ]; then \
			$(RM) $(BIN_PATH)/$$whichMod; \
		fi; \
		if [ -f $(SRC_PATH)/$$i/$$whichMod ]; then \
			$(LN) -s $(SRC_PATH)/$$i/$$whichMod $(BIN_PATH)/$$whichMod ; \
		fi; \
	done;

clean:
	@-if [ -f lib/Makefile ]; then cd lib; $(MAKE2) clean; fi
	@-if [ -f src/Makefile ]; then cd src; $(MAKE2) clean; fi

distclean cleandir realclean: clean
	$(RM) config.status config.cache
	$(RM) config.log include/defs.h
	@cd lib; $(MAKE2) distclean
	@cd src; $(MAKE2) distclean
	$(RM) src/Makefile lib/Makefile
	$(RM) Makefile

depend:
	@echo
	@echo "Building dependencies, this may take a moment..."
	@echo
	@cd lib; make depend
	@cd src; make depend
	@echo
	@echo "Finished building dependencies"
	@echo
