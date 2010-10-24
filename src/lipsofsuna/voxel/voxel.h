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

#ifndef __VOXEL_H__
#define __VOXEL_H__

#include <lipsofsuna/archive.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/system.h>
#include "voxel-types.h"

LIAPICALL (void, livox_voxel_init, (
	LIVoxVoxel* self,
	int         type));

LIAPICALL (int, livox_voxel_read, (
	LIVoxVoxel*  self,
	LIArcReader* reader));

LIAPICALL (void, livox_voxel_rotate, (
	LIVoxVoxel* self,
	int         axis,
	int         step));

LIAPICALL (int, livox_voxel_write, (
	LIVoxVoxel*  self,
	LIArcWriter* writer));

LIAPICALL (float, livox_voxel_get_height, (
	const LIVoxVoxel* self));

LIAPICALL (int, livox_voxel_get_type, (
	const LIVoxVoxel* self));

LIAPICALL (void, livox_voxel_get_quaternion, (
	const LIVoxVoxel* self,
	LIMatQuaternion*  value));

#endif
