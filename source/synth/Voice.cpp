#include "Voice.h"

#include "dsp/Filter.h"

#include "Envelope.h"
#include "GlideMode.h"
#include "Output.h"
#include "Parameters.h"
#include "math/constants.h"

#include <algorithm>
#include <cmath>

namespace synth
{

namespace
{

constexpr float kAnalog{0.002f};

float calculatePeriod(const int note, const float tune, const float minOscFactor, const size_t voiceIdx)
{
    float period{tune *
                 std::exp(-0.05776226505f * (static_cast<float>(note) + kAnalog * static_cast<float>(voiceIdx)))};

    while (period < 6.0f || (period * minOscFactor) < 6.0f)
    {
        period += period;
    }

    return period;
}

int calculateNoteDistance(const int note, const int lastNote, const GlideMode glideMode, const bool isLegato)
{
    int noteDistance{0};

    if (lastNote)
    {
        if (glideMode == GlideMode::Always || (glideMode == GlideMode::Legato && isLegato))
        {
            noteDistance = note - lastNote;
        }
    }

    return noteDistance;
}

} // namespace

Voice::Voice()
{
    for (auto& slot : oscillators_)
    {
        slot[static_cast<size_t>(WaveformType::Sawtooth)] = std::make_unique<SawtoothOscillator>();
        slot[static_cast<size_t>(WaveformType::Sine)] = std::make_unique<SineOscillator>();
        slot[static_cast<size_t>(WaveformType::Pulse)] = std::make_unique<PulseOscillator>();
    }
}

void Voice::reset()
{
    note_ = 0;
    for (auto& slot : oscillators_)
        for (auto& osc : slot)
            osc->reset();
    envelope_.reset();
    filterEnvelope_.reset();
    filter_.reset();
    panLeft_ = 0.707f;
    panRight_ = 0.707f;
}

void Voice::setWaveforms(const WaveformType waveform0, const WaveformType waveform1)
{
    if (waveform_[0] != waveform0)
    {
        waveform_[0] = waveform0;
        oscillators_[0][static_cast<size_t>(waveform_[0])]->reset();
    }
    if (waveform_[1] != waveform1)
    {
        waveform_[1] = waveform1;
        oscillators_[1][static_cast<size_t>(waveform_[1])]->reset();
    }
}

void Voice::noteOn(const int note,
                   const int lastNote,
                   const int velocity,
                   const float sampleRate,
                   const size_t voiceIdx,
                   const bool isPlayingLegatoStyle,
                   const Parameters& parameters)
{
    const float adjustedVelocity{(0.004f * static_cast<float>((velocity + 64) * (velocity + 64))) - 8.0f};
    const float minFactor{std::min(parameters.oscillator.osc1.tune, parameters.oscillator.osc2.tune)};
    const float refPeriod{calculatePeriod(note, parameters.oscillator.globalTune, minFactor, voiceIdx)};
    const int noteDistance{calculateNoteDistance(note, lastNote, parameters.glide.mode, isPlayingLegatoStyle)};

    note_ = note;
    cutoff_ = sampleRate / (refPeriod * math::pi);
    cutoff_ *= std::exp(parameters.filter.velocitySensitivity * static_cast<float>(velocity - 64));
    targetPeriod_ = refPeriod;
    period_ = refPeriod * std::pow(1.059463094359f, static_cast<float>(noteDistance) - parameters.glide.bendSemitones);

    if (period_ < 6.0f)
    {
        period_ = 6.0f;
    }

    const size_t wt0{static_cast<size_t>(waveform_[0])};
    const size_t wt1{static_cast<size_t>(waveform_[1])};
    Oscillator& osc0{*oscillators_[0][wt0]};
    Oscillator& osc1{*oscillators_[1][wt1]};

    const float baseAmplitude{parameters.output.volumeTrim * adjustedVelocity};
    osc0.setAmplitude(baseAmplitude * parameters.oscillator.osc1.volume);
    osc1.setAmplitude(baseAmplitude * parameters.oscillator.osc2.volume);

    osc0.noteOn(period_ * parameters.oscillator.osc1.tune);
    osc1.noteOn(period_ * parameters.oscillator.osc2.tune);

    updatePanning();
}

void Voice::noteOnRestart(
    const int note, const int velocity, const float sampleRate, const size_t voiceIdx, const Parameters& parameters)
{
    const float minFactor{std::min(parameters.oscillator.osc1.tune, parameters.oscillator.osc2.tune)};
    const float refPeriod{calculatePeriod(note, parameters.oscillator.globalTune, minFactor, voiceIdx)};

    note_ = note;
    cutoff_ = sampleRate / (refPeriod * math::pi);
    cutoff_ *= std::exp(parameters.filter.velocitySensitivity * static_cast<float>(velocity - 64));

    targetPeriod_ = refPeriod;

    if (GlideMode::Off == parameters.glide.mode)
    {
        period_ = refPeriod;
    }

    envelope_.nudgeLevelUp();
    updatePanning();
}

void Voice::noteOff(const int note, const bool sustainPedalPressed)
{
    if (note_ == note)
    {
        if (sustainPedalPressed)
        {
            note_ = sustain;
        }
        else
        {
            envelope_.release();
            note_ = 0;
        }
    }
}

void Voice::release()
{
    envelope_.release();
    filterEnvelope_.release();
}

void Voice::updatePanning()
{
    const float panning{std::clamp(static_cast<float>(note_ - 60) / 24.0f, -1.0f, 1.0f)};
    panLeft_ = std::sin(math::quarterPi * (1.0f - panning));
    panRight_ = std::sin(math::quarterPi * (1.0f + panning));
}

void Voice::updateLfo(const float glideRate,
                      const float filterMod,
                      const float filterQ,
                      const float pitchBend,
                      const float filterEnvDepth)
{
    period_ += glideRate * (targetPeriod_ - period_);

    const float filterEnvelopeValue{filterEnvelope_.nextValue() * filterEnvDepth};

    float modulatedCutoff{cutoff_ * std::exp(filterMod + filterEnvelopeValue) / pitchBend};
    modulatedCutoff = std::clamp(modulatedCutoff, 30.0f, 20000.0f);
    filter_.updateCoefficients(modulatedCutoff, filterQ);
}

void Voice::updatePeriod(const float pitchBend, const float osc1Tune, const float osc2Tune)
{
    const size_t wt0{static_cast<size_t>(waveform_[0])};
    const size_t wt1{static_cast<size_t>(waveform_[1])};
    oscillators_[0][wt0]->setPeriod(period_ * osc1Tune * pitchBend * vibratoMod_);
    oscillators_[1][wt1]->setPeriod(period_ * osc2Tune * pitchBend * vibratoMod_);
}

void Voice::setModulation(const float vibratoMod, const float pwmMod0, const float pwmMod1)
{
    if (!envelope_.isActive())
        return;
    vibratoMod_ = vibratoMod;
    oscillators_[0][static_cast<size_t>(waveform_[0])]->setModulation(pwmMod0);
    oscillators_[1][static_cast<size_t>(waveform_[1])]->setModulation(pwmMod1);
}

Output Voice::render(const float input, const float pitchBend, const float osc1Tune, const float osc2Tune)
{
    Output output;

    updatePeriod(pitchBend, osc1Tune, osc2Tune);

    if (!envelope_.isActive())
    {
        envelope_.reset();
        filter_.reset();
        return output;
    }

    const size_t wt0{static_cast<size_t>(waveform_[0])};
    const size_t wt1{static_cast<size_t>(waveform_[1])};
    const float oscillatorOutput{oscillators_[0][wt0]->nextSample() + oscillators_[1][wt1]->nextSample()};

    output.left = oscillatorOutput + input;
    output.right = output.left;
    output.filter(filter_);
    output *= envelope_.nextValue();

    output.left *= panLeft_;
    output.right *= panRight_;

    return output;
}

int Voice::note() const
{
    return note_;
}

int& Voice::note()
{
    return note_;
}

const Envelope& Voice::envelope() const
{
    return envelope_;
}

Envelope& Voice::envelope()
{
    return envelope_;
}

const Envelope& Voice::filterEnvelope() const
{
    return filterEnvelope_;
}

Envelope& Voice::filterEnvelope()
{
    return filterEnvelope_;
}

const dsp::Filter& Voice::filter() const
{
    return filter_;
}

dsp::Filter& Voice::filter()
{
    return filter_;
}

} // namespace synth
