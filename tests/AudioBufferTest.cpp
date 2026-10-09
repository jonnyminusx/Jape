#include <catch2/catch_test_macros.hpp>

#include "dsp/AudioBuffer.h"

#include <limits>
#include <span>
#include <vector>

namespace
{

// Builds an AudioBuffer whose channel spans point into `channels`.
// `channels` must outlive the returned buffer.
dsp::AudioBuffer makeBuffer(std::vector<std::vector<float>>& channels)
{
    std::vector<std::span<float>> spans;
    for (auto& ch : channels)
        spans.push_back(std::span<float>(ch));
    return dsp::AudioBuffer(std::move(spans));
}

} // namespace

// ─── Clean audio ─────────────────────────────────────────────────────────────

TEST_CASE("sanitise leaves clean audio unchanged", "[sanitise]")
{
    SECTION("zero buffer is unmodified")
    {
        std::vector<std::vector<float>> data = {{0.0f, 0.0f, 0.0f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }

    SECTION("values within (-1, 1) are unmodified")
    {
        std::vector<std::vector<float>> data = {{-0.9f, 0.0f, 0.5f, 0.999f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        auto ch = buffer.channelBuffer(0);
        REQUIRE(ch[0] == -0.9f);
        REQUIRE(ch[1] == 0.0f);
        REQUIRE(ch[2] == 0.5f);
        REQUIRE(ch[3] == 0.999f);
    }

    SECTION("boundary values -1.0 and 1.0 are not modified")
    {
        std::vector<std::vector<float>> data = {{-1.0f, 1.0f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        REQUIRE(buffer.channelBuffer(0)[0] == -1.0f);
        REQUIRE(buffer.channelBuffer(0)[1] == 1.0f);
    }
}

// ─── Soft clipping ───────────────────────────────────────────────────────────

TEST_CASE("sanitise clamps samples in the range (1, 2] and [-2, -1)", "[sanitise][clamp]")
{
    SECTION("positive sample above 1.0 is clamped to 1.0")
    {
        std::vector<std::vector<float>> data = {{0.0f, 1.5f, 0.0f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        auto ch = buffer.channelBuffer(0);
        REQUIRE(ch[0] == 0.0f);
        REQUIRE(ch[1] == 1.0f);
        REQUIRE(ch[2] == 0.0f);
    }

    SECTION("negative sample below -1.0 is clamped to -1.0")
    {
        std::vector<std::vector<float>> data = {{0.0f, -1.5f, 0.0f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        auto ch = buffer.channelBuffer(0);
        REQUIRE(ch[0] == 0.0f);
        REQUIRE(ch[1] == -1.0f);
        REQUIRE(ch[2] == 0.0f);
    }

    SECTION("exactly 2.0 is clamped, not silenced")
    {
        std::vector<std::vector<float>> data = {{0.5f, 2.0f, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        auto ch = buffer.channelBuffer(0);
        REQUIRE(ch[0] == 0.5f);
        REQUIRE(ch[1] == 1.0f);
        REQUIRE(ch[2] == 0.5f);
    }

    SECTION("exactly -2.0 is clamped, not silenced")
    {
        std::vector<std::vector<float>> data = {{0.5f, -2.0f, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        auto ch = buffer.channelBuffer(0);
        REQUIRE(ch[0] == 0.5f);
        REQUIRE(ch[1] == -1.0f);
        REQUIRE(ch[2] == 0.5f);
    }

    SECTION("multiple samples in the clamping range are each clamped independently")
    {
        std::vector<std::vector<float>> data = {{1.2f, -1.8f, 1.9f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        auto ch = buffer.channelBuffer(0);
        REQUIRE(ch[0] == 1.0f);
        REQUIRE(ch[1] == -1.0f);
        REQUIRE(ch[2] == 1.0f);
    }
}

// ─── Gross out-of-range silencing ────────────────────────────────────────────

TEST_CASE("sanitise silences the channel when a sample exceeds ±2", "[sanitise][silence]")
{
    SECTION("sample just above 2.0 silences the channel")
    {
        std::vector<std::vector<float>> data = {{0.5f, 2.1f, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }

    SECTION("sample just below -2.0 silences the channel")
    {
        std::vector<std::vector<float>> data = {{0.5f, -2.1f, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }

    SECTION("bad sample at the first position silences the whole channel")
    {
        std::vector<std::vector<float>> data = {{3.0f, 0.5f, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }

    SECTION("bad sample at the last position silences the whole channel")
    {
        std::vector<std::vector<float>> data = {{0.5f, 0.5f, 3.0f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }

    SECTION("a previously clamped sample is zeroed when a later bad sample triggers silence")
    {
        // 1.5f is clamped in-place to 1.0f, then 3.0f triggers channel silence,
        // so clear() wipes the already-clamped sample too.
        std::vector<std::vector<float>> data = {{1.5f, 3.0f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }
}

// ─── Non-finite values ───────────────────────────────────────────────────────

TEST_CASE("sanitise silences the channel on non-finite samples", "[sanitise][silence]")
{
    const float inf = std::numeric_limits<float>::infinity();
    const float nan = std::numeric_limits<float>::quiet_NaN();

    SECTION("NaN silences the channel")
    {
        std::vector<std::vector<float>> data = {{0.5f, nan, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }

    SECTION("positive infinity silences the channel")
    {
        std::vector<std::vector<float>> data = {{0.5f, inf, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }

    SECTION("negative infinity silences the channel")
    {
        std::vector<std::vector<float>> data = {{0.5f, -inf, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
    }
}

// ─── Multi-channel independence ──────────────────────────────────────────────

TEST_CASE("sanitise processes each channel independently", "[sanitise][multichannel]")
{
    SECTION("a corrupted channel is silenced without disturbing a clean channel")
    {
        std::vector<std::vector<float>> data = {
            {0.5f, 3.0f, 0.5f}, // channel 0: out-of-range → silenced
            {0.3f, 0.4f, 0.5f}  // channel 1: clean → unchanged
        };
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);

        auto ch1 = buffer.channelBuffer(1);
        REQUIRE(ch1[0] == 0.3f);
        REQUIRE(ch1[1] == 0.4f);
        REQUIRE(ch1[2] == 0.5f);
    }

    SECTION("a clean channel is unaffected when the other channel is corrupted")
    {
        std::vector<std::vector<float>> data = {
            {0.3f, 0.4f, 0.5f}, // channel 0: clean → unchanged
            {0.5f, 3.0f, 0.5f}  // channel 1: out-of-range → silenced
        };
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        auto ch0 = buffer.channelBuffer(0);
        REQUIRE(ch0[0] == 0.3f);
        REQUIRE(ch0[1] == 0.4f);
        REQUIRE(ch0[2] == 0.5f);

        for (float s : buffer.channelBuffer(1))
            REQUIRE(s == 0.0f);
    }

    SECTION("both channels are silenced independently when both are corrupted")
    {
        const float nan = std::numeric_limits<float>::quiet_NaN();
        std::vector<std::vector<float>> data = {{0.5f, 3.0f, 0.5f}, {0.5f, nan, 0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        for (float s : buffer.channelBuffer(0))
            REQUIRE(s == 0.0f);
        for (float s : buffer.channelBuffer(1))
            REQUIRE(s == 0.0f);
    }
}

// ─── Edge cases ──────────────────────────────────────────────────────────────

TEST_CASE("sanitise handles edge cases safely", "[sanitise][edge]")
{
    SECTION("empty buffer with no channels does not throw")
    {
        std::vector<std::vector<float>> data;
        auto buffer = makeBuffer(data);

        REQUIRE_NOTHROW(buffer.sanitise());
    }

    SECTION("single-sample buffer with a clean value is unmodified")
    {
        std::vector<std::vector<float>> data = {{0.5f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        REQUIRE(buffer.channelBuffer(0)[0] == 0.5f);
    }

    SECTION("single-sample buffer with a bad value is silenced")
    {
        std::vector<std::vector<float>> data = {{3.0f}};
        auto buffer = makeBuffer(data);

        buffer.sanitise();

        REQUIRE(buffer.channelBuffer(0)[0] == 0.0f);
    }
}
