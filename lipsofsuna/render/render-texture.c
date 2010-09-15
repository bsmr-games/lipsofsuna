/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenTexture Texture
 * @{
 */

#include "render-texture.h"

void liren_texture_init (
	LIRenTexture* self,
	LIMdlTexture* texture)
{
	self->flags = texture->flags;
	self->type = texture->type;
	self->width = texture->width;
	self->height = texture->height;

	/* Precalculate texture parameters. */
	if (self->flags & LIMDL_TEXTURE_FLAG_MIPMAP)
	{
		if (self->flags & LIMDL_TEXTURE_FLAG_BILINEAR)
		{
			self->params.magfilter = GL_LINEAR;
			self->params.minfilter = GL_LINEAR_MIPMAP_LINEAR;
		}
		else
		{
			self->params.magfilter = GL_NEAREST;
			self->params.minfilter = GL_NEAREST_MIPMAP_NEAREST;
		}
	}
	else
	{
		if (self->flags & LIMDL_TEXTURE_FLAG_BILINEAR)
		{
			self->params.magfilter = GL_LINEAR;
			self->params.minfilter = GL_NEAREST;
		}
		else
		{
			self->params.magfilter = GL_NEAREST;
			self->params.minfilter = GL_NEAREST;
		}
	}
	if (self->flags & LIMDL_TEXTURE_FLAG_CLAMP)
	{
		self->params.wraps = GL_CLAMP_TO_EDGE;
		self->params.wrapt = GL_CLAMP_TO_EDGE;
	}
	else
	{
		self->params.wraps = GL_REPEAT;
		self->params.wrapt = GL_REPEAT;
	}

	/* Create a sampler object. */
	glGenSamplers (1, &self->sampler);
	glSamplerParameteri (self->sampler, GL_TEXTURE_MIN_FILTER, self->params.minfilter);
	glSamplerParameteri (self->sampler, GL_TEXTURE_MAG_FILTER, self->params.magfilter);
	glSamplerParameteri (self->sampler, GL_TEXTURE_WRAP_S, self->params.wraps);
	glSamplerParameteri (self->sampler, GL_TEXTURE_WRAP_T, self->params.wrapt);
}

void liren_texture_free (
	LIRenTexture* self)
{
	if (self->sampler)
		glDeleteSamplers (1, &self->sampler);
}

void liren_texture_set_image (
	LIRenTexture* self,
	LIRenImage*   value)
{
	self->image = value;
	if (value != NULL && value->texture != NULL)
		self->texture = value->texture->texture;
	else
		self->texture = 0;
}

/** @} */
/** @} */
