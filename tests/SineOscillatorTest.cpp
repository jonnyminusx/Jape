#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "dsp/Goertzel.h"
#include "dsp/Rms.h"
#include "synth/SineOscillator.h"

#include <cmath>
#include <vector>

namespace
{

constexpr float sampleRate = 44100.0f;
constexpr int warmup = 4096;

std::vector<float> renderSine(float f0, float fs, int numSamples, float amplitude = 1.0f)
{
    synth::SineOscillator osc;
    osc.setAmplitude(amplitude);
    osc.noteOn(fs / f0);

    std::vector<float> output;
    output.reserve(numSamples);

    for (int i = 0; i < warmup + numSamples; ++i)
    {
        const float s = osc.nextSample();
        if (i >= warmup)
            output.push_back(s);
    }

    return output;
}

} // namespace

// ─── Amplitude scaling ────────────────────────────────────────────────────────

TEST_CASE("SineOscillator RMS scales linearly with amplitude", "[oscillator][amplitude]")
{
    constexpr int N = 16384;

    const auto sig1x = renderSine(440.0f, sampleRate, N, 1.0f);
    const auto sig2x = renderSine(440.0f, sampleRate, N, 2.0f);

    const float rms1 = dsp::rms(sig1x);
    const float rms2 = dsp::rms(sig2x);

    INFO("rms(1x) = " << rms1 << "  rms(2x) = " << rms2);
    REQUIRE(rms1 > 0.0f);
    // Doubling amplitude should double RMS, within 5%
    REQUIRE(rms2 == Catch::Approx(2.0f * rms1).epsilon(0.05f));
}

// ─── Zero amplitude → silence ─────────────────────────────────────────────────

TEST_CASE("SineOscillator zero amplitude produces silence", "[oscillator][amplitude]")
{
    constexpr int N = 8192;
    const auto sig = renderSine(440.0f, sampleRate, N, 0.0f);

    for (float s : sig)
        REQUIRE(std::abs(s) < 1e-6f);
}

// ─── Frequency accuracy ───────────────────────────────────────────────────────

TEST_CASE("SineOscillator fundamental energy is concentrated at the set frequency", "[oscillator][frequency]")
{
    constexpr int N = 16384;

    const float f0 = GENERATE(440.0f, 1000.0f);
    const auto sig = renderSine(f0, sampleRate, N);
    const float atF0   = dsp::goertzel(sig, f0,           sampleRate);
    const float atLow  = dsp::goertzel(sig, f0 - 100.0f, sampleRate);
    const float atHigh = dsp::goertzel(sig, f0 + 100.0f, sampleRate);
    INFO("f0=" << f0 << " Hz, energy at f0=" << atF0 << " at f0-100=" << atLow << " at f0+100=" << atHigh);
    REQUIRE(atF0 > 10.0f * atLow);
    REQUIRE(atF0 > 10.0f * atHigh);
}

// ─── Reset ────────────────────────────────────────────────────────────────────

TEST_CASE("SineOscillator produces identical output after reset", "[oscillator][reset]")
{
    constexpr int N = 2048;
    constexpr float f0 = 440.0f;
    const float period = sampleRate / f0;

    synth::SineOscillator osc;
    osc.setAmplitude(1.0f);
    osc.noteOn(period);

    // First run (no warmup, captures the raw startup sequence)
    std::vector<float> run1;
    run1.reserve(N);
    for (int i = 0; i < N; ++i)
        run1.push_back(osc.nextSample());

    // reset() recomputes recurrence state from the already-set normalizedFreq_
    osc.reset();
    osc.setAmplitude(1.0f);

    std::vector<float> run2;
    run2.reserve(N);
    for (int i = 0; i < N; ++i)
        run2.push_back(osc.nextSample());

    REQUIRE(run1.size() == run2.size());
    for (size_t i = 0; i < run1.size(); ++i)
        REQUIRE(run1[i] == Catch::Approx(run2[i]).epsilon(1e-5f));
}
