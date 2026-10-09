#pragma once

namespace dsp
{
class Filter;
}

namespace synth
{

struct Output
{
    void filter(dsp::Filter& filter);

    Output& operator+=(const Output& rhs);
    Output& operator*=(const float scalar);

    float left{0.0f};
    float right{0.0f};
};

} // namespace synth
