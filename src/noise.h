#ifndef NOISE_H
#define NOISE_H

#define _USE_MATH_DEFINES
#include <glm/glm.hpp>
#include <math.h>

namespace noise {

    static glm::vec2 randomGradient(int ix, int iy) {
        // No precomputed gradients mean this works for any number of grid coordinates
        const unsigned w = 8 * sizeof(unsigned);
        const unsigned s = w / 2;
        unsigned a = ix, b = iy;
        a *= 3284157443;

        b ^= a << s | a >> w - s;
        b *= 1911520717;

        a ^= b << s | b >> w - s;
        a *= 2048419325;
        float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]

        // Create the vector from the angle
        glm::vec2 v;
        v.x = sin(random);
        v.y = cos(random);

        return v;
    }


    static float cubicInterpolation(float v1, float v2, float w) {
        return (v2 - v1) * (3.0 - w * 2.0) * w * w + v1;
    }

    static float dotGradient(int ix, int iy, float x, float y) {
        glm::vec2 gradient = randomGradient(ix, iy);
        glm::vec2 distanceVector = glm::vec2(x - ix, y - iy);
        return glm::dot(distanceVector, gradient);
    }


    static float perlin(float x, float y) {
        int x0 = (int)std::floor(x);
        int y0 = (int)std::floor(y);
        int x1 = x0 + 1;
        int y1 = y0 + 1;

        float wx = x - x0;
        float wy = y - y0;

        float upLeftCorner = dotGradient(x0, y0, x, y);
        float upRightCorner = dotGradient(x1, y0, x, y);

        float downLeftCorner = dotGradient(x0, y1, x, y);
        float downRightCorner = dotGradient(x1, y1, x, y);

        float topInterpolation = cubicInterpolation(upLeftCorner, upRightCorner, wx);
        float bottomInterpolation = cubicInterpolation(downLeftCorner, downRightCorner, wx);

        float finalInterpolation = cubicInterpolation(topInterpolation, bottomInterpolation, wy);
        return finalInterpolation;
    }

    static float getHeight(int x, int y) {
        float gridSize = 400;
        int octaves = 12;
        float freq = 1;
        float amp = 1;
        float value = 0;
        for (int i = 0; i < octaves; i++) {
            value += perlin(x * (freq / gridSize), y * (freq / gridSize)) * amp;
            amp /= 2;
            freq *= 2;
        }
        value *= 1.2f;
        if (value > 1.0f) value = 1.f;
        if (value < -1.0f) value = -1.f;

        value = (value + 1.f) * 0.5f;
        return value;
    }

    static float cossineInterpolation(float y1, float y2, float m) {
        float mu2;
        mu2 = (1 - std::cos(m * M_PI)) / 2;
        return y1 * (1 - mu2) + y2 * mu2;
    }

    static float smoothstep(float x) {
        return x * x * (3 - 2 * x);
    }

    static glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t) {
        return a * (1 - t) + b * t;
    }

};

#endif

