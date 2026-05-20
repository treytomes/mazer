# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("mazer")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include sub-projects.
#add_subdirectory ("mazer")

find_package(SDL2 REQUIRED)

enable_testing()
add_subdirectory(tests)

add_executable(mazer
	"src/CGA/GraphicsMode.cpp"
	"src/CGA/TextMode.cpp"
	"src/SYSTEM/SYSTEM.cpp"
	"src/SYSTEM/Timer.cpp"
	"src/maze.cpp"
	"src/mazer.cpp"
	"src/moremath.cpp"
	"src/OEM437.cpp"
	"src/UI/Widget.cpp"
	"src/UI/Label.cpp"
	"src/UI/BorderedBox.cpp"
	"src/UI/HeartMeter.cpp"
	"src/UI/TimerDisplay.cpp"
	"src/UI/BlockCounter.cpp"
	"src/UI/HUD.cpp"
	"src/UI/PauseMenu.cpp"
	"src/UI/ScoreScreen.cpp"
	"src/UI/CurtainTransition.cpp"
	"src/UI/MainMenu.cpp"
	"src/UI/SfxEditor.cpp"
	"src/UI/SettingsScreen.cpp"
	"src/Settings.cpp"
	"src/AUDIO/SfxParams.cpp"
	"src/AUDIO/SfxGenerator.cpp"
	"src/AUDIO/AudioSystem.cpp"
	"src/AUDIO/SfxPlayer.cpp"
	"src/AUDIO/BgmParams.cpp"
	"src/AUDIO/BgmPlayer.cpp"
	"src/bitIcons.h")

# SDL2::SDL2main provides the WinMain entry-point shim required on Windows.
# It must be listed before SDL2::SDL2 so the linker resolves it first.
if(TARGET SDL2::SDL2main)
	target_link_libraries(mazer SDL2::SDL2main)
endif()
target_link_libraries(mazer SDL2::SDL2)
target_include_directories(mazer PRIVATE ${SDL2_INCLUDE_DIRS})

option(MAZER_LOGGING "Enable diagnostic logging output" OFF)
if(MAZER_LOGGING)
	target_compile_definitions(mazer PRIVATE MAZER_LOGGING)
endif()

# Copy SDL2.dll next to the executable on Windows so it can run in-place.
if(WIN32)
	add_custom_command(TARGET mazer POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
			$<TARGET_FILE:SDL2::SDL2>
			$<TARGET_FILE_DIR:mazer>)
endif()