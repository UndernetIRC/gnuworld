# Generated automatically from Makefile.in by configure.

SHLIB_CXX = g++
SHLIB_LD = g++ -shared -nostartfiles

MOD_CXX = g++
MOD_LD = g++

CXXFLAGS = -g -Wall -O0 -I/home/dan/gnuworld/include -I/home/dan/gnuworld/lib
BIN_PATH = /home/dan/gnuworld/bin

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
ccontrol.o: ccontrol.cc /home/dan/gnuworld/include/client.h \
 /home/dan/gnuworld/include/server.h \
 /home/dan/gnuworld/include/Numeric.h \
 /home/dan/gnuworld/include/iServer.h \
 /home/dan/gnuworld/include/ELog.h /home/dan/gnuworld/include/config.h \
 /home/dan/gnuworld/include/iClient.h \
 /home/dan/gnuworld/include/Channel.h \
 /home/dan/gnuworld/include/ChannelUser.h \
 /home/dan/gnuworld/lib/xparameters.h /home/dan/gnuworld/include/Ban.h \
 /home/dan/gnuworld/lib/Socket.h /usr/include/netinet/tcp.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstdio \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cerrno \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstdlib \
 /usr/include/unistd.h /usr/include/netdb.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/include/strings.h /usr/include/fcntl.h /usr/include/sys/uio.h \
 /usr/include/sys/time.h /usr/include/sys/stat.h \
 /usr/include/arpa/inet.h /home/dan/gnuworld/lib/ClientSocket.h \
 /home/dan/gnuworld/lib/Buffer.h \
 /home/dan/gnuworld/include/VectorTrie.h \
 /home/dan/gnuworld/include/VectorTrieNode.h \
 /home/dan/gnuworld/include/events.h \
 /home/dan/gnuworld/include/Gline.h /home/dan/gnuworld/lib/misc.h \
 /home/dan/gnuworld/include/moduleLoader.h \
 /home/dan/gnuworld/include/MessageNode.h \
 /home/dan/gnuworld/include/EConfig.h \
 /home/dan/gnuworld/lib/StringTokenizer.h \
 /home/dan/gnuworld/include/Network.h \
 /home/dan/gnuworld/include/ccontrol.h \
 /home/dan/gnuworld/include/CControlCommands.h
CControlCommands.o: CControlCommands.cc \
 /home/dan/gnuworld/include/ccontrol.h \
 /home/dan/gnuworld/include/client.h \
 /home/dan/gnuworld/include/server.h \
 /home/dan/gnuworld/include/Numeric.h \
 /home/dan/gnuworld/include/iServer.h \
 /home/dan/gnuworld/include/ELog.h /home/dan/gnuworld/include/config.h \
 /home/dan/gnuworld/include/iClient.h \
 /home/dan/gnuworld/include/Channel.h \
 /home/dan/gnuworld/include/ChannelUser.h \
 /home/dan/gnuworld/lib/xparameters.h /home/dan/gnuworld/include/Ban.h \
 /home/dan/gnuworld/lib/Socket.h /usr/include/netinet/tcp.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstdio \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cerrno \
 /usr/include/unistd.h /usr/include/netdb.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/include/strings.h /usr/include/fcntl.h /usr/include/sys/uio.h \
 /usr/include/sys/time.h /usr/include/sys/stat.h \
 /usr/include/arpa/inet.h /home/dan/gnuworld/lib/ClientSocket.h \
 /home/dan/gnuworld/lib/Buffer.h \
 /home/dan/gnuworld/include/VectorTrie.h \
 /home/dan/gnuworld/include/VectorTrieNode.h \
 /home/dan/gnuworld/include/events.h \
 /home/dan/gnuworld/include/Gline.h /home/dan/gnuworld/lib/misc.h \
 /home/dan/gnuworld/include/moduleLoader.h \
 /home/dan/gnuworld/include/MessageNode.h \
 /home/dan/gnuworld/include/CControlCommands.h \
 /home/dan/gnuworld/lib/StringTokenizer.h \
 /home/dan/gnuworld/include/Network.h /home/dan/gnuworld/include/ip.h
