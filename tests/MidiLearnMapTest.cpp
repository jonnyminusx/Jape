#include <catch2/catch_test_macros.hpp>

#include "midi/MidiLearnMap.h"

#include <cstring>

// ─── Construction ─────────────────────────────────────────────────────────────

TEST_CASE("MidiLearnMap initial state", "[midi][learn]")
{
    midi::MidiLearnMap map;

    SECTION("all CC slots are unassigned")
    {
        for (size_t i = 0; i < midi::MidiLearnMap::kNumParams; ++i)
            REQUIRE(map.ccAtIndex(i) == midi::MidiLearnMap::kCCUnassigned);
    }

    SECTION("learningParamId returns nullptr")
    {
        REQUIRE(map.learningParamId() == nullptr);
    }

    SECTION("tryCaptureLearning returns -1 when not learning")
    {
        REQUIRE(map.tryCaptureLearning(0x07) == -1);
    }
}

TEST_CASE("kNumParams equals 34", "[midi][learn]")
{
    REQUIRE(midi::MidiLearnMap::kNumParams == 34);
}

TEST_CASE("all kParamIds entries are non-null and non-empty", "[midi][learn]")
{
    for (size_t i = 0; i < midi::MidiLearnMap::kNumParams; ++i)
    {
        REQUIRE(midi::MidiLearnMap::kParamIds[i] != nullptr);
        REQUIRE(std::strlen(midi::MidiLearnMap::kParamIds[i]) > 0);
    }
}

// ─── beginLearn / cancelLearn ─────────────────────────────────────────────────

TEST_CASE("beginLearn sets the learning param", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.beginLearn("filterFreq");
    REQUIRE(std::string_view{map.learningParamId()} == "filterFreq");
}

TEST_CASE("cancelLearn clears learning state", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.beginLearn("filterFreq");
    map.cancelLearn();
    REQUIRE(map.learningParamId() == nullptr);
}

TEST_CASE("beginLearn with unknown ID is a no-op", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.beginLearn("doesNotExist");
    REQUIRE(map.learningParamId() == nullptr);
}

// ─── tryCaptureLearning ───────────────────────────────────────────────────────

TEST_CASE("tryCaptureLearning captures CC and returns param index", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.beginLearn("envAttack");

    // find expected index
    int expectedIdx{-1};
    for (size_t i = 0; i < midi::MidiLearnMap::kNumParams; ++i)
    {
        if (std::string_view{midi::MidiLearnMap::kParamIds[i]} == "envAttack")
        {
            expectedIdx = static_cast<int>(i);
            break;
        }
    }
    REQUIRE(expectedIdx >= 0);

    const int captured{map.tryCaptureLearning(0x12)};
    REQUIRE(captured == expectedIdx);
    REQUIRE(map.ccAtIndex(static_cast<size_t>(captured)) == 0x12);
}

TEST_CASE("tryCaptureLearning clears learning state after capture", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.beginLearn("envAttack");
    map.tryCaptureLearning(0x12);
    REQUIRE(map.learningParamId() == nullptr);
}

TEST_CASE("second tryCaptureLearning without new beginLearn returns -1", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.beginLearn("envAttack");
    map.tryCaptureLearning(0x12);
    REQUIRE(map.tryCaptureLearning(0x13) == -1);
}

TEST_CASE("tryCaptureLearning returns index within bounds", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.beginLearn("osc2PulseWidth"); // last entry
    const int idx{map.tryCaptureLearning(0x7F)};
    REQUIRE(idx >= 0);
    REQUIRE(static_cast<size_t>(idx) < midi::MidiLearnMap::kNumParams);
}

// ─── clearLearn ───────────────────────────────────────────────────────────────

TEST_CASE("clearLearn removes a CC assignment", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.setCC(0, 0x07);
    map.clearLearn(midi::MidiLearnMap::kParamIds[0]);
    REQUIRE(map.ccAtIndex(0) == midi::MidiLearnMap::kCCUnassigned);
}

TEST_CASE("clearLearn with unknown ID is a no-op", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.setCC(0, 0x07);
    map.clearLearn("doesNotExist");
    REQUIRE(map.ccAtIndex(0) == 0x07); // unchanged
}

// ─── ccForParam ───────────────────────────────────────────────────────────────

TEST_CASE("ccForParam returns kCCUnassigned for unassigned param", "[midi][learn]")
{
    midi::MidiLearnMap map;
    REQUIRE(map.ccForParam("filterFreq") == midi::MidiLearnMap::kCCUnassigned);
}

TEST_CASE("ccForParam returns the assigned CC after setCC", "[midi][learn]")
{
    midi::MidiLearnMap map;
    // find filterFreq index
    for (size_t i = 0; i < midi::MidiLearnMap::kNumParams; ++i)
    {
        if (std::string_view{midi::MidiLearnMap::kParamIds[i]} == "filterFreq")
        {
            map.setCC(i, 0x4A);
            break;
        }
    }
    REQUIRE(map.ccForParam("filterFreq") == 0x4A);
}

TEST_CASE("ccForParam returns kCCUnassigned for unknown ID", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.setCC(0, 0x07);
    REQUIRE(map.ccForParam("doesNotExist") == midi::MidiLearnMap::kCCUnassigned);
}

// ─── setCC / forEachAssignment ────────────────────────────────────────────────

TEST_CASE("forEachAssignment visits zero entries when nothing assigned", "[midi][learn]")
{
    midi::MidiLearnMap map;
    int count{0};
    map.forEachAssignment([&](const char*, uint8_t) { ++count; });
    REQUIRE(count == 0);
}

TEST_CASE("forEachAssignment visits exactly the assigned entries", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.setCC(0, 0x01);
    map.setCC(5, 0x40);

    int count{0};
    map.forEachAssignment([&](const char* id, uint8_t cc) {
        ++count;
        if (std::string_view{id} == midi::MidiLearnMap::kParamIds[0])
            REQUIRE(cc == 0x01);
        else if (std::string_view{id} == midi::MidiLearnMap::kParamIds[5])
            REQUIRE(cc == 0x40);
        else
            FAIL("unexpected param ID in forEachAssignment");
    });
    REQUIRE(count == 2);
}

// ─── Boundary ─────────────────────────────────────────────────────────────────

TEST_CASE("last slot (index kNumParams-1) can be set and read", "[midi][learn]")
{
    midi::MidiLearnMap map;
    map.setCC(midi::MidiLearnMap::kNumParams - 1, 0x7F);
    REQUIRE(map.ccAtIndex(midi::MidiLearnMap::kNumParams - 1) == 0x7F);
}
