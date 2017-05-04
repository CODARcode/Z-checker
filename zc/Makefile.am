#AM_CFLAGS = -I./include
#LDFLAGS=-fPIC -shared

AUTOMAKE_OPTIONS=foreign
include_HEADERS=include/ZC_rw.h include/ZC_Hashtable.h include/ZC_DataProperty.h include/ZC_CompareData.h\
		include/DynamicIntArray.h include/DynamicFloatArray.h include/DynamicDoubleArray.h include/DynamicByteArray.h\
		include/dictionary.h include/zc.h include/iniparser.h include/ZC_util.h include/latex.h include/ZC_ReportGenerator.h

lib_LTLIBRARIES=libzc.la
libzc_la_CFLAGS=-g -I./include
libzc_la_LDFLAGS = -version-info  0:1:0
#libsz_la_LIDADD=../zlib/.libs/libzlib.a
libzc_la_SOURCES=src/ByteToolkit.c src/gnuplot.c src/ZC_Hashtable.c src/iniparser.c src/ZC_DataProperty.c src/ZC_CompareData.c\
		src/DynamicIntArray.c src/DynamicFloatArray.c src/DynamicDoubleArray.c src/DynamicByteArray.c src/latex.c src/ZC_ReportGenerator.c\
		src/ZC_quicksort.c src/ZC_rw.c src/conf.c src/dictionary.c src/ZC_util.c src/zc.c
