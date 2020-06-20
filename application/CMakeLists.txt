# hydrogenWeb
add_executable(hydrogenWeb hydrogenWeb.cpp)
target_compile_features(hydrogenWeb PRIVATE cxx_std_17)

target_link_libraries(hydrogenWeb PRIVATE io)

# echoClient
add_executable(echoClient echoClient.cpp)
target_compile_features(echoClient PRIVATE cxx_std_17)

target_link_libraries(echoClient PRIVATE io network)

# echoServer
add_executable(echoServer echoServer.cpp)
target_compile_features(echoServer PRIVATE cxx_std_17)

target_link_libraries(echoServer PRIVATE io network)
