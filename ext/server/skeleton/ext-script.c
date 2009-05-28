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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvSkeleton Skeleton
 * @{
 */

#include <script/lips-script.h>
#include <server/lips-server.h>
#include "ext-skeleton.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Skeleton"
 * ---
 * -- FIXME
 * -- @name Skeleton
 * -- @class table
 */

/* @luadoc
 * ---
 * -- FIXME
 * --
 * -- @param self Skeleton class.
 * Skeleton.test(self)
 */
static int
Skeleton_test (lua_State* lua)
{
	liextModule* module;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_SKELETON);

	printf ("Skeleton.test\n");

	return 0;
}

/*****************************************************************************/

void
liextSkeletonScript (liscrClass* self,
                     void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SKELETON, data);
	liscr_class_insert_func (self, "test", Skeleton_test);
}

/** @} */
/** @} */
/** @} */
