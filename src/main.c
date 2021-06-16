#include <stdlib.h>
#include <stdbool.h>

#include "array.h"

#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"

triangle_t *triangles_to_render = 0;
vec3_t camera_position =  { 0.0f, 0.0f, 0.0f };
float fov_factor = 640;
bool is_running = false;
int previous_frame_time = 0;

void setup(void)
{
    // Allocate the required bytes in memory for the color buffer
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    if(!color_buffer)
    {
        fprintf(stderr, "Error creating color buffer.\n");
    }

    // Creating SDL texture that is use to display the color buffer
    color_buffer_texure = SDL_CreateTexture(renderer, 
                                            SDL_PIXELFORMAT_ARGB8888, 
                                            SDL_TEXTUREACCESS_STREAMING,
                                            window_width, window_height);
    if(!color_buffer_texure)
    {
        fprintf(stderr, "Error creating SDL color buffer texture");
    }
    
    // Loads the cube values in the mesh data structure
    load_cube_mesh_data();
    //load_obj_file_data("./assets/cube.obj");
    
    render_mode = RENDER_TRIANGLE|RENDER_WIREFRAME|RENDER_FACE_CULLING;
}

void process_intput(void)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
            {
                is_running = false;
            }break;
            case SDL_KEYDOWN:
            {
                if(event.key.keysym.sym == SDLK_ESCAPE)
                {
                    is_running = false;
                }
                if(event.key.keysym.sym == SDLK_1)
                {
                    render_mode &= RENDER_FACE_CULLING;
                    render_mode += RENDER_WIREFRAME|RENDER_DOTS;
                }
                if(event.key.keysym.sym == SDLK_2)
                {
                    render_mode &= RENDER_FACE_CULLING;
                    render_mode += RENDER_WIREFRAME;
                }
                if(event.key.keysym.sym == SDLK_3)
                {
                    render_mode &= RENDER_FACE_CULLING;
                    render_mode += RENDER_TRIANGLE;
                }
                if(event.key.keysym.sym == SDLK_4)
                {
                    render_mode &= RENDER_FACE_CULLING;
                    render_mode += RENDER_TRIANGLE|RENDER_WIREFRAME;
                }
                if(event.key.keysym.sym == SDLK_c)
                {
                    render_mode |= RENDER_FACE_CULLING;
                }
                if(event.key.keysym.sym == SDLK_d)
                {
                    render_mode &= ~RENDER_FACE_CULLING;
                }
            }break;
            case SDL_KEYUP:
            {
            }break;
        }
    }
}

vec2_t project(vec3_t point)
{
    vec2_t projected_point = 
    {
        .x = (fov_factor * point.x) / point.z, 
        .y = (fov_factor * point.y) / point.z
    };
    return projected_point;
}

void update(void)
{ 
    // Initialize array of triangles to render
    triangles_to_render = 0;

    mesh.rotation.x += 0.01f;
    mesh.rotation.y += 0.01f;
    mesh.rotation.z += 0.01f;

    mesh.scale.x += 0.002f;
    mesh.scale.y += 0.001f;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    
    int num_faces = array_length(mesh.faces);
    for(int i = 0; i < num_faces; ++i)
    {
        face_t mesh_face = mesh.faces[i];
        
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];
    
        vec4_t transformed_vertices[3];
        for(int j = 0; j < 3; ++j)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
            transformed_vertex = mat4_mul_vec4(scale_matrix, transformed_vertex);
            
            // Translate vertex away from the camera
            transformed_vertex.z += 5;
            transformed_vertices[j] = transformed_vertex;
        }

        if(render_mode & RENDER_FACE_CULLING)
        {
            // Calculate triangle normal
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

            vec3_t vector_ab = vec3_sub(vector_b, vector_a);
            vec3_t vector_ac = vec3_sub(vector_c, vector_a);
            vec3_normalize(&vector_ab);
            vec3_normalize(&vector_ac);
            vec3_t normal = vec3_cross(vector_ab, vector_ac);
            vec3_normalize(&normal);
            vec3_t camera_ray = vec3_sub(camera_position, vector_a); 
            float dot_normal_camera = vec3_dot(camera_ray, normal);
            if(dot_normal_camera < 0)
            {
                continue;
            }
        }
        
        vec2_t projected_points[3];
        for(int j = 0; j < 3; ++j)
        {
            // Projects current vertex
            projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]));

            // Scale and translate projected points to the middle of the screen;
            projected_points[j].x += (window_width / 2);
            projected_points[j].y += (window_height / 2);
        }
        
        // Calculate avg_depth for the face
        float avg_depth = 0;
        avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0f;
        
        triangle_t projected_triangle = 
        {
            .points = 
            {
                projected_points[0],
                projected_points[1],
                projected_points[2],
            },
            .color = mesh_face.color,
            .avg_depth = avg_depth,
        };
        // Save projected triangle in array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }

    // Sort triangles to render by the avg depth
    int num_triangles = array_length(triangles_to_render);
    for(int i = 0; i < num_triangles; ++i)
    {
        for(int j = i; j < num_triangles; ++j)
        {
            if(triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth)
            {
                triangle_t tmp =  triangles_to_render[i];
                triangles_to_render[i] = triangles_to_render[j];
                triangles_to_render[j] = tmp;
            }
        }
    }
}

void render(void)
{
    draw_grid();
    
    int num_triangles = array_length(triangles_to_render); 
    for(int i = 0; i < num_triangles ; ++i)
    {
        triangle_t triangle = triangles_to_render[i];
        if(render_mode & RENDER_TRIANGLE)
        {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          triangle.color); 
        }
        if(render_mode & RENDER_WIREFRAME)
        {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          0xFFFFFFFF);
        }
        if(render_mode & RENDER_DOTS)
        {
            int s = 6;
            draw_rect(triangle.points[0].x-s/2, triangle.points[0].y-s/2, s, s, 0xFFFF0000);
            draw_rect(triangle.points[1].x-s/2, triangle.points[1].y-s/2, s, s, 0xFFFF0000);
            draw_rect(triangle.points[2].x-s/2, triangle.points[2].y-s/2, s, s, 0xFFFF0000);
        }
    }
    
    // Clear array of triangles 
    array_free(triangles_to_render);

    render_color_buffer();
    clear_color_buffer(0xFF000000);
    
    SDL_RenderPresent(renderer);
}

void free_resources(void)
{
    free(color_buffer);
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

int main(int argc, char **argv)
{
    is_running = initialize_window();
    
    setup();
    
    previous_frame_time = SDL_GetTicks();
    while(is_running)
    {
        process_intput();
        update();
        render();
        
        int current_ms = (SDL_GetTicks() - previous_frame_time);
        if(current_ms < FRAME_TARGET_TIME)
        {
            int ms_to_sleep = FRAME_TARGET_TIME - current_ms;
            SDL_Delay(ms_to_sleep);
            current_ms = (SDL_GetTicks() - previous_frame_time);
        }
        previous_frame_time = current_ms;
    }

    destroy_window();
    free_resources();

    return 0;
}
