# Setting headers just for showing them up in IDEs
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${hydrogenWeb_SOURCE_DIR}/include/*.hpp")

# Will be static or dynamic based on user setting
add_library(io sio.cpp robustio.cpp ${HEADER_LIST})
add_library(network socket.cpp ${HEADER_LIST})
add_library(unixUtility unixUtility.cpp ${HEADER_LIST})

# Include
target_include_directories(io PUBLIC ../include)
target_include_directories(network PUBLIC ../include)
target_include_directories(unixUtility PUBLIC ../include)

# link dependence
target_link_libraries(network PRIVATE unixUtility)

# Just c++17
target_compile_features(io PUBLIC cxx_std_17)
target_compile_features(network PUBLIC cxx_std_17)
target_compile_features(unixUtility PUBLIC cxx_std_17)