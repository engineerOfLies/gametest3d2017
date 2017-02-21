#ifndef __MESH_H__
#define __MESH_H__

#include "vector.h"

typedef struct
{
	unsigned int v;	/**<vertex index for the corner*/
	unsigned int vt;/**<texel index for the corner*/
	unsigned int vn;/**<normal index for the corner*/
}TriCorner;

typedef struct
{
	TriCorner c[3];	/**<the three corners for the triangle*/
}Triangle;

typedef struct
{
	Vector3D *vertices;/**<vertex array*/
	Vector2D *texels;  /**<texture element array*/
	Vector3D *normals; /**<vertex normal array*/
	Triangle *tris;    /**<triangle array*/
}Mesh;

/**
	@brief load an obj file into the mesh data type
	@param filename the path to the obj file to load
	@returns NULL on error or an allocated mesh datastructure
*/
Mesh * mesh_load_from_obj(char * filename);

/**
	@brief frees the data associated with the mesh and set the pointer to NULL
	@param mesh a pointer to your mesh pointer
 */
void mesh_free(Mesh **mesh);

#endif