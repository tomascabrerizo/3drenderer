#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"

#define N_CUBE_VERTICES 8
#define N_CUBE_FACES (2*6)

extern vec3_t cube_vertices[N_CUBE_VERTICES];
extern face_t cube_faces[N_CUBE_FACES];

// NOTE: This thing declare a struct for dynamic sizes meshes

typedef struct {
   vec3_t *vertices; 
   face_t *faces; 
   vec3_t rotation;
} mesh_t;

extern mesh_t mesh;

void load_cube_mesh_data(void);
void load_obj_file_data(char *filename);

#endif //MESH_H

