#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "dsp/Goertzel.h"
#include "synth/PulseOscillator.h"

#include <cmath>
#include <vector>

namespace
{

constexpr float sampleRate = 44100.0f;
constexpr int warmup = 4096;

} // namespace

// ─── PWM: odd-harmonic dominance ─────────────────────────────────────────────

TEST_CASE("PulseOscillator at 50% duty cycle has odd-harmonic dominance", "[oscillator][pwm]")
{
    // At 50% duty cycle the pulse wave is a square wave with only odd harmonics.
    // noteOn() initialises the secondary BLEP at half-period offset; setModulation(1.0)
    // keeps it there (pwmMod=1.0 → dutyCycle=0.5).
    constexpr float f0 = 440.0f;
    constexpr int N = 32768;
    const float period = sampleRate / f0;

    synth::PulseOscillator osc;
    osc.setAmplitude(1.0f);
    osc.setPeriod(period);
    osc.noteOn(period); // initialises secondary at 50% duty cycle

    // Warmup to let both BLEPs and the leaky integrator settle
    for (int i = 0; i < warmup; ++i)
        osc.nextSample();

    std::vector<float> sig;
    sig.reserve(N);
    for (int i = 0; i < N; ++i)
        sig.push_back(osc.nextSample());

    const float fund = dsp::goertzel(sig, f0, sampleRate);
    const float harm2 = dsp::goertzel(sig, 2.0f * f0, sampleRate);
    const float harm3 = dsp::goertzel(sig, 3.0f * f0, sampleRate);

    INFO("fundamental=" << fund << " 2nd=" << harm2 << " 3rd=" << harm3);

    REQUIRE(fund > 0.1f);

    // 2nd harmonic (even) suppressed by ≥20 dB relative to fundamental
    const float suppression2dB = 20.0f * std::log10(fund / (harm2 + 1e-12f));
    INFO("2nd harmonic suppression: " << suppression2dB << " dB");
    REQUIRE(suppression2dB >= 20.0f);

    // 3rd harmonic (odd) must be present — for a perfect square wave it sits at
    // 20·log₁₀(3) ≈ 9.5 dB below the fundamental. Allow up to 15 dB for
    // Hann-window leakage and BLEP correction rounding.
    const float suppression3dB = 20.0f * std::log10(fund / (harm3 + 1e-12f));
    INFO("3rd harmonic level: " << suppression3dB << " dB below fundamental");
    REQUIRE(suppression3dB <= 15.0f);
}

TEST_CASE("PulseOscillator duty cycle changes with setModulation", "[oscillator][pwm]")
{
    // setModulation(pwmMod) changes the duty cycle via the secondary BLEP.
    // At 50% duty (pwmMod=1.0) the 2nd harmonic is suppressed ≥20 dB.
    // At 25% duty (pwmMod=0.5) the 2nd harmonic becomes significant.
    constexpr float f0 = 440.0f;
    constexpr int N = 32768;
    const float period = sampleRate / f0;

    auto renderPulse = [&](float pwmMod) {
        synth::PulseOscillator osc;
        osc.setAmplitude(1.0f);
        osc.setPeriod(period);
        osc.noteOn(period);
        osc.setModulation(pwmMod);
        for (int i = 0; i < warmup; ++i)
            osc.nextSample();
        std::vector<float> sig;
        sig.reserve(N);
        for (int i = 0; i < N; ++i)
            sig.push_back(osc.nextSample());
        return sig;
    };

    const auto sig50 = renderPulse(1.0f);  // 50% duty → square wave
    const auto sig25 = renderPulse(0.5f);  // 25% duty → narrow pulse

    const float fund50  = dsp::goertzel(sig50, f0, sampleRate);
    const float harm2at50 = dsp::goertzel(sig50, 2.0f * f0, sampleRate);
    const float harm2at25 = dsp::goertzel(sig25, 2.0f * f0, sampleRate);

    INFO("50% duty: fundamental=" << fund50 << " 2nd=" << harm2at50);
    INFO("25% duty: 2nd=" << harm2at25);

    // 50% duty: even harmonics suppressed
    const float supp2at50 = 20.0f * std::log10(fund50 / (harm2at50 + 1e-12f));
    INFO("2nd harmonic suppression at 50%: " << supp2at50 << " dB");
    REQUIRE(supp2at50 >= 20.0f);

    // 25% duty: 2nd harmonic present (narrower pulse has stronger even harmonics)
    REQUIRE(harm2at25 > harm2at50);
}
