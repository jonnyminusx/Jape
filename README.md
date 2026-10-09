# Synple Engine

JUCE-free C++20 engine layers for Synple-based audio plugins. The public CMake targets are `Synple::Math`, `Synple::Midi`, `Synple::Dsp`, and `Synple::Synth`.

## Build and test

Tests are enabled by default when this repository is configured as the top-level project. Catch2 is fetched only when tests are enabled.

```bash
cmake -S . -B build -DSYNPLE_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Build only the engine libraries without fetching Catch2:

```bash
cmake -S . -B build -DSYNPLE_BUILD_TESTS=OFF
cmake --build build --target synple_synth
```

## Plugin development

Plugin repositories can point `SYNPLE_ENGINE_SOURCE_DIR` at this checkout to build directly from the local engine source. This is useful when developing the engine and plugin together; keep the path in a local CMake cache or user preset rather than committing a machine-specific path.

```bash
cmake -B build-plugin \
    -DSYNPLE_ENGINE_SOURCE_DIR=/path/to/Jape \
    -DSYNPLE_BUILD_TESTS=OFF
cmake --build build-plugin
```