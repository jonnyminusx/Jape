#pragma once

namespace synth
{

class Oscillator
{
  public:
    virtual ~Oscillator() = default;
    virtual void reset() = 0;
    virtual void setAmplitude(float amplitude) = 0;
    virtual void setPeriod(float period) = 0;
    virtual void setModulation(float modulation) = 0;
    virtual void noteOn(float period) = 0;
    virtual float nextSample() = 0;
};

} // namespace synth
