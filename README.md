# Jape

C++20 engine layers for audio plugins. The public CMake targets are `Jape::Math`, `Jape::Midi`, `Jape::Dsp`, and `Jape::Synth`.

## Build and test

Tests are enabled by default when this repository is configured as the top-level project. Catch2 is fetched only when tests are enabled.

```bash
cmake -S . -B build -DJAPE_BUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Build only the engine libraries without fetching Catch2:

```bash
cmake -S . -B build -DJAPE_BUILD_TESTS=OFF
cmake --build build --target jape_synth
```

## Plugin development

Plugin repositories can point `JAPE_SOURCE_DIR` at this checkout to build directly from the local engine source. This is useful when developing the engine and plugin together; keep the path in a local CMake cache or user preset rather than committing a machine-specific path.

```bash
cmake -B build-plugin \
    -DJAPE_SOURCE_DIR=/path/to/Jape \
    -DJAPE_BUILD_TESTS=OFF
cmake --build build-plugin
```