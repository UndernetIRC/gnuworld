# Generated automatically from Makefile.in by configure.

SHLIB_CXX = g++ -fPIC
SHLIB_LD = /usr/ccs/bin/ld -G -z text

MOD_CXX = g++ -fPIC
MOD_LD = g++

CXXFLAGS = -g -Wall -O0 -I/tmp/gnuworld/include -I/tmp/gnuworld/lib

OBJS = cservice.o
RM = rm

all: $(OBJS) cservice.o finish

.cc.o:
	$(MOD_CXX) $(CXXFLAGS) -I/usr/local/pgsql/include -c -o $@ $<

# LIBS is passed in by make
finish:
	$(SHLIB_LD) -o cservice.so cservice.o -L/usr/local/pgsql/lib -lpq++ $(LIBS) \
	$(ABSOBJ)

clean:
	$(RM) -f $(OBJS) cservice.so cservice.o

distclean: clean
	$(RM) -f config.status config.cache

depend:
	$(CXX) $(CXXFLAGS) -MM cservice.cc >> Makefile

# DO NOT DELETE THIS LINE
