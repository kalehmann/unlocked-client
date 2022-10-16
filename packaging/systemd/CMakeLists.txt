configure_file(${CMAKE_CURRENT_SOURCE_DIR}/unlocked-client@.service.in
               ${PROJECT_BINARY_DIR}/systemd/unlocked-client@.service)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/unlocked-client@.socket.in
               ${PROJECT_BINARY_DIR}/systemd/unlocked-client@.socket)

install(FILES ${PROJECT_BINARY_DIR}/systemd/unlocked-client@.service DESTINATION ${SYSTEMD_UNIT_DIR} )
install(FILES ${PROJECT_BINARY_DIR}/systemd/unlocked-client@.socket DESTINATION ${SYSTEMD_UNIT_DIR} )
