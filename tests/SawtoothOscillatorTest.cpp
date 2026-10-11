#include <doctest/doctest.h>

#include "dsp/Goertzel.h"
#include "dsp/Rms.h"
#include "synth/SawtoothOscillator.h"

#include <cmath>
#include <vector>

namespace
{

constexpr float sampleRate = 44100.0f;
constexpr int warmup = 4096;

// Renders the integrated sawtooth output from a single SawtoothOscillator.
std::vector<float> renderRaw(float f0, float fs, int numSamples, float amplitude = 1.0f)
{
    synth::SawtoothOscillator osc;
    osc.setAmplitude(amplitude);
    osc.setPeriod(fs / f0);
    osc.setModulation(1.0f);

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

// Renders the integrated sawtooth at the given frequency and sample rate.
std::vector<float> renderAliasSawtooth(float f0, float fs, int numSamples, int numWarmup = warmup)
{
    const float period = fs / f0;

    synth::SawtoothOscillator osc;
    osc.setAmplitude(1.0f);
    osc.setPeriod(period);
    osc.setModulation(1.0f);

    std::vector<float> output;
    output.reserve(numSamples);

    for (int i = 0; i < numWarmup + numSamples; ++i)
    {
        const float s = osc.nextSample();
        if (i >= numWarmup)
            output.push_back(s);
    }

    return output;
}

struct AliasingResult
{
    float fundamentalMag;
    float worstAliasMag;
    float worstAliasFreq;

    float suppressionDb() const
    {
        if (worstAliasMag < 1e-12f)
            return 200.0f;
        return 20.0f * std::log10(fundamentalMag / worstAliasMag);
    }
};

// For each harmonic of f0 that exceeds Nyquist (up to maxHarmonic), its
// alias lands at (sampleRate - k*f0). This function finds the worst-case
// alias magnitude relative to the fundamental, skipping alias bins that are
// within minSeparationHz of a true harmonic (to avoid measuring harmonic
// energy rather than alias energy due to spectral leakage).
AliasingResult measureAliasing(
    const std::vector<float>& signal, float f0, float fs, int maxHarmonic = 30, float minSeparationHz = 100.0f)
{
    const float nyquist = fs / 2.0f;
    const float fundamentalMag = dsp::goertzel(signal, f0, fs);

    float worstAlias = 0.0f;
    float worstAliasFreq = 0.0f;

    for (int k = 2; k <= maxHarmonic; ++k)
    {
        const float harmonic = static_cast<float>(k) * f0;
        if (harmonic <= nyquist)
            continue;

        const float aliasFreq = fs - harmonic;
        if (aliasFreq <= 0.0f || aliasFreq >= nyquist)
            continue;

        // Skip if the alias bin is too close to a real harmonic — spectral
        // leakage from the harmonic would inflate the alias measurement.
        bool nearHarmonic = false;
        for (int j = 1; static_cast<float>(j) * f0 <= nyquist; ++j)
        {
            if (std::abs(aliasFreq - static_cast<float>(j) * f0) < minSeparationHz)
            {
                nearHarmonic = true;
                break;
            }
        }
        if (nearHarmonic)
            continue;

        const float mag = dsp::goertzel(signal, aliasFreq, fs);
        if (mag > worstAlias)
        {
            worstAlias = mag;
            worstAliasFreq = aliasFreq;
        }
    }

    return {fundamentalMag, worstAlias, worstAliasFreq};
}

} // namespace

// ─── Amplitude scaling ────────────────────────────────────────────────────────

TEST_CASE("Sawtooth RMS scales linearly with amplitude" "[oscillator][amplitude]")
{
    constexpr int N = 16384;

    const auto sig1x = renderRaw(440.0f, sampleRate, N, 1.0f);
    const auto sig2x = renderRaw(440.0f, sampleRate, N, 2.0f);

    const float rms1 = dsp::rms(sig1x);
    const float rms2 = dsp::rms(sig2x);

    INFO("rms(1x) = " << rms1 << "  rms(2x) = " << rms2);
    REQUIRE(rms1 > 0.0f);
    // Doubling amplitude should double RMS, within 5%
    REQUIRE(rms2 == doctest::Approx(2.0f * rms1).epsilon(0.05f));
}

// ─── Zero amplitude → silence ─────────────────────────────────────────────────

TEST_CASE("Zero amplitude produces silence" "[oscillator][amplitude]")
{
    constexpr int N = 8192;
    const auto sig = renderRaw(440.0f, sampleRate, N, 0.0f);

    for (float s : sig)
        REQUIRE(std::abs(s) < 1e-6f);
}

// ─── Frequency accuracy ───────────────────────────────────────────────────────

TEST_CASE("Oscillator fundamental energy is concentrated at the set frequency" "[oscillator][frequency]")
{
    constexpr int N = 16384;

    SUBCASE("440 Hz")
    {
        const float f0 = 440.0f;
        const auto sig = renderRaw(f0, sampleRate, N);
        const float atF0   = dsp::goertzel(sig, f0,           sampleRate);
        const float atLow  = dsp::goertzel(sig, f0 - 100.0f, sampleRate);
        const float atHigh = dsp::goertzel(sig, f0 + 100.0f, sampleRate);
        INFO("f0=" << f0 << " Hz, energy at f0=" << atF0 << " at f0-100=" << atLow << " at f0+100=" << atHigh);
        REQUIRE(atF0 > 10.0f * atLow);
        REQUIRE(atF0 > 10.0f * atHigh);
    }
    SUBCASE("1000 Hz")
    {
        const float f0 = 1000.0f;
        const auto sig = renderRaw(f0, sampleRate, N);
        const float atF0   = dsp::goertzel(sig, f0,           sampleRate);
        const float atLow  = dsp::goertzel(sig, f0 - 100.0f, sampleRate);
        const float atHigh = dsp::goertzel(sig, f0 + 100.0f, sampleRate);
        INFO("f0=" << f0 << " Hz, energy at f0=" << atF0 << " at f0-100=" << atLow << " at f0+100=" << atHigh);
        REQUIRE(atF0 > 10.0f * atLow);
        REQUIRE(atF0 > 10.0f * atHigh);
    }
}

// ─── Reset ────────────────────────────────────────────────────────────────────

TEST_CASE("Oscillator produces identical output after reset" "[oscillator][reset]")
{
    constexpr int N = 2048;
    constexpr float f0 = 440.0f;
    const float period = sampleRate / f0;

    synth::SawtoothOscillator osc;
    osc.setAmplitude(1.0f);
    osc.setPeriod(period);
    osc.setModulation(1.0f);

    // First run (no warmup, captures the raw startup sequence)
    std::vector<float> run1;
    run1.reserve(N);
    for (int i = 0; i < N; ++i)
        run1.push_back(osc.nextSample());

    // Reset and re-run with the same parameters
    osc.reset();
    osc.setAmplitude(1.0f);
    osc.setPeriod(period);
    osc.setModulation(1.0f);

    std::vector<float> run2;
    run2.reserve(N);
    for (int i = 0; i < N; ++i)
        run2.push_back(osc.nextSample());

    REQUIRE(run1.size() == run2.size());
    for (size_t i = 0; i < run1.size(); ++i)
        REQUIRE(run1[i] == doctest::Approx(run2[i]).epsilon(1e-5f));
}

// ─── Aliasing: sanity check ───────────────────────────────────────────────────

TEST_CASE("Oscillator produces a signal at the expected fundamental frequency" "[oscillator][aliasing]")
{
    constexpr int N = 16384;

    SUBCASE("440 Hz")
    {
        const float f0 = 440.0f;
        const auto signal = renderAliasSawtooth(f0, sampleRate, N);
        const float mag = dsp::goertzel(signal, f0, sampleRate);
        INFO("f0=" << f0 << " Hz, magnitude=" << mag);
        REQUIRE(mag > 1.0f);
    }
    SUBCASE("3000 Hz")
    {
        const float f0 = 3000.0f;
        const auto signal = renderAliasSawtooth(f0, sampleRate, N);
        const float mag = dsp::goertzel(signal, f0, sampleRate);
        INFO("f0=" << f0 << " Hz, magnitude=" << mag);
        REQUIRE(mag > 1.0f);
    }
    SUBCASE("7000 Hz")
    {
        const float f0 = 7000.0f;
        const auto signal = renderAliasSawtooth(f0, sampleRate, N);
        const float mag = dsp::goertzel(signal, f0, sampleRate);
        INFO("f0=" << f0 << " Hz, magnitude=" << mag);
        REQUIRE(mag > 1.0f);
    }
}

// ─── Aliasing: suppression ────────────────────────────────────────────────────

// Thresholds reflect the current BLEP implementation's measured performance.
// They are intentionally set below the measured values so regressions are
// caught (removing the band-limiting entirely would drop suppression to the
// natural harmonic roll-off level of ~13–20 dB for these frequencies).
//
// Measured suppression as of baseline:
//   3000 Hz → 51.7 dB   (threshold 45 dB)
//   5000 Hz → 47.0 dB   (threshold 40 dB)
//   7000 Hz → 40.6 dB   (threshold 35 dB)

// Alias frequencies by fundamental:
//   3000 Hz → first alias at 20100 Hz
//   5000 Hz → aliases at 19100, 14100, 9100, 4100 Hz
//   7000 Hz → many aliases fold back
TEST_CASE("Sawtooth oscillator suppresses aliasing relative to the fundamental" "[oscillator][aliasing]")
{
    constexpr int N = 16384;

    struct TestCase { float f0; float thresholdDb; };
    SUBCASE("3000 Hz")
    {
        constexpr float f0 = 3000.0f;
        constexpr float thresholdDb = 45.0f;
        const auto signal = renderAliasSawtooth(f0, sampleRate, N);
        const auto r = measureAliasing(signal, f0, sampleRate);
        INFO("f0=" << f0 << " Hz, worst alias: " << r.worstAliasFreq << " Hz, suppression: " << r.suppressionDb() << " dB");
        REQUIRE(r.suppressionDb() >= thresholdDb);
    }
    SUBCASE("5000 Hz")
    {
        constexpr float f0 = 5000.0f;
        constexpr float thresholdDb = 40.0f;
        const auto signal = renderAliasSawtooth(f0, sampleRate, N);
        const auto r = measureAliasing(signal, f0, sampleRate);
        INFO("f0=" << f0 << " Hz, worst alias: " << r.worstAliasFreq << " Hz, suppression: " << r.suppressionDb() << " dB");
        REQUIRE(r.suppressionDb() >= thresholdDb);
    }
    SUBCASE("7000 Hz")
    {
        constexpr float f0 = 7000.0f;
        constexpr float thresholdDb = 35.0f;
        const auto signal = renderAliasSawtooth(f0, sampleRate, N);
        const auto r = measureAliasing(signal, f0, sampleRate);
        INFO("f0=" << f0 << " Hz, worst alias: " << r.worstAliasFreq << " Hz, suppression: " << r.suppressionDb() << " dB");
        REQUIRE(r.suppressionDb() >= thresholdDb);
    }
}

// ─── Aliasing: suppression across sample rates ────────────────────────────────

TEST_CASE("Alias suppression holds across common sample rates" "[oscillator][aliasing]")
{
    // 3000 Hz is chosen because its harmonic aliases land clearly between
    // harmonics at all three sample rates, giving unambiguous measurements.
    constexpr float f0 = 3000.0f;
    constexpr float thresholdDb = 35.0f;
    constexpr int N = 16384;

    SUBCASE("44100 Hz sample rate")
    {
        const auto signal = renderAliasSawtooth(f0, 44100.0f, N);
        const auto r = measureAliasing(signal, f0, 44100.0f);
        INFO("fs=44100 Hz, worst alias: " << r.worstAliasFreq << " Hz, suppression: " << r.suppressionDb() << " dB");
        REQUIRE(r.suppressionDb() >= thresholdDb);
    }
    SUBCASE("48000 Hz sample rate")
    {
        const auto signal = renderAliasSawtooth(f0, 48000.0f, N);
        const auto r = measureAliasing(signal, f0, 48000.0f);
        INFO("fs=48000 Hz, worst alias: " << r.worstAliasFreq << " Hz, suppression: " << r.suppressionDb() << " dB");
        REQUIRE(r.suppressionDb() >= thresholdDb);
    }
    SUBCASE("96000 Hz sample rate")
    {
        const auto signal = renderAliasSawtooth(f0, 96000.0f, N);
        const auto r = measureAliasing(signal, f0, 96000.0f);
        INFO("fs=96000 Hz, worst alias: " << r.worstAliasFreq << " Hz, suppression: " << r.suppressionDb() << " dB");
        REQUIRE(r.suppressionDb() >= thresholdDb);
    }
}
