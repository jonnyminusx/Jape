#pragma once

#include <span>

namespace dsp
{

float rms(std::span<const float> v);

} // namespace dsp
