#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "dsp/Filter.h"
#include "dsp/Goertzel.h"

#include <cmath>
#include <numbers>
#include <vector>

namespace
{

constexpr float sampleRate = 44100.0f;
constexpr int warmup = 4096;
constexpr int measureWindow = 8192;

// Renders a sine wave at `freq` through the filter, discarding warmup samples.
std::vector<float> renderSine(dsp::Filter& filter, float freq, int numSamples = measureWindow)
{
    std::vector<float> output;
    output.reserve(numSamples);
    for (int i = 0; i < warmup + numSamples; ++i)
    {
        const float x = std::sin(2.0f * std::numbers::pi_v<float> * freq * static_cast<float>(i) / sampleRate);
        const float y = filter.render(x);
        if (i >= warmup)
            output.push_back(y);
    }
    return output;
}

// Returns the filter's steady-state gain at `freq` relative to the unfiltered input.
float measureGain(dsp::Filter& filter, float freq)
{
    const std::vector<float> output = renderSine(filter, freq);

    std::vector<float> reference;
    reference.reserve(measureWindow);
    for (int i = 0; i < measureWindow; ++i)
    {
        reference.push_back(
            std::sin(2.0f * std::numbers::pi_v<float> * freq * static_cast<float>(i + warmup) / sampleRate));
    }

    const float outMag = dsp::goertzel(output, freq, sampleRate);
    const float refMag = dsp::goertzel(reference, freq, sampleRate);
    return outMag / refMag;
}

} // namespace

// ─── reset() semantics ───────────────────────────────────────────────────────

TEST_CASE("Filter reset clears state but preserves coefficients", "[filter]")
{
    dsp::Filter filter;
    filter.setSampleRate(sampleRate);
    filter.updateCoefficients(1000.0f, 0.707f);

    // Load up the state variables with some audio.
    for (int i = 0; i < 1000; ++i)
        (void)filter.render(std::sin(2.0f * std::numbers::pi_v<float> * 100.0f * static_cast<float>(i) / sampleRate));

    filter.reset();

    // Coefficients intact: DC should still settle to unity gain.
    float out = 0.0f;
    for (int i = 0; i < 4096; ++i)
        out = filter.render(1.0f);

    REQUIRE(out == Catch::Approx(1.0f).margin(0.01f));
}

// ─── DC gain ─────────────────────────────────────────────────────────────────

TEST_CASE("Filter passes DC at unity gain", "[filter]")
{
    const float fs = GENERATE(44100.0f, 96000.0f);
    dsp::Filter filter;
    filter.setSampleRate(fs);
    filter.updateCoefficients(1000.0f, 0.707f);

    float out = 0.0f;
    for (int i = 0; i < 4096; ++i)
        out = filter.render(1.0f);

    REQUIRE(out == Catch::Approx(1.0f).margin(0.01f));
}

// ─── Frequency response ───────────────────────────────────────────────────────

TEST_CASE("Filter gain at cutoff is -3 dB for Butterworth Q", "[filter][frequency]")
{
    // Q = 1/√2 is the Butterworth alignment: |H(fc)| = 1/√2 exactly.
    constexpr float fc = 1000.0f;
    constexpr float butterworthQ = 1.0f / std::numbers::sqrt2_v<float>;

    dsp::Filter filter;
    filter.setSampleRate(sampleRate);
    filter.updateCoefficients(fc, butterworthQ);

    const float gain = measureGain(filter, fc);
    const float gainDb = 20.0f * std::log10(gain);
    INFO("Gain at fc: " << gain << " (" << gainDb << " dB), expected -3 dB");
    REQUIRE(gainDb == Catch::Approx(-3.0f).margin(1.0f));
}

TEST_CASE("Filter attenuates two octaves above cutoff by at least 20 dB", "[filter][frequency]")
{
    // 2nd-order LP rolls off at -12 dB/oct; two octaves (4×fc) → ~24 dB of relative attenuation.
    constexpr float fc = 500.0f;

    dsp::Filter filter;
    filter.setSampleRate(sampleRate);
    filter.updateCoefficients(fc, 0.707f);

    const float gainAtFc = measureGain(filter, fc);
    filter.reset();
    const float gainAt4fc = measureGain(filter, 4.0f * fc);

    const float attenuationDb = 20.0f * std::log10(gainAtFc / gainAt4fc);
    INFO("Attenuation from fc to 4×fc: " << attenuationDb << " dB");
    REQUIRE(attenuationDb >= 20.0f);
}

TEST_CASE("Filter exhibits resonance peak at cutoff with high Q", "[filter][frequency]")
{
    constexpr float fc = 1000.0f;

    dsp::Filter filter;
    filter.setSampleRate(sampleRate);
    filter.updateCoefficients(fc, 5.0f);

    const float gainAtFc = measureGain(filter, fc);
    filter.reset();
    const float gainAtHalfFc = measureGain(filter, fc / 2.0f);

    INFO("Gain at fc: " << gainAtFc << ", gain at fc/2: " << gainAtHalfFc);
    REQUIRE(gainAtFc > gainAtHalfFc);
}

// ─── Stability and edge cases ─────────────────────────────────────────────────

TEST_CASE("Filter impulse response decays to silence", "[filter][stability]")
{
    dsp::Filter filter;
    filter.setSampleRate(sampleRate);
    filter.updateCoefficients(1000.0f, 0.707f);

    (void)filter.render(1.0f); // impulse

    float out = 0.0f;
    for (int i = 0; i < 8192; ++i)
        out = filter.render(0.0f);

    REQUIRE(std::abs(out) < 1e-6f);
}

TEST_CASE("Filter with uninitialised coefficients returns zero without NaN", "[filter][stability]")
{
    // A freshly constructed filter has all-zero coefficients.
    // render() must return 0.0f, not NaN or infinity.
    dsp::Filter filter;
    const float out = filter.render(1.0f);
    REQUIRE(std::isfinite(out));
    REQUIRE(out == 0.0f);
}

// ─── Instance independence ────────────────────────────────────────────────────

TEST_CASE("Two filter instances operate independently", "[filter]")
{
    // filterA has a very low cutoff (200 Hz); filterB has a high cutoff (5000 Hz).
    // At 1000 Hz, filterA attenuates heavily while filterB passes almost fully.
    dsp::Filter filterA;
    filterA.setSampleRate(sampleRate);
    filterA.updateCoefficients(200.0f, 1.0f);

    dsp::Filter filterB;
    filterB.setSampleRate(sampleRate);
    filterB.updateCoefficients(5000.0f, 1.0f);

    const float gainA = measureGain(filterA, 1000.0f);
    const float gainB = measureGain(filterB, 1000.0f);

    INFO("filterA (fc=200) gain at 1kHz: " << gainA << ", filterB (fc=5000): " << gainB);
    REQUIRE(gainB > gainA * 2.0f);
}
