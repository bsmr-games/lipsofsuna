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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxVoxel Voxel
 * @{
 */

#ifndef __VOXEL_H__
#define __VOXEL_H__

#include <math/lips-math.h>
#include "voxel-types.h"

void
livox_voxel_init (livoxVoxel* self,
                  int         type);

void
livox_voxel_rotate (livoxVoxel* self,
                    int         axis,
                    int         step);

int
livox_voxel_get_type (const livoxVoxel* self);

void
livox_voxel_get_quaternion (const livoxVoxel* self,
                            limatQuaternion*  value);

#endif

/** @} */
/** @} */
