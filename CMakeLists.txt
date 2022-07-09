cmake_minimum_required( VERSION 3.0 )
project( unlocked-client C )
cmake_policy(SET CMP0076 NEW)

set(SOURCES
   vendor/cJSON/cJSON.c
)
add_executable( unlocked-client ${SOURCES} )
set_target_properties( unlocked-client PROPERTIES LINKER_LANGUAGE C )

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(vendor/libcheck)

target_link_libraries( unlocked-client PRIVATE libunlocked )
enable_testing()
add_test(NAME check_unlocked_client COMMAND check_unlocked_client)
