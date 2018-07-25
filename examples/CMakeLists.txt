add_executable (analyzeDataProperty analyzeDataProperty.c)
target_link_libraries (analyzeDataProperty zc)

add_executable (analyzeDataProperty_multivars analyzeDataProperty_multivars.c)
target_link_libraries (analyzeDataProperty_multivars zc)

add_executable (compareDataSets compareDataSets.c)
target_link_libraries (compareDataSets zc)

add_executable (generateGNUPlot generateGNUPlot.c)
target_link_libraries (generateGNUPlot zc)

add_executable (generateReport generateReport.c)
target_link_libraries (generateReport zc)

add_executable (modifyZCConfig modifyZCConfig.c)
target_link_libraries (modifyZCConfig zc)

add_executable (runOfflineCase runOfflineCase.c)
target_link_libraries (runOfflineCase zc)

add_executable (testRscript_double testRscript_double.c)
target_link_libraries (testRscript_double zc)

add_executable (testRscript_float testRscript_float.c)
target_link_libraries (testRscript_float zc)

add_executable (testRscript_readBinFloat testRscript_readBinFloat.c)
target_link_libraries (testRscript_readBinFloat zc)

if (MPI_FOUND)
  add_executable (analyzeDataProperty_online analyzeDataProperty_online.c)
  target_link_libraries (analyzeDataProperty_online zc)

  add_executable (compareDataSets_online compareDataSets_online.c)
  target_link_libraries (compareDataSets_online zc)
endif ()

## needs sz
# add_executable (heatdis heatdis.c)
# target_link_libraries (heatdis zc)

# add_executable (testdouble_CompDecomp testdouble_CompDecomp.c)
# target_link_libraries (testdouble_CompDecomp zc)

