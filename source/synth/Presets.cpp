#include "Presets.h"

#include "Preset.h"

namespace synth
{

Presets::Presets()
{
    createPrograms();
}

void Presets::createPrograms()
{
    // clang-format off
    // Columns: GlideMode, GlideRate, GlideBend,
    //          FilterFreq, FilterReso, FilterEnv, FilterLFO, Velocity,
    //          FilterAtk, FilterDcy, FilterSus, FilterRel,
    //          EnvAtk, EnvDcy, EnvSus, EnvRel,
    //          LFORate, Vibrato, Noise, Octave, Tuning, OutputLevel, Poly, PWMDepth,
    //          Osc1Vol, Osc2Vol, Osc1Tune, Osc2Tune, Osc1Fine, Osc2Fine,
    //          Osc1Wave, Osc2Wave, Osc1PW, Osc2PW
    presets_.clear();
    presets_.reserve(53);
    presets_.emplace_back("Init",                    0.00f, 35.00f,   0.00f, 100.00f,  15.00f,  50.00f,   0.00f,   0.00f,   0.00f,  30.00f,   0.00f,  25.00f,  0.00f,  50.00f, 100.00f,  30.00f, 0.81f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("5th Sweep Pad",           1.00f, 32.00f,   0.00f,  90.00f,  60.00f, -76.00f,   0.00f,   0.00f,  90.00f,  89.00f,  90.00f,  73.00f,  0.00f,  50.00f, 100.00f,  71.00f, 0.81f,  30.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f, 100.00f,  0.00f,  -7.00f,  0.00f,  -6.30f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Echo Pad [SA]": old vibrato=-74 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Echo Pad [SA]",           0.00f, 49.00f,   0.00f,  46.00f,  76.00f,  38.00f,  10.00f,  38.00f, 100.00f,  86.00f,  76.00f,  57.00f, 30.00f,  80.00f,  68.00f,  66.00f, 0.79f,   0.00f,  25.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  88.00f,  0.00f,   0.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Space Chimes [SA]": old vibrato=-32 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Space Chimes [SA]",       0.00f, 49.00f,   0.00f,  49.00f,  82.00f,  32.00f,   8.00f,  78.00f,  85.00f,  69.00f,  76.00f,  47.00f, 12.00f,  22.00f,  55.00f,  66.00f, 0.89f,   0.00f,   0.00f,  2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  88.00f,  0.00f,   0.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Solid Backing",           0.00f, 35.00f,   0.00f,  30.00f,  25.00f,  40.00f,   0.00f,  26.00f,   0.00f,  35.00f,   0.00f,  25.00f,  0.00f,  50.00f, 100.00f,  30.00f, 0.81f,   0.00f,  50.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f, 100.00f,  0.00f, -12.00f,  0.00f, -18.70f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Velocity Backing [SA]",   0.00f,  8.00f,  -1.68f,  49.00f,   1.00f, -32.00f,   0.00f,  86.00f,  61.00f,  87.00f, 100.00f,  93.00f, 11.00f,  48.00f,  98.00f,  32.00f, 0.81f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  41.00f,  0.00f,   0.00f,  0.00f,   9.70f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Rubber Backing [ZF]",     0.00f, 18.00f,   5.06f,  35.00f,  15.00f,  54.00f,  14.00f,   8.00f,   0.00f,  42.00f,  13.00f,  21.00f,  0.00f,  56.00f,   0.00f,  32.00f, 0.20f,  16.00f,  22.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  29.00f,  0.00f,  12.00f,  0.00f,  -5.60f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("808 State Lead",          2.00f, 34.00f,  12.35f,  65.00f,  63.00f,  50.00f,  16.00f,   0.00f,   0.00f,  30.00f,   0.00f,  25.00f, 17.00f,  50.00f, 100.00f,   3.00f, 0.81f,   0.00f,   0.00f,  1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f, 100.00f,  0.00f,   7.00f,  0.00f,  -7.10f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Mono Glide",              2.00f, 46.00f,   0.00f,  51.00f,   0.00f,   0.00f,   0.00f,-100.00f,   0.00f,  30.00f,   0.00f,  25.00f, 37.00f,  50.00f, 100.00f,  38.00f, 0.81f,  24.00f,   0.00f,  0.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Detuned Techno Lead": old vibrato=-82 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Detuned Techno Lead",     2.00f, 41.00f,  -0.15f,  54.00f,   1.00f,  16.00f,  21.00f,  34.00f,   0.00f,   9.00f, 100.00f,  25.00f, 20.00f,  85.00f, 100.00f,  30.00f, 0.83f,   0.00f,  40.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  84.00f,  0.00f,   0.00f,  0.00f, -17.20f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Hard Lead [SA]": old vibrato=-34 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Hard Lead [SA]",          0.00f, 24.00f,  36.00f,  56.00f,  52.00f,  38.00f,  19.00f,  40.00f, 100.00f,  14.00f,  65.00f,  95.00f,  7.00f,  91.00f, 100.00f,  15.00f, 0.84f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  71.00f,  0.00f,  12.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Bubble",                  0.00f, 71.00f,   0.00f,  23.00f,  77.00f,  60.00f,  32.00f,  26.00f,  40.00f,  18.00f,  66.00f,  14.00f,  0.00f,  38.00f,  65.00f,  16.00f, 0.48f,   0.00f,   0.00f,  1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,  -0.20f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Monosynth": old vibrato=-40 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Monosynth",               1.00f, 35.00f,   0.02f,  64.00f,  39.00f,   2.00f,  65.00f,-100.00f,   7.00f,  52.00f,  24.00f,  84.00f, 13.00f,  30.00f,  76.00f,  21.00f, 0.58f,   0.00f,   0.00f, -1.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,  62.00f,  0.00f, -12.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Moogcury Lite",           1.00f, 15.00f,  -0.97f,  39.00f,  17.00f,  38.00f,  40.00f,  24.00f,   0.00f,  47.00f,  19.00f,  37.00f,  0.00f,  50.00f,  20.00f,  33.00f, 0.38f,   6.00f,   0.00f, -2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,  81.00f,  0.00f,  24.00f,  0.00f,  -9.80f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Gangsta Whine": old vibrato=-2 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Gangsta Whine",           2.00f, 44.00f,   0.00f,  41.00f,  46.00f,   0.00f,   0.00f,-100.00f,   0.00f,   0.00f, 100.00f,  25.00f, 15.00f,  50.00f, 100.00f,  32.00f, 0.81f,   0.00f,   0.00f,  2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,   0.00f,  0.00f,   0.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Higher Synth [ZF]",       0.00f,  0.00f,   0.00f,  50.00f,  47.00f,  46.00f,  30.00f,  60.00f,   0.00f,  10.00f,   0.00f,   7.00f,  0.00f,  42.00f,   0.00f,  22.00f, 0.21f,  18.00f,  16.00f,  2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  48.00f,  0.00f,   0.00f,  0.00f,  -8.80f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "303 Saw Bass": old vibrato=-2 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("303 Saw Bass",            1.00f, 49.00f,   0.00f,  55.00f,  75.00f,  38.00f,  35.00f,   0.00f,   0.00f,  56.00f,   0.00f,  56.00f,  0.00f,  80.00f, 100.00f,  24.00f, 0.26f,   0.00f,   0.00f, -2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,   0.00f,  0.00f,   0.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "303 Square Bass": old vibrato=-2 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("303 Square Bass",         1.00f, 49.00f,   0.00f,  55.00f,  75.00f,  38.00f,  35.00f,   0.00f,  14.00f,  49.00f,   0.00f,  39.00f,  0.00f,  80.00f, 100.00f,  24.00f, 0.26f,   0.00f,   0.00f, -2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,  75.00f,  0.00f,   0.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Analog Bass",             1.00f, 19.00f,   0.00f,  30.00f,  51.00f,  70.00f,   9.00f,-100.00f,   0.00f,  88.00f,   0.00f,  21.00f,  0.00f,  50.00f, 100.00f,  46.00f, 0.81f,   0.00f,   0.00f, -1.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f, 100.00f,  0.00f, -12.00f,  0.00f, -10.90f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Analog Bass 2",           0.00f, 19.00f,  13.44f,  48.00f,  43.00f,  88.00f,   0.00f,  60.00f,   0.00f,   0.00f,   0.00f,   0.00f,  0.00f,  61.00f, 100.00f,  32.00f, 0.81f,   0.00f,   0.00f, -1.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f, 100.00f,  0.00f, -12.00f,  0.00f, -10.90f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Low Pulses": old vibrato=-68 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Low Pulses",              0.00f, 35.00f,   0.00f,  80.00f,  40.00f,   4.00f,   0.00f,   0.00f,   0.00f,  77.00f,   0.00f,  25.00f,  0.00f,  50.00f, 100.00f,  30.00f, 0.81f,   0.00f,   0.00f, -2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  97.00f,  0.00f, -12.00f,  0.00f,  -3.30f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Sine Infra-Bass",         0.00f, 35.00f,   0.00f,  33.00f,  76.00f,   6.00f,   0.00f,   0.00f,   0.00f,  30.00f,   0.00f,  25.00f,  0.00f,  55.00f,  25.00f,  30.00f, 0.81f,   4.00f,   0.00f, -2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Wobble Bass [SA]": old vibrato=-8 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Wobble Bass [SA]",        0.00f, 82.00f,   0.21f,  72.00f,  47.00f, -32.00f,  34.00f,  64.00f,  20.00f,  69.00f, 100.00f,  15.00f,  9.00f,  50.00f, 100.00f,   7.00f, 0.81f,   0.00f,   0.00f, -1.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f, 100.00f,  0.00f, -12.00f,  0.00f,  -8.80f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Squelch Bass": old vibrato=-8 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Squelch Bass",            0.00f, 35.00f,   0.00f,  67.00f,  70.00f, -48.00f,   0.00f,   0.00f,  48.00f,  69.00f, 100.00f,  15.00f,  0.00f,  50.00f, 100.00f,   7.00f, 0.81f,   0.00f,   0.00f, -1.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f, 100.00f,  0.00f, -12.00f,  0.00f,  -8.80f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Rubber Bass [ZF]",        1.00f, 35.00f,   0.00f,  36.00f,  15.00f,  50.00f,  20.00f,   0.00f,   0.00f,  38.00f,   0.00f,  25.00f,  0.00f,  60.00f, 100.00f,  22.00f, 0.19f,   0.00f,   0.00f, -2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,  49.00f,  0.00f, -12.00f,  0.00f,   1.60f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Soft Pick Bass": old vibrato=-12 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Soft Pick Bass",          0.00f, 22.00f,   0.00f,  33.00f,  47.00f,  42.00f,  16.00f,  18.00f,   0.00f,   0.00f,   0.00f,  25.00f,  4.00f,  58.00f,   0.00f,  22.00f, 0.15f,   0.00f,  33.00f, -2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,  37.00f,  0.00f,   0.00f,  0.00f,   7.80f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Fretless Bass",           1.00f, 34.00f,   0.00f,  51.00f,   0.00f,  16.00f,   0.00f,  34.00f,   0.00f,   9.00f,   0.00f,  25.00f, 20.00f,  85.00f,   0.00f,  30.00f, 0.81f,  40.00f,   0.00f, -2.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,  50.00f,  0.00f,   0.00f,  0.00f, -14.40f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Whistler",                0.00f, 35.00f,   0.00f,  33.00f, 100.00f,   0.00f,   0.00f,   0.00f,   0.00f,  29.00f,   0.00f,  25.00f, 68.00f,  39.00f,  58.00f,  36.00f, 0.81f,  28.00f,  38.00f,  2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  23.00f,  0.00f,   0.00f,  0.00f,  -0.70f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Very Soft Pad",           2.00f, 12.00f,   0.00f,  35.00f,  78.00f,   0.00f,   0.00f,   0.00f,   0.00f,  30.00f,   0.00f,  25.00f, 35.00f,  50.00f,  80.00f,  70.00f, 0.81f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  39.00f,  0.00f,   0.00f,  0.00f,  -4.90f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Pizzicato",               0.00f, 35.00f,   0.00f,  23.00f,  20.00f,  50.00f,   0.00f,   0.00f,   0.00f,  22.00f,   0.00f,  25.00f,  0.00f,  47.00f,   0.00f,  30.00f, 0.81f,   0.00f,  80.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Synth Strings",           0.00f,  0.00f,  -0.97f,  42.00f,  26.00f,  50.00f,  14.00f,  38.00f,   0.00f,  67.00f,  55.00f,  97.00f, 82.00f,  70.00f, 100.00f,  42.00f, 0.84f,  34.00f,  30.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f, 100.00f,  0.00f,   0.00f,  0.00f,  -7.10f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Synth Strings 2": old vibrato=-46 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Synth Strings 2",         0.00f, 49.00f,   0.00f,  55.00f,  16.00f,  38.00f,   8.00f, -60.00f,  76.00f,  29.00f,  76.00f, 100.00f, 46.00f,  80.00f, 100.00f,  39.00f, 0.79f,   0.00f,   0.00f,  1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  75.00f,  0.00f,   0.00f,  0.00f,  -3.80f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Leslie Organ": old vibrato=-52 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Leslie Organ",            0.00f, 13.00f,  -0.38f,  38.00f,  74.00f,   8.00f,  20.00f,-100.00f,   0.00f,  55.00f,  52.00f,  31.00f,  0.00f,  17.00f,  73.00f,  28.00f, 0.87f,   0.00f,   0.00f, -1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,   0.00f,  0.00f,   0.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Click Organ": old vibrato=-2 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Click Organ",             0.00f, 35.00f,   0.00f,  44.00f,  50.00f,  30.00f,  16.00f,-100.00f,   0.00f,   0.00f,  18.00f,   0.00f,  0.00f,  75.00f,  80.00f,   0.00f, 0.81f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  50.00f,  0.00f,  12.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Hard Organ": old vibrato=-2 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Hard Organ",              0.00f, 35.00f,   0.00f,  51.00f,  62.00f,   8.00f,   0.00f,-100.00f,   0.00f,  37.00f,   0.00f, 100.00f,  4.00f,   8.00f,  72.00f,   4.00f, 0.77f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  89.00f,  0.00f,  19.00f,  0.00f,  -0.90f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Bass Clarinet": old vibrato=-2 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Bass Clarinet",           1.00f,  0.00f,   0.00f,  51.00f,  10.00f,   0.00f,  11.00f,   0.00f,   0.00f,   0.00f,   0.00f,  25.00f, 35.00f,  65.00f,  65.00f,  32.00f, 0.79f,   0.00f,  20.00f, -1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f, 100.00f,  0.00f,   0.00f,  0.00f,   0.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Trumpet",                 1.00f,  6.00f,   0.00f,  57.00f,   0.00f, -36.00f,  15.00f,   0.00f,  21.00f,  15.00f,   0.00f,  25.00f, 24.00f,  60.00f,  80.00f,  10.00f, 0.75f,  10.00f,  25.00f,  1.00f,  0.00f,  0.00f, 0.00f,  0.00f, 100.00f,   0.00f,  0.00f,   0.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Soft Horn",               0.00f, 35.00f,   0.00f,  50.00f,  21.00f, -42.00f,  12.00f,  20.00f,   0.00f,  35.00f,  36.00f,  25.00f,  8.00f,  50.00f, 100.00f,  27.00f, 0.83f,   2.00f,  10.00f, -1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  12.00f,  0.00f,  19.00f,  0.00f,   1.90f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Brass Section",           0.00f, 28.00f,  -0.79f,  50.00f,   0.00f,  18.00f,   0.00f,   0.00f,  24.00f,  16.00f,  91.00f,   8.00f, 17.00f,  50.00f,  80.00f,  45.00f, 0.81f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  43.00f,  0.00f,  12.00f,  0.00f,  -7.90f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Synth Brass",             0.00f, 30.00f,  -3.07f,  39.00f,  15.00f,  50.00f,   0.00f,   0.00f,  39.00f,  30.00f,  82.00f,  25.00f, 33.00f,  74.00f,  76.00f,  41.00f, 0.81f,   0.00f,  23.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  40.00f,  0.00f,   0.00f,  0.00f,  -6.30f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Detuned Syn Brass [ZF]": old vibrato=-26 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Detuned Syn Brass [ZF]",  0.00f, 31.00f,   0.50f,  26.00f,   7.00f,  70.00f,   0.00f,  32.00f,   0.00f,  83.00f,   0.00f,   5.00f,  0.00f,  75.00f,  54.00f,  32.00f, 0.76f,   0.00f,  29.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  68.00f,  0.00f,   0.00f,  0.00f,  31.80f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Power PWM": old vibrato=-26 (PWM mode) → Pulse waveform, vibrato=0, pwmDepth=26
    presets_.emplace_back("Power PWM",               0.00f, 35.00f,   0.00f,  82.00f,  13.00f,  50.00f,   0.00f,-100.00f,  24.00f,  30.00f,  88.00f,  34.00f,  0.00f,  50.00f, 100.00f,  48.00f, 0.71f,   0.00f,   0.00f, -1.00f,  0.00f,  0.00f, 1.00f, 26.00f, 100.00f, 100.00f,  0.00f, -12.00f,  0.00f,  -8.80f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Water Velocity [SA]",     0.00f, 49.00f,   0.00f,  87.00f,  67.00f, 100.00f,  32.00f, -82.00f,  95.00f,  56.00f,  72.00f, 100.00f,  4.00f,  76.00f,  11.00f,  46.00f, 0.88f,  44.00f,   0.00f, -1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  76.00f,  0.00f,   0.00f,  0.00f,  -1.40f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Ghost [SA]",              2.00f, 16.00f,   0.00f,  38.00f,  58.00f,  50.00f,  16.00f,  62.00f,   0.00f,  30.00f,  40.00f,  31.00f, 37.00f,  50.00f, 100.00f,  54.00f, 0.85f,  66.00f,  43.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  75.00f,  0.00f,   0.00f,  0.00f,  -7.10f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Soft E.Piano",            0.00f, 35.00f,   0.00f,  34.00f,  26.00f,   6.00f,   0.00f,  26.00f,   0.00f,  22.00f,   0.00f,  39.00f,  0.00f,  80.00f,   0.00f,  44.00f, 0.81f,   2.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  31.00f,  0.00f,   0.00f,  0.00f,  -0.20f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Thumb Piano",             0.00f, 35.00f,   0.00f,  37.00f,  47.00f,   8.00f,   0.00f,   0.00f,   0.00f,  45.00f,   0.00f,  39.00f,  0.00f,  39.00f,   0.00f,  48.00f, 0.81f,  20.00f,   0.00f,  1.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  72.00f,  0.00f,  15.00f,  0.00f,  50.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    // "Steel Drums [ZF]": old vibrato=-28 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Steel Drums [ZF]",        0.00f, 18.00f,   2.30f,  40.00f,  30.00f,   8.00f,  17.00f, -20.00f,   0.00f,  42.00f,  23.00f,  47.00f, 12.00f,  48.00f,   0.00f,  49.00f, 0.53f,   0.00f,  34.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  81.00f,  0.00f,  12.00f,  0.00f, -12.00f, 2.00f, 2.00f, 50.00f, 50.00f);
    // "Car Horn": old vibrato=-24 (PWM mode) → Pulse waveform, vibrato=0
    presets_.emplace_back("Car Horn",                0.00f, 35.00f,   0.00f,  46.00f,   0.00f,  36.00f,   0.00f,   0.00f,  46.00f,  30.00f, 100.00f,  23.00f, 30.00f,  50.00f, 100.00f,  31.00f, 1.00f,   0.00f,   0.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  57.00f,  0.00f,  -1.00f,  0.00f,  -2.80f, 2.00f, 2.00f, 50.00f, 50.00f);
    presets_.emplace_back("Helicopter",              0.00f, 35.00f,   0.00f,   8.00f,  36.00f,  38.00f, 100.00f,   0.00f, 100.00f, 100.00f,   0.00f, 100.00f, 96.00f,  50.00f, 100.00f,  92.00f, 0.97f,   0.00f, 100.00f, -2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Arctic Wind",             0.00f, 35.00f,   0.00f,  16.00f,  85.00f,   0.00f,  28.00f,   0.00f,  37.00f,  30.00f,   0.00f,  25.00f, 89.00f,  50.00f, 100.00f,  89.00f, 0.24f,   0.00f, 100.00f,  2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Thip",                    0.00f, 35.00f,   0.00f,   0.00f, 100.00f,  94.00f,   0.00f,   0.00f,   2.00f,  20.00f,   0.00f,  20.00f,  0.00f,  46.00f,   0.00f,  30.00f, 0.81f,   0.00f,  78.00f,  0.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f, 100.00f,  0.00f,  -7.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Synth Tom",               0.00f, 76.00f,  24.53f,  30.00f,  33.00f,  52.00f,   0.00f,  36.00f,   0.00f,  59.00f,   0.00f,  59.00f, 10.00f,  50.00f,   0.00f,  50.00f, 0.81f,   0.00f,  70.00f, -2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,   0.00f,  0.00f, -12.00f,  0.00f,   0.00f, 0.00f, 0.00f, 50.00f, 50.00f);
    presets_.emplace_back("Squelchy Frog",           2.00f, 77.00f, -36.00f,  40.00f,  65.00f,  90.00f,   0.00f,   0.00f,  33.00f,  50.00f,   0.00f,  25.00f,  0.00f,  70.00f,  65.00f,  18.00f, 0.32f, 100.00f,   0.00f, -2.00f,  0.00f,  0.00f, 1.00f,  0.00f, 100.00f,  50.00f,  0.00f,  -5.00f,  0.00f,  -7.90f, 0.00f, 0.00f, 50.00f, 50.00f);
    // clang-format on
}

} // namespace synth
