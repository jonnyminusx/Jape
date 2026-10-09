#include "AudioBuffer.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <string_view>
#include <utility>
#ifndef NDEBUG
#include <iostream>
#endif

namespace dsp
{

namespace
{

constexpr float kClampThreshold{1.0f};
constexpr float kSilenceThreshold{2.0f};

#ifndef NDEBUG
void printWarning(std::string_view problem, std::string_view solution, size_t channel, size_t sample)
{
    std::cerr << "Warning: Sample value is " << problem << " at channel: " << channel << ", sample: " << sample << " - "
              << solution << "\n";
}
#else
void printWarning(std::string_view, std::string_view, size_t, size_t) {}
#endif

} // namespace

AudioBuffer::AudioBuffer(std::vector<std::span<float>> channels) : channels_(std::move(channels))
{
    if (!channels_.empty())
    {
        [[maybe_unused]] const auto expectedSize = channels_[0].size();
        assert(std::all_of(channels_.begin(), channels_.end(), [expectedSize](const auto& span) {
            return span.size() == expectedSize;
        }));
    }
}

std::span<float> AudioBuffer::channelBuffer(const size_t channel)
{
    assert(channel < channelCount());
    return channels_[channel];
}

std::span<const float> AudioBuffer::channelBuffer(const size_t channel) const
{
    assert(channel < channelCount());
    return channels_[channel];
}

size_t AudioBuffer::sampleCount() const
{
    return channels_.empty() ? 0 : channels_[0].size();
}

size_t AudioBuffer::channelCount() const
{
    return channels_.size();
}

void AudioBuffer::clear(const size_t channel)
{
    assert(channel < channelCount());

    std::fill(channels_[channel].begin(), channels_[channel].end(), 0.0f);
}

void AudioBuffer::sanitise()
{
    for (size_t channel = 0; channel < channelCount(); ++channel)
    {
        bool shouldSilence{false};

        size_t sampleIndex{0};
        for (float& sampleValue : channelBuffer(channel))
        {
            if (!std::isfinite(sampleValue))
            {
                printWarning("non-finite", "silencing channel", channel, sampleIndex);
                shouldSilence = true;
                break;
            }
            else if (sampleValue < -kSilenceThreshold || sampleValue > kSilenceThreshold)
            {
                printWarning("out of range", "silencing channel", channel, sampleIndex);
                shouldSilence = true;
                break;
            }
            else if (sampleValue < -kClampThreshold)
            {
                printWarning("out of range", "clamping to -1.0f", channel, sampleIndex);
                sampleValue = -kClampThreshold;
            }
            else if (sampleValue > kClampThreshold)
            {
                printWarning("out of range", "clamping to 1.0f", channel, sampleIndex);
                sampleValue = kClampThreshold;
            }
            ++sampleIndex;
        }

        if (shouldSilence)
        {
            clear(channel);
        }
    }
}

} // namespace dsp
