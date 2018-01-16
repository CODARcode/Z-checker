AUTOMAKE_OPTIONS=foreign

include_HEADERS=src/ZC_callR.h src/ZC_manyArgs.h

lib_LTLIBRARIES=libzccallr.la
libzccallr_la_CFLAGS=-g -I./include -DHAVE_R $(R_HDR)

libzccallr_la_LDFLAGS = -version-info  0:1:0
libzccallr_la_LIDADD=$(R_LIBS) $(R_LIB)
libzccallr_la_SOURCES=src/ZC_callR.c src/ZC_manyArgs.c
