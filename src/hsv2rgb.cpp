#include "hsv2rgb.hpp"

// Source: https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include <glm/vec3.hpp>

glm::vec3 RGBtoHSV(glm::vec3 rgb) {
    glm::vec3 hsv(0.f);

    float fCMax = max(max(rgb.r, rgb.g), rgb.b);
    float fCMin = min(min(rgb.r, rgb.g), rgb.b);
    float fDelta = fCMax - fCMin;

    if (fDelta > 0) {
        if (fCMax == rgb.r) {
            hsv.x = 60 * (fmod(((rgb.g - rgb.b) / fDelta), 6));
        } else if (fCMax == rgb.g) {
            hsv.x = 60 * (((rgb.b - rgb.r) / fDelta) + 2);
        } else if (fCMax == rgb.b) {
            hsv.x = 60 * (((rgb.r - rgb.g) / fDelta) + 4);
        }

        if (fCMax > 0) {
            hsv.y = fDelta / fCMax;
        } else {
            hsv.y = 0;
        }

        hsv.z = fCMax;
    } else {
        hsv.x = 0;
        hsv.y = 0;
        hsv.z = fCMax;
    }

    if (hsv.x < 0) {
        hsv.x = 360 + hsv.x;
    }

    return hsv;
}

glm::vec3 HSVtoRGB(glm::vec3 hsv) {
    glm::vec3 rgb;

    float fC = hsv.z * hsv.y; // Chroma
    float fHPrime = fmod(hsv.x / 60.0, 6);
    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM = hsv.z - fC;

    if (0 <= fHPrime && fHPrime < 1) {
        rgb.r = fC;
        rgb.g = fX;
        rgb.b = 0;
    } else if (1 <= fHPrime && fHPrime < 2) {
        rgb.r = fX;
        rgb.g = fC;
        rgb.b = 0;
    } else if (2 <= fHPrime && fHPrime < 3) {
        rgb.r = 0;
        rgb.g = fC;
        rgb.b = fX;
    } else if (3 <= fHPrime && fHPrime < 4) {
        rgb.r = 0;
        rgb.g = fX;
        rgb.b = fC;
    } else if (4 <= fHPrime && fHPrime < 5) {
        rgb.r = fX;
        rgb.g = 0;
        rgb.b = fC;
    } else if (5 <= fHPrime && fHPrime < 6) {
        rgb.r = fC;
        rgb.g = 0;
        rgb.b = fX;
    } else {
        rgb.r = 0;
        rgb.g = 0;
        rgb.b = 0;
    }

    rgb.r += fM;
    rgb.g += fM;
    rgb.b += fM;

    return rgb;
}
