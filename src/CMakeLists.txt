set(LIB_SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/cli.c
   ${CMAKE_CURRENT_SOURCE_DIR}/client.c
   ${CMAKE_CURRENT_SOURCE_DIR}/error.c
   ${CMAKE_CURRENT_SOURCE_DIR}/https-client.c
   ${CMAKE_CURRENT_SOURCE_DIR}/log.c
   ${CMAKE_CURRENT_SOURCE_DIR}/sockets.c
   ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/cJSON/cJSON.c
)

set(SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)
add_executable( unlocked-client ${SOURCES} )
set_target_properties( unlocked-client PROPERTIES LINKER_LANGUAGE C )
target_include_directories( unlocked-client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/iniparser/src
)

add_library( libunlocked STATIC ${LIB_SOURCES} )
target_include_directories( libunlocked PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/cJSON
  ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/iniparser/src
)
add_subdirectory(mod)

find_package( CURL REQUIRED )
find_package( OpenSSL REQUIRED )
find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)

target_link_libraries( libunlocked PRIVATE CURL::libcurl iniparser OpenSSL::SSL )
target_link_libraries( libunlocked INTERFACE PkgConfig::SYSTEMD)
target_link_libraries( unlocked-client PRIVATE libunlocked )
