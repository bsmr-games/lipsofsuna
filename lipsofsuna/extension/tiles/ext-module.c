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
 * \addtogroup LIExtVoxel Voxel
 * @{
 */

#include "ext-module.h"

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_tiles_info =
{
	LIMAI_EXTENSION_VERSION, "tiles",
	liext_tiles_new,
	liext_tiles_free
};

LIExtModule* liext_tiles_new (
	LIMaiProgram* program)
{
	LIExtModule* self;
	LISerServer* server;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Create voxel manager. */
	self->voxels = livox_manager_new (program->callbacks, program->sectors);
	if (self->voxels == NULL)
	{
		liext_tiles_free (self);
		return NULL;
	}

	/* Register component. */
	if (!limai_program_insert_component (program, "voxels", self->voxels))
	{
		liext_tiles_free (self);
		return NULL;
	}

	/* Set database for sector I/O if server. */
	/* FIXME: Scripts should take care of loading and saving sectors. */
	server = limai_program_find_component (program, "server");
	if (server != NULL)
		livox_manager_set_sql (self->voxels, server->sql);

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_tick, self, self->calls + 0))
	{
		liext_tiles_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Material", liext_script_material, self);
	liscr_script_create_class (program->script, "Tile", liext_script_tile, self);
	liscr_script_create_class (program->script, "Voxel", liext_script_voxel, self);

	return self;
}

void liext_tiles_free (
	LIExtModule* self)
{
	/* Unregister component. */
	if (self->voxels != NULL)
		limai_program_remove_component (self->program, "voxels");

	/* Free resources. */
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);

	lisys_free (self);
}

int liext_tiles_write (
	LIExtModule* self,
	LIArcSql*    sql)
{
	return livox_manager_write (self->voxels);
}

/**
 * \brief Gets the voxel manager of the module.
 *
 * This function is used by other modules, such as the NPC module, to interact
 * with the voxel terrain.
 *
 * \warning Accessing the terrain from a different thread isn't safe.
 *
 * \param self Module.
 * \return Voxel manager.
 */
LIVoxManager* liext_tiles_get_voxels (
	LIExtModule* self)
{
	return self->voxels;
}

int liext_tiles_set_materials (
	LIExtModule* self,
	LIArcReader* reader)
{
	uint8_t skip;

	/* Skip type. */
	if (!reader->pos && !liarc_reader_get_uint8 (reader, &skip))
		return 0;

	/* Read materials. */
	if (!livox_manager_read_materials (self->voxels, reader))
		return 0;

	return 1;
}

/*****************************************************************************/

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	livox_manager_mark_updates (self->voxels);
	livox_manager_update_marked (self->voxels);

	return 1;
}

/** @} */
/** @} */
