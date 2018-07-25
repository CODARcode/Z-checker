set (zc_sources
  DynamicByteArray.c       ZC_CompareData.c         ZC_DataProperty_float.c  ZC_ReportGenerator.c     ZC_rw.c
  DynamicDoubleArray.c     ZC_CompareData_double.c  ZC_DataSetHandler.c      ZC_conf.c                ZC_util.c
  DynamicFloatArray.c      ZC_CompareData_float.c   ZC_gnuplot.c             dictionary.c
  DynamicIntArray.c        ZC_DataProperty.c        ZC_Hashtable.c           ZC_latex.c               iniparser.c
  ZC_ByteToolkit.c         ZC_DataProperty_double.c ZC_quicksort.c           zc.c
)

if (FFTW_FOUND)
  list (APPEND zc_sources ZC_FFTW3_math.c)
  list (APPEND zc_dependencies ${FFTW_LIBRARIES})
endif ()

# ZC_R_math.c // R

add_library (zc SHARED ${zc_sources})
target_link_libraries (zc ${zc_dependencies})
