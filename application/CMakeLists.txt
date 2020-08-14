# hydrogenWeb
add_executable(hydrogenWeb hydrogenWeb.cpp)
target_compile_features(hydrogenWeb PRIVATE cxx_std_17)

target_link_libraries(hydrogenWeb PRIVATE network core spdlog::spdlog sqlite3 -lbcrypt)

# echoClient
add_executable(echoClient echoClient.cpp)
target_compile_features(echoClient PRIVATE cxx_std_17)

target_link_libraries(echoClient PRIVATE io network)

# echoServer (iterative)
add_executable(echoServer echoServer.cpp)
target_compile_features(echoServer PRIVATE cxx_std_17)

target_link_libraries(echoServer PRIVATE io network)

# echoServer (multi-process)
add_executable(echoServerp echoServerp.cpp)
target_compile_features(echoServerp PRIVATE cxx_std_17)

target_link_libraries(echoServerp PRIVATE io network)

# echoServer (prethreading)
add_executable(echoServert echoServert.cpp)
target_compile_features(echoServert PRIVATE cxx_std_17)

target_link_libraries(echoServert PRIVATE io network sem)

# echoServer (I/O multiplexing)
add_executable(echoServerm echoServerm.cpp)
target_compile_features(echoServerm PRIVATE cxx_std_17)

target_link_libraries(echoServerm PRIVATE io network)

