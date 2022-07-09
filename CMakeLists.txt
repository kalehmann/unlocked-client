cmake_minimum_required( VERSION 3.0 )
project( unlocked-client C )
cmake_policy(SET CMP0076 NEW)

set(SOURCES
   vendor/cJSON/cJSON.c
)
add_executable( unlocked-client ${SOURCES} )
set_target_properties( unlocked-client PROPERTIES LINKER_LANGUAGE C )
target_include_directories( unlocked-client PRIVATE vendor/cJSON )

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(vendor/libcheck)

find_package( CURL REQUIRED )
find_package( OpenSSL REQUIRED )
target_link_libraries(unlocked-client PRIVATE CURL::libcurl OpenSSL::SSL)

enable_testing()
add_test(NAME check_unlocked_client COMMAND check_unlocked_client)
