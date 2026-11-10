/**
 * @file math.h
 * @brief Nintendo CTR-SDK Mathematics Library (`nn::math`).
 */
#pragma once

#include "types.h"
#include "nn/Types.h"

#ifdef __cplusplus
#include "nn/math/math_VEC3.h"

namespace nn {
namespace math {

    struct VEC2 {
        float x;
        float y;

        VEC2() : x(0.0f), y(0.0f) {}
        VEC2(float _x, float _y) : x(_x), y(_y) {}
    };

    struct VEC4 {
        float x;
        float y;
        float z;
        float w;

        VEC4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        VEC4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    };

    struct MTX34 {
        float m[3][4];
    };

    struct MTX44 {
        float m[4][4];
    };

    struct QUAT {
        float x;
        float y;
        float z;
        float w;

        QUAT() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        QUAT(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
    };

    /* Common Math Utilities */
    float Sin(float rad);
    float Cos(float rad);
    float Atan2(float y, float x);
    float Sqrt(float val);

    float VEC3Dot(const VEC3* a, const VEC3* b);
    void  VEC3Cross(VEC3* out, const VEC3* a, const VEC3* b);
    void  VEC3Normalize(VEC3* out, const VEC3* in);

    void  MTX34Identity(MTX34* out);
    void  MTX34Mult(MTX34* out, const MTX34* a, const MTX34* b);
    void  MTX44Identity(MTX44* out);
    void  MTX44Mult(MTX44* out, const MTX44* a, const MTX44* b);

} // namespace math
} // namespace nn

#else /* !__cplusplus (Pure C) */

typedef struct nn_math_VEC2 {
    float x;
    float y;
} nn_math_VEC2;

typedef struct nn_math_VEC3 {
    float x;
    float y;
    float z;
} nn_math_VEC3;

typedef struct nn_math_VEC4 {
    float x;
    float y;
    float z;
    float w;
} nn_math_VEC4;

typedef struct nn_math_MTX34 {
    float m[3][4];
} nn_math_MTX34;

typedef struct nn_math_MTX44 {
    float m[4][4];
} nn_math_MTX44;

typedef struct nn_math_QUAT {
    float x;
    float y;
    float z;
    float w;
} nn_math_QUAT;

#endif /* __cplusplus */
