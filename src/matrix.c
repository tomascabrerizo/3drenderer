#include "matrix.h"

mat4_t mat4_add(mat4_t a, mat4_t b)
{
    mat4_t result = {0};
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            result.m[i][j] = a.m[i][j] + b.m[i][j]; 
        }
    }
    return result;
}

mat4_t mat4_sub(mat4_t a, mat4_t b)
{
    mat4_t result = {0};
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            result.m[i][j] = a.m[i][j] - b.m[i][j]; 
        }
    }
    return result;
}

mat4_t mat4_mul(mat4_t a, mat4_t b)
{
    mat4_t result = {0};
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            result.m[i][j] = a.m[i][0] * b.m[0][j] +
                             a.m[i][1] * b.m[1][j] +
                             a.m[i][2] * b.m[2][j] +
                             a.m[i][3] * b.m[3][j]; 
        }
    }
    return result;
}

mat4_t mat4_identity(void)
{
    mat4_t result = {0};
    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;
    result.m[3][3] = 1.0f;
    return result;
}

mat4_t mat4_make_scale(float sx, float sy, float sz)
{
    mat4_t result = mat4_identity();
    result.m[0][0] = sx;
    result.m[1][1] = sy;
    result.m[2][2] = sz;
    return result;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
    vec4_t result = {0};
    for(int i = 0; i < 4; ++i)
    {
        result.v[i] = m.m[i][0] * v.x +
                      m.m[i][1] * v.y +
                      m.m[i][2] * v.z +
                      m.m[i][3] * v.w;
    }
    return result;
}

