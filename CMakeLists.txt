cmake_minimum_required( VERSION 3.0 )
project( unlocked-client C )
cmake_policy(SET CMP0076 NEW)

###
# Build the iniparser library using its Makefile
##
set( INIPARSER_LIBRARY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/iniparser/libiniparser.a )
add_custom_target(build_iniparser ALL 
                   COMMAND ${CMAKE_MAKE_PROGRAM}
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/iniparser
                   COMMENT "Original iniparser makefile target")
add_library(iniparser STATIC IMPORTED)
set_property(TARGET iniparser APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(iniparser PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${INIPARSER_LIBRARY}")
add_dependencies(iniparser build_iniparser)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(vendor/libcheck)

enable_testing()
add_test(NAME check_unlocked_client COMMAND check_unlocked_client)

