# Generated automatically from Makefile.in by configure.

SHLIB_CXX = g++ -fPIC
SHLIB_LD = /usr/ccs/bin/ld -G -z text

MOD_CXX = g++ -fPIC -shared
MOD_LD = g++

CXXFLAGS = -g -Wall -O2 -I/tmp/gnuworld/include -I/tmp/gnuworld/lib
BIN_PATH = /tmp/gnuworld/bin

OBJS = CControlCommands.o

all: $(OBJS) ccontrol.o finish

.cc.o:
	$(MOD_CXX) $(CXXFLAGS) -c -o $@ $<

# LIBS is passed in by make
# ABSOBJ is also passed in by make
# -> This is the full path to each of the core objects for GNUWorld
finish:
	$(SHLIB_LD) -o ccontrol.so ccontrol.o CControlCommands.o $(ABSOBJ) $(LIBS)

link:
	@-if [ -f $(BIN_PATH)/ccontrol.so ]; then \
		@-echo "Removing stale link..." \
		$(RM) $(BIN_PATH)/ccontrol.so; \
	fi
	$(LN) -s ccontrol.so $(BIN_PATH)/ccontrol.so

clean:
	$(RM) -f $(OBJS) ccontrol.so ccontrol.o

distclean: clean
	$(RM) -f config.status config.cache

depend:
	$(CXX) $(CXXFLAGS) -MM ccontrol.cc CControlCommands.cc >> Makefile

# DO NOT DELETE THIS LINE
ccontrol.o: ccontrol.cc /tmp/gnuworld/include/client.h \
 /tmp/gnuworld/include/server.h /tmp/gnuworld/include/Numeric.h \
 /tmp/gnuworld/include/iServer.h /tmp/gnuworld/include/ELog.h \
 /tmp/gnuworld/include/config.h /tmp/gnuworld/include/iClient.h \
 /tmp/gnuworld/include/Channel.h /tmp/gnuworld/include/ChannelUser.h \
 /tmp/gnuworld/lib/xparameters.h /tmp/gnuworld/include/Ban.h \
 /tmp/gnuworld/lib/Socket.h /usr/include/netinet/tcp.h \
 /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.2/../../../../include/g++-3/cstdio \
 /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.2/../../../../include/g++-3/cerrno \
 /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.2/../../../../include/g++-3/cstdlib \
 /usr/include/unistd.h /usr/include/netdb.h \
 /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.2/include/limits.h \
 /usr/include/strings.h /usr/include/fcntl.h /usr/include/sys/stat.h \
 /usr/include/arpa/inet.h /tmp/gnuworld/lib/ClientSocket.h \
 /tmp/gnuworld/lib/Buffer.h /tmp/gnuworld/include/VectorTrie.h \
 /tmp/gnuworld/include/VectorTrieNode.h /tmp/gnuworld/include/events.h \
 /tmp/gnuworld/include/Gline.h /tmp/gnuworld/lib/misc.h \
 /tmp/gnuworld/include/moduleLoader.h \
 /tmp/gnuworld/include/MessageNode.h /tmp/gnuworld/include/EConfig.h \
 /tmp/gnuworld/lib/StringTokenizer.h /tmp/gnuworld/include/Network.h \
 /tmp/gnuworld/include/ccontrol.h \
 /tmp/gnuworld/include/CControlCommands.h
CControlCommands.o: CControlCommands.cc \
 /tmp/gnuworld/include/ccontrol.h /tmp/gnuworld/include/client.h \
 /tmp/gnuworld/include/server.h /tmp/gnuworld/include/Numeric.h \
 /tmp/gnuworld/include/iServer.h /tmp/gnuworld/include/ELog.h \
 /tmp/gnuworld/include/config.h /tmp/gnuworld/include/iClient.h \
 /tmp/gnuworld/include/Channel.h /tmp/gnuworld/include/ChannelUser.h \
 /tmp/gnuworld/lib/xparameters.h /tmp/gnuworld/include/Ban.h \
 /tmp/gnuworld/lib/Socket.h /usr/include/netinet/tcp.h \
 /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.2/../../../../include/g++-3/cstdio \
 /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.2/../../../../include/g++-3/cerrno \
 /usr/include/unistd.h /usr/include/netdb.h \
 /usr/local/lib/gcc-lib/sparc-sun-solaris2.8/2.95.2/include/limits.h \
 /usr/include/strings.h /usr/include/fcntl.h /usr/include/sys/stat.h \
 /usr/include/arpa/inet.h /tmp/gnuworld/lib/ClientSocket.h \
 /tmp/gnuworld/lib/Buffer.h /tmp/gnuworld/include/VectorTrie.h \
 /tmp/gnuworld/include/VectorTrieNode.h /tmp/gnuworld/include/events.h \
 /tmp/gnuworld/include/Gline.h /tmp/gnuworld/lib/misc.h \
 /tmp/gnuworld/include/moduleLoader.h \
 /tmp/gnuworld/include/MessageNode.h \
 /tmp/gnuworld/include/CControlCommands.h \
 /tmp/gnuworld/lib/StringTokenizer.h /tmp/gnuworld/include/Network.h \
 /tmp/gnuworld/include/ip.h
