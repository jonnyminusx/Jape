#include "midi/MidiProcessor.h"

#include "midi/NoteHandler.h"

#include <cmath>

namespace midi
{

MidiProcessor::MidiProcessor(NoteHandler& handler) : handler_(handler) {}

void MidiProcessor::process(const uint8_t data0, const uint8_t data1, const uint8_t data2)
{
    switch (data0 & 0xF0)
    {
    case 0x80: // Note Off
    {
        const uint8_t note{static_cast<uint8_t>(data1 & 0x7F)};
        handler_.noteOff(note);
        break;
    }
    case 0x90: // Note On
    {
        const uint8_t note{static_cast<uint8_t>(data1 & 0x7F)};
        const uint8_t velocity{static_cast<uint8_t>(data2 & 0x7F)};
        if (velocity == 0)
        {
            handler_.noteOff(note);
        }
        else
        {
            handler_.noteOn(note, velocity);
        }
        break;
    }
    case 0xB0: // Control Change
        controlChange(data1 & 0x7F, data2 & 0x7F);
        break;
    case 0xC0: // Program Change — handled by the plugin layer
        break;
    case 0xD0: // Channel Aftertouch
    {
        const uint8_t pressure{static_cast<uint8_t>(data1 & 0x7F)};
        state_.pressure = 0.0001f * float(pressure * pressure);
        break;
    }
    case 0xE0: // Pitch Bend
    {
        const int raw14{(static_cast<int>(data2) << 7) | static_cast<int>(data1)};
        state_.pitchBend = std::exp(-0.000014102f * float(raw14 - 8192));
        break;
    }
    default:
        break;
    }
}

void MidiProcessor::controlChange(const uint8_t controller, const uint8_t value)
{
    switch (controller)
    {
    case 0x01: // Mod Wheel
        state_.modWheel = float(value) / 127.0f;
        break;

    case 0x40: // Sustain pedal
        state_.sustainPedal = value >= 64;
        if (!state_.sustainPedal)
        {
            handler_.sustainPedalReleased();
        }
        break;

    default:
        if (controller >= 0x78)
        {
            state_.sustainPedal = false;
            handler_.allNotesOff();
        }
        break;
    }
}

} // namespace midi
