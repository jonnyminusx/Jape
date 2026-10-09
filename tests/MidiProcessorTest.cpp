#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "midi/MidiProcessor.h"
#include "midi/NoteHandler.h"

#include <cmath>
#include <vector>

namespace {

struct FakeHandler : midi::NoteHandler
{
    struct NoteOnEvent { int note; int velocity; };

    std::vector<NoteOnEvent> noteOns;
    std::vector<int>         noteOffs;
    int sustainPedalReleasedCount{0};
    int allNotesOffCount{0};

    void noteOn(int note, int velocity) override { noteOns.push_back({note, velocity}); }
    void noteOff(int note) override               { noteOffs.push_back(note); }
    void allNotesOff() override                   { ++allNotesOffCount; }
    void sustainPedalReleased() override          { ++sustainPedalReleasedCount; }
};

} // namespace

// ─── Initial state ────────────────────────────────────────────────────────────

TEST_CASE("MidiState defaults", "[midi]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    REQUIRE(proc.state().pitchBend    == Catch::Approx(1.0f));
    REQUIRE(proc.state().modWheel     == Catch::Approx(0.0f));
    REQUIRE(proc.state().pressure     == Catch::Approx(0.0f));
    REQUIRE(proc.state().sustainPedal == false);
}

// ─── Note On / Note Off ───────────────────────────────────────────────────────

TEST_CASE("Note On fires noteOn with correct note and velocity", "[midi][note]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    proc.process(0x90, 60, 100);

    REQUIRE(handler.noteOns.size() == 1);
    REQUIRE(handler.noteOns[0].note     == 60);
    REQUIRE(handler.noteOns[0].velocity == 100);
}

TEST_CASE("Note On with velocity 0 is treated as Note Off", "[midi][note]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    proc.process(0x90, 60, 0);

    REQUIRE(handler.noteOns.empty());
    REQUIRE(handler.noteOffs.size() == 1);
    REQUIRE(handler.noteOffs[0] == 60);
}

TEST_CASE("Note Off fires noteOff", "[midi][note]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    proc.process(0x80, 48, 64);

    REQUIRE(handler.noteOffs.size() == 1);
    REQUIRE(handler.noteOffs[0] == 48);
}

TEST_CASE("MIDI channel nibble is ignored", "[midi][note]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    proc.process(0x95, 48, 80); // Note On on channel 6

    REQUIRE(handler.noteOns.size() == 1);
    REQUIRE(handler.noteOns[0].note == 48);
}

TEST_CASE("Note data bytes are masked to 7 bits", "[midi][note]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    proc.process(0x90, 0x80 | 60, 0x80 | 100); // bit 7 set on data bytes

    REQUIRE(handler.noteOns.size() == 1);
    REQUIRE(handler.noteOns[0].note     == 60);
    REQUIRE(handler.noteOns[0].velocity == 100);
}

// ─── Mod Wheel ────────────────────────────────────────────────────────────────

TEST_CASE("Mod wheel (CC 0x01) maps value to 0..1", "[midi][cc]")
{
    struct TestCase { int ccValue; float expected; };
    auto [ccValue, expected] = GENERATE(values<TestCase>({
        {.ccValue = 0,   .expected = 0.0f},
        {.ccValue = 127, .expected = 1.0f},
        {.ccValue = 64,  .expected = 64.0f / 127.0f}
    }));
    FakeHandler handler;
    midi::MidiProcessor proc{handler};
    proc.process(0xB0, 0x01, ccValue);
    REQUIRE(proc.state().modWheel == Catch::Approx(expected));
}

// ─── Sustain Pedal ────────────────────────────────────────────────────────────

TEST_CASE("Sustain pedal (CC 0x40)", "[midi][cc]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    SECTION("value >= 64 sets sustainPedal")
    {
        proc.process(0xB0, 0x40, 64);
        REQUIRE(proc.state().sustainPedal == true);
        REQUIRE(handler.sustainPedalReleasedCount == 0);
    }

    SECTION("value < 64 clears sustainPedal and calls sustainPedalReleased")
    {
        proc.process(0xB0, 0x40, 127);
        proc.process(0xB0, 0x40, 0);
        REQUIRE(proc.state().sustainPedal == false);
        REQUIRE(handler.sustainPedalReleasedCount == 1);
    }

    SECTION("sustainPedalReleased not called when pedal first pressed")
    {
        proc.process(0xB0, 0x40, 127);
        REQUIRE(handler.sustainPedalReleasedCount == 0);
    }

    SECTION("threshold boundary: 63 clears, 64 sets")
    {
        proc.process(0xB0, 0x40, 63);
        REQUIRE(proc.state().sustainPedal == false);
        proc.process(0xB0, 0x40, 64);
        REQUIRE(proc.state().sustainPedal == true);
    }
}

// ─── All Notes Off ────────────────────────────────────────────────────────────

TEST_CASE("All Notes Off (CC >= 0x78)", "[midi][cc]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    SECTION("CC 0x78 triggers allNotesOff and clears sustain")
    {
        proc.process(0xB0, 0x40, 127); // press sustain
        proc.process(0xB0, 0x78, 0);
        REQUIRE(handler.allNotesOffCount == 1);
        REQUIRE(proc.state().sustainPedal == false);
    }

    SECTION("CC 0x7B (All Notes Off) also triggers")
    {
        proc.process(0xB0, 0x7B, 0);
        REQUIRE(handler.allNotesOffCount == 1);
    }

    SECTION("CC 0x77 does not trigger (below threshold)")
    {
        proc.process(0xB0, 0x77, 0);
        REQUIRE(handler.allNotesOffCount == 0);
    }
}

// ─── Channel Aftertouch ───────────────────────────────────────────────────────

TEST_CASE("Channel Aftertouch (0xD0)", "[midi]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    SECTION("zero input gives zero pressure")
    {
        proc.process(0xD0, 0, 0);
        REQUIRE(proc.state().pressure == Catch::Approx(0.0f));
    }

    SECTION("pressure follows quadratic curve")
    {
        proc.process(0xD0, 127, 0);
        REQUIRE(proc.state().pressure == Catch::Approx(0.0001f * 127.0f * 127.0f));
    }

    SECTION("data1 is masked to 7 bits")
    {
        // 0xFF masked to 0x7F = 127, should give same result as passing 127 directly
        proc.process(0xD0, 127, 0);
        float expected = proc.state().pressure;

        proc.process(0xD0, 0xFF, 0);
        REQUIRE(proc.state().pressure == Catch::Approx(expected));
    }
}

// ─── Pitch Bend ───────────────────────────────────────────────────────────────

TEST_CASE("Pitch Bend (0xE0)", "[midi]")
{
    FakeHandler handler;
    midi::MidiProcessor proc{handler};

    SECTION("center position (data1=0, data2=64) gives pitchBend = 1.0")
    {
        proc.process(0xE0, 0, 64);
        REQUIRE(proc.state().pitchBend == Catch::Approx(1.0f).epsilon(1e-5f));
    }

    SECTION("full bend up (data1=127, data2=127) reduces pitchBend below 1.0")
    {
        proc.process(0xE0, 127, 127);
        const float expected = std::exp(-0.000014102f * float((127 << 7 | 127) - 8192));
        REQUIRE(proc.state().pitchBend == Catch::Approx(expected).epsilon(1e-5f));
        REQUIRE(proc.state().pitchBend < 1.0f);
    }

    SECTION("full bend down (data1=0, data2=0) raises pitchBend above 1.0")
    {
        proc.process(0xE0, 0, 0);
        const float expected = std::exp(-0.000014102f * float(0 - 8192));
        REQUIRE(proc.state().pitchBend == Catch::Approx(expected).epsilon(1e-5f));
        REQUIRE(proc.state().pitchBend > 1.0f);
    }

    SECTION("MSB-only bend (data2=65, data1=0) differs from center")
    {
        // Old buggy formula gave 1.0 here; correct formula does not.
        proc.process(0xE0, 0, 65);
        REQUIRE(proc.state().pitchBend != Catch::Approx(1.0f).epsilon(1e-4f));
    }

    SECTION("pitch bend is symmetric: up * down = 1.0")
    {
        // +512 ticks from center (data2=68, data1=0 → raw14 = 8704)
        proc.process(0xE0, 0, 68);
        const float up = proc.state().pitchBend;

        // -512 ticks from center (data2=60, data1=0 → raw14 = 7680)
        proc.process(0xE0, 0, 60);
        const float down = proc.state().pitchBend;

        REQUIRE(up * down == Catch::Approx(1.0f).epsilon(1e-4f));
    }

    SECTION("LSB contributes to the 14-bit value")
    {
        // Same data2, different data1 should give different pitchBend values
        proc.process(0xE0, 0, 64);
        float a = proc.state().pitchBend;

        proc.process(0xE0, 64, 64);
        float b = proc.state().pitchBend;

        REQUIRE(a != Catch::Approx(b).epsilon(1e-5f));
    }
}
