cmake_minimum_required(VERSION 3.24)

project(SynpleEngine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SYNPLE_BUILD_TESTS "Build engine functionality tests" ${PROJECT_IS_TOP_LEVEL})
option(ENABLE_SANITIZERS "Build engine tests with ASan and UBSan" OFF)

add_library(synple_math INTERFACE)
target_include_directories(synple_math INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/source")

add_library(synple_midi STATIC
    source/midi/MidiLearnMap.cpp
    source/midi/MidiProcessor.cpp
)
target_include_directories(synple_midi PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")

add_library(synple_dsp STATIC
    source/dsp/AudioBuffer.cpp
    source/dsp/Filter.cpp
    source/dsp/Goertzel.cpp
    source/dsp/Mean.cpp
    source/dsp/NoiseGenerator.cpp
    source/dsp/Rms.cpp
)
target_include_directories(synple_dsp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")
target_link_libraries(synple_dsp PUBLIC synple_math)

add_library(synple_synth STATIC
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
target_include_directories(synple_synth PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/source")
target_link_libraries(synple_synth PUBLIC synple_dsp synple_midi synple_math)

set_target_properties(synple_midi synple_dsp synple_synth PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Synple::Math ALIAS synple_math)
add_library(Synple::Midi ALIAS synple_midi)
add_library(Synple::Dsp ALIAS synple_dsp)
add_library(Synple::Synth ALIAS synple_synth)

if (SYNPLE_BUILD_TESTS)
    enable_testing()
    include(CTest)

    include(FetchContent)
    FetchContent_Declare(
        doctest
        GIT_REPOSITORY https://github.com/doctest/doctest.git
        GIT_TAG v2.5.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(doctest)
    FetchContent_GetProperties(doctest SOURCE_DIR SYNPLE_DOCTEST_SOURCE_DIR)

    add_executable(MidiTests tests/MidiLearnMapTest.cpp tests/MidiProcessorTest.cpp)
    add_executable(DspTests tests/AudioBufferTest.cpp tests/FilterTest.cpp)
    add_executable(SynthTests
        tests/PolyBlepOscillatorTest.cpp
        tests/PulseOscillatorTest.cpp
        tests/SawtoothOscillatorTest.cpp
        tests/SineOscillatorTest.cpp
    )

    target_link_libraries(MidiTests PRIVATE Synple::Midi doctest::doctest_with_main)
    target_link_libraries(DspTests PRIVATE Synple::Dsp doctest::doctest_with_main)
    target_link_libraries(SynthTests PRIVATE Synple::Synth doctest::doctest_with_main)

    set(kTestTargets MidiTests DspTests SynthTests)

    if (ENABLE_SANITIZERS)
        set(kSanitizerFlags -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
        foreach(LIB IN ITEMS synple_midi synple_dsp synple_synth)
            target_compile_options(${LIB} PRIVATE ${kSanitizerFlags})
            target_link_options(${LIB} PRIVATE -fsanitize=address,undefined)
        endforeach()
        foreach(TARGET IN LISTS kTestTargets)
            target_compile_options(${TARGET} PRIVATE ${kSanitizerFlags})
            target_link_options(${TARGET} PRIVATE -fsanitize=address,undefined)
        endforeach()
    endif()

    include("${SYNPLE_DOCTEST_SOURCE_DIR}/scripts/cmake/doctest.cmake")
    foreach(TARGET IN LISTS kTestTargets)
        doctest_discover_tests(${TARGET})
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND "$<TARGET_FILE:${TARGET}>"
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "Running ${TARGET}..."
            VERBATIM
        )
    endforeach()
endif()