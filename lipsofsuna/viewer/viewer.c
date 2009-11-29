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
 * \addtogroup livie Viewer
 * @{
 * \addtogroup livieViewer Viewer
 * @{
 */

#include <sys/time.h>
#include "viewer.h"

#define ZOOM_SPEED 0.5f
#define ROTATION_SPEED 0.01f

static int
private_init (livieViewer* self);

static int
private_init_model (livieViewer* self,
                    const char*  model);

static void
private_clear_model (livieViewer* self);

static void
private_reload_image (livieViewer* self,
                      const char*  name);

static void
private_reload_model (livieViewer* self,
                      const char*  name);

static int
private_resize (livieViewer* self,
                int          width,
                int          height,
                int          fsaa);

/*****************************************************************************/

livieViewer*
livie_viewer_new (lividCalls* video,
                  lipthPaths* paths,
                  const char* model)
{
	char buf[256];
	livieViewer* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (livieViewer));
	if (self == NULL)
		return NULL;
	self->video = *video;
	self->paths = paths;
	self->file = listr_dup (model);
	if (self->file == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize SDL. */
	if (self->video.SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
	{
		lisys_error_set (ENOTSUP, "initializing SDL failed");
		lisys_free (self);
		return NULL;
	}

	/* Initialize subsystems. */
	if (!private_init (self) ||
	    !private_init_model (self, model))
	{
		livie_viewer_free (self);
		return NULL;
	}
	snprintf (buf, 256, "%s - Lips of Suna Model Viewer", model);
	self->video.SDL_WM_SetCaption (buf, buf);

	return self;
}

void
livie_viewer_free (livieViewer* self)
{
	private_clear_model (self);
	if (self->lights.key != NULL)
	{
		lirnd_lighting_remove_light (self->scene->lighting, self->lights.key);
		lirnd_light_free (self->lights.key);
	}
	if (self->lights.fill != NULL)
	{
		lirnd_lighting_remove_light (self->scene->lighting, self->lights.fill);
		lirnd_light_free (self->lights.fill);
	}
	if (self->reload != NULL)
		lirel_reload_free (self->reload);
	if (self->camera != NULL)
		limat_camera_free (self->camera);
	if (self->scene != NULL)
		lirnd_scene_free (self->scene);
	if (self->render != NULL)
		lirnd_render_free (self->render);
	if (self->screen != NULL)
		self->video.SDL_FreeSurface (self->screen);
	self->video.SDL_Quit ();
	lisys_free (self->file);
	lisys_free (self);
}

int
livie_viewer_main (livieViewer* self)
{
	float secs;
	struct timeval curr_tick;
	struct timeval prev_tick;
	SDL_Event event;
	limatAabb aabb;
	limatFrustum frustum;
	limatMatrix modelview;
	limatMatrix projection;
	limatTransform transform;

	/* Main loop. */
	gettimeofday (&prev_tick, NULL);
	while (1)
	{
		gettimeofday (&curr_tick, NULL);
		secs = curr_tick.tv_sec - prev_tick.tv_sec +
			  (curr_tick.tv_usec - prev_tick.tv_usec) * 0.000001;
		prev_tick = curr_tick;

		/* Handle events. */
		while (self->video.SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
					return 1;
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_s)
					{
						lirnd_render_set_shaders_enabled (self->render,
							!lirnd_render_get_shaders_enabled (self->render));
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == 4)
						limat_camera_zoom (self->camera, -ZOOM_SPEED);
					else if (event.button.button == 5)
						limat_camera_zoom (self->camera, ZOOM_SPEED);
					break;
				case SDL_MOUSEMOTION:
					if (self->video.SDL_GetMouseState (NULL, NULL))
					{
						limat_camera_turn (self->camera, ROTATION_SPEED * -event.motion.xrel);
						limat_camera_tilt (self->camera, ROTATION_SPEED * -event.motion.yrel);
					}
					break;
				case SDL_VIDEORESIZE:
					private_resize (self, event.resize.w, event.resize.h, self->mode.fsaa);
					limat_camera_set_viewport (self->camera, 0, 0, event.resize.w, event.resize.h);
					glViewport (0, 0, event.resize.w, event.resize.h);
					break;
			}
		}

		/* Update resources. */
		lirel_reload_update (self->reload);

		/* Update camera. */
		lirnd_object_get_bounds (self->object, &aabb);
		lirnd_object_get_transform (self->object, &transform);
		transform.position = limat_vector_add (transform.position,
			limat_vector_multiply (limat_vector_add (aabb.min, aabb.max), 0.5f));
		limat_camera_set_center (self->camera, &transform);
		limat_camera_update (self->camera, secs);
		limat_camera_warp (self->camera);

		/* Update lights. */
		limat_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (9, 6, -1));
		lirnd_light_set_transform (self->lights.key, &transform);
		limat_camera_get_transform (self->camera, &transform);
		transform.position = limat_transform_transform (transform, limat_vector_init (-4, 2, 0));
		lirnd_light_set_transform (self->lights.fill, &transform);

		/* Render scene. */
		glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		limat_camera_get_frustum (self->camera, &frustum);
		limat_camera_get_modelview (self->camera, &modelview);
		limat_camera_get_projection (self->camera, &projection);
		lirnd_scene_render (self->scene, NULL, &modelview, &projection, &frustum);
		self->video.SDL_GL_SwapBuffers ();
		self->video.SDL_Delay (100);
	}

	return 1;
}

/*****************************************************************************/

static int
private_init (livieViewer* self)
{
	GLint viewport[4];
	const float equation[3] = { 1.0f, 0.0f, 0.001f };
	const float diffuse0[4] = { 0.6f, 0.6f, 0.6f, 1.0f };
	const float diffuse1[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	/* Create main window. */
	if (private_resize (self, 1024, 768, livid_features_get_max_samples ()) +
	    private_resize (self, 1024, 768, 0) == 0)
		return 0;
	livid_features_init ();

	/* Allocate camera. */
	self->camera = limat_camera_new ();
	if (self->camera == NULL)
		return 0;
	glGetIntegerv (GL_VIEWPORT, viewport);
	limat_camera_set_driver (self->camera, LIMAT_CAMERA_THIRDPERSON);
	limat_camera_set_viewport (self->camera, viewport[0], viewport[1], viewport[2], viewport[3]);

	/* Allocate scene. */
	self->render = lirnd_render_new (self->paths->module_data);
	if (self->render == NULL)
		return 0;
	self->scene = lirnd_scene_new (self->render);
	if (self->scene == NULL)
		return 0;

	/* Allocate lights. */
	self->lights.key = lirnd_light_new (self->scene, diffuse0, equation, M_PI, 0.0f, 0);
	if (self->lights.key == NULL)
		return 0;
	lirnd_lighting_insert_light (self->scene->lighting, self->lights.key);
	self->lights.fill = lirnd_light_new (self->scene, diffuse1, equation, M_PI, 0.0f, 0);
	if (self->lights.fill == NULL)
		return 0;
	lirnd_lighting_insert_light (self->scene->lighting, self->lights.fill);

	/* Initialize reloading. */
	self->reload = lirel_reload_new (self->paths);
	if (self->reload == NULL)
		return 0;
	lirel_reload_set_image_callback (self->reload, private_reload_image, self);
	lirel_reload_set_model_callback (self->reload, private_reload_model, self);
	lirel_reload_set_enabled (self->reload, 1);
	lirel_reload_run (self->reload);

	return 1;
}

static int
private_init_model (livieViewer* self,
                    const char*  model)
{
	char* path;
	limdlModel* mdl;

	/* Format path. */
	path = lisys_path_format (self->paths->module_data,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, model, ".lmdl", NULL);
	if (path == NULL)
		return 0;

	/* Load model. */
	mdl = limdl_model_new_from_file (path);
	lisys_free (path);
	if (mdl == NULL)
	{
		lisys_error_set (EINVAL, "Cannot open model `%s'", model);
		lisys_error_report ();
		return 1;
	}
	self->model = lirnd_model_new (self->render, mdl);
	if (self->model == NULL)
	{
		limdl_model_free (mdl);
		return 0;
	}

	/* Create object. */
	self->object = lirnd_object_new (self->scene, 0);
	if (self->object == NULL)
	{
		lirnd_model_free (self->model);
		limdl_model_free (mdl);
		return 0;
	}
	lirnd_object_set_model (self->object, self->model);
	lirnd_object_set_realized (self->object, 1);

	return 1;
}

static void
private_clear_model (livieViewer* self)
{
	if (self->object != NULL)
	{
		lirnd_object_free (self->object);
		self->object = NULL;
	}
	if (self->model != NULL)
	{
		limdl_model_free (self->model->model);
		lirnd_model_free (self->model);
		self->model = NULL;
	}
}

static void
private_reload_image (livieViewer* self,
                      const char*  name)
{
	printf ("Reloading texture `%s'\n", name);
	if (lirnd_render_find_image (self->render, name))
		lirnd_render_load_image (self->render, name);
}

static void
private_reload_model (livieViewer* self,
                      const char*  name)
{
	printf ("Reloading model `%s'\n", name);
	if (!strcmp (name, self->file))
	{
		private_clear_model (self);
		private_init_model (self, name);
	}
}

static int
private_resize (livieViewer* self,
                int          width,
                int          height,
                int          fsaa)
{
	int depth;
	GLenum error;

	/* Recreate surface. */
	for ( ; fsaa >= 0 ; fsaa--)
	{
		for (depth = 32 ; depth ; depth -= 8)
		{
			self->video.SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, depth);
			self->video.SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 1);
			self->video.SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
			self->video.SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, fsaa? 1 : 0);
			self->video.SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, fsaa);
			self->screen = self->video.SDL_SetVideoMode (width, height, 0, SDL_OPENGL | SDL_RESIZABLE);
			if (self->screen != NULL)
				break;
		}
		if (self->screen != NULL)
			break;
	}
	if (self->screen == NULL)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot set video mode");
		return 0;
	}

	/* Initialize GLEW. */
	error = glewInit ();
	if (error != GLEW_OK)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "%s", glewGetErrorString (error));
		return 0;
	}

	/* Store mode. */
	self->mode.width = width;
	self->mode.height = height;
	self->mode.fsaa = fsaa;
	if (fsaa)
		glEnable (GL_MULTISAMPLE_ARB);
	else
		glDisable (GL_MULTISAMPLE_ARB);

	return 1;
}

/** @} */
/** @} */
