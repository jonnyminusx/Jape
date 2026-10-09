#include "Envelope.h"

namespace synth
{

namespace
{
constexpr float kSilence{0.0001f};
}

void Envelope::reset()
{
    level_ = 0.0f;
    target_ = 0.0f;
    multiplier_ = 0.0f;
}

void Envelope::attack()
{
    level_ += kSilence + kSilence;
    target_ = 2.0f;
    multiplier_ = adsr_.attack;
}

void Envelope::release()
{
    target_ = 0.0f;
    multiplier_ = adsr_.release;
}

bool Envelope::isActive() const
{
    return level_ > kSilence;
}

bool Envelope::isInAttack() const
{
    return target_ >= 2.0f;
}

void Envelope::nudgeLevelUp()
{
    level_ += kSilence + kSilence;
}

void Envelope::setLevel(const float level)
{
    level_ = level;
}

void Envelope::setTarget(const float target)
{
    target_ = target;
}

void Envelope::setMultiplier(const float multiplier)
{
    multiplier_ = multiplier;
}

void Envelope::setADSR(const Parameters::ADSR& adsr)
{
    adsr_ = adsr;
}

float Envelope::nextValue()
{
    level_ = multiplier_ * (level_ - target_) + target_;

    if (level_ + target_ > 3.0f)
    {
        multiplier_ = adsr_.decay;
        target_ = adsr_.sustain;
    }

    return level_;
}

float Envelope::currentValue() const
{
    return level_;
}

} // namespace synth
