# This CMake file is for compiling as standalone application.
# To compile as LMMS Plugin, see plguin/ folder.

cmake_minimum_required(VERSION 3.16)

project(scanner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic generation of Meta-Object compiler, UI compiler, and Resource compiler files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Multimedia)

add_executable(scanner 
    src/PCH.h
    src/Configurations.h
    src/FormatFileSize.h
    src/WavInfo.h
    src/CreateWaveformImage.h
    src/WaveformDelegate.h
    src/ParseWavFile.h
    src/CustomTableWidget.h
    src/PopulateWavTable.h
    src/ProcessGUI.h
    src/KeysFilter.h
    src/main.cpp
)

# Link the Qt5 libraries to your application executable
target_link_libraries(scanner PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Multimedia
)

# Set deployment properties for Windows and macOS (removes console window background if target is GUI)
set_target_properties(scanner PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

# Define where the target should be installed
# sudo make install
install(TARGETS scanner DESTINATION bin)

# Testing: scanner_tests
enable_testing()
find_package(Qt5 REQUIRED COMPONENTS Test)
add_executable(scanner_tests tests/Checks.cpp)
target_link_libraries(scanner_tests PRIVATE Qt5::Test)
add_test(NAME scanner_tests COMMAND scanner_tests)

# Automatically run the tests binary after building 'scanner_tests'
add_custom_command(
    TARGET scanner_tests
    POST_BUILD
    COMMAND $<TARGET_FILE:scanner_tests>
    COMMENT "Running unit tests..."
)

# make && ctest --output-on-failure
# ctest
