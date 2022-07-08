add_subdirectory(mod)

set(SOURCES
   ${CMAKE_CURRENT_SOURCE_DIR}/cli.c
   ${CMAKE_CURRENT_SOURCE_DIR}/client.c
   ${CMAKE_CURRENT_SOURCE_DIR}/error.c
   ${CMAKE_CURRENT_SOURCE_DIR}/https-client.c
   ${CMAKE_CURRENT_SOURCE_DIR}/log.c
   ${CMAKE_CURRENT_SOURCE_DIR}/main.c
   ${CMAKE_CURRENT_SOURCE_DIR}/sockets.c
)
target_sources( unlocked-client PRIVATE ${SOURCES} )
