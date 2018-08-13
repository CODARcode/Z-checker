include_directories (${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/third_party/json)

list (APPEND zserver_dependencies ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

if (LevelDB_FOUND) 
  list (APPEND zserver_dependencies ${LevelDB_LIBRARY})
endif ()

add_library (zserver SHARED zserver.cpp)
target_link_libraries (zserver ${zserver_dependencies})

# install (FILES zserver.h DESTINATION include)
install (TARGETS zserver DESTINATION lib)
# install (TARGETS test_zserver DESTINATION bin)
