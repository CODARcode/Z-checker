# - Find SZ
# Find the native SZ includes and library
#
#  SZ_INCLUDES    - where to find sz.h
#  SZ_LIBRARIES   - List of libraries when using SZ.
#  SZ_FOUND       - True if SZ found.

if (SZ_INCLUDES)
  # Already in cache, be silent
  set (SZ_FIND_QUIETLY TRUE)
endif (SZ_INCLUDES)

find_path (SZ_INCLUDES sz.h)

find_library (SZ_LIBRARIES NAMES sz)

# handle the QUIETLY and REQUIRED arguments and set SZ_FOUND to TRUE if
# all listed variables are TRUE
include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (SZ DEFAULT_MSG SZ_LIBRARIES SZ_INCLUDES)

mark_as_advanced (SZ_LIBRARIES SZ_INCLUDES)
