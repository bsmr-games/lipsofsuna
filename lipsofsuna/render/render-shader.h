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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenShader Shader
 * @{
 */

#ifndef __RENDER_SHADER_H__
#define __RENDER_SHADER_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/model.h>
#include "render-light.h"
#include "render-types.h"
#include "render-uniform.h"

struct _LIRenShader
{
	char* name;
	GLint program;
	GLint vertex;
	GLint fragment;
	LIRenRender* render;
	struct
	{
		int count;
	} lights;
	struct
	{
		int count;
		LIRenUniform* array;
	} uniforms;
};

LIRenShader*
liren_shader_new (LIRenRender* render);

LIRenShader*
liren_shader_new_from_data (LIRenRender* render,
                            LIArcReader* reader);

LIRenShader*
liren_shader_new_from_file (LIRenRender* render,
                            const char*  path);

void
liren_shader_free (LIRenShader* self);

#endif

/** @} */
/** @} */
