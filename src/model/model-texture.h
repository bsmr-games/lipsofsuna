/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlTexture Texture
 * @{
 */

#ifndef __MODEL_TEXTURE_H__
#define __MODEL_TEXTURE_H__

#include <string.h>
#include <archive/lips-archive.h>
#include "model-texture.h"

typedef int limdlTextureFlags;
typedef int limdlTextureType;

enum
{
	LIMDL_TEXTURE_FLAG_BILINEAR = 0x01,
	LIMDL_TEXTURE_FLAG_CLAMP    = 0x02,
	LIMDL_TEXTURE_FLAG_MIPMAP   = 0x04,
	LIMDL_TEXTURE_FLAG_REPEAT   = 0x08
};

enum
{
	LIMDL_TEXTURE_TYPE_EMPTY,
	LIMDL_TEXTURE_TYPE_ENVMAP,
	LIMDL_TEXTURE_TYPE_IMAGE
};

typedef struct _limdlTexture limdlTexture;
struct _limdlTexture
{
	limdlTextureType type;
	limdlTextureFlags flags;
	int width;
	int height;
	char* string;
};

#ifdef __cplusplus
extern "C" {
#endif

int
limdl_texture_compare (const limdlTexture* self,
                       const limdlTexture* texture);

int
limdl_texture_set_string (limdlTexture* self,
                          const char*   value);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
