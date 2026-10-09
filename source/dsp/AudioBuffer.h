#pragma once

#include <cstddef>
#include <span>
#include <vector>

namespace dsp
{

class AudioBuffer
{
  public:
    AudioBuffer(std::vector<std::span<float>> channels);

    std::span<float> channelBuffer(const size_t channel);
    std::span<const float> channelBuffer(const size_t channel) const;
    size_t sampleCount() const;
    size_t channelCount() const;
    void clear(const size_t channel);
    void sanitise();

  private:
    std::vector<std::span<float>> channels_;
};

} // namespace dsp
