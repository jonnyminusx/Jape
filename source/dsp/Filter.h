#pragma once

namespace dsp
{

class Filter
{
  public:
    void updateCoefficients(float cutoff, float Q);
    void reset();
    [[nodiscard]] float render(float x);
    void setSampleRate(float sampleRate);

  private:
    float sampleRate_{0.0f};

    // Bilinear-transform SVF coefficients — recomputed each updateCoefficients call.
    float g_{0.0f};       // tan(π·fc/fs): normalized angular frequency
    float damping_{0.0f}; // 1/Q: resonance damping factor
    float a1_{0.0f};
    float a2_{0.0f};
    float a3_{0.0f};

    float s1_{0.0f}; // first integrator state
    float s2_{0.0f}; // second integrator state
};

} // namespace dsp
