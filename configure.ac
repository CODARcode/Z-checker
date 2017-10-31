#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(Z-CHECKER, 0.1, sdi1@anl.gov)
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AC_CONFIG_SRCDIR([zc/src/zc.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.

##
## MPI
##

AC_ARG_ENABLE(mpi, [  --enable-mpi            use MPI], ok=$enableval, ok=no)
AM_CONDITIONAL([MPI], [test "x$enable_mpi" = "xyes"])

if test "$ok" = "yes"; then
	AC_CHECK_PROG(MPICC,mpicc,yes,no)
if test "$MPICC" = no; then
	AC_MSG_ERROR([Cound not find mpicc. Please install mpi library first.])
else
	CC=mpicc
	MPI_FLAGS="-DHAVE_MPI"
fi
fi

##
## HDF5
##

AC_ARG_ENABLE(hdf5, [  --enable-hdf5            use HDF5], ok=$enableval, ok=no)
AM_CONDITIONAL([HDF5], [test "x$enable_hdf5" = "xyes"])

if test "$ok" = "yes"; then
	AC_CHECK_PROG(H5CC,h5cc,yes,no)
if test "$H5CC" = no; then
	AC_MSG_ERROR([Cound not find h5cc. Please install hdf5 first.])
else
	CC=h5cc
	HDF5_FLAGS="-DHAVE_HDF5"
fi
fi

##--------------------------------------------------------------------
##
## NetCDF
##
##--------------------------------------------------------------------


AC_ARG_ENABLE(netcdf, [  --enable-netcdf            use NETCDF], ok=$enableval, ok=no)
AM_CONDITIONAL([NETCDF], [test "x$enable_netcdf" = "xyes"])

AC_SUBST(NETCDF_CFLAGS)

# Check NETCDF library

if test "$ok" = "yes"; then
AC_ARG_WITH([netcdf-prefix], [AS_HELP_STRING([--with-netcdf-prefix=DIR], 
            [installation prefix for netcdf])], 
            [with_netcdf_include_path=$with_netcdf_prefix/include; 
             with_netcdf_lib_path=$with_netcdf_prefix/lib])

AC_ARG_WITH([netcdf-include-path], [AS_HELP_STRING([--with-netcdf-include-path=DIR],
            [path to the netcdf directory, defaults to /usr/include])], [])

if test "x$with_netcdf_include_path" = x ; then
    with_netcdf_include_path="/usr/include"
fi

AC_CHECK_FILE([${with_netcdf_include_path}/nc.h], [],
              AC_MSG_ERROR([Invalid path to nc.h]))
NETCDF_HDR="-I$with_netcdf_include_path"

AC_ARG_WITH([netcdf-lib-path], [AS_HELP_STRING([--with-netcdf-lib-path],
            [path to the  netcdf libraries, defaults to /usr/lib])], [])
if test "x$with_netcdf_lib_path" = x ; then
    with_netcdf_lib_path="/usr/lib"
fi
NETCDF_LIB="-L$with_netcdf_lib_path"
LIBS="$NETCDF_LIB"

if test "x$BUILD_STATIC" = xyes ; then
    NETCDF_STATIC="${with_netcdf_lib_path}/libnetcdf.a"
    NETCDF_LIB=
else
    NETCDF_STATIC="-lnetcdf"
fi

AC_SUBST(NETCDF_HDR)
AC_SUBST(NETCDF_LIB)
AC_SUBST(NETCDF_STATIC)

else
  AC_MSG_WARN([NETCDF disabled])
fi

AC_SUBST(MPI_FLAGS)

AC_PROG_RANLIB
AM_PROG_AR
AC_PROG_LIBTOOL
#m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

AM_MAINTAINER_MODE([disable])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h limits.h malloc.h stddef.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memchr memset strchr strtol])

AC_CONFIG_FILES([Makefile
		 NetCDFReader/Makefile
                 zc/Makefile
		 examples/Makefile])
AC_OUTPUT
