include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/check_unlocked_client.c
)

add_executable(check_unlocked_client ${TEST_SOURCES})
target_include_directories( check_unlocked_client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/libcheck/src/
)
target_link_libraries( check_unlocked_client PRIVATE check )
