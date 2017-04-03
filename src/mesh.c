#include <stdlib.h>
#include <stdio.h>

#include "mesh.h"
#include "graphics3d.h"
#include "sprite.h"
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
    mesh->triCount = numfaces;
    mesh->normals = (Vector3D *)malloc(sizeof(Vector3D)*numnormals);
    mesh->normCount = numnormals;
    mesh->texels = (Vector2D *)malloc(sizeof(Vector2D)*numtexels);
    mesh->texCount = numtexels;
    mesh->vertices = (Vector3D *)malloc(sizeof(Vector3D)*numvertices);
    mesh->vertCount = numvertices;
    
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
    return mesh;
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

void mesh_reorder_arrays(Mesh *mesh)
{
    int t,c,i,tex;
    float *vArray,*nArray,*tArray;
    
    if (!mesh)return; // fuck off with that shit
    
    vArray = (float *)malloc(sizeof(float)*mesh->triCount * 9);
    nArray = (float *)malloc(sizeof(float)*mesh->triCount * 9);
    tArray = (float *)malloc(sizeof(float)*mesh->triCount * 6);
    for (t = 0,i=0,tex=0; t< mesh->triCount;t++)
    {
        for (c = 2; c >= 0;c--,i+=3,tex+=2)
        {
            vArray[i] = mesh->vertices[mesh->tris[t].c[c].v].x;
            vArray[i+1] = mesh->vertices[mesh->tris[t].c[c].v].y;
            vArray[i+2] = mesh->vertices[mesh->tris[t].c[c].v].z;
            nArray[i] = mesh->normals[mesh->tris[t].c[c].vn].x;
            nArray[i+1] = mesh->normals[mesh->tris[t].c[c].vn].y;
            nArray[i+2] = mesh->normals[mesh->tris[t].c[c].vn].z;
            tArray[tex] = mesh->texels[mesh->tris[t].c[c].v].x;
            tArray[tex+1] = mesh->texels[mesh->tris[t].c[c].v].y;
        }
    }
    free(mesh->vertices);
    free(mesh->normals);
    free(mesh->tris);
    free(mesh->texels);
    mesh->vertCount = mesh->triCount * 9;
    mesh->normCount = mesh->triCount * 9;
    mesh->texCount = mesh->triCount * 6;
    mesh->texels = (Vector2D*)tArray;
    mesh->normals = (Vector3D*)nArray;
    mesh->vertices = (Vector3D*)vArray;
    mesh->tris = NULL;
}

void mesh_draw(Mesh * mesh,Sprite *sprite)
{
    if (!mesh)return;
    glBindVertexArray(mesh->arrayBuffer);
    glUseProgram(graphics3d_get_shader_program());
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vertBuffer); //bind the buffer we're applying attributes to
    glEnableVertexAttribArray(0); //0 is our index, refer to "location = 0" in the vertex shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //tell gl (shader!) how to interpret our vertex data
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->normalBuffer); //bind the buffer we're applying attributes to
    glEnableVertexAttribArray(1); //0 is our index, refer to "location = 0" in the vertex shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, 0); //tell gl (shader!) how to interpret our vertex data
    
    glBindBuffer(GL_ARRAY_BUFFER, mesh->texelBuffer); //bind the buffer we're applying attributes to
    glEnableVertexAttribArray(2); //0 is our index, refer to "location = 0" in the vertex shader
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0); //tell gl (shader!) how to interpret our vertex data
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    
    glUseProgram(0);
    
    glBindVertexArray(0);
}

void mesh_gpu_load(Mesh * mesh)
{
    GLuint vao;
    GLuint vertexBufferObject,normalBufferObject,texelBufferObject;
    
    if (!mesh)return;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao); //make our vertex array object, we need it to restore state we set after binding it. Re-binding reloads the state associated with it.
    mesh->arrayBuffer = vao;
    
    //vertex buffer
    glGenBuffers(1, &vertexBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->vertCount, mesh->vertices, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers
    
    //normal buffer
    glGenBuffers(1, &normalBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->normCount, mesh->normals, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers
    
    //texels buffer
    glGenBuffers(1, &texelBufferObject); //create the buffer
    glBindBuffer(GL_ARRAY_BUFFER, texelBufferObject); //we're "using" this one now
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*mesh->texCount, mesh->texels, GL_STATIC_DRAW); //formatting the data for the buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind any buffers
    
    glBindVertexArray(0);
    
    mesh->vertBuffer = vertexBufferObject;
    mesh->texelBuffer = texelBufferObject;
    mesh->normalBuffer = normalBufferObject;
}


/*eol@eof*/
