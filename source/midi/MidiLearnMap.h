#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <string_view>

namespace midi
{

class MidiLearnMap
{
  public:
    static constexpr uint8_t kCCUnassigned{0xFF};
    static constexpr size_t kNumParams{34};
    static constexpr std::array<const char*, kNumParams> kParamIds{{
        "glideMode",    "glideRate",      "glideBend",      "filterFreq",     "filterReso",    "filterEnv",
        "filterLFO",    "filterVelocity", "filterAttack",   "filterDecay",    "filterSustain", "filterRelease",
        "envAttack",    "envDecay",       "envSustain",     "envRelease",     "lfoRate",       "vibrato",
        "noise",        "octave",         "tuning",         "outputLevel",    "polyMode",      "pwmDepth",
        "osc1Volume",   "osc2Volume",     "osc1Tune",       "osc2Tune",       "osc1Fine",      "osc2Fine",
        "osc1Waveform", "osc2Waveform",   "osc1PulseWidth", "osc2PulseWidth",
    }};

    MidiLearnMap();

    void beginLearn(std::string_view paramId);
    void cancelLearn();
    void clearLearn(std::string_view paramId);

    uint8_t ccForParam(std::string_view paramId) const;
    const char* learningParamId() const;

    // Audio-thread: if learning active, stores cc, clears learn state, returns captured index.
    // Returns -1 if not learning.
    int tryCaptureLearning(uint8_t cc);

    uint8_t ccAtIndex(size_t idx) const;
    void setCC(size_t idx, uint8_t cc);

    template <typename Visitor> void forEachAssignment(Visitor&& visitor) const
    {
        for (size_t i = 0; i < kNumParams; ++i)
        {
            const uint8_t cc{midiCCMap_[i].load()};
            if (cc != kCCUnassigned)
                visitor(kParamIds[i], cc);
        }
    }

  private:
    int indexForId(std::string_view paramId) const;

    std::array<std::atomic<uint8_t>, kNumParams> midiCCMap_{};
    std::atomic<int> midiLearnIndex_{-1};
};

} // namespace midi
