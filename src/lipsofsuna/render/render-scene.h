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

#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/system.h>
#include "render-deferred.h"
#include "render-light.h"
#include "render-lighting.h"
#include "render-object.h"
#include "render-sort.h"
#include "render-types.h"

struct _LIRenScene
{
	float time;
	LIAlgPtrdic* groups;
	LIAlgU32dic* objects;
	LIRenRender* render;
	LIRenSort* sort;
	LIRenLighting* lighting;
	struct
	{
		int postproc_passes;
		int rendering;
		GLint original_viewport[4];
		LIRenContext* context;
		LIRenDeferred* framebuffer;
	} state;
	struct
	{
		LIRenObject* model;
	} sky;
};

LIAPICALL (LIRenScene*, liren_scene_new, (
	LIRenRender* render));

LIAPICALL (void, liren_scene_free, (
	LIRenScene* self));

LIAPICALL (LIRenObject*, liren_scene_find_object, (
	LIRenScene* self,
	int         id));

LIAPICALL (int, liren_scene_render_begin, (
	LIRenScene*    self,
	LIRenDeferred* framebuffer,
	LIMatMatrix*   modelview,
	LIMatMatrix*   projection,
	LIMatFrustum*  frustum));

LIAPICALL (void, liren_scene_render_end, (
	LIRenScene* self));

LIAPICALL (void, liren_scene_render_pass, (
	LIRenScene* self,
	int         pass,
	int         sorting));

LIAPICALL (void, liren_scene_render_postproc, (
	LIRenScene* self,
	const char* name,
	int         mipmaps));

LIAPICALL (void, liren_scene_update, (
	LIRenScene* self,
	float       secs));

#endif
