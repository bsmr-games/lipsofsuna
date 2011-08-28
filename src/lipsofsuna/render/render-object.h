/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __RENDER_OBJECT_H__
#define __RENDER_OBJECT_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include "render.h"

LIAPICALL (int, liren_render_object_new, (
	LIRenRender* render,
	int          id));

LIAPICALL (void, liren_render_object_free, (
	LIRenRender* self,
	int          id));

LIAPICALL (void, liren_render_object_deform, (
	LIRenRender* self,
	int          id));

LIAPICALL (void, liren_render_object_particle_animation, (
	LIRenRender* self,
	int          id,
	float        start,
	int          loop));

LIAPICALL (int, liren_render_object_set_effect, (
	LIRenRender* self,
	int          id,
	const char*  shader,
	const float* params));

LIAPICALL (int, liren_render_object_set_model, (
	LIRenRender* self,
	int          id,
	int          model));

LIAPICALL (int, liren_render_object_set_pose, (
	LIRenRender* self,
	int          id,
	LIMdlPose*   pose));

LIAPICALL (int, liren_render_object_set_realized, (
	LIRenRender* self,
	int          id,
	int          value));

LIAPICALL (void, liren_render_object_set_transform, (
	LIRenRender*          self,
	int                   id,
	const LIMatTransform* value));

#endif
