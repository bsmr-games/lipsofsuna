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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup LIVoxMaterial Material
 * @{
 */

#ifndef __VOXEL_MATERIAL_H__
#define __VOXEL_MATERIAL_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/system.h>
#include "voxel-types.h"

#define LIVOX_MATERIAL_FORMAT 0

enum
{
	LIVOX_MATERIAL_FLAG_OCCLUDER = 0x01
};

enum
{
	LIVOX_MATERIAL_TYPE_CORNER,
	LIVOX_MATERIAL_TYPE_HEIGHT,
	LIVOX_MATERIAL_TYPE_TILE,
	LIVOX_MATERIAL_TYPE_MAX
};

struct _LIVoxMaterial
{
	int id;
	int flags;
	int type;
	char* name;
	float friction;
	LIEngModel* model;
	LIMdlMaterial mat_side;
	LIMdlMaterial mat_top;
};

LIAPICALL (LIVoxMaterial*, livox_material_new, ());

LIAPICALL (LIVoxMaterial*, livox_material_new_copy, (
	const LIVoxMaterial* src));

LIAPICALL (void, livox_material_free, (
	LIVoxMaterial* self));

LIAPICALL (int, livox_material_set_name, (
	LIVoxMaterial* self,
	const char*    value));

LIAPICALL (int, livox_material_set_model, (
	LIVoxMaterial* self,
	LIEngModel*    model));

#endif

/** @} */
/** @} */
