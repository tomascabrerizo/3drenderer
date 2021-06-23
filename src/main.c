#include <stdlib.h>
#include <stdbool.h>

#include "array.h"

#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "matrix.h"
#include "light.h"
#include "texture.h"
#include "triangle.h"

bool is_running = false;
int previous_frame_time = 0;
triangle_t *triangles_to_render = 0;
vec3_t camera_position =  { 0.0f, 0.0f, 0.0f };
mat4_t proj_matrix;

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
    
    render_mode = RENDER_TRIANGLE|RENDER_FACE_CULLING;
    // Initialize perspective projection matrix
    float fov = (60*M_PI)/180;
    float aspect = (float)window_height/(float)window_width;
    float znear = 0.1f;
    float zfar = 100.0f;
    proj_matrix = mat4_make_perspective(fov, aspect, znear, zfar);
    
    // Load texture hardcode texture data
    mesh_texture = (uint32_t *)REDBRICK_TEXTURE;

    // Loads the cube values in the mesh data structure
    load_cube_mesh_data();
    //load_obj_file_data("./assets/cube.obj");
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
                if(event.key.keysym.sym == SDLK_5)
                {
                    render_mode &= RENDER_FACE_CULLING;
                    render_mode += RENDER_TEXTURE;
                }
                if(event.key.keysym.sym == SDLK_6)
                {
                    render_mode &= RENDER_FACE_CULLING;
                    render_mode += RENDER_TEXTURE|RENDER_WIREFRAME;
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

void update(void)
{ 
    // Initialize array of triangles to render
    triangles_to_render = 0;

    mesh.rotation.x += 0.01f;
    mesh.rotation.y += 0.02f;
    //mesh.rotation.z += 0.03f;

    //mesh.scale.x += 0.002f;
    //mesh.scale.y += 0.001f;

    // Translate vertex away from the camera
    //mesh.translation.x += 0.01f;
    mesh.translation.z = 5.0f;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);
    mat4_t rotation_matrix = mat4_mul_mat4(mat4_mul_mat4(mat4_make_rotation_x(mesh.rotation.x), 
                                                         mat4_make_rotation_y(mesh.rotation.y)), 
                                                         mat4_make_rotation_z(mesh.rotation.z));
    // Creating World matrix combining scale, rotation and translation
    mat4_t world_matrix = mat4_identity();
    // Order matters: First Scale the Rotate, Then Translate [T]*[R]*[S]*V
    world_matrix = mat4_mul_mat4(translation_matrix, mat4_mul_mat4(rotation_matrix, scale_matrix));
    
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
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
            
            transformed_vertices[j] = transformed_vertex;
        }

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

        if(render_mode & RENDER_FACE_CULLING)
        {
            vec3_t camera_ray = vec3_sub(camera_position, vector_a); 
            float dot_normal_camera = vec3_dot(camera_ray, normal);
            if(dot_normal_camera < 0)
            {
                continue;
            }
        }
        
        vec4_t projected_points[3];
        for(int j = 0; j < 3; ++j)
        {
            // Projects current vertex
            projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);
            
            // Scale and translate into the view port
            // Scale into the view
            projected_points[j].x *= (window_width / 2.0f);
            projected_points[j].y *= (window_height / 2.0f);

            // Invert the y values to account for flip screen
            projected_points[j].y *= -1;

            // Translate projected points to the middle of the screen;
            projected_points[j].x += (window_width / 2.0f);
            projected_points[j].y += (window_height / 2.0f);
        }
        
        // Calculate avg_depth for the face
        float avg_depth = 0;
        avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0f;
       
        // Calculate the light intensity
        vec3_normalize(&light.direction);
        float light_intensity_factor = -vec3_dot(normal, light.direction);
        uint32_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

        triangle_t projected_triangle = 
        {
            .points = 
            {
                { projected_points[0].x, projected_points[0].y },
                { projected_points[1].x, projected_points[1].y },
                { projected_points[2].x, projected_points[2].y },
            },
            .texcoords = 
            {
                { mesh_face.a_uv.u, mesh_face.a_uv.v },
                { mesh_face.b_uv.u, mesh_face.b_uv.v },
                { mesh_face.c_uv.u, mesh_face.c_uv.v },
            },
            .color = triangle_color,
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

        if(render_mode & RENDER_TEXTURE)
        {
            draw_texture_triangle(triangle.points[0].x, triangle.points[0].y, triangle.texcoords[0].u, triangle.texcoords[0].v,
                                  triangle.points[1].x, triangle.points[1].y, triangle.texcoords[1].u, triangle.texcoords[1].v,
                                  triangle.points[2].x, triangle.points[2].y, triangle.texcoords[2].u, triangle.texcoords[2].v,
                                  mesh_texture); 
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
        
        int current_time = SDL_GetTicks();
        int current_ms = (current_time - previous_frame_time);
        if(current_ms < FRAME_TARGET_TIME)
        {
            int ms_to_sleep = FRAME_TARGET_TIME - current_ms;
            SDL_Delay(ms_to_sleep);
            current_time = SDL_GetTicks();
            current_ms = (current_time - previous_frame_time);
        }
        previous_frame_time = current_time;
    }

    destroy_window();
    free_resources();

    return 0;
}
