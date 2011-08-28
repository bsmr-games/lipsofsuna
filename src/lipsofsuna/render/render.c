/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#include "lipsofsuna/system.h"
#include "lipsofsuna/video.h"
#include "render.h"
#include "render-overlay.h"
#include "render-private.h"
#include "render32/render-private.h"

static void private_render_overlay (
	LIRenRender*  self,
	LIRenOverlay* overlay,
	int           width,
	int           height);

/*****************************************************************************/

LIRenRender* liren_render_new (
	LIPthPaths* paths)
{
	LIRenRender* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenRender));
	if (self == NULL)
		return NULL;
	lialg_random_init (&self->random, lisys_time (NULL));

	/* Initialize the image dictionary. */
	self->images = lialg_strdic_new ();
	if (self->images == NULL)
		return 0;

	/* Initialize the shader dictionary. */
	self->shaders = lialg_strdic_new ();
	if (self->shaders == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the light dictionary. */
	self->lights = lialg_u32dic_new ();
	if (self->lights == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the model dictionary. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the object dictionary. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the overlay dictionary. */
	self->overlays = lialg_u32dic_new ();
	if (self->overlays == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Initialize the backend. */
	if (GLEW_VERSION_3_2 && getenv ("LOS_OPENGL21") == NULL)
	{
		self->v32 = liren_render32_new (self, paths);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_render21_new (self, paths);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	return self;
}

void liren_render_free (
	LIRenRender* self)
{
	LIAlgStrdicIter iter1;
	LIAlgU32dicIter iter2;

	/* Free lights. */
	if (self->lights != NULL)
		lialg_u32dic_free (self->lights);

	/* Free objects. */
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);

	/* Free models. */
	if (self->models != NULL)
	{
		LIALG_U32DIC_FOREACH (iter2, self->models)
			liren_render_model_free (self, iter2.key);
		lialg_u32dic_free (self->models);
	}

	/* Free overlays. */
	if (self->overlays != NULL)
	{
		LIALG_U32DIC_FOREACH (iter2, self->overlays)
			liren_render_overlay_free (self, iter2.key);
		lialg_u32dic_free (self->overlays);
	}

	/* Free images. */
	if (self->images != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->images)
			liren_image_free (iter1.value);
		lialg_strdic_free (self->images);
	}

	/* Free shaders. */
	if (self->shaders != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->shaders)
			liren_shader_free (iter1.value);
		lialg_strdic_free (self->shaders);
	}

	/* Free the backend. */
	if (self->v21 != NULL)
		liren_render21_free (self->v21);
	if (self->v32 != NULL)
		liren_render32_free (self->v32);
	lisys_free (self);
}

void liren_render_draw_clipped_buffer (
	LIRenRender*       self,
	LIRenShader*       shader,
	const LIMatMatrix* modelview,
	const LIMatMatrix* projection,
	GLuint             texture,
	const float*       diffuse,
	const int*         scissor,
	int                start,
	int                count,
	LIRenBuffer*       buffer)
{
	if (self->v32 != NULL)
	{
		liren_render32_draw_clipped_buffer (self->v32, shader->v32, modelview,
			projection, texture, diffuse, scissor, start, count, buffer->v32);
	}
	else
	{
		liren_render21_draw_clipped_buffer (self->v21, shader->v21, modelview,
			projection, texture, diffuse, scissor, start, count, buffer->v21);
	}
}

void liren_render_draw_indexed_triangles_T2V3 (
	LIRenRender*      self,
	LIRenShader*      shader,
	LIMatMatrix*      matrix,
	GLuint            texture,
	const float*      diffuse,
	const float*      vertex_data,
	const LIRenIndex* index_data,
	int               index_count)
{
	if (self->v32 != NULL)
	{
		liren_render32_draw_indexed_triangles_T2V3 (self->v32, shader->v32, matrix,
			texture, diffuse, vertex_data, index_data, index_count);
	}
	else
	{
		liren_render21_draw_indexed_triangles_T2V3 (self->v21, shader->v21, matrix,
			texture, diffuse, vertex_data, index_data, index_count);
	}
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
LIRenImage* liren_render_find_image (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a model by ID.
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
	LIRenImage* image;

	if (self->v32 != NULL)
	{
		image = liren_render_find_image (self, name);
		if (image != NULL)
			return liren_render32_reload_image (self->v32, image->v32);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}
	else
	{
		image = liren_render_find_image (self, name);
		if (image != NULL)
			return liren_render21_reload_image (self->v21, image->v21);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}

	return 1;
}

/**
 * \brief Reloads all images, shaders and other graphics state.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads all data that was lost when the context was erased.
 *
 * \param self Renderer.
 * \param pass Reload pass.
 */
void liren_render_reload (
	LIRenRender* self,
	int          pass)
{
	LIAlgU32dicIter iter;
	LIRenOverlay* overlay;

	/* Reload overlays. */
	LIALG_U32DIC_FOREACH (iter, self->overlays)
	{
		overlay = iter.value;
		if (overlay->buffer != NULL)
		{
			liren_buffer_free (overlay->buffer);
			overlay->buffer = NULL;
		}
	}

	/* Reload others. */
	if (self->v32 != NULL)
		liren_render32_reload (self->v32, pass);
	else
		liren_render21_reload (self->v21, pass);
}

void liren_render_remove_model (
	LIRenRender* self,
	int          id)
{
	LIRenModel* model;

	model = lialg_u32dic_find (self->models, id);
	if (model == NULL)
		return;

	if (model->v32 != NULL)
		return liren_render32_remove_model (self->v32, model->v32);
	else
		return liren_render21_remove_model (self->v21, model->v21);
}

/**
 * \brief Renders the overlays.
 * \param self Renderer.
 * \param width Screen width.
 * \param height Screen height.
 */
void liren_render_render (
	LIRenRender* self,
	int          width,
	int          height)
{
	glViewport (0, 0, width, height);
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	if (self->root_overlay != NULL)
		private_render_overlay (self, self->root_overlay, width, height);
	SDL_GL_SwapBuffers ();
}

/**
 * \brief Renders the scene.
 * \param self Renderer.
 * \param framebuffer Render target framebuffer.
 * \param viewport Viewport array.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projeciton matrix of the camera.
 * \param frustum Frustum of the camera.
 * \param render_passes Array of render passes.
 * \param render_passes_num Number of render passes.
 * \param postproc_passes Array of post-processing passes.
 * \param postproc_passes_num Number of post-processing passes.
 */
void liren_render_render_scene (
	LIRenRender*       self,
	LIRenFramebuffer*  framebuffer,
	const GLint*       viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num)
{
	if (self->v32 != NULL)
	{
		return liren_render32_render (self->v32, framebuffer->v32, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
	else
	{
		return liren_render21_render (self->v21, framebuffer->v21, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
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
	if (self->v32 != NULL)
		liren_render32_update (self->v32, secs);
	else
		liren_render21_update (self->v21, secs);
}

int liren_render_get_anisotropy (
	const LIRenRender* self)
{
	if (self->v32 != NULL)
		return liren_render32_get_anisotropy (self->v32);
	else
		return liren_render21_get_anisotropy (self->v21);
}

void liren_render_set_anisotropy (
	LIRenRender* self,
	int          value)
{
	if (self->v32 != NULL)
		liren_render32_set_anisotropy (self->v32, value);
	else
		liren_render21_set_anisotropy (self->v21, value);
}

/**
 * \brief Gets the size of an image.
 * \param self Renderer.
 * \param name Image name.
 * \param result Return location for two integers.
 * \return Nonzero on success.
 */
int liren_render_get_image_size (
	LIRenRender* self,
	const char*  name,
	int*         result)
{
	LIRenImage* image;

	/* Load the image. */
	image = lialg_strdic_find (self->images, name);
	if (image == NULL)
	{
		liren_render_load_image (self, name);
		image = lialg_strdic_find (self->images, name);
		if (image == NULL)
			return 0;
	}

	/* Return the size. */
	result[0] = liren_image_get_width (image);
	result[1] = liren_image_get_height (image);

	return 1;
}

/*****************************************************************************/

static void private_render_overlay (
	LIRenRender*  self,
	LIRenOverlay* overlay,
	int           width,
	int           height)
{
	int i;
	GLuint texture;
	GLint scissor[4];
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenOverlayElement* element;
	static const LIRenFormat overlay_format = { 32, GL_FLOAT, 24, GL_FLOAT, 12, GL_FLOAT, 0 };

	if (!overlay->visible)
		return;
	projection = limat_matrix_ortho (0, width, height, 0, -1.0f, 1.0f);

	/* Render the scene. */
	if (overlay->scene.enabled)
	{
		/* Update the framebuffer. */
		if (overlay->scene.framebuffer == NULL)
		{
			overlay->scene.framebuffer = liren_framebuffer_new (self,
				overlay->scene.viewport[2], overlay->scene.viewport[3],
				overlay->scene.samples, overlay->scene.hdr);
		}
		else
		{
			liren_framebuffer_resize (overlay->scene.framebuffer,
				overlay->scene.viewport[2], overlay->scene.viewport[3],
				overlay->scene.samples, overlay->scene.hdr);
		}

		/* Render the scene. */
		if (overlay->scene.framebuffer != NULL)
		{
			if (self->v32 != NULL)
			{
				liren_render32_render (self->v32, overlay->scene.framebuffer->v32, overlay->scene.viewport,
					&overlay->scene.modelview, &overlay->scene.projection, &overlay->scene.frustum,
					overlay->scene.render_passes, overlay->scene.render_passes_num,
					overlay->scene.postproc_passes, overlay->scene.postproc_passes_num);
			}
			else
			{
				liren_render21_render (self->v21, overlay->scene.framebuffer->v21, overlay->scene.viewport,
					&overlay->scene.modelview, &overlay->scene.projection, &overlay->scene.frustum,
					overlay->scene.render_passes, overlay->scene.render_passes_num,
					overlay->scene.postproc_passes, overlay->scene.postproc_passes_num);
			}
		}
	}

	/* Render elements. */
	if (overlay->elements.count)
	{
		/* Update the buffer. */
		if (overlay->buffer == NULL)
		{
			overlay->buffer = liren_buffer_new (self, NULL, 0, &overlay_format,
				overlay->vertices.array, overlay->vertices.count, LIREN_BUFFER_TYPE_STATIC);
		}

		/* Render each element. */
		if (overlay->buffer != NULL)
		{
			modelview = limat_matrix_translation (overlay->position.x, overlay->position.y, 0.0f);
			for (i = 0 ; i < overlay->elements.count ; i++)
			{
				element = overlay->elements.array + i;
				if (element->scissor_enabled)
				{
					scissor[0] = (int) overlay->position.x + element->scissor_rect[0];
					scissor[1] = height - (int) overlay->position.y - element->scissor_rect[1] - element->scissor_rect[3];
					scissor[2] = element->scissor_rect[2];
					scissor[3] = element->scissor_rect[3];
				}
				else
				{
					scissor[0] = 0;
					scissor[1] = 0;
					scissor[2] = width;
					scissor[3] = height;
				}
				if (element->image != NULL)
					texture = liren_image_get_handle (element->image);
				else
					texture = element->font->texture;
				liren_render_draw_clipped_buffer (self, element->shader, &modelview,
					&projection, texture, element->color, scissor,
					element->buffer_start, element->buffer_count, overlay->buffer);
			}
		}
	}

	/* Render child overlays. */
	for (i = overlay->overlays.count - 1 ; i >= 0 ; i--)
	{
		if (overlay->overlays.array[i]->behind)
			private_render_overlay (self, overlay->overlays.array[i], width, height);
	}
	for (i = overlay->overlays.count - 1 ; i >= 0 ; i--)
	{
		if (!overlay->overlays.array[i]->behind)
			private_render_overlay (self, overlay->overlays.array[i], width, height);
	}
}

/** @} */
/** @} */
