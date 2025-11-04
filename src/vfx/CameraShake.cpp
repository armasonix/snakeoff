#include "vfx/CameraShake.h"
#include <cmath>

float CameraShake::offsetX() const { return active() ? amp_ * std::sin(phaseX_ + t_ * freqX_) : 0.f; }
float CameraShake::offsetY() const { return active() ? amp_ * std::cos(phaseY_ + t_ * freqY_) : 0.f; }