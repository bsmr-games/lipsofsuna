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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include "lipsofsuna/network.h"
#include "render-internal.h"

static void private_create_mesh (
	LIRenModel* self,
	LIMdlModel* model);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 * \return Model or NULL on failure.
 */
LIRenModel* liren_model_new (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id)
{
	LIRenModel* self;

	/* Allocate self. */
	self = new LIRenModel ();
	if (self == NULL)
		return 0;
	self->id = 0;
	self->render = render;

	/* Choose a unique ID. */
	while (!id)
	{
		id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (render->objects, id))
			id = 0;
	}
	self->id = id;

	/* Load the model. */
	if (model != NULL)
		private_create_mesh (self, model);

	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->models, id, self))
	{
		liren_model_free (self);
		return 0;
	}

	return self;
}

void liren_model_free (
	LIRenModel* self)
{
	/* Remove from objects. */
	/* Keeping the model alive when it's assigned to objects is the job of scripts.
	   If they don't reference the model, we'll remove it even if it's in use. We
	   prevent crashing by removing it from objects in such a case. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->render->objects)
	{
		LIRenObject* object = (LIRenObject*) iter.value;
		liren_object_remove_model (object, self);
	}

	/* Remove from the model dictionary. */
	lialg_u32dic_remove (self->render->models, self->id);
	delete self;
}

/**
 * \brief Gets the ID of the model.
 * \param self Model.
 * \return ID.
 */
int liren_model_get_id (
	LIRenModel* self)
{
	return self->id;
}

/**
 * \brief Returns nonzero if the model has been fully background loaded.
 * \param self Model.
 * \return Nonzero if background loaded, zero if not.
 */
int liren_model_get_loaded (
	LIRenModel* self)
{
	if (self->mesh.isNull ())
		return 0;
	return self->mesh->isLoaded ();
}

LIMdlModel* liren_model_get_model (
	LIRenModel* self)
{
	if (self->mesh.isNull ())
		return NULL;

	LIRenMeshBuilder* builder = (LIRenMeshBuilder*) lialg_strdic_find (
		self->render->data->mesh_builders, self->mesh->getName ().c_str ());
	if (builder == NULL)
		return NULL;

	return builder->get_model ();
}

int liren_model_set_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	private_create_mesh (self, model);

	return 1;
}

/*****************************************************************************/

static void private_create_mesh (
	LIRenModel* self,
	LIMdlModel* model)
{
	/* Create the resource loader. */
	/* Ogre doesn't free the manual resource loader, nor does it allow us to
	   store custom data to meshes. Because of those reasons, we store the
	   loader to a dictionary that is searched by mesh name. The render class
	   will also use the dictionary to garbage collect unused loaders. */
	LIRenMeshBuilder* builder = new LIRenMeshBuilder (self->render, model);
	Ogre::String name = self->render->data->id.next ();
	lialg_strdic_insert (self->render->data->mesh_builders, name.c_str (), builder);

	/* Create the manual mesh. */
	Ogre::String group = LIREN_RESOURCES_TEMPORARY;
	self->mesh = Ogre::MeshManager::getSingleton ().createManual (name, group, builder);

	/* Tell objects using the old mesh to rebuild. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->render->objects)
	{
		LIRenObject* object = (LIRenObject*) iter.value;
		liren_object_model_changed (object, self);
	}
}

/** @} */
/** @} */
/** @} */
