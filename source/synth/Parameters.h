#pragma once

#include "GlideMode.h"
#include "OscillatorParameters.h"

namespace synth
{

struct Parameters
{
    struct ADSR
    {
        float attack{0.0f};
        float decay{0.0f};
        float sustain{1.0f};
        float release{0.0f};
    };

    struct Oscillator
    {
        OscillatorParameters osc1{.volume = 1.0f};
        OscillatorParameters osc2{};
        float globalTune{1.0f}; // period multiplier from sampleRate × octave × global tuning
        float noiseMix{0.0f};   // normalised 0-1
    } oscillator;

    struct Filter
    {
        float keyTracking{0.0f};
        float q{0.0f};
        float lfoDepth{0.0f};
        float envelopeDepth{0.0f};
        float velocitySensitivity{0.0f};
        ADSR envelope{};
    } filter;

    struct Lfo
    {
        float increment{0.0f};
        float vibratoAmount{0.0f};
        float pwmDepth{0.0f};
    } lfo;

    struct Glide
    {
        GlideMode mode{GlideMode::Off};
        float rateCoefficient{0.0f};
        float bendSemitones{0.0f};
    } glide;

    struct Output
    {
        float gain{0.0f};
        float volumeTrim{0.0f};
        bool polyphonic{true};
        bool ignoreVelocity{false};
    } output;

    ADSR envelope{}; // amplitude envelope
};

} // namespace synth
