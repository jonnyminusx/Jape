#pragma once

#include <numbers>

namespace math
{

constexpr float pi{std::numbers::pi_v<float>};
constexpr float quarterPi{pi / 4.0f};
constexpr float tau{2.0f * pi};

} // namespace math
