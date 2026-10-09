#pragma once

#include <cstddef>
#include <cstring>

namespace synth
{

inline constexpr std::size_t kNumParams{34};

class Preset
{
  public:
    Preset(const char* name,
           const float p0,
           const float p1,
           const float p2,
           const float p3,
           const float p4,
           const float p5,
           const float p6,
           const float p7,
           const float p8,
           const float p9,
           const float p10,
           const float p11,
           const float p12,
           const float p13,
           const float p14,
           const float p15,
           const float p16,
           const float p17,
           const float p18,
           const float p19,
           const float p20,
           const float p21,
           const float p22,
           const float p23,
           const float p24,
           const float p25,
           const float p26,
           const float p27,
           const float p28,
           const float p29,
           const float p30,
           const float p31,
           const float p32,
           const float p33)
    {
        std::strncpy(this->name_, name, sizeof(name_) - 1);
        name_[sizeof(name_) - 1] = '\0';

        parameters_[0] = p0;   // Glide Mode
        parameters_[1] = p1;   // Glide Rate
        parameters_[2] = p2;   // Glide Bend
        parameters_[3] = p3;   // Filter Freq
        parameters_[4] = p4;   // Filter Reso
        parameters_[5] = p5;   // Filter Env
        parameters_[6] = p6;   // Filter LFO
        parameters_[7] = p7;   // Velocity
        parameters_[8] = p8;   // Filter Attack
        parameters_[9] = p9;   // Filter Decay
        parameters_[10] = p10; // Filter Sustain
        parameters_[11] = p11; // Filter Release
        parameters_[12] = p12; // Env Attack
        parameters_[13] = p13; // Env Decay
        parameters_[14] = p14; // Env Sustain
        parameters_[15] = p15; // Env Release
        parameters_[16] = p16; // LFO Rate
        parameters_[17] = p17; // Vibrato
        parameters_[18] = p18; // Noise
        parameters_[19] = p19; // Octave
        parameters_[20] = p20; // Tuning
        parameters_[21] = p21; // Output Level
        parameters_[22] = p22; // Polyphony
        parameters_[23] = p23; // PWM Depth
        parameters_[24] = p24; // Osc 1 Volume
        parameters_[25] = p25; // Osc 2 Volume
        parameters_[26] = p26; // Osc 1 Tune
        parameters_[27] = p27; // Osc 2 Tune
        parameters_[28] = p28; // Osc 1 Fine
        parameters_[29] = p29; // Osc 2 Fine
        parameters_[30] = p30; // Osc 1 Waveform
        parameters_[31] = p31; // Osc 2 Waveform
        parameters_[32] = p32; // Osc 1 Pulse Width
        parameters_[33] = p33; // Osc 2 Pulse Width
    }

    const char* name() const { return name_; }

    const float* parameters() const { return parameters_; }

  private:
    char name_[40];
    float parameters_[kNumParams];
};

} // namespace synth
