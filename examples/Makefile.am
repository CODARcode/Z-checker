AUTOMAKE_OPTIONS=foreign
if FFTW3
AM_CPPFLAGS = -I../zc/include -lfftw3 -DHAVE_FFTW3
else
AM_CPPFLAGS = -I../zc/include
endif
if R
AM_CPPFLAGS += $(R_HDR) -lR -I../R/src -DHAVE_R
endif
if ZSERVER
AM_CPPFLAGS += $(ZSERVER_HDR) $(ZSERVER_STATIC) $(ZSERVER_LIBS) $(ZSERVER_LIB) -DHAVE_ONLINEVIS
endif
if MPI
AM_CPPFLAGS += -DHAVE_MPI
endif
if LIBPRESSIOOPT
AM_CPPFLAGS += $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB) -DHAVE_LIBPRESSIOOPT
endif

bin_PROGRAMS=modifyZCConfig analyzeDataProperty compareDataSets generateGNUPlot generateReport runOfflineCase derAnalyzer

if MPI
bin_PROGRAMS+=analyzeDataProperty_online compareDataSets_online
endif
analyzeDataProperty_SOURCES=analyzeDataProperty.c
analyzeDataProperty_LDADD=-lzc -L../zc/.libs -lm
analyzeDataProperty_LDFLAGS=$(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)

if R
analyzeDataProperty_LDADD+=../R/.libs/libzccallr.a
endif
compareDataSets_SOURCES=compareDataSets.c
compareDataSets_LDADD=-lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)
if R
compareDataSets_LDADD+=../R/.libs/libzccallr.a
endif
generateGNUPlot_SOURCES=generateGNUPlot.c
generateGNUPlot_LDADD=-lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)
if R
generateGNUPlot_LDADD+=../R/.libs/libzccallr.a
endif
generateReport_SOURCES=generateReport.c
generateReport_LDADD=-lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)
if R
generateReport_LDADD+=../R/.libs/libzccallr.a
endif
#testRscript_float_SOURCES=testRscript_float.c
#testRscript_float_LDADD=../zc/.libs/libzc.a -lm
#testRscript_double_SOURCES=testRscript_double.c
#testRscript_double_LDADD=../zc/.libs/libzc.a -lm
modifyZCConfig_SOURCES=modifyZCConfig.c
modifyZCConfig_LDADD= -lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)

if R
modifyZCConfig_LDADD+=../R/.libs/libzccallr.a
endif
runOfflineCase_SOURCES=runOfflineCase.c
runOfflineCase_LDADD=-lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)

derAnalyzer_SOURCES=derAnalyzer.cpp
derAnalyzer_LDADD=-lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)

if R
runOfflineCase_LDADD+=../R/.libs/libzccallr.a
endif
if MPI
analyzeDataProperty_online_SOURCES=analyzeDataProperty_online.c
analyzeDataProperty_online_LDADD=-lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)
if R
analyzeDataProperty_online_LDADD+=../R/.libs/libzccallr.a
endif
#heatdis_SOURCES=heatdis.c
#heatdis_LDADD=../zc/.libs/libzc.a -lm /home/sdi/Install/sz-1.4.13-install/lib/libsz.a
compareDataSets_online_SOURCES=compareDataSets_online.c
compareDataSets_online_LDADD=-lzc -L../zc/.libs -lm $(LIBPRESSIOOPT_HDR) $(LIBPRESSIOOPT_STATIC) $(LIBPRESSIOOPT_LIBS) $(LIBPRESSIOOPT_LIB)
if R
compareDataSets_online_LDADD+=../R/.libs/libzccallr.a
endif
endif

if R
all-local:
	./setRScript.sh
endif
