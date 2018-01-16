AUTOMAKE_OPTIONS=foreign
if FFTW3
AM_CFLAGS = -I../zc/include -lfftw3 -DHAVE_FFTW3
else
AM_CFLAGS = -I../zc/include
endif
if R
AM_CFLAGS += $(R_HDR) -lR -I../R/src -DHAVE_R
endif
if MPI
AM_CFLAGS += -DHAVE_MPI
endif
bin_PROGRAMS=analyzeDataProperty compareDataSets generateGNUPlot generateReport modifyZCConfig runOfflineCase

if MPI
bin_PROGRAMS+=analyzeDataProperty_online compareDataSets_online
endif
analyzeDataProperty_SOURCES=analyzeDataProperty.c
analyzeDataProperty_LDADD=../zc/.libs/libzc.a -lm
if R
analyzeDataProperty_LDADD+=../R/.libs/libzccallr.a
endif
compareDataSets_SOURCES=compareDataSets.c
compareDataSets_LDADD=../zc/.libs/libzc.a -lm
if R
compareDataSets_LDADD+=../R/.libs/libzccallr.a
endif
generateGNUPlot_SOURCES=generateGNUPlot.c
generateGNUPlot_LDADD=../zc/.libs/libzc.a -lm
if R
generateGNUPlot_LDADD+=../R/.libs/libzccallr.a
endif
generateReport_SOURCES=generateReport.c
generateReport_LDADD=../zc/.libs/libzc.a -lm
if R
generateReport_LDADD+=../R/.libs/libzccallr.a
endif
#testRscript_float_SOURCES=testRscript_float.c
#testRscript_float_LDADD=../zc/.libs/libzc.a -lm
#testRscript_double_SOURCES=testRscript_double.c
#testRscript_double_LDADD=../zc/.libs/libzc.a -lm
modifyZCConfig_SOURCES=modifyZCConfig.c
modifyZCConfig_LDADD=../zc/.libs/libzc.a -lm
if R
modifyZCConfig_LDADD+=../R/.libs/libzccallr.a
endif
runOfflineCase_SOURCES=runOfflineCase.c
runOfflineCase_LDADD=../zc/.libs/libzc.a -lm
if R
runOfflineCase_LDADD+=../R/.libs/libzccallr.a
endif
if MPI
analyzeDataProperty_online_SOURCES=analyzeDataProperty_online.c
analyzeDataProperty_online_LDADD=../zc/.libs/libzc.a -lm
if R
analyzeDataProperty_online_LDADD+=../R/.libs/libzccallr.a
endif
compareDataSets_online_SOURCES=compareDataSets_online.c
compareDataSets_online_LDADD=../zc/.libs/libzc.a -lm
if R
compareDataSets_online_LDADD+=../R/.libs/libzccallr.a
endif
endif

all-local:
	./setRScript.sh
