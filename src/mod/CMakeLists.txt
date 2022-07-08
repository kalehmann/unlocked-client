set(SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/mod_stdout.c
   ${CMAKE_CURRENT_SOURCE_DIR}/module.c
)

target_sources( unlocked-client PRIVATE ${SOURCES} )
