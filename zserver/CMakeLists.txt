include (CMakeRC.cmake)

cmrc_add_resource_library(zserver_public
  public/index.html
  public/style.css
  public/css/bootstrap.min.css
  public/css/d3-tip.min.css
  public/css/dc.css
  public/css/dc.min.css
  public/js/charts.js
  public/js/client.js
  public/js/gui.js
  public/js/master.js
  public/js/lib/bootstrap.min.js
  public/js/lib/d3-tip.min.js
  public/js/lib/d3.v3.min.js
  public/js/lib/d3.v4.min.js
  public/js/lib/highcharts.js
  public/js/lib/jquery-3.1.1.min.js
  NAMESPACE zserver_public
)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/third_party/websocketpp)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/third_party/json)

list (APPEND zserver_dependencies ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

if (RocksDB_FOUND)
  list (APPEND zserver_dependencies ${RocksDB_LIBRARY})
elseif (LevelDB_FOUND) 
  list (APPEND zserver_dependencies ${LevelDB_LIBRARY})
endif ()

if (PNG_FOUND)
  list (APPEND zserver_dependencies ${PNG_LIBRARIES})
endif ()

add_library (zserver zserver.cpp base64.cpp)
target_link_libraries (zserver ${zserver_dependencies} zserver_public)

# install (FILES zserver.h DESTINATION include)
install (TARGETS zserver DESTINATION lib)
# install (TARGETS test_zserver DESTINATION bin)
