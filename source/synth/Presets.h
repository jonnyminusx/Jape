#pragma once

#include "Preset.h"

#include <cstddef>
#include <vector>

namespace synth
{

class Presets
{
  public:
    Presets();

    size_t size() const { return presets_.size(); }
    const Preset& operator[](size_t index) const { return presets_[index]; }

  private:
    void createPrograms();

    std::vector<Preset> presets_;
};

} // namespace synth