cmake_minimum_required (VERSION 3.1.3)
project (zchecker)

set (CMAKE_DISABLE_SOURCE_CHANGES ON)
set (CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

find_package (MPI)
if (MPI_FOUND)
  set (CMAKE_C_COMPILER ${MPI_C_COMPILER})
  set (CMAKE_CXX_COMPILER ${MPI_CXX_COMPILER})
  add_definitions (-DHAVE_MPI)
endif ()

find_package (FFTW)
if (FFTW_FOUND)
  include_directories (${FFTW_INCLUDES})
  add_definitions (-DHAVE_FFTW3)
endif ()

find_package (NETCDF)
if (NETCDF_FOUND)
  include_directories (${NETCDF_INCLUDE_DIRS})
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/zc/include)
# include_directories (${ZLIB_INCLUDE_DIRS})

add_subdirectory (zc/src)
add_subdirectory (examples)
