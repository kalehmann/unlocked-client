set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/mod_sd_socket.c
    ${CMAKE_CURRENT_SOURCE_DIR}/mod_stdout.c
    ${CMAKE_CURRENT_SOURCE_DIR}/module.c)

target_sources(libunlocked PRIVATE ${SOURCES})
