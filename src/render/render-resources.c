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
 * \addtogroup lirndResources Resources
 * @{
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "render-resources.h"

/**
 * \brief Creates a new resource manager.
 *
 * \param self Renderer.
 * \return New resource manager or NULL.
 */
lirndResources*
lirnd_resources_new (lirndRender* render)
{
	lirndResources* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lirndResources));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->render = render;
	self->images = lialg_strdic_new ();
	if (self->images == NULL)
	{
		free (self);
		return NULL;
	}
	self->shaders = lialg_strdic_new ();
	if (self->shaders == NULL)
	{
		lialg_strdic_free (self->images);
		free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the resources.
 *
 * \param self Resources.
 */
void
lirnd_resources_free (lirndResources* self)
{
	lialgStrdicIter iter;
	lirndImage* image;
	lirndShader* shader;

	/* Free shaders. */
	if (self->shaders != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->shaders)
		{
			shader = iter.value;
			lirnd_shader_free (shader);
		}
		lialg_strdic_free (self->shaders);
	}

	/* Free images. */
	if (self->images != NULL)
	{
		LI_FOREACH_STRDIC (iter, self->images)
		{
			image = iter.value;
			if (image->texture != NULL)
				liimg_texture_free (image->texture);
			free (image->name);
			free (image->path);
			free (image);
		}
		lialg_strdic_free (self->images);
	}

	free (self);
}

/**
 * \brief Finds an image by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Image or NULL.
 */
lirndImage*
lirnd_resources_find_image (lirndResources* self,
                            const char*     name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a shader by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Shader or NULL.
 */
lirndShader*
lirnd_resources_find_shader (lirndResources* self,
                             const char*     name)
{
	char* path;
	lirndShader* shader;

	/* Try existing. */
	shader = lialg_strdic_find (self->shaders, name);
	if (shader != NULL)
		return shader;

	/* Try loading. */
	path = lisys_path_format (self->render->config.dir,
		LISYS_PATH_SEPARATOR, "shaders",
		LISYS_PATH_SEPARATOR, name, NULL);
	if (path == NULL)
		return NULL;
	shader = lirnd_shader_new_from_file (self->render, path);
	free (path);

	/* Try fallback. */
	if (shader == NULL)
		shader = lirnd_shader_new (self->render);
	if (shader == NULL)
		return NULL;

	/* Insert to dictionary. */
	shader->name = strdup (name);
	if (shader->name == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lirnd_shader_free (shader);
		return NULL;
	}
	if (!lialg_strdic_insert (self->shaders, name, shader))
	{
		lirnd_shader_free (shader);
		return NULL;
	}

	return shader;
}

/**
 * \brief Inserts an image to the resource list.
 *
 * \param self Resources.
 * \param name Image name.
 * \return Image or NULL.
 */
lirndImage*
lirnd_resources_insert_image (lirndResources* self,
                              const char*     name)
{
	lirndImage* image;

	image = calloc (1, sizeof (lirndImage));
	if (image == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	image->name = strdup (name);
	image->path = lisys_path_format (self->render->config.dir,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, name, ".dds", NULL);
	if (image->name == NULL || image->path == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (image->name);
		free (image);
		return NULL;
	}
	if (!lialg_strdic_insert (self->images, name, image))
	{
		free (image->name);
		free (image->path);
		free (image);
		return NULL;
	}

	return image;
}

int
lirnd_resources_get_image_count (lirndResources* self)
{
	return self->images->size;
}

int
lirnd_resources_get_shader_count (lirndResources* self)
{
	return self->shaders->size;
}

/** @} */
/** @} */
