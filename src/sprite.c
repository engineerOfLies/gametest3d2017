#include <string.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include "sprite.h"
#include "simple_logger.h"

static Sprite * __sprite_list = NULL;
static int	__sprite_max = 0;

void sprite_close()
{
	int i;
	for (i= 0; i < __sprite_max;i++)
	{
		if (__sprite_list[i]._refcount)
		{
			__sprite_list[i]._refcount = 0;
			sprite_free(&__sprite_list[i]);
		}
	}
	free(__sprite_list);
}

void sprite_init(unsigned int count)
{
	if (count == 0)
	{
		slog("cannot manage zero sprites");
		return;
	}
	__sprite_list = (Sprite *)malloc(sizeof(Sprite)*count);
	if (!__sprite_list)
	{
		slog("failed to allocate %i sprites",count);
		return;
	}
	__sprite_max = count;
	memset(__sprite_list,0,sizeof(Sprite)*count);
	atexit(sprite_close);
}

Sprite * sprite_find_by_file(char * filename)
{
	int i;
	for (i = 0 ; i < __sprite_max;i++)
	{
		if (!__sprite_list[i]._refcount)continue;
		if (strncmp(filename,__sprite_list[i].filename,128) == 0)
		{
			return &__sprite_list[i];
		}
	}
	return NULL;
}

Sprite * sprite_new()
{
	int i;
	for (i = 0 ; i < __sprite_max;i++)
	{
		if (!__sprite_list[i]._refcount)
		{
			memset(&__sprite_list[i],0,sizeof(Sprite));
			__sprite_list[i]._refcount = 1;
			return &__sprite_list[i];
		}
	}
	slog("no more sprites available");
	return NULL;
}

Sprite * sprite_load(char * filename)
{
	Sprite * sprite;
	SDL_Surface *surf,*temp1;
	sprite = sprite_find_by_file(filename);
	if (sprite)
	{
		sprite->_refcount++;
		return sprite;
	}
	surf = IMG_Load(filename);
	if (!surf)
	{
		slog("failed to load image %s",filename);
		return NULL;
	}
	sprite = sprite_new();
	if (!sprite)
	{
		SDL_FreeSurface(surf);
		return NULL;
	}
	sprite->texture = SDL_CreateTextureFromSurface(graphics_get_renderer(),sprite->surface);
    if (sprite->texture)
    {
        SDL_SetTextureBlendMode(sprite->texture,SDL_BLENDMODE_BLEND);        
        SDL_UpdateTexture(sprite->texture,
                        NULL,
                        sprite->surface->pixels,
                        sprite->surface->pitch);
	}
	return sprite;
}

void sprite_bind(Sprite *sprite)
{
	SDL_GL_BindTexture(sprite->texture,NULL,NULL);
}

void sprite_unbind(Sprite *sprite)
{
	SDL_GL_UnbindTexture(sprite->texture);
}

void sprite_free(Sprite *sprite)
{
	if (!sprite)return;
	sprite->_refcount--;
	if (sprite->_refcount > 0)
	{
		// still in use
		return;
	}
	if (sprite->texture)
	{
		SDL_DestroyTexture(sprite->texture);
		sprite->texture = NULL;
	}
	if (sprite->surface)
	{
		SDL_FreeSurface(sprite->surface);
		sprite->surface = NULL;
	}
}

/*eol@eof*/
