cmake_minimum_required(VERSION 3.28)
project(simul)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(cmake/raylib-config.cmake)
find_package(Freetype)

include_directories(src ${raylib_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

add_library(
        Circuit
        STATIC
        src/Circuit.cpp
        src/Device.cpp
        src/Graphics.cpp
        src/Latch.cpp
        src/LogicGate.cpp
        src/Memory.cpp
        src/Oscillator.cpp
        src/Pin.cpp
        src/PushButton.h
        src/UtilityDevice.cpp
)

target_link_libraries(
        Circuit
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        m
)

add_library(
        LS
        STATIC
        src/IC/LS00.cpp
        src/IC/LS04.cpp
        src/IC/LS08.cpp
        src/IC/LS32.cpp
        src/IC/LS86.cpp
        src/IC/LS138.cpp
        src/IC/LS139.cpp
        src/IC/LS157.cpp
        src/IC/LS193.cpp
        src/IC/LS245.cpp
        src/IC/LS377.cpp
        src/IC/LS382.cpp
        src/IC/LS574.cpp
        src/IC/LS382.cpp
)

target_link_libraries(
        LS
        Circuit
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        m
)

add_executable(
        simul
        src/Simul.cpp
        src/Addr_Register.cpp
        src/ControlBus.cpp
        src/GP_Register.cpp
        src/Monitor.cpp
        src/System.cpp
        src/Mem_Register.cpp
)

target_link_libraries(
        simul
        Circuit
        LS
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        m
)

add_executable(
        ChipTester
        src/ChipTester/ChipTester.cpp
        src/IC/LS382.cpp
)

target_link_libraries(
        ChipTester
        Circuit
        LS
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        m
)

add_executable(
        TestBoard
        src/TestBoard.cpp
)

target_link_libraries(
        TestBoard
        Circuit
        LS
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        m
)
