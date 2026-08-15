cmake_minimum_required(VERSION 3.16)

project(scanner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable automatic generation of Meta-Object compiler, UI compiler, and Resource compiler files
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find the required Qt5 components
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Multimedia)

# Define your source executable
add_executable(scanner 
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
