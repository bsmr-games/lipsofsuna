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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/extension.h>

#define LIEXT_SCRIPT_LIGHT "Light"
#define LIEXT_SCRIPT_RENDER "Render"
#define LIEXT_SCRIPT_SHADER "Shader"

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LICliClient* client;
	LIMaiProgram* program;
	LIRenRender* render;
};

LIExtModule* liext_render_new (
	LIMaiProgram* program);

void liext_render_free (
	LIExtModule* self);

/*****************************************************************************/

typedef struct _LIExtLight LIExtLight;
struct _LIExtLight
{
	LIExtModule* module;
	int id;
};

/*****************************************************************************/

void liext_script_light (
	LIScrScript* self);

void liext_script_render (
	LIScrScript* self);

void liext_script_shader (
	LIScrScript* self);

#endif
