#ifndef LIGHT_H
#define LIGHT_H


#include "common.h"


struct Light {
    Vec3 position;
    Colour intensity;  // RGB intensity of the light
    Light(const Vec3& pos, const Colour& intensity) : position(pos), intensity(intensity) {}
};

#endif