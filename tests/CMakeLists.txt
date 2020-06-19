## Google Test
#FetchContent_Declare(
#        googletest
#        GIT_REPOSITORY https://github.com/google/googletest.git
#        GIT_TAG        release-1.10.0
#)
#
#FetchContent_MakeAvailable(googletest)

add_executable(robustio_unittest robustio_unittest.cpp)

# Just using c++17
target_compile_features(robustio_unittest PRIVATE cxx_std_17)

target_link_libraries(robustio_unittest PRIVATE GTest::GTest GTest::Main
        robustio)

# Adds tests
gtest_discover_tests(robustio_unittest)
