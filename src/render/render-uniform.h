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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndShader Shader
 * @{
 */

#ifndef __RENDER_UNIFORM_H__
#define __RENDER_UNIFORM_H__

#include <video/lips-video.h>
#include "render-types.h"

enum
{
	LIRND_UNIFORM_NONE,
	LIRND_UNIFORM_CUBETEXTURE0,
	LIRND_UNIFORM_CUBETEXTURE1,
	LIRND_UNIFORM_CUBETEXTURE2,
	LIRND_UNIFORM_CUBETEXTURE3,
	LIRND_UNIFORM_CUBETEXTURE4,
	LIRND_UNIFORM_CUBETEXTURE5,
	LIRND_UNIFORM_CUBETEXTURE6,
	LIRND_UNIFORM_CUBETEXTURE7,
	LIRND_UNIFORM_CUBETEXTURE8,
	LIRND_UNIFORM_CUBETEXTURE9,
	LIRND_UNIFORM_DIFFUSETEXTURE0,
	LIRND_UNIFORM_DIFFUSETEXTURE1,
	LIRND_UNIFORM_DIFFUSETEXTURE2,
	LIRND_UNIFORM_DIFFUSETEXTURE3,
	LIRND_UNIFORM_DIFFUSETEXTURE4,
	LIRND_UNIFORM_DIFFUSETEXTURE5,
	LIRND_UNIFORM_DIFFUSETEXTURE6,
	LIRND_UNIFORM_DIFFUSETEXTURE7,
	LIRND_UNIFORM_DIFFUSETEXTURE8,
	LIRND_UNIFORM_DIFFUSETEXTURE9,
	LIRND_UNIFORM_LIGHTMATRIX0,
	LIRND_UNIFORM_LIGHTMATRIX1,
	LIRND_UNIFORM_LIGHTMATRIX2,
	LIRND_UNIFORM_LIGHTMATRIX3,
	LIRND_UNIFORM_LIGHTMATRIX4,
	LIRND_UNIFORM_LIGHTMATRIX5,
	LIRND_UNIFORM_LIGHTMATRIX6,
	LIRND_UNIFORM_LIGHTMATRIX7,
	LIRND_UNIFORM_LIGHTMATRIX8,
	LIRND_UNIFORM_LIGHTMATRIX9,
	LIRND_UNIFORM_MODELVIEWINVERSE,
	LIRND_UNIFORM_SHADOWTEXTURE0,
	LIRND_UNIFORM_SHADOWTEXTURE1,
	LIRND_UNIFORM_SHADOWTEXTURE2,
	LIRND_UNIFORM_SHADOWTEXTURE3,
	LIRND_UNIFORM_SHADOWTEXTURE4,
	LIRND_UNIFORM_SHADOWTEXTURE5,
	LIRND_UNIFORM_SHADOWTEXTURE6,
	LIRND_UNIFORM_SHADOWTEXTURE7,
	LIRND_UNIFORM_SHADOWTEXTURE8,
	LIRND_UNIFORM_SHADOWTEXTURE9
};

typedef struct _lirndUniform lirndUniform;
struct _lirndUniform
{
	char* name;
	GLint binding;
	GLint sampler;
	int value;
};

static inline int
lirnd_uniform_get_sampler (lirndUniform* self)
{
	if (self->value >= LIRND_UNIFORM_CUBETEXTURE0 &&
	    self->value <= LIRND_UNIFORM_CUBETEXTURE9)
		return 1;
	if (self->value >= LIRND_UNIFORM_DIFFUSETEXTURE0 &&
	    self->value <= LIRND_UNIFORM_DIFFUSETEXTURE9)
		return 1;
	if (self->value >= LIRND_UNIFORM_SHADOWTEXTURE0 &&
	    self->value <= LIRND_UNIFORM_SHADOWTEXTURE9)
		return 1;
	return 0;
}

#endif

/** @} */
/** @} */
