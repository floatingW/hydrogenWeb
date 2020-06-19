# Setting headers just for showing them up in IDEs
file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${hydrogenWeb_SOURCE_DIR}/include/robustio/*.hpp")

# Will be static or dynamic based on user setting
add_library(robustio robustio.cpp ${HEADER_LIST})

# Include
target_include_directories(robustio PUBLIC ../include)

# Just c++17
target_compile_features(robustio PUBLIC cxx_std_17)
