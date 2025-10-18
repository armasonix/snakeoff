#include "CameraShake.h"
#include <cmath>

float CameraShake::offsetX() const { return active() ? amp_ * std::sin(t_ * 70.f) : 0.f; }
float CameraShake::offsetY() const { return active() ? amp_ * std::cos(t_ * 65.f) : 0.f; }