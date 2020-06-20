## Google Test
#FetchContent_Declare(
#        googletest
#        GIT_REPOSITORY https://github.com/google/googletest.git
#        GIT_TAG        release-1.10.0
#)
#
#FetchContent_MakeAvailable(googletest)

add_executable(robustio_unittest robustio_unittest.cpp)
add_executable(sio_unittest sio_unittest.cpp)

# For static functions
target_include_directories(sio_unittest PUBLIC ../src)

# Just using c++17
target_compile_features(robustio_unittest PRIVATE cxx_std_17)
target_compile_features(sio_unittest PRIVATE cxx_std_17)

target_link_libraries(robustio_unittest PRIVATE GTest::GTest GTest::Main
        io)
target_link_libraries(sio_unittest PRIVATE GTest::GTest GTest::Main io)

# Adds tests
gtest_discover_tests(robustio_unittest)
gtest_discover_tests(sio_unittest)
