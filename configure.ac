#Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([openblox-studio], [0.1.1], [johnmh@openblox.org])
AC_CONFIG_HEADERS([src/config.h])
AC_PROG_CXX
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

AC_CANONICAL_TARGET
AC_CANONICAL_SYSTEM

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.10 foreign])


libobv="libopenblox"

#Options
AC_ARG_WITH(libopenblox, AS_HELP_STRING([--with-libopenblox=LIBOPENBLOX_VER], [Compile with a specific version of libopenblox.]), [libobv="$withval"])

#Dependency checks

AX_PKG_CHECK_MODULES([LOPENBLOX], [$libobv], [])

AC_SUBST(LOPENBLOX_CFLAGS)
AC_SUBST(LOPENBLOX_LIBS)

AC_CHECK_LIB(Irrlicht, createDevice, [have_irrlicht=yes], AC_MSG_ERROR([createDevice was not found in libIrrlicht (Irrlicht not installed?)]))

AS_IF([test "x$have_irrlicht" = "xyes"],
      [LIRRLICHT_LIBS="-lIrrlicht"] [AC_DEFINE_UNQUOTED(HAVE_IRRLICHT, 1, [Define to 1 if you have the `Irrlicht' library (-lIrrlicht).],
      [LIRRLICHT_LIBS=""])])


AC_SUBST(LIRRLICHT_LIBS)

AX_PKG_CHECK_MODULES([LBULLET], [bullet], [], [AC_DEFINE_UNQUOTED(HAVE_BULLET, 1, [Define to 1 if you have the `bullet' library (-lbullet).])])

PKG_CHECK_MODULES([LQT], [
	Qt5Core
	Qt5Gui
	Qt5Widgets
])

AC_CHECK_PROGS(RCC, [rcc rcc-qt5])

AS_IF([test "x$target_os" == "xmingw32"],
      [LGL_LIB="-lopengl32"],
      [LGL_LIB="-lGL"])

AC_SUBST(LGL_LIB)

LT_INIT

AC_CONFIG_FILES(Makefile
                src/Makefile)
AC_OUTPUT
