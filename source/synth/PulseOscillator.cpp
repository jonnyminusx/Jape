#include "PulseOscillator.h"

#include <algorithm>

namespace synth
{

void PulseOscillator::reset()
{
    primary_.reset();
    secondary_.reset();
    saw_ = 0.0f;
}

void PulseOscillator::setAmplitude(const float amplitude)
{
    primary_.setAmplitude(amplitude);
    secondary_.setAmplitude(amplitude);
}

void PulseOscillator::setPeriod(const float period)
{
    period_ = period;
    primary_.setPeriod(period);
    secondary_.setPeriod(period);
}

void PulseOscillator::setModulation(const float pwmMod)
{
    // pwmMod = 2 * pulseWidth + sin * pwmDepth, centred at 1.0 for 50% duty.
    // dutyCycle = pwmMod / 2, clamped to keep the pulse wave well-formed.
    const float dutyCycle{std::clamp(pwmMod * 0.5f, 0.05f, 0.95f)};
    secondary_.squareWave(primary_, period_, dutyCycle);
}

void PulseOscillator::noteOn(const float period)
{
    period_ = period;
    primary_.setPeriod(period);
    secondary_.squareWave(primary_, period, 0.5f);
}

float PulseOscillator::nextSample()
{
    saw_ = saw_ * 0.997f + primary_.nextSample() - secondary_.nextSample();
    return saw_;
}

} // namespace synth
