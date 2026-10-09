#pragma once

#include "dsp/Filter.h"

#include "Envelope.h"
#include "PulseOscillator.h"
#include "SawtoothOscillator.h"
#include "SineOscillator.h"
#include "WaveformType.h"

#include <array>
#include <cstddef>
#include <memory>

namespace synth
{

struct Output;
struct Parameters;

class Voice
{
  public:
    const static int sustain{-1};

    Voice();
    void reset();
    void setWaveforms(WaveformType waveform0, WaveformType waveform1);
    void noteOn(const int note,
                const int lastNote,
                const int velocity,
                const float sampleRate,
                const size_t voiceIdx,
                const bool isPlayingLegatoStyle,
                const Parameters& parameters);
    void noteOnRestart(const int note,
                       const int velocity,
                       const float sampleRate,
                       const size_t voiceIdx,
                       const Parameters& parameters);
    void noteOff(const int note, const bool sustainPedalPressed);
    void release();
    void updatePanning();
    void updateLfo(const float glideRate,
                   const float filterMod,
                   const float filterQ,
                   const float pitchBend,
                   const float filterEnvDepth);
    void updatePeriod(const float pitchBend, const float osc1Tune, const float osc2Tune);
    void setModulation(const float vibratoMod, const float pwmMod0, const float pwmMod1);
    Output render(const float input, const float pitchBend, const float osc1Tune, const float osc2Tune);

    int note() const;
    int& note();
    const Envelope& envelope() const;
    Envelope& envelope();

    const Envelope& filterEnvelope() const;
    Envelope& filterEnvelope();

    const dsp::Filter& filter() const;
    dsp::Filter& filter();

  private:
    int note_{0};

    float period_{0.0f};
    float targetPeriod_{0.0f};
    float panLeft_{0.0f};
    float panRight_{0.0f};
    float cutoff_{0.0f};
    float vibratoMod_{1.0f};

    // Per-slot array indexed by waveform type: oscillators_[slot][waveformIndex].
    // All instances are constructed at Voice initialisation — no audio-thread allocation.
    // setWaveforms() only updates waveform_; it never allocates or deallocates.
    static constexpr size_t kWaveformCount{static_cast<size_t>(WaveformType::Count)};
    static constexpr size_t kOscillatorCount{2};
    std::array<WaveformType, kOscillatorCount> waveform_{WaveformType::Sawtooth, WaveformType::Sawtooth};
    std::array<std::array<std::unique_ptr<Oscillator>, kWaveformCount>, kOscillatorCount> oscillators_;

    Envelope envelope_;
    Envelope filterEnvelope_;
    dsp::Filter filter_;
};

} // namespace synth
