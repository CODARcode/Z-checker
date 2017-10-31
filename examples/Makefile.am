AUTOMAKE_OPTIONS=foreign
AM_CFLAGS = -I../zc/include
bin_PROGRAMS=analyzeDataProperty compareDataSets generateGNUPlot generateReport modifyZCConfig

if MPI
bin_PROGRAMS+=analyzeDataProperty_online
endif

analyzeDataProperty_SOURCES=analyzeDataProperty.c
analyzeDataProperty_LDADD=../zc/.libs/libzc.a -lm
compareDataSets_SOURCES=compareDataSets.c
compareDataSets_LDADD=../zc/.libs/libzc.a -lm
generateGNUPlot_SOURCES=generateGNUPlot.c
generateGNUPlot_LDADD=../zc/.libs/libzc.a -lm
generateReport_SOURCES=generateReport.c
generateReport_LDADD=../zc/.libs/libzc.a -lm
#testRscript_float_SOURCES=testRscript_float.c
#testRscript_float_LDADD=../zc/.libs/libzc.a -lm
#testRscript_double_SOURCES=testRscript_double.c
#testRscript_double_LDADD=../zc/.libs/libzc.a -lm
modifyZCConfig_SOURCES=modifyZCConfig.c
modifyZCConfig_LDADD=../zc/.libs/libzc.a -lm
if MPI
analyzeDataProperty_online_SOURCES=analyzeDataProperty_online.c
analyzeDataProperty_online_LDADD=../zc/.libs/libzc.a -lm
endif
