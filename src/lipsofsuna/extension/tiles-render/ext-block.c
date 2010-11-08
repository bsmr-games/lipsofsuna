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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTilesRender TilesRender
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

LIExtBlock* liext_tiles_render_block_new (
	LIExtModule* module)
{
	LIExtBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBlock));
	if (self == NULL)
		return NULL;
	self->module = module;

	return self;
}

void liext_tiles_render_block_free (
	LIExtBlock* self)
{
	if (self->object != NULL)
		liren_object_free (self->object);
	if (self->model != NULL)
		liren_model_free (self->model);
	lisys_free (self);
}

void liext_tiles_render_block_clear (
	LIExtBlock* self)
{
	if (self->object != NULL)
	{
		liren_object_free (self->object);
		self->object = NULL;
	}
	if (self->model != NULL)
	{
		liren_model_free (self->model);
		self->model = NULL;
	}
}

int liext_tiles_render_block_build (
	LIExtBlock*     self,
	LIVoxBlockAddr* addr)
{
	LIMdlModel* model = NULL;

	/* Free old objects. */
	liext_tiles_render_block_clear (self);

	/* Build new objects. */
	if (!livox_build_block (self->module->voxels, self->module->program->engine, NULL, addr, &model, NULL))
		return 0;

	/* Create render model if not empty. */
	if (model != NULL)
	{
		self->model = liren_model_new (self->module->client->render, model, 0);
		limdl_model_free (model);
		if (self->model != NULL)
		{
			self->object = liren_object_new (self->module->client->scene, 0);
			if (self->object != NULL)
				liren_object_set_model (self->object, self->model);
		}
	}

	/* Realize if not empty. */
	if (self->object != NULL)
		liren_object_set_realized (self->object, 1);

	return 1;
}

/** @} */
/** @} */
