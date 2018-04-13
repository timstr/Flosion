cmake_minimum_required(VERSION 3.9.0)

project(flosion C CXX)

add_executable(flosion Flosion.cpp)

add_subdirectory(tims-gui)
add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(util)
add_subdirectory(objects)

target_link_libraries(flosion
	tims-gui
	flosion-core
	flosion-ui
	flosion-util
	flosion-objects
)

# copy fonts to current build directory
add_custom_command(TARGET flosion POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/ui/fonts"
		"$<TARGET_FILE_DIR:flosion>/fonts"
)

# copy fonts to binary directory for debugger
add_custom_command(TARGET flosion POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${PROJECT_SOURCE_DIR}/ui/fonts"
		"${PROJECT_BINARY_DIR}/fonts"
)

# copy openal32.dll to executable directory on windows
if(WIN32)
	add_custom_command(TARGET flosion POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${SFML_DIR}/../../../bin/openal32.dll"
			"$<TARGET_FILE_DIR:flosion>"
	)
endif()

