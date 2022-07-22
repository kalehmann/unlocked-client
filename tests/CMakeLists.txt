include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

set(TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/check_unlocked_client.c
  ${CMAKE_CURRENT_SOURCE_DIR}/check_cli.c
  ${CMAKE_CURRENT_SOURCE_DIR}/check_https-client.c
)

add_executable(check_unlocked_client ${TEST_SOURCES})
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package( Threads REQUIRED )
target_link_libraries( check_unlocked_client PRIVATE check libunlocked Threads::Threads )
target_include_directories( check_unlocked_client PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../vendor/libcheck/src/
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mod)
