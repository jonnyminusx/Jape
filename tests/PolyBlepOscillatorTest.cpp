#include <doctest/doctest.h>

#include "dsp/Mean.h"
#include "dsp/Rms.h"
#include "synth/PolyBlepOscillator.h"

#include <cmath>
#include <vector>

namespace
{

constexpr float sampleRate = 44100.0f;
constexpr int warmup = 4096;

// Renders raw BLEP output (no leaky integrator) — tests DC compensation and
// modulation effects directly on the PolyBlepOscillator layer.
std::vector<float> renderBlep(float f0, float fs, int numSamples, float amplitude = 1.0f)
{
    synth::PolyBlepOscillator osc;
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

} // namespace

// ─── DC offset ───────────────────────────────────────────────────────────────

// Tests the BLEP generator's own DC compensation (raw output, no leaky integrator).
// The leaky integrator's DC gain is 1/(1-0.997) ≈ 333×, so even a tiny
// residual DC in the raw output would read as a large mean in the integrated
// signal — testing PolyBlepOscillator directly isolates the compensation logic.
TEST_CASE("Raw BLEP output has near-zero DC after warmup" "[oscillator][dc]")
{
    constexpr int N = 16384;
    constexpr float tolerance = 0.01f;

    SUBCASE("440 Hz")
    {
        const auto sig = renderBlep(440.0f, sampleRate, N);
        INFO("f0=440 Hz, mean=" << dsp::mean(sig));
        REQUIRE(std::abs(dsp::mean(sig)) < tolerance);
    }
    SUBCASE("1000 Hz")
    {
        const auto sig = renderBlep(1000.0f, sampleRate, N);
        INFO("f0=1000 Hz, mean=" << dsp::mean(sig));
        REQUIRE(std::abs(dsp::mean(sig)) < tolerance);
    }
    SUBCASE("5000 Hz")
    {
        const auto sig = renderBlep(5000.0f, sampleRate, N);
        INFO("f0=5000 Hz, mean=" << dsp::mean(sig));
        REQUIRE(std::abs(dsp::mean(sig)) < tolerance);
    }
}

// ─── Modulation ───────────────────────────────────────────────────────────────

TEST_CASE("Modulation parameter changes oscillator output" "[oscillator][modulation]")
{
    // setModulation() scales halfPeriod, which changes halfPhase_ and increment_.
    // Tested on PolyBlepOscillator directly: the leaky integrator in
    // SawtoothOscillator smooths the waveform and makes the RMS difference
    // between modulation values too small to assert reliably.
    constexpr int N = 4096;
    const float period = sampleRate / 440.0f;

    auto renderWithMod = [&](float mod) -> std::vector<float> {
        synth::PolyBlepOscillator osc;
        osc.setAmplitude(1.0f);
        osc.setPeriod(period);
        osc.setModulation(mod);
        std::vector<float> out;
        out.reserve(N);
        for (int i = 0; i < N; ++i)
            out.push_back(osc.nextSample());
        return out;
    };

    const float rms1 = dsp::rms(renderWithMod(1.0f));
    const float rmsHalf = dsp::rms(renderWithMod(0.5f));

    INFO("rms(mod=1.0)=" << rms1 << "  rms(mod=0.5)=" << rmsHalf);
    REQUIRE(std::abs(rms1 - rmsHalf) > 0.01f);
}

TEST_CASE("setModulation clamps near-zero and negative values to prevent NaN" "[oscillator][modulation]")
{
    // Without the clamp in setModulation(), modulation=0 causes
    // increment_ = halfPhase_ / 0 → ±inf, producing NaN output.
    // Tested on PolyBlepOscillator where the clamp logic lives.
    constexpr int N = 2048;
    const float period = sampleRate / 440.0f;

    for (const float mod : {0.0f, -0.5f, -1.0f})
    {
        synth::PolyBlepOscillator osc;
        osc.setAmplitude(1.0f);
        osc.setPeriod(period);
        osc.setModulation(mod);

        bool allFinite = true;
        for (int i = 0; i < N; ++i)
        {
            if (!std::isfinite(osc.nextSample()))
            {
                allFinite = false;
                break;
            }
        }
        INFO("modulation = " << mod);
        REQUIRE(allFinite);
    }
}
