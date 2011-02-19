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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenRender Render
 * @{
 */

#include <lipsofsuna/system.h>
#include "render.h"
#include "render-draw.h"

static int private_init_resources (
	LIRenRender* self);

/*****************************************************************************/

LIRenRender* liren_render_new (
	LIPthPaths* paths)
{
	LIRenRender* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenRender));
	if (self == NULL)
		return NULL;
	self->paths = paths;
	lialg_random_init (&self->random, lisys_time (NULL));

	/* Allocate scene list. */
	self->scenes = lialg_ptrdic_new ();
	if (self->scenes == NULL)
		goto error;

	/* Load data. */
	if (!private_init_resources (self))
		goto error;

	return self;

error:
	liren_render_free (self);
	return NULL;
}

void liren_render_free (
	LIRenRender* self)
{
	LIAlgStrdicIter iter1;
	LIAlgPtrdicIter iter2;
	LIRenImage* image;
	LIRenModel* model;
	LIRenShader* shader;

	/* Free helper resources. */
	glDeleteTextures (1, &self->helpers.noise);
	glDeleteTextures (1, &self->helpers.depth_texture_max);
	if (self->helpers.empty_image != NULL)
		liren_image_free (self->helpers.empty_image);

	/* Free shaders. */
	if (self->shaders != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->shaders)
		{
			shader = iter1.value;
			liren_shader_free (shader);
		}
		lialg_strdic_free (self->shaders);
	}

	/* Free models. */
	if (self->models != NULL)
		lialg_u32dic_free (self->models);
	if (self->models_ptr != NULL)
	{
		LIALG_PTRDIC_FOREACH (iter2, self->models_ptr)
		{
			model = iter2.value;
			liren_model_free (model);
		}
		lialg_ptrdic_free (self->models_ptr);
	}

	/* Free images. */
	if (self->images != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->images)
		{
			image = iter1.value;
			liren_image_free (image);
		}
		lialg_strdic_free (self->images);
	}

	/* Free scenes. */
	if (self->scenes != NULL)
	{
		lisys_assert (self->scenes->size == 0);
		lialg_ptrdic_free (self->scenes);
	}

	if (self->helpers.unit_quad != NULL)
	{
		liren_buffer_free (self->helpers.unit_quad);
		lisys_free (self->helpers.unit_quad);
	}
	if (self->immediate.buffer != NULL)
	{
		liren_buffer_free (self->immediate.buffer);
		lisys_free (self->immediate.buffer);
	}

	if (self->context != NULL)
	{
		liren_uniforms_clear (&self->context->uniforms);
		lisys_free (self->context);
	}

	lisys_free (self);
}

/**
 * \brief Finds a shader by name.
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader or NULL.
 */
LIRenShader* liren_render_find_shader (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->shaders, name);
}

/**
 * \brief Finds a texture by name.
 *
 * Searches for a texture from the texture cache and returns the match, if any.
 * If no match is found, NULL is returned.
 *
 * \param self Renderer.
 * \param name Name of the texture.
 * \return Texture or NULL.
 */
LIRenImage*
liren_render_find_image (LIRenRender* self,
                         const char*  name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a model by ID.
 *
 * \param self Renderer.
 * \param id Model ID.
 * \return Model.
 */
LIRenModel* liren_render_find_model (
	LIRenRender* self,
	int          id)
{
	return lialg_u32dic_find (self->models, id);
}

/**
 * \brief Forces the renderer to load or reload a texture image.
 *
 * Reloads the requested texture and updates any materials that reference it
 * to point to the new texture. Any other references to the texture become
 * invalid and need to be manually replaced.
 *
 * \param self Renderer.
 * \param name Texture name.
 * \return Nonzero on success.
 */
int liren_render_load_image (
	LIRenRender* self,
	const char*  name)
{
	LIAlgPtrdicIter iter;
	LIRenImage* image;
	LIRenModel* model;

	/* Just create new image if no old one. */
	image = lialg_strdic_find (self->images, name);
	if (image == NULL)
	{
		image = liren_image_new_from_file (self, name);
		if (image == NULL)
		{
			lisys_error_report ();
			image = liren_image_new (self, name);
			if (image == NULL)
				return 0;
		}
		return 1;
	}

	/* Reload existing image. */
	if (!liren_image_load (image))
		return 0;

	/* Replace in all models. */
	LIALG_PTRDIC_FOREACH (iter, self->models_ptr)
	{
		model = iter.value;
		liren_model_replace_image (model, image);
	}

	return 1;
}

/**
 * \brief Updates the renderer state.
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void liren_render_update (
	LIRenRender* self,
	float        secs)
{
	/* Update time. */
	self->helpers.time += secs;
}

LIRenContext* liren_render_get_context (
	LIRenRender* self)
{
	return self->context;
}

/*****************************************************************************/

static int private_init_resources (
	LIRenRender* self)
{
	int x;
	int y;
	unsigned char value;
	unsigned char* pixel;
	unsigned char* pixels;
	const float depth_texture[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	uint32_t quad_index_data[] = { 0, 1, 2, 1, 2, 3 };
	LIRenFormat immediate_vertex_format = { 32, GL_FLOAT, 24, GL_FLOAT, 12, GL_FLOAT, 0 };
	LIRenFormat quad_vertex_format = { 20, GL_FLOAT, 12, 0, 0, GL_FLOAT, 0 };
	const float quad_vertex_data[] =
	{
		-1, -1, 0, 0, 0,
		1, -1, 0, 1, 0,
		-1, 1, 0, 0, 1,
		1, 1, 0, 1, 1
	};
	static const int perlin_grad[16][3] =
	{
		{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
		{1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
		{1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0},
		{1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}
	};
	static const int perlin_perm[256] =
	{
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,
		142,8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,
		219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,
		68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,
		133,230,220,105,92,41,55,46,245,40,244,102,143,54, 65,25,63,161,1,216,
		80,73,209,76,132,187,208, 89,18,169,200,196,135,130,116,188,159,86,164,
		100,109,198,173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,
		255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,223,183,170,213,
		119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,129,22,39,253,
		19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,34,242,
		193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,
		214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,
		236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	/* Initialize context manager. */
	self->context = lisys_calloc (1, sizeof (LIRenContext));
	if (self->context == NULL)
		return 0;
	self->context->render = self;
	liren_uniforms_init (&self->context->uniforms);
	liren_context_init (self->context);

	/* Initialize image dictionary. */
	self->images = lialg_strdic_new ();
	if (self->images == NULL)
		return 0;

	/* Initialize empty image. */
	self->helpers.empty_image = liren_image_new (self, "empty");
	if (self->helpers.empty_image == NULL)
		return 0;

	/* Initialize model dicrionaries. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
		return 0;
	self->models_ptr = lialg_ptrdic_new ();
	if (self->models_ptr == NULL)
		return 0;

	/* Initialize shader dictionary. */
	self->shaders = lialg_strdic_new ();
	if (self->shaders == NULL)
		return 0;

	/* Initialize default depth texture. */
	glGenTextures (1, &self->helpers.depth_texture_max);
	glBindTexture (GL_TEXTURE_2D, self->helpers.depth_texture_max);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2, 2, 0, GL_DEPTH_COMPONENT, GL_FLOAT, depth_texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Initialize noise texture. */
	pixels = lisys_malloc (4 * 256 * 256);
	pixel = pixels;
	if (pixels == NULL)
		return 0;
	for (y = 0 ; y < 256 ; y++)
	{
		for (x = 0 ; x < 256 ; x++)
		{
			value = perlin_perm[(x + perlin_perm[y]) & 0xFF];
			*(pixel++) = perlin_grad[value & 0x0F][0] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][1] * 64 + 64;
			*(pixel++) = perlin_grad[value & 0x0F][2] * 64 + 64;
			*(pixel++) = value;
		}
	}
	glGenTextures (1, &self->helpers.noise);
	glActiveTexture (GL_TEXTURE0 + LIREN_SAMPLER_NOISE_TEXTURE);
	glBindTexture (GL_TEXTURE_2D, self->helpers.noise);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap (GL_TEXTURE_2D);
	glActiveTexture (GL_TEXTURE0);
	lisys_free (pixels);

	/* Initialize unit quad buffer. */
	self->helpers.unit_quad = lisys_calloc (1, sizeof (LIRenBuffer));
	if (self->helpers.unit_quad == NULL)
		return 0;
	if (!liren_buffer_init (self->helpers.unit_quad, quad_index_data, 6,
	     &quad_vertex_format, &quad_vertex_data, 4, LIREN_BUFFER_TYPE_STATIC))
		return 0;

	/* Initialize immediate mode style vertex buffer. */
	self->immediate.buffer = lisys_calloc (1, sizeof (LIRenBuffer));
	if (self->immediate.buffer == NULL)
		return 0;
	if (!liren_buffer_init (self->immediate.buffer, NULL, 0,
	     &immediate_vertex_format, NULL, 300000, LIREN_BUFFER_TYPE_STREAM))
		return 0;

	return 1;
}

/** @} */
/** @} */
