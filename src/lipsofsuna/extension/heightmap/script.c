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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtHeightmap Heightmap
 * @{
 */

#include "module.h"

static void Heightmap_new (LIScrArgs* args)
{
	int size;
	float spacing;
	LIExtHeightmap* heightmap;
	LIExtHeightmapModule* module;
	LIMatVector position;
	LIScrData* data;

	/* Get arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_HEIGHTMAP);
	if (!liscr_args_geti_vector (args, 0, &position))
		position = limat_vector_init (0.0f, 0.0f, 0.0f);
	if (!liscr_args_geti_int (args, 1, &size))
		size = 32;
	else if (size < 0)
		size = 0;
	if (!liscr_args_geti_float (args, 2, &spacing))
		spacing = 1.0f;
	else if (spacing <= 0.0f)
		spacing = 1.0f;

	/* Allocate the heightmap. */
	heightmap = liext_heightmap_new (module, &position, size, spacing);
	if (heightmap == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, heightmap, LIEXT_SCRIPT_HEIGHTMAP, liext_heightmap_free);
	if (data == NULL)
	{
		liext_heightmap_free (heightmap);
		return;
	}
	liscr_args_seti_stack (args);
}

/*****************************************************************************/

void liext_script_heightmap (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_HEIGHTMAP, "heightmap_new", Heightmap_new);
}

/** @} */
/** @} */
