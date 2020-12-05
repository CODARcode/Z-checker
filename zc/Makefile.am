#AM_CFLAGS = -I./include
#LDFLAGS=-fPIC -shared

AUTOMAKE_OPTIONS=foreign
include_HEADERS=include/ZC_ByteToolkit.h include/ZC_conf.h include/ZC_gnuplot.h include/ZC_latex.h include/ZC_quicksort.h\
		include/ZC_rw.h include/ZC_Hashtable.h include/ZC_DataProperty.h include/ZC_CompareData.h\
		include/DynamicIntArray.h include/DynamicFloatArray.h include/DynamicDoubleArray.h include/DynamicByteArray.h\
		include/dictionary.h include/zc.h include/iniparser.h include/ZC_util.h include/ZC_ReportGenerator.h include/ZC_DataSetHandler.h include/ZC_ssim.h\
		include/der.hpp  include/idx.hpp  include/matrix.hpp  include/m_basic.hpp  include/m_der.hpp  include/m_der_metrics.hpp\
		include/m_norm.hpp  include/norm.hpp  include/others.hpp  include/ssim.hpp include/ZC_LibpressioOpt.h

lib_LTLIBRARIES=libzc.la
if MPI
#libzc_la_CFLAGS=-g -I./include -DHAVE_MPI
libzc_la_CPPFLAGS=-g -I./include -DHAVE_MPI
else
#libzc_la_CFLAGS=-g -I./include
libzc_la_CPPFLAGS=-g -I./include
endif

if HDF5
#libzc_la_CFLAGS+=-DHAVE_HDF5
libzc_la_CPPFLAGS+=-DHAVE_HDF5
endif

if LIBPRESSIOOPT
libzc_la_CPPFLAGS+=-DHAVE_LIBPRESSIOOPT $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC)
endif


if NETCDF
#libzc_la_CFLAGS+=-DHAVE_NETCDF $(NETCDF_HDR) $(NETCDF_STATIC)
libzc_la_CPPFLAGS+=-DHAVE_NETCDF $(NETCDF_HDR) $(NETCDF_STATIC)
endif

if FFTW3
#libzc_la_CFLAGS+=-DHAVE_FFTW3 $(FFTW3_HDR) $(FFTW3_STATIC)
libzc_la_CPPFLAGS+=-DHAVE_FFTW3 $(FFTW3_HDR) $(FFTW3_STATIC)
include_HEADERS+=include/3rdParty/autocorr.h include/ZC_FFTW3_math.h
endif

if R
#libzc_la_CFLAGS+=-DHAVE_R $(R_HDR) $(R_STATIC) -I../R/src
libzc_la_CPPFLAGS+=-DHAVE_FFTW3 $(FFTW3_HDR) $(FFTW3_STATIC)
include_HEADERS+=include/ZC_R_math.h
endif

if ZSERVER
#libzc_la_CFLAGS+=-DHAVE_ONLINEVIS $(ZSERVER_HDR) $(ZSERVER_STATIC)
libzc_la_CPPFLAGS+=-DHAVE_ONLINEVIS $(ZSERVER_HDR) $(ZSERVER_STATIC)
endif

libzc_la_LDFLAGS = -version-info  0:6:0
libzc_la_LIDADD=$(FFTW3_LIBS) $(FFTW3_LIB) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB) $(NETCDF_LIBS) $(NETCDF_LIB) $(ZSERVER_LIBS) $(ZSERVER_LIB)
if R
libzc_la_LIDADD+=../R/.libs/libzccallr.a
endif
libzc_la_SOURCES=src/ZC_ByteToolkit.c src/ZC_gnuplot.c src/ZC_Hashtable.c src/iniparser.c src/ZC_DataProperty_float.c src/ZC_DataProperty_double.c src/ZC_DataProperty.c\
		src/ZC_CompareData_float.cpp src/ZC_CompareData_double.cpp src/ZC_CompareData.c\
		src/DynamicIntArray.c src/DynamicFloatArray.c src/DynamicDoubleArray.c src/DynamicByteArray.c src/ZC_latex.c src/ZC_ReportGenerator.c\
		src/ZC_quicksort.c src/ZC_rw.c src/ZC_conf.c src/dictionary.c src/ZC_util.c src/zc.c src/ZC_DataSetHandler.c src/ZC_ssim.c\
		src/zc_derivativeMetrics.cpp

if LIBPRESSIOOPT
libzc_la_SOURCES+=src/ZC_LibpressioOpt.c
endif

if FFTW3
libzc_la_SOURCES+=src/ZC_FFTW3_math.c
endif

if R
libzc_la_SOURCES+=src/ZC_R_math.c
endif
