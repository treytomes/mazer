cmake_minimum_required(VERSION 3.14)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.3
)
FetchContent_MakeAvailable(Catch2)

# Sources shared with the game that have no SDL dependency at runtime.
# GraphicsMode.cpp is included because it only calls SDL at initialize()/render();
# we never call those in tests — the mock overrides the virtual interface.
set(MAZER_TESTABLE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/CGA/GraphicsMode.cpp
    ${CMAKE_SOURCE_DIR}/src/maze.cpp
    ${CMAKE_SOURCE_DIR}/src/moremath.cpp
    ${CMAKE_SOURCE_DIR}/src/OEM437.cpp
    ${CMAKE_SOURCE_DIR}/src/SYSTEM/SYSTEM.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/Widget.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/Label.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/BorderedBox.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/HeartMeter.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/TimerDisplay.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/BlockCounter.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/HUD.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/PauseMenu.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/CurtainTransition.cpp
    ${CMAKE_SOURCE_DIR}/src/UI/ScoreScreen.cpp
)

add_executable(mazer_tests
    test_stack.cpp
    test_moremath.cpp
    test_maze.cpp
    test_widget.cpp
    test_label.cpp
    test_score_screen.cpp
    test_curtain.cpp
    ${MAZER_TESTABLE_SOURCES}
)

target_include_directories(mazer_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${SDL2_INCLUDE_DIRS}
)
target_link_libraries(mazer_tests PRIVATE Catch2::Catch2WithMain SDL2::SDL2)

# Code coverage (GCC/Clang only)
option(MAZER_COVERAGE "Enable gcov code coverage" OFF)
if(MAZER_COVERAGE)
    target_compile_options(mazer_tests PRIVATE --coverage -O0 -g)
    target_link_options(mazer_tests PRIVATE --coverage)
endif()

include(CTest)
include(Catch)
catch_discover_tests(mazer_tests)
