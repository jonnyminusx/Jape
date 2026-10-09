#pragma once

#include "Oscillator.h"
#include "PolyBlepOscillator.h"

namespace synth
{

class PulseOscillator : public Oscillator
{
  public:
    void reset() override;
    void setAmplitude(float amplitude) override;
    void setPeriod(float period) override;
    void setModulation(float pwmMod) override;
    void noteOn(float period) override;
    float nextSample() override;

  private:
    PolyBlepOscillator primary_;
    PolyBlepOscillator secondary_;
    float saw_{0.0f};
    float period_{0.0f};
};

} // namespace synth
