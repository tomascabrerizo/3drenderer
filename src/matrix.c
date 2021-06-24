#include <math.h>
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

mat4_t mat4_make_translation(float tx, float ty, float tz)
{
    mat4_t result = mat4_identity();
    result.m[0][3] = tx;
    result.m[1][3] = ty;
    result.m[2][3] = tz;
    return result;
}

mat4_t mat4_make_rotation_x(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    mat4_t result = mat4_identity();
    result.m[1][1] =  c; 
    result.m[1][2] = -s;
    result.m[2][1] =  s;
    result.m[2][2] =  c;
    return result;
}

mat4_t mat4_make_rotation_y(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    mat4_t result = mat4_identity();
    result.m[0][0] =  c; 
    result.m[0][2] =  s;
    result.m[2][0] = -s;
    result.m[2][2] =  c;
    return result;
}

mat4_t mat4_make_rotation_z(float angle)
{
    float c = cosf(angle);
    float s = sinf(angle);
    mat4_t result = mat4_identity();
    result.m[0][0] =  c; 
    result.m[0][1] = -s;
    result.m[1][0] =  s;
    result.m[1][1] =  c;
    return result;
}

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
    vec4_t result = {0};
    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
    return result;
}

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b)
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

mat4_t mat4_make_perspective(float fov, float aspect, float znear, float zfar)
{
    mat4_t result = {0};
    result.m[0][0] = aspect * (1.0f / (tanf(fov / 2.0f))); 
    result.m[1][1] = 1.0f / tanf(fov / 2.0f); 
    result.m[2][2] = zfar / (zfar - znear); 
    result.m[2][3] = (-zfar * znear) / (zfar - znear); 
    result.m[3][2] = 1.0f;
    return result;
}

vec4_t mat4_mul_vec4_project(mat4_t m, vec4_t v)
{
    vec4_t result = mat4_mul_vec4(m, v);
    if(result.w != 0.0f)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    return result;
}
