cmake_minimum_required(VERSION 3.9.0)

project(flosion C CXX)

add_executable(flosion Flosion.cpp)

add_subdirectory(tims-gui)
add_subdirectory(core)
add_subdirectory(objects)
add_subdirectory(ui)
add_subdirectory(util)

target_link_libraries(flosion
	PUBLIC tims-gui
	PUBLIC flosion-core
	PUBLIC flosion-objects
	PUBLIC flosion-ui
	PUBLIC flosion-util
)

target_include_directories(flosion
	PUBLIC tims-gui
	PUBLIC flosion-core
	PUBLIC flosion-objects
	PUBLIC flosion-ui
	PUBLIC flosion-util
)
