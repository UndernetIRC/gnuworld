dnl $Id: configure.ac,v 1.22 2003/12/04 00:44:22 dan_karrels Exp $
dnl ----------------------------------------------------------

echo Configuring GNUWorld...
echo

AC_REVISION($Revision: 1.22 $)
AC_INIT
AC_CONFIG_SRCDIR([src/main.cc])
AM_INIT_AUTOMAKE(gnuworld,2.3)
AM_CONFIG_HEADER(include/defs.h)

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
AM_PROG_LIBTOOL

CXXFLAGS="-W -Wall -pipe -I\$(top_srcdir)/libltdl"
CFLAGS="$CFLAGS"

AC_PREFIX_DEFAULT(`pwd`)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_LANG([C++])

AC_PROG_MAKE_SET(gmake)

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

check_pgsql=yes
AC_ARG_WITH(pgsql,
	AC_HELP_STRING([--without-pgsql],
			[Do not check for pgsql.  This will cause \
several modules to be uncompilable]),
	[check_pgsql=$withval]
)

PGSQL_LIB=/usr/local/pgsql/lib
AC_ARG_WITH(pgsql-lib,
	AC_HELP_STRING([--with-pgsql-lib=PGSQLLIBDIR],
			[Specify location to find libpq++.so, default is \
/usr/local/pgsql/lib]),
	[PGSQL_LIB=$withval]
)
dnl echo PGSQL_LIB: $PGSQL_LIB

PGSQL_INCLUDE=/usr/local/pgsql/include
AC_ARG_WITH(pgsql-include,
	AC_HELP_STRING([--with-pgsql-include=PGSQLINCLUDEDIR],
			[Specify location to find libpq++.h, default is \
/usr/local/pgsql/include]),
	[PGSQL_INCLUDE=$withval]
)
dnl echo PGSQL_INCLUDE: $PGSQL_INCLUDE

build_dbdir=false
if [test $check_pgsql = "yes" ]; then
	build_dbdir=true
	AC_MSG_CHECKING(for libpq++.h)
	if [test -f "$PGSQL_INCLUDE/libpq++.h"]; then
		/bin/true
	elif [test -f "/usr/local/include/libpq++.h"]; then
		PGSQL_INCLUDE=/usr/local/include
	elif [test -f "/usr/include/postgresql/libpq++.h"]; then
		PGSQL_INCLUDE=/usr/include/postgresql
	elif [test -f "/usr/include/pgsql/libpq++.h"]; then
		PGSQL_INCLUDE=/usr/include/pgsql
	elif [test -f "/usr/include/libpq++.h"]; then
		PGSQL_INCLUDE=/usr/include
	else
	        AC_MSG_ERROR( Unable to find libpq++.h )
	fi
	AC_MSG_RESULT(yes)
fi

if [test $check_pgsql = "yes" ]; then
	AC_MSG_CHECKING(for libpq++.so)
	if [test -f "$PGSQL_LIB/libpq++.so"]; then
		/bin/true
	elif [test -f "/usr/local/lib/libpq++.so"]; then
		PGSQL_LIB=/usr/local/lib
	elif [test -f "/usr/lib/libpq++.so"]; then
		PGSQL_LIB=/usr/lib
	elif [test -f "/usr/lib/postgresql/libpq++.so"]; then
		PGSQL_LIB=/usr/lib/postgresql
	elif [test -f "/usr/lib/pgsql/libpq++.so"]; then
		PGSQL_LIB=/usr/lib/pgsql
	elif [test -f "/usr/local/pgsql/lib/libpq++.so"]; then
		PGSQL_LIB=/usr/local/pgsql/lib
	elif [test -f "/usr/local/postgresql/lib/libpq++.so"]; then
		PGSQL_LIB=/usr/local/postgresql/lib
	else
	        AC_MSG_ERROR( Unable to find libpq++.so)
	fi
	AC_MSG_RESULT(yes)
fi
AM_CONDITIONAL(COND_DBDIR,[test $build_dbdir = true])

dnl echo "PGSQL_INCLUDE: $PGSQL_INCLUDE"
dnl echo "PGSQL_LIB: $PGSQL_LIB"
dnl exit

dnl CXXFLAGS="$CXXFLAGS -I${ac_top_srcdir}/include -I${ac_top_srcdir}/libgnuworld"

echo
echo "Detecting modules..."
echo

build_modccontrol=false
build_modclientExample=false
build_modcloner=false
build_modcservice=false
build_moddronescan=false
build_modgnutest=false
build_modnickserv=false
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
	    mod.clientExample)	build_modclientExample=true;;
	    mod.cloner)		build_modcloner=true;;
	    mod.cservice)	build_modcservice=true;;
	    mod.dronescan)	build_moddronescan=true;;
	    mod.gnutest)	build_modgnutest=true;;
	    mod.nickserv)	build_modnickserv=true;;
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
	mod_list=`\ls | \grep "mod."`
dnl	echo "mod_list: $mod_list"

	for file in $mod_list; do
		if [ ! test -d $file ]; then
			echo "$file is not a directory!"
			continue
		fi

		if [ test -f $file/Makefile.in ]; then
		  echo Adding module `echo $file | \cut -f2 -d"."`
		  case "$file" in
		    mod.ccontrol)	build_modccontrol=true;;
		    mod.clientExample)	build_modclientExample=true;;
		    mod.cloner)		build_modcloner=true;;
		    mod.cservice)	build_modcservice=true;;
		    mod.dronescan)	build_moddronescan=true;;
		    mod.gnutest)	build_modgnutest=true;;
		    mod.nickserv)	build_modnickserv=true;;
		    mod.scanner)	build_modscanner=true;;
		    mod.stats)		build_modstats=true;;
		    mod.snoop)		build_modsnoop=true;;
		    *)			NEW_MODS="$NEW_MODS $file";;
		  esac
		fi
	 done
fi

AM_CONDITIONAL(COND_MODCCONTROL,[test $build_modccontrol = true])
AM_CONDITIONAL(COND_MODCLIENTEXAMPLE,[test $build_modclientExample = true])
AM_CONDITIONAL(COND_MODCLONER,[test $build_modcloner = true])
AM_CONDITIONAL(COND_MODCSERVICE,[test $build_modcservice = true])
AM_CONDITIONAL(COND_MODDRONESCAN,[test $build_moddronescan = true])
AM_CONDITIONAL(COND_MODGNUTEST,[test $build_modgnutest = true])
AM_CONDITIONAL(COND_MODNICKSERV,[test $build_modnickserv = true])
AM_CONDITIONAL(COND_MODSCANNER,[test $build_modscanner = true])
AM_CONDITIONAL(COND_MODSTATS,[test $build_modstats = true])
AM_CONDITIONAL(COND_MODSNOOP,[test $build_modsnoop = true])

for file in $NEW_MODS; do
	AC_CONFIG_SUBDIRS($file)
done

LIBGNUWORLD="\$(top_builddir)/libgnuworld/libgnuworld.la"
AC_SUBST(LIBGNUWORLD)

AC_SUBST(NEW_MODS)
AC_SUBST(DB)
AC_SUBST(CXXFLAGS)
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)
AC_SUBST(LIBTOOL_DEPS)
AC_SUBST(PGSQL_INCLUDE)
AC_SUBST(PGSQL_LIB)

AC_CONFIG_FILES([Makefile \
db/Makefile \
libgnuworld/Makefile \
libircu/Makefile \
src/Makefile \
test/Makefile \
mod.ccontrol/Makefile \
mod.clientExample/Makefile \
mod.cloner/Makefile \
mod.cservice/Makefile \
mod.dronescan/Makefile \
mod.gnutest/Makefile \
mod.nickserv/Makefile \
mod.scanner/Makefile \
mod.stats/Makefile \
mod.snoop/Makefile \
bin/server_command_map \
bin/ccontrol.example.conf \
bin/clientExample.example.conf \
bin/cloner.example.conf \
bin/cservice.example.conf \
bin/gnutest.example.conf \
bin/GNUWorld.example.conf \
bin/nickserv.example.conf \
bin/scanner.example.conf \
bin/stats.example.conf \
bin/snoop.example.conf \
bin/dronescan.example.conf])

AC_OUTPUT

echo
echo Configuration has been finished, and everything seems to be all right.
echo Now you have the chance to modify include/config.h to set your
echo preferred options...
echo
