# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("mazer")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include sub-projects.
#add_subdirectory ("mazer")

find_package(SDL2 REQUIRED)

add_executable(mazer
	"mazer/CGA/GraphicsMode.cpp"
	"mazer/CGA/TextMode.cpp"
	"mazer/SYSTEM/SYSTEM.cpp"
	"mazer/SYSTEM/Timer.cpp"
	"mazer/maze.cpp"
	"mazer/mazer.cpp"
	"mazer/moremath.cpp"
	"mazer/OEM437.cpp"
	"mazer/UI/Widget.cpp"
	"mazer/UI/Label.cpp"
	"mazer/UI/BorderedBox.cpp"
	"mazer/UI/HeartMeter.cpp"
	"mazer/UI/TimerDisplay.cpp"
	"mazer/UI/BlockCounter.cpp"
	"mazer/UI/HUD.cpp"
	"mazer/UI/PauseMenu.cpp"
	"mazer/UI/ScoreScreen.cpp"
	"mazer/UI/CurtainTransition.cpp"
	"mazer/bitIcons.h")

target_link_libraries(mazer SDL2::SDL2)
target_include_directories(mazer PRIVATE ${SDL2_INCLUDE_DIRS})

option(MAZER_LOGGING "Enable diagnostic logging output" OFF)
if(MAZER_LOGGING)
	target_compile_definitions(mazer PRIVATE MAZER_LOGGING)
endif()

if(WIN32)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		add_custom_command(TARGET mazer POST_BUILD			# Adds a post-build event to dossim
			COMMAND ${CMAKE_COMMAND} -E copy_if_different	# which executes "cmake - E copy_if_different..."
				"${SDL2MAIN_LIBRARY}/x64/SDL2.dll"			# <--this is in-file
				$<TARGET_FILE_DIR:mazer>)					# <--this is out-file path
	else()
		add_custom_command(TARGET mazer POST_BUILD			# Adds a post-build event to dossim
			COMMAND ${CMAKE_COMMAND} -E copy_if_different	# which executes "cmake - E copy_if_different..."
				"${SDL2MAIN_LIBRARY}/x86/SDL2.dll"			# <--this is in-file
				$<TARGET_FILE_DIR:mazer>)					# <--this is out-file path
	endif()

endif(WIN32)