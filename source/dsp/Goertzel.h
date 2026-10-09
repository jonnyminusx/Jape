#pragma once

#include <span>

namespace dsp
{

// Evaluates the energy at a specific frequency using the Goertzel algorithm.
// A Hann window is applied before evaluation to suppress sidelobes, which
// prevents energy from nearby harmonics leaking into alias frequency bins.
// Returns a raw magnitude comparable across frequencies on the same signal.
[[nodiscard]] float goertzel(std::span<const float> samples, float targetFreq, float sampleRate);

} // namespace dsp
