#include <stdlib.h>
#include <stdio.h>

#include "mesh.h"
#include "simple_logger.h"

Mesh * mesh_load_from_obj(char * filename)
{
	FILE *file;
	Mesh *mesh = NULL;
	char buf[512];
	int numvertices = 0,numnormals = 0,numtexels = 0,numfaces = 0;
	int v,t,n,f;

	if (!filename)
	{
		slog("mesh_load_from_obj: no filename provided to load");
		return NULL;
	}
	file = fopen(filename,"r");
	if (!file)
	{
		slog("mesh_load_from_obj: failed top open file %s",filename);
		return NULL;
	}

	while(fscanf(file, "%s", buf) != EOF)
	{
		switch(buf[0])
		{
			case 'v':
				switch(buf[1])
				{
				case '\0':
					fgets(buf, sizeof(buf), file);
					numvertices++;
					break;
				case 'n':
					fgets(buf, sizeof(buf), file);
					numnormals++;
					break;
				case 't':
					fgets(buf, sizeof(buf), file);
					numtexels++;
					break;
				default:
					break;
				}
				break;
			case 'f':
				fgets(buf, sizeof(buf), file);
				numfaces++;
				break;
			default:
				fgets(buf, sizeof(buf), file);
				break;
		}
	}
	// allocate the datastructres we need
	mesh = (Mesh *)malloc(sizeof(Mesh));
	if (!mesh)
	{
		slog("failed to allocate mesh");
		fclose(file);
		return NULL;
	}
	mesh->tris = (Triangle *)malloc(sizeof(Triangle)*numfaces);
	mesh->normals = (Vector3D *)malloc(sizeof(Vector3D)*numnormals);
	mesh->texels = (Vector2D *)malloc(sizeof(Vector2D)*numtexels);
	mesh->vertices = (Vector3D *)malloc(sizeof(Vector3D)*numvertices);

	rewind(file);
	v = t = n = f = 0;
	while(fscanf(file, "%s", buf) != EOF)
	{
		switch(buf[0])
		{
			case 'v':
				switch(buf[1])
				{
				case '\0':
					fscanf(file,"%f %f %f",&mesh->vertices[v].x,&mesh->vertices[v].y,&mesh->vertices[v].z);
					v++;
					break;
				case 'n':
					fscanf(file,"%f %f %f",&mesh->normals[n].x,&mesh->normals[n].y,&mesh->normals[n].z);
					n++;
					break;
				case 't':
					fscanf(file,"%f %f",&mesh->texels[t].x,&mesh->texels[t].y);
					t++;
					break;
				default:
					break;
				}
				break;
			case 'f':
				fscanf(file,"%d/%d/%d %d/%d/%d %d/%d/%d",
					&mesh->tris[f].c[0].v,&mesh->tris[f].c[0].vt,&mesh->tris[f].c[0].vn,
					&mesh->tris[f].c[1].v,&mesh->tris[f].c[1].vt,&mesh->tris[f].c[1].vn,
					&mesh->tris[f].c[2].v,&mesh->tris[f].c[2].vt,&mesh->tris[f].c[2].vn
					);
				f++;
				break;
			default:
				fgets(buf, sizeof(buf), file);
				break;
		}
	}

	fclose(file);
}

void mesh_free(Mesh **mesh)
{
	Mesh *m;
	if (!mesh)
	{
		slog("mesh_free: no mesh pointer provided");
		return;
	}
	m = *mesh;
	if (!m)
	{
		slog("mesh_free: no mesh data provided");
		return;
	}
	if (m->tris)
	{
		free(m->tris);
	}
	if (m->normals)
	{
		free(m->normals);
	}
	if (m->texels)
	{
		free(m->texels);
	}
	if (m->vertices)
	{
		free(m->vertices);
	}
	*mesh = NULL;
}


/*eol@eof*/
