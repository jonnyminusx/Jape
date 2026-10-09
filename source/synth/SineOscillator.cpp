#include "SineOscillator.h"

#include "math/constants.h"
#include <cmath>

namespace synth
{

void SineOscillator::reset()
{
    phase_ = 0.0f;

    // Store unit-amplitude recurrence state; amplitude_ is applied in nextSample().
    // sinPrevious_ represents sin(phase - normalizedFreq_), i.e. one step back.
    // Using -normalizedFreq_ here so the first nextSample() returns sin(+freq),
    // giving a sine that rises from zero rather than one that dips negative.
    sinCurrent_ = std::sin(math::tau * phase_);
    sinPrevious_ = std::sin(-math::tau * normalizedFreq_);
    sinRecurrenceCoeff_ = 2.0f * std::cos(normalizedFreq_ * math::tau);
}

void SineOscillator::setAmplitude(const float amplitude)
{
    amplitude_ = amplitude;
}

void SineOscillator::setPeriod(const float period)
{
    normalizedFreq_ = 1.0f / period;
    sinRecurrenceCoeff_ = 2.0f * std::cos(normalizedFreq_ * math::tau);
}

void SineOscillator::noteOn(const float period)
{
    setPeriod(period);
    reset();
}

float SineOscillator::nextSample()
{
    const float sinx{(sinRecurrenceCoeff_ * sinCurrent_) - sinPrevious_};
    sinPrevious_ = sinCurrent_;
    sinCurrent_ = sinx;
    return sinx * amplitude_;
}

} // namespace synth
