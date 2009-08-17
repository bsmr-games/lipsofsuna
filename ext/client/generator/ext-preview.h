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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#ifndef __EXT_PREVIEW_H__
#define __EXT_PREVIEW_H__

#include <client/lips-client.h>
#include <generator/lips-generator.h>
#include <engine/lips-engine.h>
#include <widget/lips-widget.h>

#define LIEXT_PREVIEW(o) ((liextPreview*)(o))

#define LIEXT_CALLBACK_PRESSED 100
#define LIEXT_CALLBACK_TRANSFORM 101

enum
{
	LIEXT_PREVIEW_COLOR_VOXEL,
	LIEXT_PREVIEW_ERASE_POINT,
	LIEXT_PREVIEW_ERASE_VOXEL,
	LIEXT_PREVIEW_ERASE_SPHERE,
	LIEXT_PREVIEW_PAINT_POINT,
	LIEXT_PREVIEW_PAINT_VOXEL,
	LIEXT_PREVIEW_PAINT_SPHERE
};

typedef struct _liextPreview liextPreview;
struct _liextPreview
{
	liwdgRender base;
	lialgPtrdic* objects;
	licliModule* module;
	ligenGenerator* generator;
	liengCamera* camera;
	limatVector drag;
	limatTransform transform;
	lirndLight* light0;
	lirndLight* light1;
	lirndRender* render;
	lirndScene* scene;
	int mode;
};

extern const liwdgClass liextPreviewType;

liwdgWidget*
liext_preview_new (liwdgManager* manager,
                   licliModule*  module);

void
liext_preview_build (liextPreview* self);

int
liext_preview_build_box (liextPreview* self,
                         int           xs,
                         int           ys,
                         int           zs,
                         int           material);

int
liext_preview_clear (liextPreview* self);

void
liext_preview_copy_voxels (liextPreview* self,
                           int           sx,
                           int           sy,
                           int           sz,
                           livoxVoxel*   result);

int
liext_preview_insert_object (liextPreview*         self,
                             const limatTransform* transform,
                             const char*           model);

int
liext_preview_insert_stroke (liextPreview* self,
                             int           x,
                             int           y,
                             int           z,
                             int           brush);

void
liext_preview_paint_terrain (liextPreview* self,
                             limatVector*  point,
                             int           mode,
                             int           material,
                             float         radius);

int
liext_preview_replace_materials (liextPreview* self,
                                 livoxManager* voxels);

void
liext_preview_get_transform (liextPreview*   self,
                             limatTransform* value);

#endif

/** @} */
/** @} */
/** @} */
