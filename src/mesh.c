#include "mesh.h"
#include "array.h"

mesh_t mesh = {
    .vertices = 0,
    .faces = 0
};

// TODO: Create implemtation form mesh.h functions
vec3_t cube_vertices[N_CUBE_VERTICES] = 
{
    { .x = -1.0f, .y = -1.0f,  .z = -1.0f }, // 1
    { .x = -1.0f, .y =  1.0f,  .z = -1.0f }, // 2
    { .x =  1.0f, .y =   1.0f, .z = -1.0f }, // 3
    { .x =  1.0f, .y =  -1.0f, .z = -1.0f }, // 4
    { .x =  1.0f, .y =  1.0f,  .z =  1.0f }, // 5
    { .x =  1.0f, .y = -1.0f,  .z =  1.0f }, // 6
    { .x = -1.0f, .y =  1.0f,  .z =  1.0f }, // 7
    { .x = -1.0f, .y = -1.0f,  .z =  1.0f }  // 8
};

face_t cube_faces[N_CUBE_FACES] = 
{
    // Front face
    { .a = 1, .b = 2, .c = 3 },
    { .a = 1, .b = 3, .c = 4 },
    // Right face
    { .a = 4, .b = 3, .c = 5 },
    { .a = 4, .b = 5, .c = 6 },
    // Back face
    { .a = 6, .b = 5, .c = 7 },
    { .a = 6, .b = 7, .c = 8 },
    // Left face
    { .a = 8, .b = 7, .c = 2 },
    { .a = 8, .b = 2, .c = 1 },
    // Top face
    { .a = 2, .b = 7, .c = 5 },
    { .a = 2, .b = 5, .c = 3 },
    // Bottom face
    { .a = 6, .b = 8, .c = 1 },
    { .a = 6, .b = 1, .c = 4 }
};

void load_cube_mesh_data(void)
{
    for(int i = 0; i < N_CUBE_VERTICES; ++i)
    {
        vec3_t cube_vertex = cube_vertices[i];
        array_push(mesh.vertices, cube_vertex);
    }
    for(int i = 0; i < N_CUBE_FACES; ++i)
    {
        face_t cube_face = cube_faces[i];
        array_push(mesh.faces, cube_face);
    }
}
