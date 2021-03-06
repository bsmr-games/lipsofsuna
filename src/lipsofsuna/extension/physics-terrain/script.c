/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExtPhysicsTerrain PhysicsTerrain
 * @{
 */

#include "lipsofsuna/extension/terrain/module.h"
#include "module.h"

static void PhysicsTerrain_new (LIScrArgs* args)
{
	LIExtPhysicsTerrain* self;
	LIExtPhysicsTerrainModule* module;
	LIScrData* data;

	/* Get the arguments. */
	if (!liscr_args_geti_data (args, 0, LIEXT_SCRIPT_TERRAIN, &data))
		return;
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_TERRAIN);

	/* Allocate the object. */
	self = liext_physics_terrain_new (module, liscr_data_get_data (data), 0x0001, 0xFFFF);
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_PHYSICS_TERRAIN, liext_physics_terrain_free);
	if (data == NULL)
	{
		liext_physics_terrain_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void PhysicsTerrain_update (LIScrArgs* args)
{
	liext_physics_terrain_update (args->self, 0.0f);
}

static void PhysicsTerrain_set_collision_group (LIScrArgs* args)
{
	int value;

	if (!liscr_args_geti_int (args, 0, &value))
		return;

	liext_physics_terrain_set_collision_group (args->self, value);
}

static void PhysicsTerrain_set_collision_mask (LIScrArgs* args)
{
	int value;

	if (!liscr_args_geti_int (args, 0, &value))
		return;

	liext_physics_terrain_set_collision_mask (args->self, value);
}

static void PhysicsTerrain_set_friction (LIScrArgs* args)
{
	float value;

	if (!liscr_args_geti_float (args, 0, &value))
		return;
	value = LIMAT_MAX (0.0f, value);

	liext_physics_terrain_set_friction (args->self, value);
}

static void PhysicsTerrain_set_id (LIScrArgs* args)
{
	int value;

	if (!liscr_args_geti_int (args, 0, &value) || value <= 0)
		return;

	liext_physics_terrain_set_id (args->self, value);
}

static void PhysicsTerrain_set_unloaded_collision (LIScrArgs* args)
{
	int value;

	if (!liscr_args_geti_bool (args, 0, &value))
		return;

	liext_physics_terrain_set_unloaded_collision (args->self, value);
}

static void PhysicsTerrain_get_valid (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liext_physics_terrain_get_valid (args->self));
}

static void PhysicsTerrain_set_visible (LIScrArgs* args)
{
	int value;

	if (!liscr_args_geti_bool (args, 0, &value))
		return;

	liext_physics_terrain_set_visible (args->self, value);
}

/*****************************************************************************/

void liext_script_physics_terrain (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_new", PhysicsTerrain_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_update", PhysicsTerrain_update);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_set_collision_group", PhysicsTerrain_set_collision_group);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_set_collision_mask", PhysicsTerrain_set_collision_mask);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_set_friction", PhysicsTerrain_set_friction);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_set_id", PhysicsTerrain_set_id);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_set_unloaded_collision", PhysicsTerrain_set_unloaded_collision);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_get_valid", PhysicsTerrain_get_valid);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_TERRAIN, "physics_terrain_set_visible", PhysicsTerrain_set_visible);
}

/** @} */
/** @} */
