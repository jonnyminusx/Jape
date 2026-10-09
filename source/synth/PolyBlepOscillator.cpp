#include "PolyBlepOscillator.h"

#include "math/constants.h"
#include <algorithm>
#include <cmath>

namespace synth
{

void PolyBlepOscillator::reset()
{
    increment_ = 0.0f;
    phase_ = 0.0f;
    dcOffset_ = 0.0f;
}

void PolyBlepOscillator::setAmplitude(const float amplitude)
{
    amplitude_ = amplitude;
}

void PolyBlepOscillator::setPeriod(const float period)
{
    period_ = period;
}

void PolyBlepOscillator::setModulation(const float modulation)
{
    // Clamp away from zero: halfPeriod = (period/2) * modulation_ feeds a
    // divisor below; zero or negative values would produce ±inf increment_.
    modulation_ = std::max(modulation, 0.01f);
}

void PolyBlepOscillator::squareWave(const PolyBlepOscillator& other, const float newPeriod, const float dutyCycle)
{
    reset();
    period_ = newPeriod;

    const float halfPeriod{(newPeriod / 2.0f) * modulation_};
    const float halfPhaseUnscaled{std::floor(0.5f + halfPeriod) - 0.5f};
    dcOffset_ = 0.5f * amplitude_ / halfPhaseUnscaled;
    halfPhase_ = halfPhaseUnscaled * math::pi;
    const float incMag{halfPhase_ / halfPeriod};

    // Compute the primary's normalised cycle angle θ ∈ [0, τ).
    // Upswing (increment > 0): θ ∈ [0, π). Downswing (increment < 0): θ ∈ [π, τ).
    float theta{0.0f};
    if (other.halfPhase_ > 0.0f)
    {
        if (other.increment_ > 0.0f)
            theta = (other.phase_ / other.halfPhase_) * math::pi;
        else if (other.increment_ < 0.0f)
            theta = (2.0f - other.phase_ / other.halfPhase_) * math::pi;
    }

    float deltaTheta{math::tau * dutyCycle - theta};
    if (deltaTheta < 0.0f)
        deltaTheta += math::tau;

    // ratio ∈ [0, 2): ≤ 1 → secondary is descending toward fire point,
    // > 1 → secondary already fired and is ascending.
    const float ratio{deltaTheta / math::pi};
    if (ratio <= 1.0f)
    {
        phase_ = ratio * halfPhase_;
        increment_ = -incMag;
    }
    else
    {
        phase_ = (2.0f - ratio) * halfPhase_;
        increment_ = incMag;
    }

    sinCurrent_ = amplitude_ * std::sin(phase_);
    sinPrevious_ = amplitude_ * std::sin(phase_ - increment_);
    sinRecurrenceCoeff_ = 2.0f * std::cos(increment_);
}

float PolyBlepOscillator::nextSample()
{
    float output{0.0f};

    phase_ += increment_;

    if (phase_ <= math::quarterPi)
    {
        const float halfPeriod{(period_ / 2.0f) * modulation_};
        halfPhase_ = std::floor(0.5f + halfPeriod) - 0.5f;
        dcOffset_ = 0.5f * amplitude_ / halfPhase_;
        halfPhase_ *= math::pi;

        increment_ = halfPhase_ / halfPeriod;
        phase_ = -phase_;

        sinCurrent_ = amplitude_ * std::sin(phase_);
        sinPrevious_ = amplitude_ * std::sin(phase_ - increment_);
        sinRecurrenceCoeff_ = 2.0f * std::cos(increment_);

        if (phase_ * phase_ > 1e-9)
        {
            output = sinCurrent_ / phase_;
        }
        else
        {
            output = amplitude_;
        }
    }
    else
    {
        if (phase_ > halfPhase_)
        {
            phase_ = (2 * halfPhase_) - phase_;
            increment_ = -increment_;
        }

        const float sinp{(sinRecurrenceCoeff_ * sinCurrent_) - sinPrevious_};
        sinPrevious_ = sinCurrent_;
        sinCurrent_ = sinp;

        output = sinCurrent_ / phase_;
    }

    return output - dcOffset_;
}

} // namespace synth
