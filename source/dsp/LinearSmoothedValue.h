#pragma once

namespace dsp
{

template <typename FloatType> class LinearSmoothedValue
{
  public:
    LinearSmoothedValue() = default;
    explicit LinearSmoothedValue(FloatType initialValue) : current_{initialValue}, target_{initialValue} {}

    void reset(FloatType sampleRate, double rampLengthInSeconds)
    {
        numSteps_ = static_cast<int>(sampleRate * static_cast<FloatType>(rampLengthInSeconds));
        setCurrentAndTargetValue(current_);
    }

    void setTargetValue(FloatType newTarget)
    {
        target_ = newTarget;
        if (numSteps_ <= 0)
        {
            current_ = target_;
            countdown_ = 0;
            step_ = FloatType{};
            return;
        }
        countdown_ = numSteps_;
        step_ = (target_ - current_) / static_cast<FloatType>(numSteps_);
    }

    void setCurrentAndTargetValue(FloatType newValue)
    {
        current_ = newValue;
        target_ = newValue;
        step_ = FloatType{};
        countdown_ = 0;
    }

    FloatType getNextValue()
    {
        if (countdown_ <= 0)
            return target_;
        --countdown_;
        current_ += step_;
        if (countdown_ == 0)
            current_ = target_;
        return current_;
    }

  private:
    FloatType current_{};
    FloatType target_{};
    FloatType step_{};
    int numSteps_{};
    int countdown_{};
};

} // namespace dsp
