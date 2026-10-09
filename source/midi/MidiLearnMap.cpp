#include "midi/MidiLearnMap.h"

namespace midi
{

MidiLearnMap::MidiLearnMap()
{
    for (std::atomic<uint8_t>& entry : midiCCMap_)
        entry.store(kCCUnassigned);
}

int MidiLearnMap::indexForId(std::string_view paramId) const
{
    for (size_t i = 0; i < kNumParams; ++i)
    {
        if (kParamIds[i] == paramId)
            return static_cast<int>(i);
    }
    return -1;
}

void MidiLearnMap::beginLearn(std::string_view paramId)
{
    const int idx{indexForId(paramId)};
    if (idx >= 0)
        midiLearnIndex_.store(idx);
}

void MidiLearnMap::cancelLearn()
{
    midiLearnIndex_.store(-1);
}

void MidiLearnMap::clearLearn(std::string_view paramId)
{
    const int idx{indexForId(paramId)};
    if (idx >= 0)
        midiCCMap_[static_cast<size_t>(idx)].store(kCCUnassigned);
}

uint8_t MidiLearnMap::ccForParam(std::string_view paramId) const
{
    const int idx{indexForId(paramId)};
    if (idx >= 0)
        return midiCCMap_[static_cast<size_t>(idx)].load();
    return kCCUnassigned;
}

const char* MidiLearnMap::learningParamId() const
{
    const int idx{midiLearnIndex_.load()};
    if (idx >= 0 && static_cast<size_t>(idx) < kNumParams)
        return kParamIds[static_cast<size_t>(idx)];
    return nullptr;
}

int MidiLearnMap::tryCaptureLearning(uint8_t cc)
{
    int idx{midiLearnIndex_.load()};
    if (idx < 0)
        return -1;
    if (!midiLearnIndex_.compare_exchange_strong(idx, -1))
        return -1;
    midiCCMap_[static_cast<size_t>(idx)].store(cc);
    return idx;
}

uint8_t MidiLearnMap::ccAtIndex(size_t idx) const
{
    return midiCCMap_[idx].load(std::memory_order_relaxed);
}

void MidiLearnMap::setCC(size_t idx, uint8_t cc)
{
    midiCCMap_[idx].store(cc);
}

} // namespace midi
