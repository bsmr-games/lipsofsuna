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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlBone Bone
 * @{
 */

#ifndef __MODEL_BONE_H__
#define __MODEL_BONE_H__

#include <archive/lips-archive.h>
#include <math/lips-math.h>
#include <string/lips-string.h>
#include "model-types.h"

struct _limdlBone
{
	limatVector length;
	limatVector tail;
};

int
limdl_bone_read (limdlNode* self,
                 liReader*  reader);

int
limdl_bone_write (const limdlNode* self,
                  liarcWriter*     writer);

void
limdl_bone_get_pose_head (const limdlNode* self,
                          limatVector*     head);

void
limdl_bone_get_pose_tail (const limdlNode* self,
                          limatVector*     tail);

#endif

/** @} */
/** @} */
