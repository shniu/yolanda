add_executable(client_conn client_conn.c)
target_link_libraries(client_conn yolanda)

add_executable(make_socket make_socket.c)
target_link_libraries(make_socket yolanda)

