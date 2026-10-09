#pragma once

namespace synth
{

class PolyBlepOscillator
{
  public:
    void reset();
    void setAmplitude(float amplitude);
    void setPeriod(float period);
    void setModulation(float modulation);

    void squareWave(const PolyBlepOscillator& other, float newPeriod, float dutyCycle = 0.5f);
    float nextSample();

  private:
    float period_{0.0f};
    float modulation_{1.0f};
    float amplitude_{0.0f};

    float increment_{0.0f};
    float phase_{0.0f};
    float halfPhase_{0.0f};

    float sinCurrent_{0.0f};
    float sinPrevious_{0.0f};
    float sinRecurrenceCoeff_{0.0f};

    float dcOffset_{0.0f};
};

} // namespace synth
