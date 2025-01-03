cmake_minimum_required(VERSION 3.28)
project(simul)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(cmake/raylib-config.cmake)
find_package(Freetype)

include_directories(src)

add_executable(
        simul
        src/Simul.cpp
        src/Graphics.h
        src/Pin.cpp
        src/Device.h
        src/Circuit.h
        src/GP_Register.cpp
        src/Latch.cpp
        src/LogicGate.cpp
        src/LS04.cpp
        src/LS08.cpp
        src/LS32.cpp
        src/LS86.cpp
        src/LS138.cpp
        src/LS139.cpp
        src/LS245.cpp
        src/LS377.cpp
        src/LS574.cpp
        src/Oscillator.cpp
        src/UtilityDevice.cpp
        src/GP_Register.cpp
        src/GP_Register.h
)

target_include_directories(simul PRIVATE ${raylib_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(
        simul
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        m
)
