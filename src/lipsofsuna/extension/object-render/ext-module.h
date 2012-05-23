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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/render.h"
#include "lipsofsuna/extension/animation/ext-module.h"

#define LIEXT_SCRIPT_RENDER_OBJECT "RenderObject"

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LICalHandle calls[9];
	LIMaiProgram* program;
	LIRenRender* render;
};

LIExtModule* liext_object_render_new (
	LIMaiProgram* program);

void liext_object_render_free (
	LIExtModule* self);

/*****************************************************************************/

void liext_script_render_object (
	LIScrScript* self);

#endif
