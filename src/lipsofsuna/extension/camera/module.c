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
 * \addtogroup LIExtCamera Camera
 * @{
 */

#include "lipsofsuna/extension/physics/ext-module.h"
#include "module.h"

LIMaiExtensionInfo liext_camera_info =
{
	LIMAI_EXTENSION_VERSION, "Camera",
	liext_camera_module_new,
	liext_camera_module_free,
	liext_camera_module_get_memstat
};

LIExtCameraModule* liext_camera_module_new (
	LIMaiProgram* program)
{
	LIExtCameraModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtCameraModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_CAMERA, self);
	liext_script_camera (program->script);

	return self;
}

void liext_camera_module_free (
	LIExtCameraModule* self)
{
	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_camera_module_get_memstat (
	LIExtCameraModule* self,
	LIMaiMemstat*      stat)
{
/* TODO: memstat */
}

/** @} */
/** @} */
