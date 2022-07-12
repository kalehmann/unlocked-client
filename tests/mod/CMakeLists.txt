set(TEST_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/check_module.c
)
target_sources( check_unlocked_client PRIVATE ${TEST_SOURCES} )
