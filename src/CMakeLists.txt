add_subdirectory(mod)

set(LIB_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/cli.c
   ${CMAKE_CURRENT_SOURCE_DIR}/client.c
   ${CMAKE_CURRENT_SOURCE_DIR}/error.c
   ${CMAKE_CURRENT_SOURCE_DIR}/https-client.c
   ${CMAKE_CURRENT_SOURCE_DIR}/log.c
   ${CMAKE_CURRENT_SOURCE_DIR}/sockets.c
)

set(SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
target_sources( unlocked-client PRIVATE ${SOURCES} )
add_library( libunlocked STATIC ${LIB_SOURCES} )
target_include_directories( libunlocked PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/cJSON )

find_package( CURL REQUIRED )
find_package( OpenSSL REQUIRED )
target_link_libraries( libunlocked PRIVATE CURL::libcurl OpenSSL::SSL )
