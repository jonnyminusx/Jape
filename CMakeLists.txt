cmake_minimum_required(VERSION 3.24)

project(Jape VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(JAPE_BUILD_TESTS "Build engine functionality tests" ${PROJECT_IS_TOP_LEVEL})
option(ENABLE_SANITIZERS "Build engine tests with ASan and UBSan" OFF)

add_library(jape_math INTERFACE)
target_include_directories(jape_math INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/source")

add_library(jape_midi STATIC
    source/midi/MidiLearnMap.cpp
    source/midi/MidiProcessor.cpp
)
target_include_directories(jape_midi PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")

add_library(jape_dsp STATIC
    source/dsp/AudioBuffer.cpp
    source/dsp/Filter.cpp
    source/dsp/Goertzel.cpp
    source/dsp/Mean.cpp
    source/dsp/NoiseGenerator.cpp
    source/dsp/Rms.cpp
)
target_include_directories(jape_dsp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")
target_link_libraries(jape_dsp PUBLIC jape_math)

add_library(jape_synth STATIC
    source/synth/Envelope.cpp
    source/synth/PolyBlepOscillator.cpp
    source/synth/PulseOscillator.cpp
    source/synth/SawtoothOscillator.cpp
    source/synth/Output.cpp
    source/synth/Presets.cpp
    source/synth/SineOscillator.cpp
    source/synth/Synth.cpp
    source/synth/Voice.cpp
)
target_include_directories(jape_synth PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")
target_link_libraries(jape_synth PUBLIC jape_dsp jape_midi jape_math)

set_target_properties(jape_midi jape_dsp jape_synth PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Jape::Math ALIAS jape_math)
add_library(Jape::Midi ALIAS jape_midi)
add_library(Jape::Dsp ALIAS jape_dsp)
add_library(Jape::Synth ALIAS jape_synth)

if (JAPE_BUILD_TESTS)
    enable_testing()
    include(CTest)

    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)
    FetchContent_GetProperties(Catch2 SOURCE_DIR JAPE_CATCH2_SOURCE_DIR)

    add_executable(MidiTests tests/MidiLearnMapTest.cpp tests/MidiProcessorTest.cpp)
    add_executable(DspTests tests/AudioBufferTest.cpp tests/FilterTest.cpp)
    add_executable(SynthTests
        tests/PolyBlepOscillatorTest.cpp
        tests/PulseOscillatorTest.cpp
        tests/SawtoothOscillatorTest.cpp
        tests/SineOscillatorTest.cpp
    )

    target_link_libraries(MidiTests PRIVATE Jape::Midi Catch2::Catch2WithMain)
    target_link_libraries(DspTests PRIVATE Jape::Dsp Catch2::Catch2WithMain)
    target_link_libraries(SynthTests PRIVATE Jape::Synth Catch2::Catch2WithMain)

    set(kTestTargets MidiTests DspTests SynthTests)

    if (ENABLE_SANITIZERS)
        set(kSanitizerFlags -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
        foreach(LIB IN ITEMS jape_midi jape_dsp jape_synth)
            target_compile_options(${LIB} PRIVATE ${kSanitizerFlags})
            target_link_options(${LIB} PRIVATE -fsanitize=address,undefined)
        endforeach()
        foreach(TARGET IN LISTS kTestTargets)
            target_compile_options(${TARGET} PRIVATE ${kSanitizerFlags})
            target_link_options(${TARGET} PRIVATE -fsanitize=address,undefined)
        endforeach()
    endif()

    include("${JAPE_CATCH2_SOURCE_DIR}/extras/Catch.cmake")
    foreach(TARGET IN LISTS kTestTargets)
        catch_discover_tests(${TARGET})
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND "$<TARGET_FILE:${TARGET}>"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "Running ${TARGET}..."
            VERBATIM
        )
    endforeach()
endif()