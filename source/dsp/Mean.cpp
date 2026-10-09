#include "Mean.h"

#include <numeric>

namespace dsp
{

float mean(std::span<const float> v)
{
    if (v.empty())
        return 0.0f;
    return std::accumulate(v.begin(), v.end(), 0.0f) / static_cast<float>(v.size());
}

} // namespace dsp
