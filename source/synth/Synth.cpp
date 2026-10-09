#include "Synth.h"

#include "Envelope.h"
#include "GlideMode.h"
#include "Output.h"
#include "dsp/AudioBuffer.h"
#include "math/constants.h"
#include "midi/MidiState.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <span>

namespace
{
constexpr int kLfoMaxSamplesPerUpdate{32};
} // namespace

namespace synth
{

Synth::Synth() : midiProcessor_(*this) {}

void Synth::allocateResources(const float sampleRate, [[maybe_unused]] const int samplesPerBlock)
{
    sampleRate_ = sampleRate;

    for (Voice& voice : voices_)
    {
        voice.filter().setSampleRate(sampleRate_);
    }
}

void Synth::deallocateResources() const {}

void Synth::reset()
{
    for (Voice& voice : voices_)
    {
        voice.reset();
    }

    noiseGenerator_.reset();
    midiProcessor_.state() = midi::MidiState{};
    outputLevelSmoother_.reset(sampleRate_, 0.05);
    lfo_ = 0.0f;
    lfoStep_ = 0;
    filterZip_ = 0.0f;
}

void Synth::render(dsp::AudioBuffer& audioBuffer)
{
    const auto audioBufferLeft{audioBuffer.channelBuffer(0)};
    const auto audioBufferRight{audioBuffer.channelCount() > 1 ? audioBuffer.channelBuffer(1) : std::span<float>{}};

    for (size_t sampleIndex = 0; sampleIndex < audioBuffer.sampleCount(); ++sampleIndex)
    {
        updateLfo();

        const float noise{noiseGenerator_.nextValue() * parameters_.oscillator.noiseMix};

        Output output;

        for (Voice& voice : voices_)
        {
            output += voice.render(noise,
                                   midiProcessor_.state().pitchBend,
                                   parameters_.oscillator.osc1.tune,
                                   parameters_.oscillator.osc2.tune);
        }

        output *= outputLevelSmoother_.getNextValue();

        if (audioBuffer.channelCount() > 1)
        {
            audioBufferLeft[sampleIndex] = output.left;
            audioBufferRight[sampleIndex] = output.right;
        }
        else
        {
            audioBufferLeft[sampleIndex] = (output.left + output.right) * 0.5f;
        }
    }

    audioBuffer.sanitise();
}

void Synth::noteOn(const int note, const int velocity)
{
    int vel{velocity};
    if (parameters_.output.ignoreVelocity)
    {
        vel = 80;
    }

    const size_t voiceIdx{selectVoiceIndexToUse()};
    if (isPolyphonic())
    {
        startVoice(voiceIdx, note, vel);
    }
    else
    {
        if (voices_[voiceIdx].note() > 0)
        {
            shiftQueuedNotes();
            restartMonoVoice(note, vel);
        }
        else
        {
            startVoice(voiceIdx, note, vel);
        }
    }
}

void Synth::noteOff(const int note)
{
    processLastNotePriority(note);

    for (Voice& voice : voices_)
    {
        voice.noteOff(note, midiProcessor_.state().sustainPedal);
    }
}

void Synth::allNotesOff()
{
    for (Voice& voice : voices_)
    {
        voice.reset();
    }
}

void Synth::sustainPedalReleased()
{
    if (!isPolyphonic() && voices_[0].note() == Voice::sustain)
    {
        const int queued{nextQueuedNote()};
        if (queued > 0)
        {
            restartMonoVoice(queued, -1);
            return;
        }
    }

    for (Voice& voice : voices_)
    {
        voice.noteOff(Voice::sustain, false);
    }
}

void Synth::updateLfo()
{
    if (--lfoStep_ < 0)
    {
        lfoStep_ = kLfoMaxSamplesPerUpdate;

        lfo_ += parameters_.lfo.increment;
        if (lfo_ > math::pi)
        {
            lfo_ -= math::tau;
        }

        const midi::MidiState& midi{midiProcessor_.state()};

        const float sineValue{std::sin(lfo_)};
        const float vibratoModulation{1.0f + sineValue * (midi.modWheel + parameters_.lfo.vibratoAmount)};
        const float pwmMod0{2.0f * parameters_.oscillator.osc1.pulseWidth +
                            sineValue * (midi.modWheel + parameters_.lfo.pwmDepth)};
        const float pwmMod1{2.0f * parameters_.oscillator.osc2.pulseWidth +
                            sineValue * (midi.modWheel + parameters_.lfo.pwmDepth)};
        const float filterMod{parameters_.filter.keyTracking +
                              (parameters_.filter.lfoDepth + midi.pressure) * sineValue};
        filterZip_ += 0.005f * (filterMod - filterZip_);

        for (Voice& voice : voices_)
        {
            voice.setModulation(vibratoModulation, pwmMod0, pwmMod1);
            voice.updateLfo(parameters_.glide.rateCoefficient,
                            filterZip_,
                            parameters_.filter.q,
                            midi.pitchBend,
                            parameters_.filter.envelopeDepth);
            voice.updatePeriod(midi.pitchBend, parameters_.oscillator.osc1.tune, parameters_.oscillator.osc2.tune);
        }
    }
}

void Synth::shiftQueuedNotes()
{
    for (size_t voiceIdx = kMaxNumVoices - 1; voiceIdx > 0; --voiceIdx)
    {
        voices_[voiceIdx].note() = voices_[voiceIdx - 1].note();
        voices_[voiceIdx].release();
    }
}

int Synth::nextQueuedNote()
{
    int note{0};
    for (size_t voiceIdx = 1; voiceIdx < kMaxNumVoices; ++voiceIdx)
    {
        const int voiceNote{voices_[voiceIdx].note()};
        if (voiceNote > 0)
        {
            voices_[voiceIdx].note() = 0;
            note = voiceNote;
            break;
        }
    }

    return note;
}

void Synth::startVoice(const size_t voiceIdx, const int note, const int velocity)
{
    if (voiceIdx >= voices_.size())
    {
        return;
    }

    Voice& voice{voices_[voiceIdx]};

    Envelope& envelope{voice.envelope()};
    envelope.attack();

    Envelope& filterEnvelope{voice.filterEnvelope()};
    filterEnvelope.attack();

    voice.noteOn(note, lastNote_, velocity, sampleRate_, voiceIdx, isPlayingLegatoStyle(), parameters_);
    lastNote_ = note;
}

void Synth::restartMonoVoice(const int note, [[maybe_unused]] const int velocity)
{
    voices_[0].noteOnRestart(note, velocity, sampleRate_, 0, parameters_);
}

size_t Synth::selectVoiceIndexToUse() const
{
    if (!isPolyphonic())
    {
        return 0;
    }

    size_t idx{0};
    float minLevel{std::numeric_limits<float>::max()};

    for (size_t i = 1, n = voices_.size(); i < n; ++i)
    {
        const Voice& voice{voices_[i]};

        if (!voice.envelope().isInAttack())
        {
            const float currentLevel{voice.envelope().currentValue()};
            if (currentLevel < minLevel)
            {
                minLevel = currentLevel;
                idx = i;
            }
        }
    }

    return idx;
}

bool Synth::isPolyphonic() const
{
    return numVoices_ > 1;
}

bool Synth::isPlayingLegatoStyle() const
{
    return std::any_of(voices_.begin(), voices_.end(), [](const Voice& voice) { return voice.note() > 0; });
}

void Synth::processLastNotePriority(const int note)
{
    if (isPolyphonic())
    {
        return;
    }

    if (voices_[0].note() != note)
    {
        return;
    }

    int queuedNote{nextQueuedNote()};

    if (queuedNote > 0)
    {
        restartMonoVoice(queuedNote, -1);
    }
}

void Synth::setParameters(const Parameters& params)
{
    for (Voice& voice : voices_)
    {
        voice.envelope().setADSR(params.envelope);
        voice.filterEnvelope().setADSR(params.filter.envelope);
        voice.setWaveforms(params.oscillator.osc1.waveform, params.oscillator.osc2.waveform);
    }
    outputLevelSmoother_.setTargetValue(params.output.gain);
    numVoices_ = params.output.polyphonic ? kMaxNumVoices : 1;
    parameters_ = params;
}

void Synth::setOutputLevelInstantly(const float outputLevel)
{
    outputLevelSmoother_.setCurrentAndTargetValue(outputLevel);
}

int Synth::lfoMaxSamplesPerUpdate()
{
    return kLfoMaxSamplesPerUpdate;
}

} // namespace synth
