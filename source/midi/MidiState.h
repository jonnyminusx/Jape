#pragma once

namespace midi
{

struct MidiState
{
    float pitchBend{1.0f};
    float modWheel{0.0f};
    float pressure{0.0f};
    bool sustainPedal{false};
};

} // namespace midi
