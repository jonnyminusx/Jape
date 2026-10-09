#pragma once

#include "Oscillator.h"

namespace synth
{

class SineOscillator : public Oscillator
{
  public:
    void reset() override;
    void setAmplitude(float amplitude) override;
    void setPeriod(float period) override;
    void setModulation([[maybe_unused]] float modulation) override {}
    void noteOn(float period) override;

    float nextSample() override;

  private:
    float amplitude_{0.0f};
    float normalizedFreq_{0.0f};
    float phase_{0.0f};

    float sinCurrent_{0.0f};
    float sinPrevious_{0.0f};
    float sinRecurrenceCoeff_{0.0f};
};

} // namespace synth
