
add_executable(homeclient homeclient.c)
target_link_libraries(homeclient yolanda)

add_executable(homeserver homeserver.c)
target_link_libraries(homeserver yolanda)

add_executable(telnet_cli telnet_cli.c)
target_link_libraries(telnet_cli yolanda)

add_executable(telnet_srv telnet_srv.c)
target_link_libraries(telnet_srv yolanda)
