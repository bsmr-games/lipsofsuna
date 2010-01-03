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
 * \addtogroup LIRenScene Scene
 * @{
 */

#ifndef __RENDER_SCENE_H__
#define __RENDER_SCENE_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/particle.h>
#include <lipsofsuna/system.h>
#include "render-deferred.h"
#include "render-light.h"
#include "render-lighting.h"
#include "render-object.h"
#include "render-types.h"

struct _LIRenScene
{
	LIAlgPtrdic* groups;
	LIAlgU32dic* objects;
	LIParManager* particles;
	LIRenRender* render;
	LIRenLighting* lighting;
	struct
	{
		LIRenObject* model;
	} sky;
};

LIRenScene*
liren_scene_new (LIRenRender* render);

void
liren_scene_free (LIRenScene* self);

LIRenObject*
liren_scene_find_object (LIRenScene* self,
                         int         id);

LIParPoint*
liren_scene_insert_particle (LIRenScene*        self,
                             const LIMatVector* position,
                             const LIMatVector* velocity);

int
liren_scene_pick (LIRenScene*     self,
                  LIMatMatrix*    modelview,
                  LIMatMatrix*    projection,
                  LIMatFrustum*   frustum,
                  const int*      viewport,
                  int             x,
                  int             y,
                  int             size,
                  LIRenSelection* result);

void
liren_scene_render (LIRenScene*    self,
                    LIRenDeferred* framebuffer,
                    LIMatMatrix*   modelview,
                    LIMatMatrix*   projection,
                    LIMatFrustum*  frustum);

void
liren_scene_update (LIRenScene* self,
                    float       secs);

int
liren_scene_set_sky (LIRenScene* self,
                     LIRenModel* model);

#endif

/** @} */
/** @} */
