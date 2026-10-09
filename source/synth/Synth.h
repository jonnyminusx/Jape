#pragma once

#include "Parameters.h"
#include "Voice.h"
#include "dsp/LinearSmoothedValue.h"
#include "dsp/NoiseGenerator.h"
#include "midi/MidiProcessor.h"
#include "midi/NoteHandler.h"

#include <array>
#include <cstddef>

namespace dsp
{
class AudioBuffer;
} // namespace dsp

namespace synth
{

class Synth : public midi::NoteHandler
{
  public:
    Synth();

    void allocateResources(const float sampleRate, const int samplesPerBlock);
    void deallocateResources() const;
    void reset();
    void render(dsp::AudioBuffer& audioBuffer);

    // midi::NoteHandler interface
    void noteOn(int note, int velocity) override;
    void noteOff(int note) override;
    void allNotesOff() override;
    void sustainPedalReleased() override;

    midi::MidiProcessor& midiProcessor() { return midiProcessor_; }

    void setParameters(const Parameters& parameters);
    void setOutputLevelInstantly(const float outputLevel);

    static int lfoMaxSamplesPerUpdate();

  private:
    void startVoice(const size_t voiceIdx, const int note, const int velocity);
    void restartMonoVoice(const int note, const int velocity);
    size_t selectVoiceIndexToUse() const;
    bool isPolyphonic() const;
    bool isPlayingLegatoStyle() const;
    void shiftQueuedNotes();
    int nextQueuedNote();
    void processLastNotePriority(const int note);
    void updateLfo();

    static constexpr int kMaxNumVoices{8};
    int numVoices_{1};
    std::array<Voice, kMaxNumVoices> voices_;
    int lastNote_{0};

    Parameters parameters_{};

    midi::MidiProcessor midiProcessor_;

    dsp::NoiseGenerator noiseGenerator_;

    dsp::LinearSmoothedValue<float> outputLevelSmoother_{};

    float sampleRate_{44100.0f};

    float lfo_{0.0f};
    int lfoStep_{0};

    float filterZip_{0.0f};
};

} // namespace synth
