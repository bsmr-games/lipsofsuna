/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "module.h"

LIMaiExtensionInfo liext_terrain_info =
{
	LIMAI_EXTENSION_VERSION, "Terrain",
	liext_terrain_module_new,
	liext_terrain_module_free,
	liext_terrain_module_get_memstat
};

LIExtTerrainModule* liext_terrain_module_new (
	LIMaiProgram* program)
{
	LIExtTerrainModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtTerrainModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_TERRAIN, self);
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_TERRAIN_CHUNK, self);
	liext_script_terrain (program->script);
	liext_script_terrain_chunk (program->script);

	return self;
}

void liext_terrain_module_free (
	LIExtTerrainModule* self)
{
	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_terrain_module_get_memstat (
	LIExtTerrainModule* self,
	LIMaiMemstat*       stat)
{
/* TODO: memstat */
}

/** @} */
/** @} */
