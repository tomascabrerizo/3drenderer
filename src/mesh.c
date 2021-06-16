#include <stdio.h>
#include <string.h>
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
    { .a = 1, .b = 2, .c = 3, .color = 0xFFFF0000 },
    { .a = 1, .b = 3, .c = 4, .color = 0xFFFF0000 },
    // Right face
    { .a = 4, .b = 3, .c = 5, .color = 0xFF00FF00 },
    { .a = 4, .b = 5, .c = 6, .color = 0xFF00FF00 },
    // Back face
    { .a = 6, .b = 5, .c = 7, .color = 0xFF0000FF },
    { .a = 6, .b = 7, .c = 8, .color = 0xFF0000FF },
    // Left face
    { .a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00 },
    { .a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00 },
    // Top face
    { .a = 2, .b = 7, .c = 5, .color = 0xFFFF00FF },
    { .a = 2, .b = 5, .c = 3, .color = 0xFFFF00FF },
    // Bottom face
    { .a = 6, .b = 8, .c = 1, .color = 0xFF00FFFF },
    { .a = 6, .b = 1, .c = 4, .color = 0xFF00FFFF }
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

void load_obj_file_data(char *filename)
{
    // TODO: Read the content of the obj file and load
    // vertices and faces in our mesh.vertices and mesh.faces
    FILE *obj_file = fopen(filename, "r");
    if(obj_file)
    {
        char line[1024];
        while(fgets(line, sizeof(line), obj_file))
        {
            if(strncmp(line, "v ", 2) == 0)
            {
                vec3_t vertex = {0};
                sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z); 
                array_push(mesh.vertices, vertex)
            }
            else if(strncmp(line, "f ", 2) == 0)
            {
                face_t v = {0};
                face_t t = {0};
                face_t n = {0};
                sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", 
                &v.a, &t.a, &n.a, &v.b, &t.b, &n.b, &v.c, &t.c, &n.c);
                array_push(mesh.faces, v);
            }
        }
    }
    fclose(obj_file);
}
