#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(Z-CHECKER, 0.5, sdi1@anl.gov)
AM_INIT_AUTOMAKE([foreign -Wall -Werror subdir-objects])
AC_CONFIG_SRCDIR([zc/src/zc.c])
AC_CONFIG_HEADERS([config.h])

AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC

# Checks for programs.
#: ${CFLAGS=-O3 -std=c99 -Wall}
#: ${CFLAGS=-O3 -std=gnu99 -Wall}
: ${CXXFLAGS=-O3 -Wall}
: ${CFLAGS=-O3 -std=gnu99 -Wall}

##
## MPI
##

#CC=g++

AC_ARG_ENABLE(mpi, [  --enable-mpi            use MPI], ok=$enableval, ok=no)
AM_CONDITIONAL([MPI], [test "x$enable_mpi" = "xyes"])

if test "$ok" = "yes"; then
	AC_CHECK_PROG(MPICC,mpicc,yes,no)
if test "$MPICC" = no; then
	AC_MSG_ERROR([Cound not find mpicc. Please install mpi library first.])
else
	CC=mpicc
	CXX=mpicxx
	MPI_FLAGS="-DHAVE_MPI"
fi
fi

##
## HDF5
##

AC_ARG_ENABLE(hdf5, [  --enable-hdf5            use HDF5], hdf5_ok=$enableval, hdf5_ok=no)
AM_CONDITIONAL([HDF5], [test "x$enable_hdf5" = "xyes"])

if test "$hdf5_ok" = "yes"; then
	AC_CHECK_PROG(H5CC,h5cc,yes,no)
if test "$H5CC" = no; then
	AC_MSG_ERROR([Cound not find h5cc. Please install hdf5 first.])
else
	CC=h5cc
	HDF5_FLAGS="-DHAVE_HDF5"
fi
fi

# Check NETCDF library
AC_ARG_ENABLE(netcdf, [  --enable-netcdf            use NETCDF], netcdf_ok=$enableval, netcdf_ok=no)
AM_CONDITIONAL([NETCDF], [test "x$enable_netcdf" = "xyes"])

if test "$netcdf_ok" = "yes"; then
#	NETCDF_CFLAGS="-DHAVE_NETCDF"
	AC_ARG_WITH([netcdf-prefix], [AS_HELP_STRING([--with-netcdf-prefix=DIR], 
            [installation prefix for netcdf])], 
            [with_netcdf_include_path=$with_netcdf_prefix/include; 
             with_netcdf_lib_path=$with_netcdf_prefix/lib])

	AC_ARG_WITH([netcdf-include-path], [AS_HELP_STRING([--with-netcdf-include-path=DIR],
            [path to the netcdf directory, defaults to /usr/include])], [])

	if test "x$with_netcdf_include_path" = x ; then
    		with_netcdf_include_path="/usr/include"
	fi

	AC_CHECK_FILE([${with_netcdf_include_path}/netcdf.h], [],
              AC_MSG_ERROR([Invalid path to netcdf.h]))
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

#	AC_SUBST(NETCDF_CFLAGS)
	AC_SUBST(NETCDF_HDR)
	AC_SUBST(NETCDF_LIB)
	AC_SUBST(NETCDF_STATIC)

else
  	AC_MSG_WARN([NETCDF disabled])
fi

# Check LIBPRESSIOOPT library
AC_ARG_ENABLE(libpressioopt, [  --enable-libpressioopt            use LIBPRESSIOOPT], libpressioopt_ok=$enableval, libpressioopt_ok=no)
AM_CONDITIONAL([LIBPRESSIOOPT], [test "x$enable_libpressioopt" = "xyes"])

if test "$libpressioopt_ok" = "yes"; then
#       LIBPRESSIOOPT_CFLAGS="-DHAVE_LIBPRESSIOOPT"
        AC_ARG_WITH([libpressioopt-prefix], [AS_HELP_STRING([--with-libpressioopt-prefix=DIR],
            [installation prefix for libpressioopt])],
            [with_libpressioopt_include_path=$with_libpressioopt_prefix/include;
             with_libpressioopt_lib_path=$with_libpressioopt_prefix/lib64])

        AC_ARG_WITH([libpressioopt-include-path], [AS_HELP_STRING([--with-libpressioopt-include-path=DIR],
            [path to the libpressioopt directory, defaults to /usr/include])], [])

        if test "x$with_libpressioopt_include_path" = x ; then
                with_libpressioopt_include_path="/usr/include"
        fi

        AC_CHECK_FILE([${with_libpressioopt_include_path}/libpressio_opt/pressio_search.h], [],
              AC_MSG_ERROR([Invalid path to pressio_search.h]))
        LIBPRESSIOOPT_HDR="-I$with_libpressioopt_include_path -I$with_libpressioopt_include_path/sz -I$with_libpressioopt_include_path/zfp -I$with_libpressioopt_include_path/libpressio -I$with_libpressioopt_include_path/libpressio_opt -I$with_libpressioopt_include_path/std_compat -I$with_libpressioopt_include_path/boost -I$with_libpressioopt_include_path/libdistributed"

        AC_ARG_WITH([libpressioopt-lib-path], [AS_HELP_STRING([--with-libpressioopt-lib-path],
            [path to the  libpressioopt libraries, defaults to /usr/lib])], [])
        if test "x$with_libpressioopt_lib_path" = x ; then
                with_libpressioopt_lib_path="/usr/lib"
        fi

        LIBPRESSIOOPT_LIB="-L$with_libpressioopt_lib_path"
        LIBS="$LIBPRESSIOOPT_LIB"

        if test "x$BUILD_STATIC" = xyes ; then
                LIBPRESSIOOPTSTATIC="${with_libpressioopt_lib_path}/liblibpressio_opt.a"
                LIBPRESSIOOPT_LIB=
        else
                LIBPRESSIOOPT_STATIC="-llibpressio_opt -lSZ -lzfp -ldlib -llibpressio -lstd_compat -llibdistributed"
        fi

#       AC_SUBST(LIBPRESSIOOPT_CFLAGS)
        AC_SUBST(LIBPRESSIOOPT_HDR)
        AC_SUBST(LIBPRESSIOOPT_LIB)
        AC_SUBST(LIBPRESSIOOPT_STATIC)

else
        AC_MSG_WARN([LIBPRESSIOOPT disabled])
fi



# Check FFTW3 library
AC_ARG_ENABLE(fftw3, [  --enable-fftw3            use FFTW3], fftw_ok=$enableval, fftw_ok=no)
AM_CONDITIONAL([FFTW3], [test "x$enable_fftw3" = "xyes"])

if test "$fftw_ok" = "yes"; then
#	FFTW3_CFLAGS="-DHAVE_FFTW3"
	AC_ARG_WITH([fftw3-prefix], [AS_HELP_STRING([--with-fftw3-prefix=DIR], 
            [installation prefix for fftw3])], 
            [with_fftw3_include_path=$with_fftw3_prefix/include; 
             with_fftw3_lib_path=$with_fftw3_prefix/lib])

	AC_ARG_WITH([fftw3-include-path], [AS_HELP_STRING([--with-fftw3-include-path=DIR],
            [path to the fftw3 directory, defaults to /usr/include])], [])

	if test "x$with_fftw3_include_path" = x ; then
    		with_fftw3_include_path="/usr/include"
	fi

	AC_CHECK_FILE([${with_fftw3_include_path}/fftw3.h], [],
              AC_MSG_ERROR([Invalid path to fftw3.h]))
	FFTW3_HDR="-I$with_fftw3_include_path"

	AC_ARG_WITH([fftw3-lib-path], [AS_HELP_STRING([--with-fftw3-lib-path],
            [path to the fftw3 libraries, defaults to /usr/lib])], [])
	if test "x$with_fftw3_lib_path" = x ; then
    		with_fftw3_lib_path="/usr/lib"
	fi
	FFTW3_LIB="-L$with_fftw3_lib_path"
	LIBS="$FFTW3_LIB"

	if test "x$BUILD_STATIC" = xyes ; then
    		FFTW3_STATIC="${with_fftw3_lib_path}/libfftw3.a"
    		FFTW3_LIB=
	else
    		FFTW3_STATIC="-lfftw3"
	fi

#	AC_SUBST(FFTW3_CFLAGS)
	AC_SUBST(FFTW3_HDR)
	AC_SUBST(FFTW3_LIB)
	AC_SUBST(FFTW3_STATIC)

else
  	AC_MSG_WARN([FFTW3 disabled])
fi

# Check R library
AC_ARG_ENABLE(r, [  --enable-r            use R], r_ok=$enableval, r_ok=no)
AM_CONDITIONAL([R], [test "x$enable_r" = "xyes"])

if test "$r_ok" = "yes"; then 
#	R_CFLAGS="-DHAVE_R"
	AC_ARG_WITH([r-prefix], [AS_HELP_STRING([--with-r-prefix=DIR], 
			[installation prefix for r])], 
			[with_r_include_path=$with_r_prefix/include; 
			 with_r_lib_path=$with_r_prefix/lib])

	AC_ARG_WITH([r-include-path], [AS_HELP_STRING([--with-r-include-path=DIR],
		[path to the r directory, defaults to /usr/include])], [])

	if test "x$with_r_include_path" = x ; then 
		with_r_include_path="/usr/include"
	fi

	AC_CHECK_FILE([${with_r_include_path}/R.h], [],
		  AC_MSG_ERROR([Invalid path to R.h]))
	R_HDR="-I$with_r_include_path"

	AC_ARG_WITH([r-lib-path], [AS_HELP_STRING([--with-r-lib-path],
		[path to the  r libraries, defaults to /usr/lib])], [])
	if test "x$with_r_lib_path" = x ; then 
		with_r_lib_path="/usr/lib"
	fi

	R_LIB="-L$with_r_lib_path"
	LIBS="$R_LIB"

	if test "x$BUILD_STATIC" = xyes ; then
		R_STATIC="${with_r_lib_path}/libr.a"
		R_LIB=
	else
		R_STATIC="-lR"
	fi

#   AC_SUBST(R_CFLAGS)
	AC_SUBST(R_HDR)
	AC_SUBST(R_LIB)
	AC_SUBST(R_STATIC)

else
	AC_MSG_WARN([R disabled])
fi

#enable online visualization (zserver)
AC_ARG_ENABLE(zserver, [  --enable-zserver            use ZSERVER], zserver_ok=$enableval, zserver_ok=no)
AM_CONDITIONAL([ZSERVER], [test "x$enable_zserver" = "xyes"])

if test "$zserver_ok" = "yes"; then
	AC_ARG_WITH([zserver-prefix], [AS_HELP_STRING([--with-zserver-prefix=DIR], 
            [installation prefix for zserver])], 
            [with_zserver_include_path=$with_zserver_prefix/include; 
             with_zserver_lib_path=$with_zserver_prefix/lib])

	AC_ARG_WITH([zserver-include-path], [AS_HELP_STRING([--with-zserver-include-path=DIR],
            [path to the zserver directory, defaults to /usr/include])], [])

	if test "x$with_zserver_include_path" = x ; then
    		with_zserver_include_path="/usr/include"
	fi

	AC_CHECK_FILE([${with_zserver_include_path}/zserver.h], [],
              AC_MSG_ERROR([Invalid path to zserver.h]))
	ZSERVER_HDR="-I$with_zserver_include_path"

	AC_ARG_WITH([zserver-lib-path], [AS_HELP_STRING([--with-zserver-lib-path],
            [path to the zserver libraries, defaults to /usr/lib])], [])
	if test "x$with_zserver_lib_path" = x ; then
    		with_zserver_lib_path="/usr/lib"
	fi
	ZSERVER_LIB="-L$with_zserver_lib_path"
	LIBS="$ZSERVER_LIB"

	if test "x$BUILD_STATIC" = xyes ; then
    		ZSERVER_STATIC="${with_zserver_lib_path}/libzserver.a"
    		ZSERVER_LIB=
	else
    		ZSERVER_STATIC="-lzserver"
	fi

	AC_SUBST(ZSERVER_HDR)
	AC_SUBST(ZSERVER_LIB)
	AC_SUBST(ZSERVER_STATIC)

else
  	AC_MSG_WARN([ZSERVER disabled])
fi


#check for R, netcdf and fftw3
if test "$zserver_ok" = "yes"; then
	if test "$netcdf_ok" = "yes"; then
		if test "$fftw_ok" = "yes"; then
			if test "$r_ok" = "yes"; then
				LIBS="$NETCDF_LIB $FFTW3_LIB $R_LIB $ZSERVER_LIB"
			else
				LIBS="$NETCDF_LIB $FFTW3_LIB $ZSERVER_LIB"
			fi
		else
			if test "$r_ok" = "yes"; then
				LIBS="$NETCDF_LIB $R_LIB $ZSERVER_LIB"
			else
				LIBS="$NETCDF_LIB $ZSERVER_LIB"
			fi 
		fi
	else
		if test "$fftw_ok" = "yes"; then
			if test "$r_ok" = "yes"; then
				LIBS="$FFTW3_LIB $R_LIB $ZSERVER_LIB"
			else
				LIBS="$FFTW3_LIB $ZSERVER_LIB"
			fi
		else
			if test "$r_ok" = "yes"; then
				LIBS="$R_LIB $ZSERVER_LIB"
			fi 
		fi
	fi
else
	if test "$netcdf_ok" = "yes"; then
		if test "$fftw_ok" = "yes"; then
			if test "$r_ok" = "yes"; then
				LIBS="$NETCDF_LIB $FFTW3_LIB $R_LIB"
			else
				LIBS="$NETCDF_LIB $FFTW3_LIB"
			fi
		else
			if test "$r_ok" = "yes"; then
				LIBS="$NETCDF_LIB $R_LIB"
			else
				LIBS="$NETCDF_LIB"
			fi 
		fi
	else
		if test "$fftw_ok" = "yes"; then
			if test "$r_ok" = "yes"; then
				LIBS="$FFTW3_LIB $R_LIB"
			else
				LIBS="$FFTW3_LIB"
			fi
		else
			if test "$r_ok" = "yes"; then
				LIBS="$R_LIB"
			fi 
		fi
	fi
fi

#mpi
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

AC_CONFIG_FILES([Makefile])
if test "$netcdf_ok" = "yes"; then
AC_CONFIG_FILES([NetCDFReader/Makefile])
fi
if test "$hdf5_ok" = "yes"; then
AC_CONFIG_FILES([HDF5Reader/Makefile])
fi
if test "$r_ok" = "yes"; then
AC_CONFIG_FILES([R/Makefile])
fi
AC_CONFIG_FILES([zc/Makefile
                 examples/Makefile])
AC_OUTPUT

#set package dir in sz.config

