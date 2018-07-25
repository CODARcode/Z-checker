cmake_minimum_required (VERSION 3.1.3)
project (zchecker)

set (CMAKE_DISABLE_SOURCE_CHANGES ON)
set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

find_package (FFTW)
if (FFTW_FOUND)
  include_directories (${FFTW_INCLUDES})
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/zc/include)
# include_directories (${ZLIB_INCLUDE_DIRS})

add_subdirectory (zc/src)
