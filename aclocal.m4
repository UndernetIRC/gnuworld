dnl aclocal.m4
dnl   macros autoconf uses when building configure from configure.in
dnl
dnl $Id: aclocal.m4,v 1.4 2000/12/09 22:58:08 dan_karrels Exp $
dnl

dnl This entire function (minus a few minor modifications) was taken
dnl from eggdrop1.5 (www.eggheads.org).  Thanx guys!
dnl
AC_DEFUN(GNUWORLD_CHECK_OS, [dnl
LINUX=no
IRIX=no
SUNOS=no
HPUX=no
MOD_CXX="${CXX}"
MOD_LD="${CXX}"
MOD_STRIP="${STRIP}"
SHLIB_CXX="${CXX}"
SHLIB_LD="${CXX}"
SHLIB_STRIP="${STRIP}"
NEED_DL=1
DEFAULT_MAKE=debug

AC_MSG_CHECKING(your OS)

eval "cv_var_system=`${UNAME}`"
case "$cv_var_system" in
  BSD/OS)
    bsd_version=`${UNAME} -r | cut -d . -f 1`
    case "$bsd_version" in
      2)
        AC_MSG_RESULT(BSD/OS 2! statically linked modules are the only choice)
        NEED_DL=0
        DEFAULT_MAKE=static
      ;;
      3)
        AC_MSG_RESULT(BSD/OS 3! stuck with an old OS ...)
        MOD_CXX=shlicc
        MOD_LD=shlicc
        MOD_STRIP="${STRIP} -d"
        SHLIB_LD="shlicc -r"
        SHLIB_STRIP=touch
        AC_DEFINE(MODULES_OK)dnl
      ;;
      *)
        AC_MSG_RESULT(BSD/OS 4+! ok I spose)
        MOD_LD="${CXX} "
        MOD_STRIP="${STRIP} -d"
        SHLIB_LD="${CXX} -shared -nostartfiles"
        AC_DEFINE(MODULES_OK)dnl
      ;;
    esac
    ;;
  CYGWIN*)
    AC_MSG_RESULT(Cygwin)
    NEED_DL=0
    DEFAULT_MAKE=static
    ;;
  HP-UX)
    AC_MSG_RESULT([HP-UX, just shoot yourself now])
    HPUX=yes
    MOD_LD="g++ -fPIC -shared"
    SHLIB_CXX="g++ -fPIC"
    SHLIB_LD="ld -b"
    NEED_DL=0
    AC_DEFINE(MODULES_OK)dnl
    AC_DEFINE(HPUX_HACKS)dnl
    if test "x`${UNAME} -r | cut -d . -f 2`" = "x10"
    then
      AC_DEFINE(HPUX10_HACKS)dnl
    fi
    ;;
  dell)
    AC_MSG_RESULT(Dell SVR4)
    SHLIB_STRIP=touch
    NEED_DL=0
    MOD_LD="g++ -lelf -lucb"
    ;; 
  IRIX)
    AC_MSG_RESULT(you are cursed with IRIX)
    IRIX=yes
    SHLIB_STRIP=touch
    NEED_DL=0
    DEFAULT_MAKE=static
    ;;
  IRIX64)
    AC_MSG_RESULT(IRIX64)
    IRIX=yes
    SHLIB_STRIP=strip
    NEED_DL=0
    DEFAULT_MAKE=static
    ;;
  Ultrix)
    AC_MSG_RESULT(Ultrix)
    NEED_DL=0
    SHLIB_STRIP=touch
    DEFUALT_MAKE=static
    ;;
  BeOS)
    AC_MSG_RESULT(BeOS)
    NEED_DL=0
    SHLIB_STRIP=strip
    DEFUALT_MAKE=static
    ;;
  Linux)
    AC_MSG_RESULT(Linux! The choice of the GNU generation)
    LINUX=yes
    MOD_LD="${CXX}"
    SHLIB_LD="${CXX} -shared -nostartfiles"
    AC_DEFINE(MODULES_OK)dnl
    ;;
  Lynx)
    SHLIB_STRIP=touch
    AC_MSG_RESULT(Lynx OS)
    ;;
  OSF1)
    AC_MSG_RESULT(OSF...)
    case `${UNAME} -r | cut -d . -f 1` in
      V*)
        AC_MSG_RESULT([   Digital OSF])
        if test "x$AWK" = "xgawk"
        then
          AWK=awk
        fi
        MOD_CXX=g++
        MOD_LD=g++
        SHLIB_CXX=g++
        SHLIB_LD="ld -msym -S -shared -expect_unresolved \*"
        SHLIB_STRIP=touch
        AC_DEFINE(MODULES_OK)dnl
        ;;
      1.0|1.1|1.2)
        AC_MSG_RESULT([   pre 1.3])
        SHLIB_LD="ld -R -export $@:"
        AC_DEFINE(MODULES_OK)dnl
        AC_DEFINE(OSF1_HACKS)dnl
        ;;
      1.*)
        AC_MSG_RESULT([   1.3+])
        SHLIB_CXX="${CXX} -fpic"
        SHLIB_LD="ld -shared"
        AC_DEFINE(MODULES_OK)dnl
        AC_DEFINE(OSF1_HACKS)dnl
        ;;
      *)
        AC_MSG_RESULT([   Some other weird OSF! No modules for you...])
        NEED_DL=0
        DEFAULT_MAKE=static
        ;;
    esac
    AC_DEFINE(STOP_UAC)dnl
    ;;
  SunOS)
    if test "x`${UNAME} -r | cut -d . -f 1`" = "x5"
    then
      AC_MSG_RESULT(Solaris -- yay)
      SHLIB_LD="/usr/ccs/bin/ld -G -z text"
    else
      AC_MSG_RESULT(SunOS -- sigh)
      SUNOS=yes
      SHLIB_LD=ld
      SHLIB_STRIP=touch
      AC_DEFINE(DLOPEN_1)dnl
    fi
    MOD_CXX="${CXX} -fPIC"
    SHLIB_CXX="${CXX} -fPIC"
    AC_DEFINE(MODULES_OK)dnl
    ;;
  FreeBSD)
    AC_MSG_RESULT(FreeBSD)
    SHLIB_CXX="${CXX} -fpic"
    SHLIB_LD="ld -Bshareable -x"
    AC_DEFINE(MODULES_OK)dnl
    ;;
  *BSD)
    AC_MSG_RESULT(NetBSD/OpenBSD - choose your poison)
    SHLIB_CXX="${CXX} -fpic"
    SHLIB_LD="ld -Bshareable -x -r"
    AC_DEFINE(MODULES_OK)dnl
    ;;
  *)
    if test -r "/mach"
    then
      AC_MSG_RESULT([NeXT...We are borg, you will be assimilated])
      AC_MSG_RESULT([break out the static modules, it's all you'll ever get :P])
      NEED_DL=0
      DEFAULT_MAKE=static
      AC_DEFINE(BORGCUBES)dnl
    else
      if test -r "/cmds"
      then
        AC_MSG_RESULT(QNX)
        SHLIB_STRIP=touch
        NEED_DL=0
        DEFAULT_MAKE=static
      else
        AC_MSG_RESULT(Something unknown!)
        AC_MSG_RESULT([If you get dynamic modules to work, be sure to let the devel team know HOW :)])
        NEED_DL=0
        DEFAULT_MAKE=static
      fi
    fi
    ;;
esac
AC_SUBST(MOD_LD)
AC_SUBST(MOD_CXX)
AC_SUBST(MOD_STRIP)
AC_SUBST(SHLIB_LD)
AC_SUBST(SHLIB_CXX)
AC_SUBST(SHLIB_STRIP)
AC_SUBST(DEFAULT_MAKE)
])
