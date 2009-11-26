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

#include "ext-preview.h"

#define LIEXT_PREVIEW_CENTER 8160
#define LIEXT_PREVIEW_PAN 0.3f

enum
{
	LIEXT_PREVIEW_MODE_CAMERA,
	LIEXT_PREVIEW_MODE_ROTATE,
	LIEXT_PREVIEW_MODE_ROTATEX,
	LIEXT_PREVIEW_MODE_ROTATEY,
	LIEXT_PREVIEW_MODE_ROTATEZ,
	LIEXT_PREVIEW_MODE_TRANSLATE,
	LIEXT_PREVIEW_MODE_TRANSLATEX,
	LIEXT_PREVIEW_MODE_TRANSLATEY,
	LIEXT_PREVIEW_MODE_TRANSLATEZ
};

static const void*
private_base ();

static int
private_init (liextPreview* self,
              liwdgManager* manager);

static void
private_free (liextPreview* self);

static int
private_event (liextPreview* self,
               liwdgEvent*   event);

static int
private_block_build (liextPreview* self,
                     livoxBlock*   block,
                     limatVector*  offset);

static int
private_block_free (liextPreview*     self,
                    livoxUpdateEvent* event);

static int
private_block_load (liextPreview*     self,
                    livoxUpdateEvent* event);

static void
private_motion (liextPreview* self,
                liwdgEvent*   event);

static void
private_render_preview (liwdgWidget*  widget,
                        liextPreview* self);

/****************************************************************************/

const liwdgClass liextPreviewType =
{
	LIWDG_BASE_DYNAMIC, private_base, "GeneratorPreview", sizeof (liextPreview),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

liwdgWidget*
liext_preview_new (liwdgManager* manager,
                   licliModule*  module)
{
	const float diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float equation[3] = { 1.0f, 0.0f, 0.001f };
	liextPreview* data;
	limatTransform transform;
	liwdgWidget* self;

	/* Allocate self. */
	self = liwdg_widget_new (manager, &liextPreviewType);
	if (self == NULL)
		return NULL;
	data = LIEXT_PREVIEW (self);
	data->module = module;
	data->render = module->render;

	/* Allocate scene. */
	data->scene = lirnd_scene_new (data->render);
	if (data->scene == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	LIWDG_RENDER (self)->scene = data->scene;

	/* Allocate group. */
	data->group = lirnd_group_new (data->scene);
	if (data->group == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	lirnd_group_set_realized (data->group, 1);

	/* Allocate objects. */
	data->objects = lialg_ptrdic_new ();
	if (data->objects == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}

	/* Allocate generator. */
	data->generator = ligen_generator_new (module->paths);
	if (data->generator == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	ligen_generator_set_fill (data->generator, -1);
	livox_manager_set_sql (data->generator->voxels, NULL);

	/* Register voxel callbacks. */
	if (!lical_callbacks_insert_callback (data->generator->voxels->callbacks,
	     	LIVOX_CALLBACK_FREE_BLOCK, 1,
	     	private_block_free, self, data->calls + 0) ||
	    !lical_callbacks_insert_callback (data->generator->voxels->callbacks,
	     	LIVOX_CALLBACK_LOAD_BLOCK, 1,
	     	private_block_load, self, data->calls + 1))
	{
		liwdg_widget_free (self);
		return NULL;
	}

	/* Create camera. */
	data->camera = lieng_camera_new (module->engine);
	if (data->camera == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	transform.position = limat_vector_init (
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f)/* - 15*/,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + 55,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f)/* - 30.0f*/);
	transform.rotation = limat_quaternion_look (
		limat_vector_init (0.0f, -1.0f, 0.0f),
		limat_vector_init (0.0f, 0.0f, 1.0f));
	lieng_camera_set_clip (data->camera, 0);
	lieng_camera_set_driver (data->camera, LIENG_CAMERA_DRIVER_MANUAL);
	lieng_camera_set_transform (data->camera, &transform);
	lieng_camera_warp (data->camera);

	/* Create lights. */
	data->light0 = lirnd_light_new (data->scene, diffuse, equation, M_PI, 0.0f, 0);
	data->light1 = lirnd_light_new (data->scene, diffuse, equation, M_PI, 0.0f, 0);
	if (data->light0 == NULL || data->light1 == NULL)
	{
		liwdg_widget_free (self);
		return NULL;
	}
	lirnd_lighting_insert_light (data->scene->lighting, data->light0);
	lirnd_lighting_insert_light (data->scene->lighting, data->light1);

	return self;
}

void
liext_preview_build (liextPreview* self)
{
	ligen_generator_rebuild_scene (self->generator);
}

int
liext_preview_build_tile (liextPreview* self,
                          int           material)
{
	livoxVoxel voxel;

	livox_voxel_init (&voxel, material);
	livox_manager_clear (self->generator->voxels);
	livox_manager_set_voxel (self->generator->voxels,
		LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, &voxel);
	livox_manager_update (self->generator->voxels, 1.0f);

	return 1;
}

int
liext_preview_clear (liextPreview* self)
{
	lialgPtrdicIter iter;

	lirnd_group_clear (self->group);
	ligen_generator_clear_scene (self->generator);
	LI_FOREACH_PTRDIC (iter, self->objects)
		lirnd_object_free (iter.value);
	lialg_ptrdic_clear (self->objects);

	return 1;
}

void
liext_preview_copy_voxels (liextPreview* self,
                           int           sx,
                           int           sy,
                           int           sz,
                           livoxVoxel*   result)
{
	livox_manager_copy_voxels (self->generator->voxels,
		LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER,
		sx, sy, sz, result);
}

/**
 * \brief Inserts an object to the preview widget.
 *
 * \param self Preview.
 * \param transform Transformation.
 * \param model Model name.
 * \return Nonzero on success.
 */
int
liext_preview_insert_object (liextPreview*         self,
                             const limatTransform* transform,
                             const char*           model)
{
	limatTransform t;
	liengModel* emodel;
	lirndModel* rmodel;
	lirndObject* object;

	/* Find model. */
	emodel = lieng_engine_find_model_by_name (self->module->engine, model);
	rmodel = lirnd_render_find_model (self->module->render, model);
	if (rmodel == NULL)
		return 0;

	/* Create object. */
	object = lirnd_object_new (self->scene, self->objects->size);
	if (object == NULL)
		return 0;
	t = limat_convert_vector_to_transform (limat_vector_init (
		LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER));
	t = limat_transform_multiply (t, *transform);
	lirnd_object_set_transform (object, &t);
	lirnd_object_set_model (object, rmodel);
	lirnd_object_set_realized (object, 1);

	/* Add to dictionary. */
	if (!lialg_ptrdic_insert (self->objects, object, object))
	{
		lirnd_object_free (object);
		return 0;
	}

	return 1;
}

int
liext_preview_insert_stroke (liextPreview* self,
                             int           x,
                             int           y,
                             int           z,
                             int           brush)
{
	return ligen_generator_insert_stroke (self->generator, brush,
		LIEXT_PREVIEW_CENTER + x,
		LIEXT_PREVIEW_CENTER + y,
		LIEXT_PREVIEW_CENTER + z);
}

void
liext_preview_paint_terrain (liextPreview* self,
                             limatVector*  point,
                             int           mode,
                             int           material,
                             int           axis)
{
	livoxVoxel voxel;

	switch (mode)
	{
		case LIEXT_PREVIEW_ERASE_VOXEL:
			livox_manager_erase_voxel (self->generator->voxels, point);
			break;
		case LIEXT_PREVIEW_INSERT_VOXEL:
			livox_voxel_init (&voxel, material);
			livox_manager_insert_voxel (self->generator->voxels, point, &voxel);
			break;
		case LIEXT_PREVIEW_REPLACE_VOXEL:
			livox_voxel_init (&voxel, material);
			livox_manager_replace_voxel (self->generator->voxels, point, &voxel);
			break;
		case LIEXT_PREVIEW_ROTATE_VOXEL:
			livox_manager_rotate_voxel (self->generator->voxels, point, axis, 1);
			break;
	}
	livox_manager_update (self->generator->voxels, 1.0f);
}

/**
 * \brief Replaces the materials of the preview with those of the passed voxel manager.
 *
 * \param self Preview.
 * \param reader Stream reader.
 * \return Nonzero on success.
 */
int
liext_preview_replace_materials (liextPreview* self,
                                 liarcReader*  reader)
{
	livoxMaterial* material;

	livox_manager_clear_materials (self->generator->voxels);
	while (!liarc_reader_check_end (reader))
	{
		material = livox_material_new_from_stream (reader);
		if (material == NULL)
			return 0;
		if (!livox_manager_insert_material (self->generator->voxels, material))
		{
			livox_material_free (material);
			return 0;
		}
	}

	return 1;
}

void
liext_preview_setup_camera (liextPreview* self,
                            limatVector*  eye,
                            limatVector*  ctr,
                            limatVector*  up,
                            int           driver)
{
	limatTransform transform0;
	limatTransform transform1;

	transform0.rotation = limat_quaternion_identity ();
	transform0.position = limat_vector_init (
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + eye->x,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + eye->y,
		LIVOX_TILE_WIDTH * (LIEXT_PREVIEW_CENTER + 0.5f) + eye->z);
	transform1.rotation = limat_quaternion_look (limat_vector_subtract (*ctr, *eye), *up);
	transform1.position = transform0.position;
	lieng_camera_set_clip (self->camera, 0);
	lieng_camera_set_center (self->camera, &transform0);
	lieng_camera_set_driver (self->camera, driver);
	lieng_camera_set_transform (self->camera, &transform1);
	lieng_camera_warp (self->camera);
}

void
liext_preview_get_bounds (liextPreview* self,
                          limatAabb*    aabb)
{
	int i;
	int x[2];
	int y[2];
	int z[2];
	ligenStroke* stroke;

	aabb->min = limat_vector_init (LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER, LIEXT_PREVIEW_CENTER);
	aabb->min = limat_vector_multiply (aabb->min, LIVOX_TILE_WIDTH);
	aabb->max = aabb->min;
	for (i = 0 ; i < self->generator->strokes.count ; i++)
	{
		stroke = self->generator->strokes.array + i;
		x[0] = LIVOX_TILE_WIDTH * (stroke->pos[0]);
		x[1] = LIVOX_TILE_WIDTH * (stroke->pos[0] + stroke->size[0]);
		y[0] = LIVOX_TILE_WIDTH * (stroke->pos[1]);
		y[1] = LIVOX_TILE_WIDTH * (stroke->pos[1] + stroke->size[1]);
		z[0] = LIVOX_TILE_WIDTH * (stroke->pos[2]);
		z[1] = LIVOX_TILE_WIDTH * (stroke->pos[2] + stroke->size[2]);
		if (aabb->min.x > x[0]) aabb->min.x = x[0];
		if (aabb->min.y > y[0]) aabb->min.y = y[0];
		if (aabb->min.z > z[0]) aabb->min.z = z[0];
		if (aabb->max.x < x[1]) aabb->max.x = x[1];
		if (aabb->max.y < y[1]) aabb->max.y = y[1];
		if (aabb->max.z < z[1]) aabb->max.z = z[1];
	}
}

void
liext_preview_get_transform (liextPreview*   self,
                             limatTransform* value)
{
	if (self->mode != LIEXT_PREVIEW_MODE_CAMERA)
		*value = self->transform;
	else
		*value = limat_transform_identity ();
}

/****************************************************************************/

static const void*
private_base ()
{
	return &liwdgRenderType;
}

static int
private_init (liextPreview* self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIEXT_CALLBACK_PRESSED, lical_marshal_DATA_PTR_INT) ||
	    !liwdg_widget_register_callback (LIWDG_WIDGET (self), LIEXT_CALLBACK_TRANSFORM, lical_marshal_DATA_INT_INT))
		return 0;
	LIWDG_RENDER (self)->custom_render_func = (void*) private_render_preview;
	LIWDG_RENDER (self)->custom_render_data = self;
	return 1;
}

static void
private_free (liextPreview* self)
{
	lialgPtrdicIter iter;

	if (self->light0 != NULL)
	{
		lirnd_lighting_remove_light (self->scene->lighting, self->light0);
		lirnd_light_free (self->light0);
	}
	if (self->light1 != NULL)
	{
		lirnd_lighting_remove_light (self->scene->lighting, self->light1);
		lirnd_light_free (self->light1);
	}
	if (self->objects != NULL)
	{
		LI_FOREACH_PTRDIC (iter, self->objects)
			lirnd_object_free (iter.value);
		lialg_ptrdic_free (self->objects);
	}
	if (self->group != NULL)
		lirnd_group_free (self->group);
	if (self->camera != NULL)
		lieng_camera_free (self->camera);
	if (self->generator != NULL)
	{
		lical_callbacks_remove_callbacks (self->generator->voxels->callbacks,
			self->calls, sizeof (self->calls) / sizeof (licalHandle));
		ligen_generator_free (self->generator);
	}
	if (self->scene != NULL)
		lirnd_scene_free (self->scene);
}

static int
private_event (liextPreview* self,
               liwdgEvent*   event)
{
	limatMatrix modelview;
	limatMatrix projection;
	limatTransform transform;
	liwdgRect rect;

	if (event->type == LIWDG_EVENT_TYPE_BUTTON_PRESS)
	{
		if (liwdg_widget_get_grab (LIWDG_WIDGET (self)))
		{
			switch (event->button.button)
			{
				case 1:
					if (self->mode != LIEXT_PREVIEW_MODE_CAMERA)
					{
						self->mode = LIEXT_PREVIEW_MODE_CAMERA;
						lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
							LIEXT_CALLBACK_TRANSFORM, &self->transform, 1);
					}
					break;
				case 3:
					if (self->mode != LIEXT_PREVIEW_MODE_CAMERA)
					{
						self->transform = limat_transform_identity ();
						lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
							LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
						self->mode = LIEXT_PREVIEW_MODE_CAMERA;
					}
					break;
				case 4:
					lieng_camera_move (self->camera, 5.0f);
					break;
				case 5:
					lieng_camera_move (self->camera, -5.0f);
					break;
			}
		}
		else
		{
			switch (event->button.button)
			{
				case 1:
					liwdg_widget_set_grab (LIWDG_WIDGET (self), 1);
					break;
				case 3:
					if (!lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
					    LIEXT_CALLBACK_PRESSED, event->button.x, event->button.y))
						return 0;
					break;
			}
		}
		return 0;
	}
	if (event->type == LIWDG_EVENT_TYPE_MOTION)
	{
		if (liwdg_widget_get_grab (LIWDG_WIDGET (self)))
			private_motion (self, event);
	}
	if (event->type == LIWDG_EVENT_TYPE_KEY_PRESS)
	{
		switch (event->key.keycode)
		{
			case SDLK_g:
				self->drag = limat_vector_init (0.0f, 0.0f, 0.0f);
				self->mode = LIEXT_PREVIEW_MODE_TRANSLATE;
				self->transform = limat_transform_identity ();
				lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
					LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
				break;
			case SDLK_r:
				self->drag = limat_vector_init (0.0f, 0.0f, 0.0f);
				self->mode = LIEXT_PREVIEW_MODE_ROTATE;
				self->transform = limat_transform_identity ();
				lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
					LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
				break;
			case SDLK_x:
				if (self->mode >= LIEXT_PREVIEW_MODE_TRANSLATE &&
				    self->mode <= LIEXT_PREVIEW_MODE_TRANSLATEZ)
					self->mode = LIEXT_PREVIEW_MODE_TRANSLATEX;
				if (self->mode >= LIEXT_PREVIEW_MODE_ROTATE &&
				    self->mode <= LIEXT_PREVIEW_MODE_ROTATEZ)
					self->mode = LIEXT_PREVIEW_MODE_ROTATEX;
				break;
			case SDLK_y:
				if (self->mode >= LIEXT_PREVIEW_MODE_TRANSLATE &&
				    self->mode <= LIEXT_PREVIEW_MODE_TRANSLATEZ)
					self->mode = LIEXT_PREVIEW_MODE_TRANSLATEY;
				if (self->mode >= LIEXT_PREVIEW_MODE_ROTATE &&
				    self->mode <= LIEXT_PREVIEW_MODE_ROTATEZ)
					self->mode = LIEXT_PREVIEW_MODE_ROTATEY;
				break;
			case SDLK_z:
				if (self->mode >= LIEXT_PREVIEW_MODE_TRANSLATE &&
				    self->mode <= LIEXT_PREVIEW_MODE_TRANSLATEZ)
					self->mode = LIEXT_PREVIEW_MODE_TRANSLATEZ;
				if (self->mode >= LIEXT_PREVIEW_MODE_ROTATE &&
				    self->mode <= LIEXT_PREVIEW_MODE_ROTATEZ)
					self->mode = LIEXT_PREVIEW_MODE_ROTATEZ;
				break;
		}
	}
	if (event->type == LIWDG_EVENT_TYPE_UPDATE)
	{
		/* Update camera. */
		liwdg_widget_get_allocation (LIWDG_WIDGET (self), &rect);
		lieng_camera_set_viewport (self->camera, rect.x, rect.y, rect.width, rect.height);
		lieng_camera_update (self->camera, event->update.secs);
		lieng_camera_get_modelview (self->camera, &modelview);
		lieng_camera_get_projection (self->camera, &projection);

		/* Update scene. */
		liwdg_render_set_modelview (LIWDG_RENDER (self), &modelview);
		liwdg_render_set_projection (LIWDG_RENDER (self), &projection);
		lirnd_scene_update (self->scene, event->update.secs);

		/* Setup lights. */
		lieng_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (9, 6, -1));
		lirnd_light_set_transform (self->light0, &transform);
		lieng_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (-4, 2, 0));
		lirnd_light_set_transform (self->light1, &transform);
	}

	return liwdgRenderType.event (LIWDG_WIDGET (self), event);
}

static int
private_block_build (liextPreview* self,
                     livoxBlock*   block,
                     limatVector*  offset)
{
	int x;
	int y;
	int z;
	limatTransform transform;
	limatVector vector;
	lirndModel* model;
	livoxMaterial* material;
	livoxVoxel* voxel;

	/* Create new objects. */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++)
	{
		/* Type check. */
		voxel = livox_block_get_voxel (block, x, y, z);
		if (!voxel->type)
			continue;

		/* Get render model. */
		material = livox_manager_find_material (self->generator->voxels, voxel->type);
		if (material == NULL)
			continue;
		lieng_engine_find_model_by_name (self->module->engine, material->model);
		model = lirnd_render_find_model (self->module->render, material->model);
		if (model == NULL)
			continue;

		/* Add to render list. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		vector = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		transform.position = limat_vector_add (vector, *offset);
		livox_voxel_get_quaternion (voxel, &transform.rotation);
		lirnd_group_insert_model (self->group, model, &transform);
	}

	return 1;
}

static int
private_block_free (liextPreview*     self,
                    livoxUpdateEvent* event)
{
	return 1;
}

static int
private_block_load (liextPreview*     self,
                    livoxUpdateEvent* event)
{
	limatVector offset;
	limatVector vector;
	livoxBlock* vblock;
	livoxSector* vsector;

	/* Find sector. */
	vsector = livox_manager_find_sector (self->generator->voxels, LIVOX_SECTOR_INDEX (
		event->sector[0], event->sector[1], event->sector[2]));
	if (vsector == NULL)
		return 1;

	/* Find block. */
	vblock = livox_sector_get_block (vsector, LIVOX_BLOCK_INDEX (
		event->block[0], event->block[1], event->block[2]));

	/* Build block. */
	vector = limat_vector_init (event->sector[0], event->sector[1], event->sector[2]);
	vector = limat_vector_multiply (vector, LIVOX_SECTOR_WIDTH);
	offset = limat_vector_init (event->block[0], event->block[1], event->block[2]);
	offset = limat_vector_multiply (offset, LIVOX_BLOCK_WIDTH);
	offset = limat_vector_add (offset, vector);
	private_block_build (self, vblock, &offset);

	return 1;
}

static void
private_motion (liextPreview* self,
                liwdgEvent*   event)
{
	float amount;
	limatVector vx;
	limatVector vy;
	limatVector axis;
	limatVector delta;
	limatQuaternion quat;
	limatTransform transform;
	limatTransform transform1;

	self->drag.x += event->motion.dx;
	self->drag.y += event->motion.dy;
	switch (self->mode)
	{
		case LIEXT_PREVIEW_MODE_TRANSLATE:
			lieng_camera_get_transform (self->camera, &transform);
			vx = limat_quaternion_transform (transform.rotation, limat_vector_init (1.0f, 0.0f, 0.0f));
			vy = limat_quaternion_transform (transform.rotation, limat_vector_init (0.0f, 1.0f, 0.0f));
			vx = limat_vector_multiply (vx, 0.05f * self->drag.x);
			vy = limat_vector_multiply (vy, 0.05f * self->drag.y);
			delta = limat_vector_add (vx, vy);
			self->transform = limat_transform_init (delta, limat_quaternion_identity ());
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
		case LIEXT_PREVIEW_MODE_TRANSLATEX:
			amount = 0.05f * self->drag.x;
			delta = limat_vector_init (amount, 0.0f, 0.0f);
			self->transform = limat_convert_vector_to_transform (delta);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
		case LIEXT_PREVIEW_MODE_TRANSLATEY:
			amount = 0.05f * self->drag.x;
			delta = limat_vector_init (0.0f, amount, 0.0f);
			self->transform = limat_convert_vector_to_transform (delta);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
		case LIEXT_PREVIEW_MODE_TRANSLATEZ:
			amount = 0.05f * self->drag.x;
			delta = limat_vector_init (0.0f, 0.0f, amount);
			self->transform = limat_convert_vector_to_transform (delta);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
		case LIEXT_PREVIEW_MODE_ROTATE:
			lieng_camera_get_transform (self->camera, &transform);
			axis = limat_quaternion_transform (transform.rotation, limat_vector_init (0.0f, 0.0f, 1.0f));
			amount = atan2 (self->drag.y, self->drag.x);
			quat = limat_quaternion_rotation (amount, axis);
			self->transform = limat_convert_quaternion_to_transform (quat);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
		case LIEXT_PREVIEW_MODE_ROTATEX:
			amount = 0.02f * self->drag.x;
			quat = limat_quaternion_rotation (amount, limat_vector_init (1.0f, 0.0f, 0.0f));
			self->transform = limat_convert_quaternion_to_transform (quat);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
		case LIEXT_PREVIEW_MODE_ROTATEY:
			amount = 0.02f * self->drag.x;
			quat = limat_quaternion_rotation (amount, limat_vector_init (0.0f, 1.0f, 0.0f));
			self->transform = limat_convert_quaternion_to_transform (quat);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
		case LIEXT_PREVIEW_MODE_ROTATEZ:
			amount = 0.02f * self->drag.x;
			quat = limat_quaternion_rotation (amount, limat_vector_init (0.0f, 0.0f, 1.0f));
			self->transform = limat_convert_quaternion_to_transform (quat);
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks,
				LIEXT_CALLBACK_TRANSFORM, &self->transform, 0);
			return;
	}

	if (event->motion.buttons & 0x02)
	{
		lieng_camera_get_transform (self->camera, &transform);
		vx = limat_quaternion_transform (transform.rotation, limat_vector_init (1.0f, 0.0f, 0.0f));
		vy = limat_quaternion_transform (transform.rotation, limat_vector_init (0.0f, 1.0f, 0.0f));
		vx = limat_vector_multiply (vx, LIEXT_PREVIEW_PAN * event->motion.dx);
		vy = limat_vector_multiply (vy, LIEXT_PREVIEW_PAN * event->motion.dy);
		transform1 = limat_convert_vector_to_transform (limat_vector_add (vx, vy));
		transform1 = limat_transform_multiply (transform1, transform);
		lieng_camera_set_transform (self->camera, &transform1);
	}
	else
	{
		lieng_camera_turn (self->camera, -0.01 * event->motion.dx);
		lieng_camera_tilt (self->camera, 0.01 * event->motion.dy);
	}
}

static void
private_render_preview (liwdgWidget*  widget,
                        liextPreview* self)
{
	int i;
	int x[2];
	int y[2];
	int z[2];
	ligenStroke* stroke;

	/* Render stroke bounds. */
	glDisable (GL_DEPTH_TEST);
	glDisable (GL_LIGHTING);
	glDisable (GL_TEXTURE_2D);
	glBegin (GL_LINES);
	for (i = 0 ; i < self->generator->strokes.count ; i++)
	{
		stroke = self->generator->strokes.array + i;
		if (i == self->generator->strokes.count - 1)
			glColor3f (1.0f, 0.0f, 0.0f);
		else
			glColor3f (0.0f, 1.0f, 0.0f);
		x[0] = LIVOX_TILE_WIDTH * (stroke->pos[0]);
		x[1] = LIVOX_TILE_WIDTH * (stroke->pos[0] + stroke->size[0]);
		y[0] = LIVOX_TILE_WIDTH * (stroke->pos[1]);
		y[1] = LIVOX_TILE_WIDTH * (stroke->pos[1] + stroke->size[1]);
		z[0] = LIVOX_TILE_WIDTH * (stroke->pos[2]);
		z[1] = LIVOX_TILE_WIDTH * (stroke->pos[2] + stroke->size[2]);
		glVertex3f (x[0], y[0], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[0], z[0]);

		glVertex3f (x[0], y[1], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[1], y[1], z[1]);
		glVertex3f (x[1], y[1], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[0], y[1], z[0]);

		glVertex3f (x[0], y[0], z[0]);
		glVertex3f (x[0], y[1], z[0]);
		glVertex3f (x[1], y[0], z[0]);
		glVertex3f (x[1], y[1], z[0]);
		glVertex3f (x[0], y[0], z[1]);
		glVertex3f (x[0], y[1], z[1]);
		glVertex3f (x[1], y[0], z[1]);
		glVertex3f (x[1], y[1], z[1]);
	}
	glEnd ();
}

/** @} */
/** @} */
/** @} */
