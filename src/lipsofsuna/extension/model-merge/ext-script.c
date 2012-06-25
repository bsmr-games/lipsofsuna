/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtModelMerge ModelMerge
 * @{
 */

#include "lipsofsuna/model.h"
#include "ext-module.h"

static void Merger_new (LIScrArgs* args)
{
	LIMdlAsyncMerger* self;
	LIScrData* data;

	/* Allocate the merger. */
	self = limdl_async_merger_new ();
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_MERGER, limdl_async_merger_free);
	if (data == NULL)
	{
		limdl_async_merger_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Merger_add_model (LIScrArgs* args)
{
	LIEngModel* model;
	LIMdlAsyncMerger* self;
	LIScrData* data;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &data))
		return;
	model = liscr_data_get_data (data);

	/* Add the task. */
	limdl_async_merger_add_model (self, model->model);
}

static void Merger_add_model_morph (LIScrArgs* args)
{
	int i = 0;
	float value;
	const char* shape;
	LIEngModel* model;
	LIMdlAsyncMerger* self;
	LIMdlAsyncMergerMorph morphs[128];
	LIScrData* data;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &data))
		return;
	model = liscr_data_get_data (data);
	for (i = 0 ; i < 128 ; i++)
	{
		if (!liscr_args_geti_string (args, 2 * i + 1, &shape) ||
		    !liscr_args_geti_float (args, 2 * i + 2, &value))
			break;
		strncpy (morphs[i].shape, shape, LIMDL_ASYNC_MERGER_SHAPE_LENGTH_MAX);
		morphs[i].shape[LIMDL_ASYNC_MERGER_SHAPE_LENGTH_MAX - 1] = '\0';
		morphs[i].value = value;
	}

	/* Add the task. */
	limdl_async_merger_add_model_morph (self, model->model, morphs, i);
}

static void Merger_finish (LIScrArgs* args)
{
	LIMdlAsyncMerger* self;

	/* Get arguments. */
	self = args->self;

	/* Add the task. */
	limdl_async_merger_finish (self);
}

static void Merger_pop_model (LIScrArgs* args)
{
	LIEngModel* emodel;
	LIMaiProgram* program;
	LIMdlAsyncMerger* self;
	LIMdlModel* model;

	/* Get arguments. */
	self = args->self;
	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);

	/* Get the model. */
	model = limdl_async_merger_pop_model (self);
	if (model == NULL)
		return;

	/* Create the script model. */
	emodel = lieng_model_new (program->engine);
	if (emodel == NULL)
	{
		limdl_model_free (model);
		return;
	}
	emodel->model = model;
	emodel->script = liscr_data_new (args->script, args->lua, emodel, LISCR_SCRIPT_MODEL, lieng_model_free);
	if (emodel->script == NULL)
	{
		lieng_model_free (emodel);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Merger_replace_material (LIScrArgs* args)
{
	int has_diffuse = 0;
	int has_specular = 0;
	int has_textures = 0;
	int set_textures_count = 0;
	const char* match_material = NULL;
	float set_diffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float set_specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* set_material = NULL;
	const char* set_textures[4] = { NULL, NULL, NULL, NULL };
	LIMdlAsyncMerger* self;

	/* Get arguments. */
	self = args->self;
	if (!liscr_args_gets_string (args, "match_material", &match_material))
		return;
	if (liscr_args_gets_floatv (args, "diffuse", 4, set_diffuse))
		has_diffuse = 1;
	if (liscr_args_gets_floatv (args, "specular", 4, set_specular))
		has_specular = 1;
	liscr_args_gets_string (args, "material", &set_material);
	if (liscr_args_gets_table (args, "textures"))
	{
		has_textures = 1;
		for (set_textures_count = 0 ; set_textures_count < 4 ; set_textures_count++)
		{
			lua_pushnumber (args->lua, set_textures_count + 1);
			lua_gettable (args->lua, -2);
			if (lua_type (args->lua, -1) == LUA_TSTRING)
			{
				set_textures[set_textures_count] = lua_tostring (args->lua, -1);
				lua_pop (args->lua, 1);
			}
			else
			{
				lua_pop (args->lua, 1);
				break;
			}
		}
		lua_pop (args->lua, 1);
	}

	/* Add the task. */
	limdl_async_merger_replace_material (self, match_material,
		has_diffuse? set_diffuse : NULL,
		has_specular? set_specular : NULL,
		set_material,
		has_textures? set_textures : NULL,
		has_textures? set_textures_count : -1);
}

/*****************************************************************************/

void liext_script_merger (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_MERGER, "merger_new", Merger_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MERGER, "merger_add_model", Merger_add_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MERGER, "merger_add_model_morph", Merger_add_model_morph);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MERGER, "merger_finish", Merger_finish);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MERGER, "merger_pop_model", Merger_pop_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_MERGER, "merger_replace_material", Merger_replace_material);
}

/** @} */
/** @} */