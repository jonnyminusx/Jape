#pragma once

#include <random>

namespace dsp
{

class NoiseGenerator
{
  public:
    void reset();
    float nextValue();

  private:
    std::mt19937 rng_;
    std::uniform_real_distribution<float> dist_{-1.0f, 1.0f};
};

} // namespace dsp
