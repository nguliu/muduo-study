
add_executable(reactor_test01 Reactor_test01.cc)
target_link_libraries(reactor_test01 muduo_net)

add_executable(reactor_test02 Reactor_test02.cc)
target_link_libraries(reactor_test02 muduo_net)

add_executable(reactor_test03 Reactor_test03.cc)
target_link_libraries(reactor_test03 muduo_net)

add_executable(reactor_test04 Reactor_test04.cc)
target_link_libraries(reactor_test04 muduo_net)

add_executable(reactor_test05 Reactor_test05.cc)
target_link_libraries(reactor_test05 muduo_net)

add_executable(reactor_test06 Reactor_test06.cc)
target_link_libraries(reactor_test06 muduo_net)

add_executable(reactor_test07 Reactor_test07.cc)
target_link_libraries(reactor_test07 muduo_net)

add_executable(reactor_test07_asyn Reactor_test07_asyn.cc)
target_link_libraries(reactor_test07_asyn muduo_net)

#add_executable(reactor_test08 Reactor_test08.cc)
#target_link_libraries(reactor_test08 muduo_net)
#
#add_executable(reactor_test09 Reactor_test09.cc)
#target_link_libraries(reactor_test09 muduo_net)
#
#add_executable(reactor_test10 Reactor_test10.cc)
#target_link_libraries(reactor_test10 muduo_net)

add_executable(reactor_test11 Reactor_test11.cc)
target_link_libraries(reactor_test11 muduo_net)

add_executable(reactor_test12 Reactor_test12.cc)
target_link_libraries(reactor_test12 muduo_net)

add_executable(reactor_test13 Reactor_test13.cc)
target_link_libraries(reactor_test13 muduo_net)

add_executable(tcpclient_test TcpClient_test.cc)
target_link_libraries(tcpclient_test muduo_net)

add_executable(test_server_shutdown.client Test_server_shutdown.client.c)
