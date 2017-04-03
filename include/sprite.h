#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "graphics3d.h"
#include "vector.h"

typedef struct
{
	int	_refcount;
	char filename[128];
	SDL_Texture *texture;
	SDL_Surface *surface;
	GLuint w,h;
	GLuint depth;
}Sprite;


/**
	@brief initialize the sprite resource manager
	@param count set the maximum number of supported sprites
*/
void sprite_init(unsigned int count);
/**
	@brief load a sprite from disk and return a pointer to it
	@param filename the path to the file to load
	@return NULL on error or a sprite if successful
*/
Sprite * sprite_load(char * filename);

/**
	@brief Handles freeing a sprite previously loaded
	keeps sprites in memory until the memory is needed or while its reference count is greater than 0
	@param the sprite to free
*/
void sprite_free(Sprite *sprite);
void sprite_bind(Sprite *sprite);
void sprite_unbind(Sprite *sprite);


#endif
