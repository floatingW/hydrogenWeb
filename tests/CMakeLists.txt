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
add_executable(sharedBuf_unittest sharedBuf_unittest.cpp)
add_executable(EventLoop_test EventLoop_test.cpp)
add_executable(TimeStamp_unittest TimeStamp_unittest.cpp)

# For static functions
target_include_directories(sio_unittest PUBLIC ../src)

# Just using c++17
target_compile_features(robustio_unittest PRIVATE cxx_std_17)
target_compile_features(sio_unittest PRIVATE cxx_std_17)
target_compile_features(sharedBuf_unittest PRIVATE cxx_std_17)
target_compile_features(EventLoop_test PRIVATE cxx_std_17)
target_compile_features(TimeStamp_unittest PRIVATE cxx_std_17)

# link dependence
target_link_libraries(robustio_unittest PRIVATE GTest::GTest GTest::Main
        io)
target_link_libraries(sio_unittest PRIVATE GTest::GTest GTest::Main io)
target_link_libraries(sharedBuf_unittest PRIVATE GTest::GTest GTest::Main sem)
target_link_libraries(EventLoop_test PRIVATE network core)
target_link_libraries(TimeStamp_unittest PRIVATE core GTest::GTest GTest::Main)

# Adds tests
gtest_discover_tests(robustio_unittest)
gtest_discover_tests(sio_unittest)
gtest_discover_tests(sharedBuf_unittest)
gtest_discover_tests(TimeStamp_unittest)
