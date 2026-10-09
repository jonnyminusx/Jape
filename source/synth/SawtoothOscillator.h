#pragma once

#include "Oscillator.h"
#include "PolyBlepOscillator.h"

namespace synth
{

class SawtoothOscillator : public Oscillator
{
  public:
    void reset() override;
    void setAmplitude(float amplitude) override;
    void setPeriod(float period) override;
    void setModulation([[maybe_unused]] float modulation) override {}
    void noteOn(float period) override;
    float nextSample() override;

  private:
    PolyBlepOscillator primary_;
    float saw_{0.0f};
};

} // namespace synth
