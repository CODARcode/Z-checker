#AM_CFLAGS = -I./include
#LDFLAGS=-fPIC -shared

AUTOMAKE_OPTIONS=foreign
include_HEADERS=src/HDF5Reader.h

lib_LTLIBRARIES=libhdf5reader.la

libhdf5reader_la_LDFLAGS = -version-info  0:1:0
libhdf5reader_la_SOURCES=src/HDF5Reader.c
