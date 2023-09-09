cmake_minimum_required(VERSION 3.10.0)

set(flosiontest-headers
	SoundSourceTest.h
)

add_executable(flosiontest EXCLUDE_FROM_ALL FlosionTest.cpp ${flosiontest-headers})

set(gtest-headers
	gtest/internal/gtest-internal.h
	gtest/internal/gtest-string.h
	gtest/gtest-death-test.h
	gtest/gtest-message.h
	gtest/gtest-param-test.h
	gtest/gtest-printers.h
	gtest/gtest_prod.h
	gtest/gtest-test-part.h
	gtest/gtest-typed-test.h
)

set(gtest-srcs
	src/gtest-all.cc
	src/gtest-death-test.cc
	src/gtest-filepath.cc
	src/gtest-internal-inl.h
	src/gtest-port.cc
	src/gtest-printers.cc
	src/gtest-test-part.cc
	src/gtest-typed-test.cc
	src/gtest.cc
	src/gtest_main.cc
)

add_library(googletest STATIC ${gtest-headers} ${gtest-srcs})
target_include_directories(googletest PRIVATE ".")

target_link_libraries(flosiontest 
	googletest
	tims-gui
	flosion-core
	flosion-ui
	flosion-util
	flosion-objects
)