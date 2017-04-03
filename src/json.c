#include <jansson.h>
#include "mesh.h"
#include "simple_logger.h"

json_t *model_json_load(char * filename);
float *model_json_get_vertices(json_t *model,int *c);

float *model_json_get_vertices(json_t *model,int *c)
{
	float * vertices = NULL;
	json_t *vertarray = NULL;
	json_t *number = NULL;
	int count = 0,i = 0;
	if (!model)
	{
		slog("no model provided!");
		return NULL;
	}
	vertarray = json_object_get(model, "vertices");
	if (!vertarray)
	{
		slog("model contains no vertex data");
		return NULL;
	}
	count = json_array_size(vertarray);
	if (!count)
	{
		slog("no vertices in the vertex array of the model");
		json_decref(vertarray);
		return NULL;
	}
	vertices = (float *)malloc(sizeof(float)*count);
	if (!vertices)
	{
		slog("failed to allocate vertex memory");
		json_decref(vertarray);
		return NULL;
	}
	for (i = 0; i < count;i++)
	{
		number = json_array_get(vertarray, i);
		vertices[i] =  json_real_value(number);
		json_decref(number);
	}
	json_decref(vertarray);
	*c = count;
	return vertices;
}

float *model_json_get_texels(json_t *model,int *c)
{
	float * vertices = NULL;
	json_t *vertarray = NULL;
	json_t *number = NULL;
	int count = 0,i = 0;
	if (!model)
	{
		slog("no model provided!");
		return NULL;
	}
	vertarray = json_object_get(model, "uvs");
	if (!vertarray)
	{
		slog("model contains no texel data");
		return NULL;
	}
	count = json_array_size(vertarray);
	if (!count)
	{
		slog("no vertices in the texel array of the model");
		json_decref(vertarray);
		return NULL;
	}
	vertices = (float *)malloc(sizeof(float)*count);
	if (!vertices)
	{
		slog("failed to allocate texel memory");
		json_decref(vertarray);
		return NULL;
	}
	for (i = 0; i < count;i++)
	{
		number = json_array_get(vertarray, i);
		vertices[i] =  json_real_value(number);
		json_decref(number);
	}
	json_decref(vertarray);
	*c = count;
	return vertices;
}

float *model_json_get_normals(json_t *model,int *c)
{
	float * vertices = NULL;
	json_t *vertarray = NULL;
	json_t *number = NULL;
	int count = 0,i = 0;
	if (!model)
	{
		slog("no model provided!");
		return NULL;
	}
	vertarray = json_object_get(model, "normals");
	if (!vertarray)
	{
		slog("model contains no normal data");
		return NULL;
	}
	count = json_array_size(vertarray);
	if (!count)
	{
		slog("no vertices in the normal array of the model");
		json_decref(vertarray);
		return NULL;
	}
	vertices = (float *)malloc(sizeof(float)*count);
	if (!vertices)
	{
		slog("failed to allocate normal memory");
		json_decref(vertarray);
		return NULL;
	}
	for (i = 0; i < count;i++)
	{
		number = json_array_get(vertarray, i);
		vertices[i] =  json_real_value(number);
		json_decref(number);
	}
	if (vertarray->refcount)json_decref(vertarray);
	*c = count;
	return vertices;
}

int *model_json_get_faces(json_t *model,int *c)
{
	int * vertices = NULL;
	json_t *vertarray = NULL;
	json_t *number = NULL;
	int count = 0,i = 0;
	if (!model)
	{
		slog("no model provided!");
		return NULL;
	}
	vertarray = json_object_get(model, "faces");
	if (!vertarray)
	{
		slog("model contains no face data");
		return NULL;
	}
	count = json_array_size(vertarray);
	if (!count)
	{
		slog("no vertices in the face array of the model");
		json_decref(vertarray);
		return NULL;
	}
	vertices = (int *)malloc(sizeof(int)*count);
	if (!vertices)
	{
		slog("failed to allocate face memory");
		json_decref(vertarray);
		return NULL;
	}
	for (i = 0; i < count;i++)
	{
		number = json_array_get(vertarray, i);
		vertices[i] =  json_integer_value(number);
		json_decref(number);
	}
	json_decref(vertarray);
	*c = count;
	return vertices;
}

Mesh * mesh_load_from_json(char * filename)
{
	Mesh *mesh = NULL;
	json_t *json = NULL;
    json_error_t er;

	json = json_load_file(filename, JSON_DECODE_ANY, &er);

	if (!json)
	{
		slog("json error: %s in %s at %i:%i",er.text,er.source,er.line,er.column);
		return NULL;
	}

	mesh = (Mesh *)malloc(sizeof(mesh));
	if (!mesh)
	{
		slog("failed to allocate a mesh for %s",filename);
		json_decref(json);
		return NULL;
	}
	mesh->texels = (Vector2D*)model_json_get_texels(json,&mesh->texCount);
	mesh->vertices = (Vector3D*)model_json_get_vertices(json,&mesh->vertCount);
	mesh->normals = (Vector3D*)model_json_get_normals(json,&mesh->normCount);
	mesh->tris = (Triangle*)model_json_get_faces(json,&mesh->triCount);
	return mesh;
}

/*eol@eof*/
