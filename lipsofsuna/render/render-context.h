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
 * \addtogroup LIRenContext Context
 * @{
 */

#ifndef __RENDER_CONTEXT_H__
#define __RENDER_CONTEXT_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/model.h>
#include "render-buffer.h"
#include "render-light.h"
#include "render-material.h"
#include "render-scene.h"
#include "render-texture.h"
#include "render-types.h"

struct _LIRenContext
{
	int compiled;
	int fixed;
	int shadows;
	LIRenRender* render;
	LIRenScene* scene;
	LIRenShader* shader;
	LIMatFrustum frustum;
	LIMatMatrix matrix;
	LIMatMatrix modelview;
	LIMatMatrix modelviewinverse;
	LIMatMatrix projection;
	struct
	{
		int count;
		LIRenLight** array;
	} lights;
	struct
	{
		int flags;
		float parameters[4];
		float shininess;
		float diffuse[4];
		float specular[4];
	} material;
	struct
	{
		int count;
		LIRenTexture* array;
	} textures;
};

void
liren_context_init (LIRenContext* self,
                    LIRenScene*   scene);

void
liren_context_bind (LIRenContext* self);

void
liren_context_render_array (LIRenContext* self,
                            LIRenBuffer*  vertex);

void
liren_context_render_indexed (LIRenContext* self,
                              LIRenBuffer*  vertex,
                              LIRenBuffer*  index);

void
liren_context_render (LIRenContext* self,
                      LIRenBuffer*  buffer);

void
liren_context_render_vbo_array (LIRenContext*      self,
                                const LIRenFormat* format,
                                GLuint             vertices,
                                int                vertex0,
                                int                vertex1);

void
liren_context_render_vtx_array (LIRenContext*      self,
                                const LIRenFormat* format,
                                const void*        vertices,
                                int                vertex0,
                                int                vertex1);

void
liren_context_render_vbo_indexed (LIRenContext*      self,
                                  const LIRenFormat* format,
                                  GLuint             vertices,
                                  GLuint             indices,
                                  int                index0,
                                  int                index1);

void
liren_context_render_vtx_indexed (LIRenContext*      self,
                                  const LIRenFormat* format,
                                  const void*        vertices,
                                  const void*        indices,
                                  int                index0,
                                  int                index1);

void
liren_context_unbind (LIRenContext* self);

void
liren_context_set_flags (LIRenContext* self,
                         int           value);

void
liren_context_set_frustum (LIRenContext*       self,
                           const LIMatFrustum* frustum);

void
liren_context_set_lights (LIRenContext* self,
                          LIRenLight**  value,
                          int           count);

void
liren_context_set_material (LIRenContext*        self,
                            const LIRenMaterial* value);

void
liren_context_set_matrix (LIRenContext*      self,
                          const LIMatMatrix* value);

void
liren_context_set_modelview (LIRenContext*      self,
                             const LIMatMatrix* value);

void
liren_context_set_projection (LIRenContext*      self,
                              const LIMatMatrix* value);

void
liren_context_set_shader (LIRenContext* self,
                          LIRenShader*  value);

void
liren_context_set_textures (LIRenContext* self,
                            LIRenTexture* value,
                            int           count);

#endif

/** @} */
/** @} */
