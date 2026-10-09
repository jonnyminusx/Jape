#include "Goertzel.h"

#include "math/constants.h"

#include <cassert>
#include <cmath>

namespace dsp
{

float goertzel(std::span<const float> samples, float targetFreq, float sampleRate)
{
    const size_t N{samples.size()};
    assert(N >= 2);
    const float omega{math::tau * targetFreq / sampleRate};
    const float coeff{2.0f * std::cos(omega)};
    float s1{0.0f};
    float s2{0.0f};

    for (size_t i = 0; i < N; ++i)
    {
        const float w = 0.5f - 0.5f * std::cos(math::tau * static_cast<float>(i) / static_cast<float>(N - 1));
        const float s0{samples[i] * w + coeff * s1 - s2};
        s2 = s1;
        s1 = s0;
    }

    const float real{s1 - s2 * std::cos(omega)};
    const float imag{s2 * std::sin(omega)};
    return std::sqrt(real * real + imag * imag);
}

} // namespace dsp
