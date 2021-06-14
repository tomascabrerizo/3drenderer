#include <stdlib.h>
#include <stdbool.h>

#include "array.h"

#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t *triangles_to_render = 0;

vec3_t camera_position =  { 0.0f, 0.0f, 0.0f};

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
    //load_cube_mesh_data();
    load_obj_file_data("./assets/cube.obj");
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
    
    int num_faces = array_length(mesh.faces);
    for(int i = 0; i < num_faces; ++i)
    {
        face_t mesh_face = mesh.faces[i];
        
        vec3_t face_vertices[3];
        face_vertices[0] = mesh.vertices[mesh_face.a - 1];
        face_vertices[1] = mesh.vertices[mesh_face.b - 1];
        face_vertices[2] = mesh.vertices[mesh_face.c - 1];
    
        vec3_t transformed_triangle[3];
        for(int j = 0; j < 3; ++j)
        {
            vec3_t transformed_vertex = face_vertices[j];
            transformed_vertex = vec3_rotate_x(transformed_vertex, mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);
            // Translate vertex away from the camera
            transformed_vertex.z += 5;
            transformed_triangle[j] = transformed_vertex;
        }
        
        // Calculate triangle normal
        vec3_t vector_a = transformed_triangle[0];
        vec3_t vector_b = transformed_triangle[1];
        vec3_t vector_c = transformed_triangle[2];
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
        
        triangle_t projected_triangle;
        for(int j = 0; j < 3; ++j)
        {
            // Projects current vertex
            vec2_t projected_point = project(transformed_triangle[j]);

            // Scale and translate projected points to the middle of the screen;
            projected_point.x += (window_width / 2);
            projected_point.y += (window_height / 2);

            projected_triangle.points[j] = projected_point;

        }
        // Save projected triangle in array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }
}

void render(void)
{
    draw_grid();
    
    int num_triangles = array_length(triangles_to_render); 
    for(int i = 0; i < num_triangles ; ++i)
    {
        triangle_t triangle = triangles_to_render[i];
        draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                      triangle.points[1].x, triangle.points[1].y,
                      triangle.points[2].x, triangle.points[2].y,
                      0xFFFFFFFF); 
        draw_triangle(triangle.points[0].x, triangle.points[0].y,
                      triangle.points[1].x, triangle.points[1].y,
                      triangle.points[2].x, triangle.points[2].y,
                      0xFF000000); 
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
        }
        previous_frame_time = SDL_GetTicks();
        
    }

    destroy_window();
    free_resources();

    return 0;
}
