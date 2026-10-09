#pragma once

namespace midi
{

struct NoteHandler
{
    virtual ~NoteHandler() = default;
    virtual void noteOn(int note, int velocity) = 0;
    virtual void noteOff(int note) = 0;
    virtual void allNotesOff() = 0;
    virtual void sustainPedalReleased() = 0;
};

} // namespace midi
