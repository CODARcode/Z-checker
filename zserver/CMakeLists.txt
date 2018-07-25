include_directories (${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp)

add_library (zserver zserver.cpp)
target_link_libraries (zserver ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

# install (FILES zserver.h DESTINATION include)
install (TARGETS zserver DESTINATION lib)
# install (TARGETS test_zserver DESTINATION bin)
