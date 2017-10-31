#AM_CFLAGS = -I./include
#LDFLAGS=-fPIC -shared

AUTOMAKE_OPTIONS=foreign
include_HEADERS=include/ZC_ByteToolkit.h include/ZC_conf.h include/ZC_gnuplot.h include/ZC_latex.h include/ZC_quicksort.h\
		include/ZC_rw.h include/ZC_Hashtable.h include/ZC_DataProperty.h include/ZC_CompareData.h\
		include/DynamicIntArray.h include/DynamicFloatArray.h include/DynamicDoubleArray.h include/DynamicByteArray.h\
		include/dictionary.h include/zc.h include/iniparser.h include/ZC_util.h include/ZC_ReportGenerator.h\
		#include/ZC_HDF5Handler.h include/ZC_DataSetHandler.h

lib_LTLIBRARIES=libzc.la
if MPI
libzc_la_CFLAGS=-g -I./include -DHAVE_MPI
else
libzc_la_CFLAGS=-g -I./include
endif

if HDF5
libzc_la_CFLAGS+=-DHAVE_HDF5
endif

libzc_la_LDFLAGS = -version-info  0:1:0
#libsz_la_LIDADD=../zlib/.libs/libzlib.a
libzc_la_SOURCES=src/ZC_ByteToolkit.c src/ZC_gnuplot.c src/ZC_Hashtable.c src/iniparser.c src/ZC_DataProperty_float.c src/ZC_DataProperty_double.c src/ZC_DataProperty.c\
		src/ZC_CompareData_float.c src/ZC_CompareData_double.c src/ZC_CompareData.c\
		src/DynamicIntArray.c src/DynamicFloatArray.c src/DynamicDoubleArray.c src/DynamicByteArray.c src/ZC_latex.c src/ZC_ReportGenerator.c\
		src/ZC_quicksort.c src/ZC_rw.c src/ZC_conf.c src/dictionary.c src/ZC_util.c src/zc.c\
		#src/ZC_HDF5Handler.c src/ZC_DataSetHandler.c
