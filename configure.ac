dnl $Id: configure.ac,v 1.42 2008/04/16 15:25:49 danielaustin Exp $
dnl ----------------------------------------------------------

echo Configuring GNUWorld...
echo

AC_REVISION($Revision: 1.42 $)
AC_INIT
AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE(gnuworld,3.0)
AM_CONFIG_HEADER(include/defs.h)

AC_PROG_CXX
AC_LANG([C++])

AC_PROG_MAKE_SET(gmake)

AC_DISABLE_STATIC
AC_ENABLE_SHARED

AC_PROG_INSTALL
dnl Enable building of the convenience library
dnl and set LIBLTDL accordingly
AC_LIBLTDL_CONVENIENCE

dnl Substitute INCLTDL and LIBLTDL in the Makefiles
dnl For some reason, INCLTDL and LIBLTDL aren't set
dnl properly on OpenBSD
LIBLTDL="\$(top_builddir)/libltdl/libltdlc.la"

AM_MAINTAINER_MODE
AC_ISC_POSIX

dnl Check for dlopen support
AC_LIBTOOL_DLOPEN

dnl Configure libtool
AC_PROG_LIBTOOL

CXXFLAGS="-W -Wall -pipe -I\$(top_srcdir)/libltdl"
CXXFLAGS="$CXXFLAGS -I\$(top_srcdir)/db"
CFLAGS="$CFLAGS"

AC_PREFIX_DEFAULT(`pwd`)

dnl Configure libltdl
AC_CONFIG_SUBDIRS(libltdl)

AC_MSG_CHECKING([whether to detect pthread libraries])

AC_ARG_ENABLE([threads],
    AC_HELP_STRING([--disable-threads],
    [disable the linking of the pthread library (default = no)]),
    [check_threadLib=$enable_threads],
    [check_threadLib=yes])

AC_MSG_RESULT([$check_threadLib])

dnl Do we need an explicit lib for the pthreads?
gcc_support_pthread=no
if test "$check_threadLib" = yes; then
	dnl usually, gcc needs -pthread

	AC_MSG_CHECKING([whether gcc accepts the -pthread option])
	pthreadtest=`$CXX -pthread 2>&1 | grep -c unrecognized`

	if [ test "x$pthreadtest" = "x0" ]; then
		gcc_support_pthread=yes
		AC_SUBST(threadLib)
		threadLib=-pthread
	else
		gcc_support_pthread=no
	fi
	AC_MSG_RESULT([$gcc_support_pthread])
fi

if [ test x"$gcc_support_pthread" = xno ]; then   
	pthread_found=no
	AC_CHECK_LIB(pthread, pthread_create, pthread_found=yes)
	if [ test x"$pthread_found" = xyes ]; then
		AC_SUBST(threadLib)
		threadLib=-lpthread
	else
		AC_CHECK_LIB(c_r, pthread_create,threadLib=-lc_r)
		AC_CHECK_LIB(gcc_r, main, threadLib=-gcc_r)
	fi
fi

dnl The below algorithm is incorrect because gcc is given no
dnl input files

dnl ----------------------------------------------------------
dnl ----------------------------------------------------------
dnl
dnl System specific checks
dnl
dnl ----------------------------------------------------------
dnl ----------------------------------------------------------

AC_CHECK_LIB(socket,socket)
AC_CHECK_LIB(nsl,gethostbyname)

dnl Check for pcre for building a test program.
dnl If it is not present, just don't build the program.
AC_CHECK_LIB(pcre,pcre_compile,
	AM_CONDITIONAL(COND_PCRE,[/bin/true]),
	AM_CONDITIONAL(COND_PCRE,[/bin/false]))
dnl AC_CHECK_LIB(z,inflate)
dnl AC_CHECK_LIB(pthread,pthread_create)
dnl AC_SEARCH_LIBS(pthread_create,lthread pthread c c_r)
dnl AC_SEARCH_LIBS(pthread_kill,lthread pthread c c_r)
dnl AC_CHECK_FUNCS(pthread_create)

dnl ----------------------------------------------------------
dnl
dnl Checking for headers, functions, and a type declarations
dnl

AC_CHECK_HEADERS(sys/file.h unistd.h sys/stat.h sys/ioctl.h sys/resource.h )
AC_CHECK_HEADERS(sys/time.h asm/ioctls.h xti_inet.h sys/filio.h getopt.h)
AC_CHECK_HEADERS(sys/types.h sys/socket.h netinet/in.h sys/un.h pthread.h)

AC_HEADER_STDC
AC_HEADER_TIME
AC_TYPE_SIGNAL
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

found_socklen_t=no
AC_EGREP_HEADER(socklen_t,sys/socket.h, AC_DEFINE(HAVE_SOCKLEN_T) found_socklen_t=yes)

if test $found_socklen_t = yes; then
  AC_EGREP_HEADER(socklen_t,bits/socket.h,
	AC_DEFINE([HAVE_SOCKLEN_T],[],[Socket length type]))
fi

if test $found_socklen_t = yes; then
  AC_MSG_CHECKING(for socklen_t)
  AC_MSG_RESULT(yes)
else
  AC_CHECK_TYPE(socklen_t,int)
fi

AC_ARG_WITH(debug,
	AC_HELP_STRING([--without-debug],
		       [Disable debugging in the executables]),
	[dummy=true],
	[CXXFLAGS="$CXXFLAGS -g"]
)

dnl By default, do NOT build test directory
build_testdir=false
AC_ARG_WITH(test,
	AC_HELP_STRING([--with-test],
			[Enable building of test programs]),
	[build_testdir=true]
)
AM_CONDITIONAL(COND_TESTDIR,[test $build_testdir = true])

AC_ARG_WITH(pedantic,
	AC_HELP_STRING([--with-pedantic],
			[Add -pedantic to compile options]),
	[CXXFLAGS="$CXXFLAGS -pedantic"]
)

AC_ARG_WITH(optimization,
	AC_HELP_STRING([--with-optimization],
			[Optimize the executables]),
	[CXXFLAGS="$CXXFLAGS -O2"],
	[CXXFLAGS="$CXXFLAGS -O0"]
)

AC_ARG_WITH(prof,
	AC_HELP_STRING([--with-prof],
			[Generate profiling information]),
	[CXXFLAGS="$CXXFLAGS -pg"]
)

dnl The idea with this pgsql code is to check for a system installed
dnl postgresql and use it if found. However, if the user specifies
dnl an alternate install location, use that instead.

dnl Prepare to capture the user's input for the location of a pgsql
dnl installation.
pgsql_home=""
pgsql_includedir="."
pgsql_libdir="."
AC_ARG_WITH(pgsql-home,
	AC_HELP_STRING([--with-pgsql-home=PGSQLHOMEDIR],
			[Specify home directory for PGSQL, default is \
/usr/local/lib/pgsql]),
	[pgsql_home=$withval]
)

dnl If the user specified an install location, store the CXXFLAGS and
dnl LIBS for restoring later. Add the user's install location to
dnl the CXXFLAGS (for include directory) and LIBS (for library directory).
dnl This is a pretty clear indication that I have no idea
dnl what I am doing.
tmpcxxflags="$CXXFLAGS"
tmpcppflags="$CPPFLAGS"
tmplibs="$LIBS"
if [ test "$pgsql_home" != "" ]; then
	dnl User provided a home directory
	dnl Set these next two variables for the AC_CHECK_LIB and
	dnl AC_CHECK_HEADER used below.
	CXXFLAGS="$CXXFLAGS -I$pgsql_home/include"
	CPPFLAGS="$CPPFLAGS -I$pgsql_home/include"
	LIBS="$LIBS -L$pgsql_home/lib"

	pgsql_includedir="$pgsql_home/include"
	pgsql_libdir="$pgsql_home/lib"

	dnl Since the user specified the directory, record the
	dnl include directory for libpq-fe.h using a bit of a cheat.
	tmpcxxflags="$tmpcxxflags -I$pgsql_includedir"
	tmpcppflags="$tmpcppflags -I$pgsql_includedir"
fi

dnl Check for the pgsql library and header file.
dnl Need to generalize when mysql support is added.
AC_CHECK_LIB(pq, PQconnectdb)
AC_CHECK_HEADER([libpq-fe.h])
AC_DEFINE([HAVE_PGSQL],[],[Using PostGreSQL])

dnl Everything seemed to work, pgsql located.
dnl Restore the original CXXFLAGS and LIBS. The db library will
dnl link against pgsql_lib (which is empty if using system default),
dnl and will include pgsql_include (again, which is empty if using
dnl system default installation).
CXXFLAGS="$tmpcxxflags"
CPPFLAGS="$tmpcppflags"
LIBS="$tmplibs"

dnl Enable log4cplus if required
AC_ARG_WITH(log4cplus,
	AC_HELP_STRING([--with-log4cplus],
			[Check log4cplus, if not enabled some modules may not have logging support]),
	[check_log4cplus=$withval]
)

LOG4CPLUS_LIB="${check_log4cplus}/lib"
AC_ARG_WITH(log4cplus-lib,
	AC_HELP_STRING([--with-log4cplus-lib=LOG4CPLUSLIBDIR],
			[Specify location to find liblog4cplus.so]),
	[LOG4CPLUS_LIB=$withval]
)

LOG4CPLUS_INCLUDE="${check_log4cplus}/include"
AC_ARG_WITH(log4cplus-include,
	AC_HELP_STRING([--with-log4cplus-include=LOG4CPLUSINCLUDEDIR],
			[Specify location to find logger.h]),
	[LOG4CPLUS_INCLUDE=$withval]
)

if [test "x$check_log4cplus" != "x" ]; then
	AC_MSG_CHECKING(for liblog4cplus.so)
	if [test -f "$LOG4CPLUS_LIB/liblog4cplus.so"]; then
		/bin/true
	else
	        AC_MSG_ERROR( Unable to find liblog4cplus.so, \
			 please use the --with-log4cplus-lib argument to point to the right path)
	fi
	AC_MSG_RESULT(yes)
	
	AC_MSG_CHECKING(for log4cplus/logger.h)
	if [test -f "$LOG4CPLUS_INCLUDE/log4cplus/logger.h"]; then
		/bin/true
	else
	        AC_MSG_ERROR( Unable to find log4cplus/logger.h, \
			 please use the --with-log4cplus-include argument to point to the right path)
	fi
	AC_MSG_RESULT(yes)
	AC_DEFINE([ENABLE_LOG4CPLUS], [], [Enable LOG4CPLUS])
	LDFLAGS="${LDFLAGS} -L${LOG4CPLUS_LIB} -llog4cplus"
	CXXFLAGS="${CXXFLAGS} -I${LOG4CPLUS_INCLUDE}"
fi

dnl Enable liboath if required
AC_ARG_WITH(liboath,
        AC_HELP_STRING([--with-liboath],
                        [Check liboath, if not enabled TOTP functionality will not be available]),
        [check_liboath=$withval]
)
if [test "$check_liboath" == "yes"]; then
        check_liboath=/usr/local
fi
OATH_LIB="${check_liboath}/lib"
AC_ARG_WITH(liboath-lib,
        AC_HELP_STRING([--with-liboath-lib=LIBOATHLIBDIR],
                        [Specify location to find liboath.so]),
        [OATH_LIB=$withval]
)

OATH_INCLUDE="${check_liboath}/include"
AC_ARG_WITH(liboath-include,
        AC_HELP_STRING([--with-liboath-include=OATHINCLUDEDIR],
                        [Specify location to find oath.h]),
        [OATH_INCLUDE=$withval]
)

if [test "x$check_liboath" != "x" ]; then
        LDFLAGS="${LDFLAGS} -L${OATH_LIB}"
        CXXFLAGS="${CXXFLAGS} -I${OATH_INCLUDE} -L${OATH_LIB}"
        dnl AC_CHECK_FILE("$OATH_LIB/liboath.so",, [AC_MSG_ERROR([Unable to find liboath.so, \
        dnl                 please use the --with-liboath-lib argument to point to the right path],1)])
        AC_CHECK_LIB(oath,oath_totp_validate,, [AC_MSG_ERROR([Unable to find liboath.so, please use the --with-liboath-lib argument to point to the right path],1)])
        AC_CHECK_FILE("$OATH_INCLUDE/liboath/oath.h",, [AC_MSG_ERROR([Unable to find oath.h, please use the --with-liboath-include argument to point to the right path],1)])
        dnl AC_DEFINE([ENABLE_LIBOATH], [], [Enable LIBOATH])
fi

dnl Allow selection of additional includes and libraries paths
dnl just in case!

AC_ARG_WITH(extra-includes,
	AC_HELP_STRING([--with-extra-includes=INCLUDESDIR],
		[Specify location to find additional include files]),
	[CXXFLAGS="$CXXFLAGS -I$withval"]
)
AC_ARG_WITH(extra-libraries,
	AC_HELP_STRING([--with-extra-libraries=LIBRARYDIR],
		[Specify location to find additional libraries]),
	[LDFLAGS="$LDFLAGS -L$withval"]
)

dnl This calls a macro in ax_boost_thread.m4
m4_include([ac_cxx_namespaces.m4])
m4_include([ax_boost_thread.m4])
CXXFLAGS="$CXXFLAGS $threadLib"
AX_BOOST_THREAD

echo
echo "Detecting modules..."
echo

build_modccontrol=false
build_modchanfix=false
build_modclientExample=false
build_modcloner=false
build_modcservice=false
build_moddronescan=false
build_modgnutest=false
build_modnickserv=false
build_modopenchanfix=false
build_modscanner=false
build_modstats=false
build_modsnoop=false

NEW_MODS=""
MOD_DIRS=""
searchForMods=yes

AC_ARG_ENABLE(modules,
	AC_HELP_STRING([--enable-modules=mods],
		[Enable one or more modules; all modules found in the \
current directory are enabled by default.  Specify "no" (without quotes) \
to build server only]),
[
dnl echo "enableval: $enableval"
if [test $enableval = "yes"]; then
	searchForMods=yes
elif [test $enableval = "no"]; then
	searchForMods=no
else
	tmp="mod.$enableval"
dnl 	echo "tmp: $tmp"
	tmp=`echo $tmp | sed 's/,/ mod./g'`
dnl	echo "tmp: $tmp"
	MOD_DIRS="$MOD_DIRS $tmp"
	searchForMods=no

dnl	Prepare for conditionals
	for moduleName in $MOD_DIRS; do
	  case "$moduleName" in
	    mod.ccontrol)	build_modccontrol=true;;
	    mod.chanfix)	build_modchanfix=true;;
	    mod.clientExample)	build_modclientExample=true;;
	    mod.cloner)		build_modcloner=true;;
	    mod.cservice)	build_modcservice=true;;
	    mod.dronescan)	build_moddronescan=true;;
	    mod.gnutest)	build_modgnutest=true;;
	    mod.nickserv)	build_modnickserv=true;;
	    mod.openchanfix)	build_modopenchanfix=true;;
	    mod.scanner)	build_modscanner=true;;
	    mod.stats)		build_modstats=true;;
	    mod.snoop)		build_modsnoop=true;;
	    *)			NEW_MODS="$NEW_MODS $moduleName";;
	  esac
	done
fi
],
[
dnl echo "running later"
searchForMods=yes
]) dnl AC_ARG_ENABLE

dnl echo "searchForMods: $searchForMods"
if [test $searchForMods = yes]; then
	mod_list=`\ls $srcdir | \grep "mod."`
dnl	echo "mod_list: $mod_list"

	for file in $mod_list; do
		if [ ! test -d $srcdir/$file ]; then
			echo "$srcdir/$file is not a directory!"
			continue
		fi

		if [ test -f $srcdir/$file/Makefile.in ]; then
		  echo Adding module `echo $file | \cut -f2 -d"."`
		  case "$file" in
		    mod.ccontrol)	build_modccontrol=true;;
		    mod.chanfix)	build_modchanfix=true;;
		    mod.clientExample)	build_modclientExample=true;;
		    mod.cloner)		build_modcloner=true;;
		    mod.cservice)	build_modcservice=true;;
		    mod.dronescan)	build_moddronescan=true;;
		    mod.gnutest)	build_modgnutest=true;;
		    mod.nickserv)	build_modnickserv=true;;
		    mod.openchanfix)	build_modopenchanfix=true;;
		    mod.scanner)	build_modscanner=true;;
		    mod.stats)		build_modstats=true;;
		    mod.snoop)		build_modsnoop=true;;
		    *)			NEW_MODS="$NEW_MODS $file";;
		  esac
		fi
	 done
fi

AM_CONDITIONAL(COND_MODCCONTROL,[test $build_modccontrol = true])
AM_CONDITIONAL(COND_MODCHANFIX,[test $build_modchanfix = true])
AM_CONDITIONAL(COND_MODCLIENTEXAMPLE,[test $build_modclientExample = true])
AM_CONDITIONAL(COND_MODCLONER,[test $build_modcloner = true])
AM_CONDITIONAL(COND_MODCSERVICE,[test $build_modcservice = true])
AM_CONDITIONAL(COND_MODDRONESCAN,[test $build_moddronescan = true])
AM_CONDITIONAL(COND_MODGNUTEST,[test $build_modgnutest = true])
AM_CONDITIONAL(COND_MODNICKSERV,[test $build_modnickserv = true])
AM_CONDITIONAL(COND_MODOPENCHANFIX,[test $build_modopenchanfix = true])
AM_CONDITIONAL(COND_MODSCANNER,[test $build_modscanner = true])
AM_CONDITIONAL(COND_MODSTATS,[test $build_modstats = true])
AM_CONDITIONAL(COND_MODSNOOP,[test $build_modsnoop = true])

for file in $NEW_MODS; do
	AC_CONFIG_SUBDIRS($file)
done

LIBGNUWORLD="\$(top_builddir)/libgnuworld/libgnuworld.la"
AC_SUBST(LIBGNUWORLD)

AC_SUBST(NEW_MODS)
AC_SUBST(CXXFLAGS)
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)
AC_SUBST(LIBTOOL_DEPS)
AC_SUBST(pgsql_includedir)
AC_SUBST(pgsql_libdir)

AC_CONFIG_FILES([Makefile \
db/Makefile \
libgnuworld/Makefile \
libircu/Makefile \
src/Makefile \
test/Makefile \
mod.ccontrol/Makefile \
mod.chanfix/Makefile \
mod.clientExample/Makefile \
mod.cloner/Makefile \
mod.cservice/Makefile \
mod.dronescan/Makefile \
mod.gnutest/Makefile \
mod.nickserv/Makefile \
mod.openchanfix/Makefile \
mod.scanner/Makefile \
mod.stats/Makefile \
mod.snoop/Makefile \
bin/server_command_map \
bin/ccontrol.example.conf \
bin/chanfix.example.conf \
bin/clientExample.example.conf \
bin/cloner.example.conf \
bin/cservice.example.conf \
bin/gnutest.example.conf \
bin/GNUWorld.example.conf \
bin/nickserv.example.conf \
bin/openchanfix.example.conf \
bin/scanner.example.conf \
bin/stats.example.conf \
bin/snoop.example.conf \
bin/dronescan.example.conf \
bin/logging.properties \
tools/checkgnuworld.sh])

AC_OUTPUT

echo ""
echo "##########################"
echo "# GNUWorld Configuration #"
echo "##########################"
echo ""
echo "Configuration of GNUWorld is complete - you're almost ready to run 'make'..."
echo ""
echo "Check the following files for modification before building:"
echo "  include/gnuworld_config.h"

if [test $build_modcservice = true]; then
	echo "  mod.cservice/cservice_config.h"
fi

if [test $build_modopenchanfix = true]; then
	echo "  mod.openchanfix/chanfix_config.h"
fi

echo ""

