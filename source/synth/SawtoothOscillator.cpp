#include "SawtoothOscillator.h"

namespace synth
{

void SawtoothOscillator::reset()
{
    primary_.reset();
    saw_ = 0.0f;
}

void SawtoothOscillator::setAmplitude(const float amplitude)
{
    primary_.setAmplitude(amplitude);
}

void SawtoothOscillator::setPeriod(const float period)
{
    primary_.setPeriod(period);
}

void SawtoothOscillator::noteOn(const float period)
{
    primary_.setPeriod(period);
}

float SawtoothOscillator::nextSample()
{
    saw_ = saw_ * 0.997f + primary_.nextSample();
    return saw_;
}

} // namespace synth
