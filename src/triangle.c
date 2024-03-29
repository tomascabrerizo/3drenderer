#include <stdio.h>
#include <math.h>
#include "triangle.h"
#include "display.h"
#include "swap.h"

void fill_flat_bottom(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    float inv_slope_1 = (float)(x1 - x0) / (float)(y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (float)(y2 - y0);
    float x_start = x0;
    float x_end = x0;
    for(int y = y0; y <= y2; ++y)
    {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}

void fill_flat_top(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    float inv_slope_1 = (float)(x2 - x0) / (float)(y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (float)(y2 - y1);
    float x_start = x2;
    float x_end = x2;
    for(int y = y2; y >= y0; --y)
    {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    // Sort vertices by y
    if(y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    if(y1 > y2)
    {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }
    if(y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }
    
    if(y1 == y2)
    {
        fill_flat_bottom(x0, y0, x1, y1, x2, y2, color);
    }
    else if(y0 == y1)
    {
        fill_flat_top(x0, y0, x1, y1, x2, y2, color);
    }
    else
    {
        // Calculates the mid point mx my using triangle similarity
        int my = y1;
        int mx = ((float)((x2 - x0)*(y1 - y0)) / (float)(y2 - y0)) + x0;
        fill_flat_bottom(x0, y0, x1, y1, mx, my, color);
        fill_flat_top(x1, y1, mx, my, x2, y2, color);
    }
}

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    vec2_t ab = vec2_sub(b, a);
    vec2_t bc = vec2_sub(c, b);
    vec2_t ac = vec2_sub(c, a);
    vec2_t ap = vec2_sub(p, a);
    vec2_t bp = vec2_sub(p, b);

    float area_triangle_abc = (ab.x * ac.y - ac.x * ab.y);
    float alpha = (bc.x * bp.y - bp.x * bc.y) / area_triangle_abc;
    float beta = (ap.x * ac.y - ac.x * ap.y) / area_triangle_abc;
    float gamma = 1 - alpha - beta;

    vec3_t result = {alpha, beta, gamma};
    return result;

}

void draw_texel(int x, int y, uint32_t *texture,
                vec4_t point_a, vec4_t point_b, vec4_t point_c,
                tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{
    vec2_t point_p = {x, y};
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t weights = barycentric_weights(a, b, c, point_p);
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;
    
    float interpolated_u; 
    float interpolated_v; 
    float interpolated_reciprocal_w; 
    
    float rec_aw = (1.0f/point_a.w);
    float rec_bw = (1.0f/point_b.w);
    float rec_cw = (1.0f/point_c.w);

    // Performe interpolation of u/w and v/w using barycentric_weights and 1/w
    interpolated_u = (a_uv.u*rec_aw)*alpha + (b_uv.u*rec_bw)*beta + (c_uv.u*rec_cw)*gamma;
    interpolated_v = (a_uv.v*rec_aw)*alpha + (b_uv.v*rec_bw)*beta + (c_uv.v*rec_cw)*gamma;
    interpolated_reciprocal_w = (rec_aw)*alpha + (rec_bw)*beta + (rec_cw)*gamma;
    
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;
    
    // Map uvs to the full texture width and height
    int tex_x = fabs((int)(interpolated_u * (texture_width)));
    int tex_y = fabs((int)(interpolated_v * (texture_height)));

    uint32_t texture_index = ((tex_y*texture_width)+tex_x);
    // TODO: Fix the texture index
    if(texture_index > (texture_width*texture_height)) texture_index = texture_width*texture_height;
    uint32_t color = *(texture + texture_index);
    draw_pixel(x, y, color);
}

void draw_texture_triangle(int x0, int y0, float z0, float w0, float u0, float v0,
                           int x1, int y1, float z1, float w1,float u1, float v1, 
                           int x2, int y2, float z2, float w2,float u2, float v2,
                           uint32_t *texture)
{
    // Sort vertices by y
    if(y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if(y1 > y2)
    {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if(y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // Create vector points after sort the vertex
    vec4_t point_a = { x0, y0, z0, w0 };
    vec4_t point_b = { x1, y1, z1, w1 };
    vec4_t point_c = { x2, y2, z2, w2 };

    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

    // Render the upper part of the triangle (flat bottom)
    float inv_slope_1 = 0.0f;
    float inv_slope_2 = 0.0f;
    if((y1 - y0) != 0) inv_slope_1 = (float)(x1 - x0) / fabs(y1 - y0);
    if((y2 - y0) != 0) inv_slope_2 = (float)(x2 - x0) / fabs(y2 - y0);
    
    if(y1 - y0 != 0)
    {
        for(int y = y0; y <= y1; ++y)
        {
            int x_start = x1 + ((y - y1) * inv_slope_1);
            int x_end = x0 + ((y - y0) * inv_slope_2);
        
            if(x_end < x_start)
            {
                int_swap(&x_start, &x_end);
            }

            for(int x = x_start; x <= x_end; ++x)
            {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv); 
            }
        }
    }

    
    // Render the bottom part of the triangle (flat top)
    inv_slope_1 = 0.0f;
    inv_slope_2 = 0.0f;
    if((y2 - y1) != 0) inv_slope_1 = (float)(x2 - x1) / fabs(y1 - y2);
    if((y2 - y0) != 0) inv_slope_2 = (float)(x2 - x0) / fabs(y0 - y2); 

    if(y1 - y2 != 0)
    {
        for(int y = y1; y <= y2; ++y)
        {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x2 + (y - y2) * inv_slope_2;
            
            if(x_end < x_start)
            {
                int_swap(&x_start, &x_end);
            }

            for(int x = x_start; x <= x_end; ++x)
            {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv); 
            }
        }
    }
}
