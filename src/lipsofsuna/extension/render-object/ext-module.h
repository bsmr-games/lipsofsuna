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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/render.h"
#include "lipsofsuna/extension/animation/ext-module.h"
#include "lipsofsuna/extension/render-model/module.h"

#define LIEXT_SCRIPT_RENDER_OBJECT "RenderObject"

typedef struct _LIExtRenderObject LIExtRenderObject;
typedef struct _LIExtModule LIExtModule;

/*****************************************************************************/

struct _LIExtRenderObject
{
	LIExtModule* module;
	LIRenRender* render;
	LIMatTransform transform;
	int id;
};

LIExtRenderObject* liext_render_object_new (
	LIExtModule* module);

void liext_render_object_free (
	LIExtRenderObject* self);

/*****************************************************************************/

struct _LIExtModule
{
	LIAlgU32dic* objects;
	LIMaiProgram* program;
	LIRenRender* render;
};

LIExtModule* liext_object_render_new (
	LIMaiProgram* program);

void liext_object_render_free (
	LIExtModule* self);

void liext_object_render_get_memstat (
	LIExtModule*  self,
	LIMaiMemstat* stat);

/*****************************************************************************/

void liext_script_render_object (
	LIScrScript* self);

#endif
