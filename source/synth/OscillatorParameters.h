#pragma once

#include "WaveformType.h"

namespace synth
{

struct OscillatorParameters
{
    float volume{0.0f};
    float tune{1.0f};       // period multiplier (semitones + cents relative to global)
    float pulseWidth{0.5f}; // duty cycle 0.1-0.9
    WaveformType waveform{WaveformType::Sawtooth};
};

} // namespace synth
