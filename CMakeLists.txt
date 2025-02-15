cmake_minimum_required(VERSION 3.28)
project(simul)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

include(cmake/raylib-config.cmake)
find_package(Freetype)

include_directories(src ${raylib_INCLUDE_DIRS} ${FREETYPE_INCLUDE_DIRS})

add_library(
        Lib
        STATIC
        src/Lib/Error.cpp
        src/Lib/FileBuffer.cpp
        src/Lib/Lib.h
        src/Lib/Logging.cpp
        src/Lib/Options.cpp
        src/Lib/Resolve.cpp
        src/Lib/Result.h
        src/Lib/ScopeGuard.h
        src/Lib/Set.h
        src/Lib/SimpleFormat.h
        src/Lib/TaggedUnion.h
        src/Lib/Unescape.cpp

        src/Lib/Type/Type.cpp
        src/Lib/Type/Value.cpp

        src/Lib/Lexer/Config.cpp
        src/Lib/Lexer/Lexer.cpp
        src/Lib/Lexer/Token.cpp
        src/Lib/Lexer/TokenKind.cpp

        src/Lib/Grammar/Grammar.cpp
        src/Lib/Grammar/Parser.h
        src/Lib/Grammar/Rule.cpp
        src/Lib/Grammar/Sequence.cpp
        src/Lib/Grammar/Symbol.cpp

        src/Lib/GrammarParser/GrammarParser.cpp
)

add_library(
        Circuit
        STATIC
        src/Circuit/Circuit.cpp
        src/Circuit/Device.cpp
        src/Circuit/Graphics.cpp
        src/Circuit/Latch.cpp
        src/Circuit/LogicGate.cpp
        src/Circuit/Memory.cpp
        src/Circuit/Oscillator.cpp
        src/Circuit/Pin.cpp
        src/Circuit/PushButton.h
        src/Circuit/UtilityDevice.cpp
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
        src/IC/LS02.cpp
        src/IC/LS04.cpp
        src/IC/LS08.cpp
        src/IC/LS21.cpp
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
        src/App/Simul.cpp
        src/App/Addr_Register.cpp
        src/App/ALU.cpp
        src/App/ControlBus.cpp
        src/App/GP_Register.cpp
        src/App/MicroCode.cpp
        src/App/Monitor.cpp
        src/App/System.cpp
        src/App/Mem_Register.cpp
)

target_link_libraries(
        simul
        Lib
        LS
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
)

#add_executable(
#        ChipTester
#        src/ChipTester/ChipTester.cpp
#        src/IC/LS382.cpp
#)
#
#target_link_libraries(
#        ChipTester
#        Circuit
#        LS
#        ${raylib_LIBRARIES}
#        ${FREETYPE_LIBRARIES}
#        m
#)

add_executable(
        TestBoard
        src/TestBoard/TestBoard.cpp
)

target_link_libraries(
        TestBoard
        Circuit
        LS
        ${raylib_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        m
)
