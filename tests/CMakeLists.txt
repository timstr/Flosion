cmake_minimum_required(VERSION 3.10.0)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(gtest)

set(flosion_tests_srcs
	src/SoundNodeTest.cpp
)

add_executable(flosion_tests ${flosion_tests_srcs} main.cpp)

target_link_libraries(flosion_tests
	PUBLIC flosion_core
	PUBLIC gtest
	PUBLIC gtest_main
)

set_property(TARGET flosion_tests PROPERTY CXX_STANDARD 17)
