set (zc_headers 
  DynamicByteArray.h   ZC_ByteToolkit.h     ZC_Hashtable.h       ZC_latex.h           dictionary.h	ZC_ssim.h
  DynamicDoubleArray.h ZC_CompareData.h     ZC_ReportGenerator.h ZC_quicksort.h       iniparser.h
  DynamicFloatArray.h  ZC_DataProperty.h    ZC_conf.h            ZC_rw.h              zc.h
  DynamicIntArray.h    ZC_DataSetHandler.h  ZC_gnuplot.h         ZC_util.h            3rdParty/autocorr.h
  der.hpp  idx.hpp  matrix.hpp  m_basic.hpp  m_der.hpp  m_der_metrics.hpp  m_norm.hpp  norm.hpp  others.hpp  ssim.hpp)

install (FILES ${zc_headers} DESTINATION include)

