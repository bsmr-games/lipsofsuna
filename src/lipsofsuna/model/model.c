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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlModel Model
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "model.h"
#include "model-builder.h"

typedef int (*LIMdlWriteFunc)(const LIMdlModel*, LIArcWriter*);

static void private_build (
	LIMdlModel* self);

static int private_read (
	LIMdlModel*  self,
	LIArcReader* reader,
	int          mesh);

static int private_read_animations (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_bounds (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_faces (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_hairs (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_materials (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_nodes (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_particles (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_shapes (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_vertices (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_weights (
	LIMdlModel*  self,
	LIArcReader* reader);

static int private_read_vertex_weights (
	LIMdlModel*  self,
	LIMdlVertex* vertex,
	LIArcReader* reader);

static int private_write (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_block (
	const LIMdlModel* self,
	const char*       name,
	LIMdlWriteFunc    func,
	LIArcWriter*      writer);

static int private_write_animations (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_bounds (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_faces (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_hairs (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_header (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_materials (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_nodes (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_particles (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_shapes (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_vertices (
	const LIMdlModel* self,
	LIArcWriter*      writer);

static int private_write_weights (
	const LIMdlModel* self,
	LIArcWriter*      writer);

/*****************************************************************************/

LIMdlModel* limdl_model_new ()
{
	LIMdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlModel));
	if (self == NULL)
		return NULL;

	return self;
}

LIMdlModel* limdl_model_new_copy (
	LIMdlModel* model)
{
	LIMdlModel* self;
	LIArcReader* reader;
	LIArcWriter* writer;

	/* FIXME: This is lousy. */
	writer = liarc_writer_new ();
	if (writer == NULL)
		return NULL;
	if (!limdl_model_write (model, writer))
	{
		liarc_writer_free (writer);
		return NULL;
	}
	reader = liarc_reader_new (
		liarc_writer_get_buffer (writer),
		liarc_writer_get_length (writer));
	if (reader == NULL)
	{
		liarc_writer_free (writer);
		return NULL;
	}
	self = limdl_model_new_from_data (reader, 1);
	liarc_reader_free (reader);
	liarc_writer_free (writer);

	return self;
}

/**
 * \brief Loads a model from uncompressed data.
 * \param reader A reader.
 * \param mesh Zero to only load collision shapes.
 * \return A new model or NULL.
 */
LIMdlModel* limdl_model_new_from_data (
	LIArcReader* reader,
	int          mesh)
{
	LIMdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlModel));
	if (self == NULL)
	{
		lisys_error_append ("cannot load model");
		return NULL;
	}

	/* Read from stream. */
	if (!private_read (self, reader, mesh))
	{
		lisys_error_append ("cannot load model");
		limdl_model_free (self);
		return NULL;
	}

	/* Construct the rest pose. */
	private_build (self);
	return self;
}

/**
 * \brief Loads a model from a file.
 * \param path The path to the file.
 * \param mesh Zero to only load collision shapes.
 * \return A new model or NULL.
 */
LIMdlModel* limdl_model_new_from_file (
	const char* path,
	int         mesh)
{
	LIArcReader* reader;
	LIMdlModel* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlModel));
	if (self == NULL)
	{
		lisys_error_append ("cannot load model `%s'", path);
		return NULL;
	}

	/* Open the file. */
	reader = liarc_reader_new_from_file (path);
	if (reader == NULL)
		goto error;

	/* Read from stream. */
	if (!private_read (self, reader, mesh))
		goto error;
	liarc_reader_free (reader);

	/* Construct the rest pose. */
	private_build (self);
	return self;

error:
	lisys_error_append ("cannot load model `%s'", path);
	limdl_model_free (self);
	if (reader != NULL)
		liarc_reader_free (reader);
	return NULL;
}

/**
 * \brief Frees the model.
 * \param self Model.
 */
void limdl_model_free (
	LIMdlModel* self)
{
	int i;
	LIMdlMaterial* material;

	/* Free materials. */
	if (self->materials.array != NULL)
	{
		for (i = 0 ; i < self->materials.count ; i++)
		{
			material = self->materials.array + i;
			limdl_material_free (material);
		}
		lisys_free (self->materials.array);
	}

	/* Free face groups. */
	if (self->facegroups.array != NULL)
	{
		for (i = 0 ; i < self->facegroups.count ; i++)
			limdl_faces_free (self->facegroups.array + i);
		lisys_free (self->facegroups.array);
	}

	/* Free weight groups. */
	if (self->weightgroups.array != NULL)
	{
		for (i = 0 ; i < self->weightgroups.count ; i++)
		{
			lisys_free (self->weightgroups.array[i].name);
			lisys_free (self->weightgroups.array[i].bone);
		}
		lisys_free (self->weightgroups.array);
	}
	lisys_free (self->vertices.array);

	/* Free nodes. */
	if (self->nodes.array != NULL)
	{
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			if (self->nodes.array[i] != NULL)
				limdl_node_free (self->nodes.array[i]);
		}
		lisys_free (self->nodes.array);
	}

	/* Free animations. */
	if (self->animations.array != NULL)
	{
		for (i = 0 ; i < self->animations.count ; i++)
			limdl_animation_clear (self->animations.array + i);
		lisys_free (self->animations.array);
	}

	/* Free particles. */
	if (self->particlesystems.array != NULL)
	{
		for (i = 0 ; i < self->particlesystems.count ; i++)
			limdl_particle_system_clear (self->particlesystems.array + i);
		lisys_free (self->particlesystems.array);
	}
	if (self->hairs.array != NULL)
	{
		for (i = 0 ; i < self->hairs.count ; i++)
			limdl_hairs_free (self->hairs.array + i);
		lisys_free (self->hairs.array);
	}

	/* Free shapes. */
	if (self->shapes.array != NULL)
	{
		for (i = 0 ; i < self->shapes.count ; i++)
			limdl_shape_clear (self->shapes.array + i);
		lisys_free (self->shapes.array);
	}

	lisys_free (self);
}

/**
 * \brief Recalculates the bounding box of the model.
 *
 * Loops through all vertices of the model and calculates the minimum and
 * maximum axis values used.
 *
 * \param self Model.
 */
void limdl_model_calculate_bounds (
	LIMdlModel* self)
{
	int j;
	LIMatVector v;

	self->bounds.min = limat_vector_init (2.0E10, 2.0E10, 2.0E10);
	self->bounds.max = limat_vector_init (-2.0E10, -2.0E10, -2.0E10);
	for (j = 0 ; j < self->vertices.count ; j++)
	{
		v = self->vertices.array[j].coord;
		if (self->bounds.min.x > v.x)
			self->bounds.min.x = v.x;
		if (self->bounds.min.y > v.y)
			self->bounds.min.y = v.y;
		if (self->bounds.min.z > v.z)
			self->bounds.min.z = v.z;
		if (self->bounds.max.x < v.x)
			self->bounds.max.x = v.x;
		if (self->bounds.max.y < v.y)
			self->bounds.max.y = v.y;
		if (self->bounds.max.z < v.z)
			self->bounds.max.z = v.z;
	}
	if (self->bounds.min.x > self->bounds.max.x)
	{
		self->bounds.min = limat_vector_init (0.0f, 0.0f, 0.0f);
		self->bounds.max = limat_vector_init (0.0f, 0.0f, 0.0f);
	}
}

/**
 * \brief Finds an animation by name.
 * \param self Model.
 * \param name Animation name.
 * \return Animation or NULL.
 */
LIMdlAnimation* limdl_model_find_animation (
	LIMdlModel* self,
	const char* name)
{
	int i;

	for (i = 0 ; i < self->animations.count ; i++)
	{
		if (!strcmp (self->animations.array[i].name, name))
			return self->animations.array + i;
	}

	return NULL;
}

/**
 * \brief Finds a face group.
 * \param self Model.
 * \param material Material index.
 * \return Face group index or -1.
 */
int limdl_model_find_facegroup (
	LIMdlModel* self,
	int         material)
{
	int i;

	for (i = 0 ; i < self->facegroups.count ; i++)
	{
		if (self->facegroups.array[i].material == material)
			return i;
	}

	return -1;
}

/**
 * \brief Finds the material index of the material.
 *
 * Finds the first material that matches the passed material.
 * The materials are considered to match if they have the same shader,
 * shininess, diffuse color, specular color, and texture images.
 *
 * \param self Model.
 * \param material Material.
 * \return Material index or -1.
 */
int limdl_model_find_material (
	const LIMdlModel*    self,
	const LIMdlMaterial* material)
{
	int i;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		if (limdl_material_compare (self->materials.array + i, material))
			return i;
	}

	return -1;
}

/**
 * \brief Finds a material by shader and/or texture.
 * \param self Model.
 * \param shader Shader name or NULL.
 * \param texture Texture name or NULL.
 * \return Material or NULL.
 */
LIMdlMaterial* limdl_model_find_material_by_texture (
	LIMdlModel* self,
	const char* shader,
	const char* texture)
{
	int i;
	LIMdlMaterial* material;

	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.array + i;
		if (shader != NULL)
		{
			/* The name of the shader must match. */
			if (material->shader != NULL && strcmp (shader, material->shader))
				continue;
		}
		if (texture != NULL)
		{
			/* The name of the first texture must match. */
			if (!material->textures.count || material->textures.array[0].string == NULL)
				continue;
			if (strcmp (texture, material->textures.array[0].string))
				continue;
		}
		return material;
	}

	return NULL;
}

/**
 * \brief Finds a node by name.
 * \param self Model.
 * \param name Name of the node to find.
 * \return Node or NULL.
 */
LIMdlNode* limdl_model_find_node (
	const LIMdlModel* self,
	const char*       name)
{
	int i;
	LIMdlNode* node;

	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		node = limdl_node_find_node (node, name);
		if (node != NULL)
			return node;
	}

	return NULL;
}

/**
 * \brief Finds the index of a matching vertex.
 * \param self Model.
 * \param vertex Vertex.
 * \return Index in vertex array or -1 if not found.
 */
int limdl_model_find_vertex (
	LIMdlModel*        self,
	const LIMdlVertex* vertex)
{
	int i;

	for (i = 0 ; i < self->vertices.count ; i++)
	{
		if (limdl_vertex_compare (self->vertices.array + i, vertex) == 0)
			return i;
	}

	return -1;
}

/**
 * \brief Finds a matching weight group.
 * \param self Model.
 * \param name Group name.
 * \param bone Bone name.
 * \return Index in weight group array or -1 if not found.
 */
int limdl_model_find_weightgroup (
	LIMdlModel* self,
	const char* name,
	const char* bone)
{
	int i;
	LIMdlWeightGroup* group;

	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		group = self->weightgroups.array + i;
		if (!strcmp (group->name, name) &&
		    !strcmp (group->bone, bone))
			return i;
	}

	return -1;
}

int limdl_model_merge (
	LIMdlModel* self,
	LIMdlModel* model)
{
	int i;
	int j;
	int count;
	int group;
	int material;
	int* vertices = NULL;
	int* wgroups = NULL;
	uint32_t index;
	uint32_t* indices;
	LIMdlBuilder* builder = NULL;
	LIMdlFaces* dstfaces;
	LIMdlFaces* srcfaces;

	/* Create a model builder. */
	builder = limdl_builder_new (self);
	if (builder == NULL)
		goto error;

	/* Map weight groups. */
	if (model->weightgroups.count)
	{
		wgroups = lisys_calloc (model->weightgroups.count, sizeof (int));
		if (wgroups == NULL)
			return 0;
		for (i = 0 ; i < model->weightgroups.count ; i++)
		{
			group = limdl_model_find_weightgroup (self,
				model->weightgroups.array[i].name,
				model->weightgroups.array[i].bone);
			if (group == -1)
			{
				group = self->weightgroups.count;
				if (!limdl_builder_insert_weightgroup (builder, 
				    model->weightgroups.array[i].name,
				    model->weightgroups.array[i].bone))
					goto error;
			}
			lisys_assert (group < self->weightgroups.count);
			wgroups[i] = group;
		}
	}

	/* Map vertices. */
	if (model->vertices.count)
	{
		vertices = lisys_calloc (model->vertices.count, sizeof (int));
		for (i = 0 ; i < model->vertices.count ; i++)
			vertices[i] = self->vertices.count + i;
		if (!limdl_builder_insert_vertices (builder, model->vertices.array, model->vertices.count, wgroups))
			goto error;
	}

	/* Merge each face group. */
	for (i = 0 ; i < model->facegroups.count ; i++)
	{
		srcfaces = model->facegroups.array + i;

		/* Find or create material. */
		material = limdl_model_find_material (self, model->materials.array + srcfaces->material);
		if (material == -1)
		{
			material = self->materials.count;
			if (!limdl_builder_insert_material (builder, model->materials.array + srcfaces->material))
				goto error;
		}

		/* Find or create face group. */
		group = limdl_model_find_facegroup (self, material);
		if (group == -1)
		{
			group = self->facegroups.count;
			if (!limdl_builder_insert_facegroup (builder, material))
				goto error;
		}

		/* Destination group for cloned faces. */
		dstfaces = self->facegroups.array + group;

		/* Allocate space for indices. */
		count = dstfaces->indices.count + srcfaces->indices.count;
		if (dstfaces->indices.capacity < count)
		{
			indices = lisys_realloc (dstfaces->indices.array, count * sizeof (uint32_t));
			if (indices == NULL)
				goto error;
			dstfaces->indices.array = indices;
			dstfaces->indices.capacity = count;
		}

		/* Insert indices. */
		for (j = 0 ; j < srcfaces->indices.count ; j++)
		{
			index = vertices[srcfaces->indices.array[j]];
			dstfaces->indices.array[dstfaces->indices.count] = index;
			dstfaces->indices.count++;
		}
	}

	limdl_builder_finish (builder);
	limdl_builder_free (builder);
	lisys_free (wgroups);
	lisys_free (vertices);

	return 1;

error:
	if (builder != NULL)
		limdl_builder_free (builder);
	lisys_free (wgroups);
	lisys_free (vertices);
	return 0;
}

int limdl_model_write (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	return private_write (self, writer);
}

int limdl_model_write_file (
	const LIMdlModel* self,
	const char*       path)
{
	LIArcWriter* writer;

	/* Create a writer. */
	writer = liarc_writer_new_file (path);
	if (writer == NULL)
		return 0;

	/* Pack to the writer. */
	if (!limdl_model_write (self, writer))
	{
		liarc_writer_free (writer);
		return 0;
	}

	liarc_writer_free (writer);

	return 1;
}

int limdl_model_get_index_count (
	const LIMdlModel* self)
{
	int i;
	int c;

	for (i = c = 0 ; i < self->facegroups.count ; i++)
		c += self->facegroups.array[i].indices.count;

	return c;
}

/*****************************************************************************/

static void private_build (
	LIMdlModel* self)
{
	int i;
	LIMdlNode* node;
	LIMdlWeightGroup* group;

	/* Resolve node references. */
	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		group = self->weightgroups.array + i;
		group->node = limdl_model_find_node (self, group->bone);
	}

	/* Transform each node. */
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		limdl_node_rebuild (node, 1);
	}
}

static int private_read (
	LIMdlModel*  self,
	LIArcReader* reader,
	int          mesh)
{
	int i;
	int j;
	int ret;
	char* id;
	uint32_t tmp;
	uint32_t size;
	uint32_t version;
	LIMdlFaces* group;

	/* Read magic. */
	if (!liarc_reader_check_text (reader, "lips/mdl", ""))
	{
		lisys_error_set (EINVAL, "wrong file format");
		return 0;
	}

	/* Read header chunk size. */
	if (!liarc_reader_get_uint32 (reader, &size))
		return 0;
	if (size != 8)
	{
		lisys_error_set (LISYS_ERROR_VERSION, "invalid header block");
		return 0;
	}

	/* Read version. */
	if (!liarc_reader_get_uint32 (reader, &version))
		return 0;
	if (version != LIMDL_FORMAT_VERSION)
	{
		lisys_error_set (LISYS_ERROR_VERSION, "model version mismatch");
		return 0;
	}

	/* Read flags. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->flags = tmp;

	/* Read chunks. */
	while (!liarc_reader_check_end (reader))
	{
		id = NULL;
		if (!liarc_reader_get_text (reader, "", &id) ||
		    !liarc_reader_get_uint32 (reader, &size))
		{
			lisys_error_append ("reading chunk failed");
			lisys_free (id);
			return 0;
		}
		if (size > reader->length - reader->pos)
		{
			lisys_error_append ("invalid chunk size for `%s'", id);
			lisys_free (id);
			return 0;
		}
		if (!strcmp (id, "bou"))
			ret = private_read_bounds (self, reader);
		else if (mesh && !strcmp (id, "mat"))
			ret = private_read_materials (self, reader);
		else if (mesh && !strcmp (id, "ver"))
			ret = private_read_vertices (self, reader);
		else if (mesh && !strcmp (id, "fac"))
			ret = private_read_faces (self, reader);
		else if (mesh && !strcmp (id, "wei"))
			ret = private_read_weights (self, reader);
		else if (mesh && !strcmp (id, "nod"))
			ret = private_read_nodes (self, reader);
		else if (mesh && !strcmp (id, "ani"))
			ret = private_read_animations (self, reader);
		else if (mesh && !strcmp (id, "hai"))
			ret = private_read_hairs (self, reader);
		else if (mesh && !strcmp (id, "par"))
			ret = private_read_particles (self, reader);
		else if (!strcmp (id, "sha"))
			ret = private_read_shapes (self, reader);
		else
		{
			if (!liarc_reader_skip_bytes (reader, size))
			{
				lisys_error_append ("failed to skip block `%s'", id);
				lisys_free (id);
				return 0;
			}
			ret = 1;
		}
		if (!ret)
		{
			lisys_error_append ("failed to read block `%s'", id);
			lisys_free (id);
			return 0;
		}
		lisys_free (id);
	}

	/* Sanity checks. */
	for (i = 0 ; i < self->facegroups.count ; i++)
	{
		group = self->facegroups.array + i;
		if (group->material >= self->materials.count)
		{
			lisys_error_set (EINVAL, "material index out of bounds");
			return 0;
		}
	}
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		for (j = 0 ; j < LIMDL_VERTEX_WEIGHTS_MAX ; j++)
		{
			if (self->vertices.array[i].bones[j] > self->weightgroups.count)
			{
				lisys_error_set (EINVAL, "weight group index out of bounds");
				return 0;
			}
		}
	}

	return 1;
}

static int private_read_animations (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp;
	LIMdlAnimation* animation;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->animations.count = tmp;

	/* Read animations. */
	if (self->animations.count)
	{
		self->animations.array = lisys_calloc (self->animations.count, sizeof (LIMdlAnimation));
		if (self->animations.array == NULL)
			return 0;
		for (i = 0 ; i < self->animations.count ; i++)
		{
			animation = self->animations.array + i;
			if (!limdl_animation_read (animation, reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_bounds (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	LIMatVector min;
	LIMatVector max;

	if (!liarc_reader_get_float (reader, &min.x) ||
	    !liarc_reader_get_float (reader, &min.y) ||
	    !liarc_reader_get_float (reader, &min.z) ||
	    !liarc_reader_get_float (reader, &max.x) ||
	    !liarc_reader_get_float (reader, &max.y) ||
	    !liarc_reader_get_float (reader, &max.z))
		return 0;
	limat_aabb_init_from_points (&self->bounds, &min, &max);

	return 1;
}

static int private_read_faces (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp;
	LIMdlFaces* group;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->facegroups.count = tmp;

	/* Read face groups. */
	if (self->facegroups.count)
	{
		self->facegroups.array = lisys_calloc (self->facegroups.count, sizeof (LIMdlFaces));
		if (self->facegroups.array == NULL)
			return 0;
		for (i = 0 ; i < self->facegroups.count ; i++)
		{
			group = self->facegroups.array + i;
			if (!limdl_faces_read (group, reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_hairs (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t count;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &count))
		return 0;
	self->hairs.count = count;

	/* Read hairs. */
	if (self->hairs.count)
	{
		self->hairs.array = lisys_calloc (self->hairs.count, sizeof (LIMdlHairs));
		if (self->hairs.array == NULL)
			return 0;
		for (i = 0 ; i < self->hairs.count ; i++)
		{
			if (!limdl_hairs_read (self->hairs.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_materials (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->materials.count = tmp;

	/* Read materials. */
	if (self->materials.count)
	{
		self->materials.array = lisys_calloc (self->materials.count, sizeof (LIMdlMaterial));
		if (self->materials.array == NULL)
			return 0;
		for (i = 0 ; i < self->materials.count ; i++)
		{
			if (!limdl_material_read (self->materials.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_nodes (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->nodes.count = tmp;

	/* Read nodes. */
	if (self->nodes.count)
	{
		self->nodes.array = lisys_calloc (self->nodes.count, sizeof (LIMdlNode*));
		if (self->nodes.array == NULL)
			return 0;
		for (i = 0 ; i < self->nodes.count ; i++)
		{
			self->nodes.array[i] = limdl_node_new (self);
			if (self->nodes.array[i] == NULL)
				return 0;
			if (!limdl_node_read (self->nodes.array[i], reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_particles (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp[1];

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, tmp + 0))
		return 0;

	/* Allocate particle systems. */
	if (tmp[0])
	{
		self->particlesystems.array = lisys_calloc (tmp[0], sizeof (LIMdlParticleSystem));
		if (self->particlesystems.array == NULL)
			return 0;
		self->particlesystems.count = tmp[0];
	}

	/* Read particle systems. */
	for (i = 0 ; i < self->particlesystems.count ; i++)
	{
		if (!limdl_particle_system_read (self->particlesystems.array + i, reader))
			return 0;
	}

	return 1;
}

static int private_read_shapes (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->shapes.count = tmp;

	/* Read shapes. */
	if (self->shapes.count)
	{
		self->shapes.array = lisys_calloc (self->shapes.count, sizeof (LIMdlShape));
		if (self->shapes.array == NULL)
			return 0;
		for (i = 0 ; i < self->shapes.count ; i++)
		{
			if (!limdl_shape_read (self->shapes.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_vertices (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp;
	LIMdlVertex* vertex;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &tmp))
		return 0;
	self->vertices.count = tmp;

	/* Read vertices. */
	if (tmp)
	{
		self->vertices.array = lisys_calloc (tmp, sizeof (LIMdlVertex));
		if (self->vertices.array == NULL)
			return 0;
		self->vertices.count = tmp;
		for (i = 0 ; i < self->vertices.count ; i++)
		{
			vertex = self->vertices.array + i;
			vertex->weights[0] = 1.0f;
			if (!liarc_reader_get_float (reader, vertex->texcoord + 0) ||
				!liarc_reader_get_float (reader, vertex->texcoord + 1) ||
				!liarc_reader_get_float (reader, &vertex->normal.x) ||
				!liarc_reader_get_float (reader, &vertex->normal.y) ||
				!liarc_reader_get_float (reader, &vertex->normal.z) ||
				!liarc_reader_get_float (reader, &vertex->coord.x) ||
				!liarc_reader_get_float (reader, &vertex->coord.y) ||
				!liarc_reader_get_float (reader, &vertex->coord.z))
				return 0;
		}
	}

	return 1;
}

static int private_read_weights (
	LIMdlModel*  self,
	LIArcReader* reader)
{
	int i;
	uint32_t tmp[2];
	LIMdlWeightGroup* group;

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, tmp + 0))
		return 0;

	/* Read weight groups. */
	if (tmp[0])
	{
		self->weightgroups.array = lisys_calloc (tmp[0], sizeof (LIMdlWeightGroup));
		if (self->weightgroups.array == NULL)
			return 0;
		self->weightgroups.count = tmp[0];
		for (i = 0 ; i < self->weightgroups.count ; i++)
		{
			group = self->weightgroups.array + i;
			if (!liarc_reader_get_text (reader, "", &group->name) ||
				!liarc_reader_get_text (reader, "", &group->bone))
				return 0;
		}
	}

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, tmp + 1))
		return 0;
	if (tmp[1] != self->vertices.count)
	{
		lisys_error_set (EINVAL, "vertex and weight counts must be equal");
		return 0;
	}

	/* Read vertex weights. */
	if (tmp[1])
	{
		for (i = 0 ; i < self->vertices.count ; i++)
		{
			if (!private_read_vertex_weights (self, self->vertices.array + i, reader))
				return 0;
		}
	}

	return 1;
}

static int private_read_vertex_weights (
	LIMdlModel*  self,
	LIMdlVertex* vertex,
	LIArcReader* reader)
{
	uint32_t i;
	uint32_t j;
	uint32_t count;
	uint32_t group;
	float weight;
	float weights_tmp[LIMDL_VERTEX_WEIGHTS_MAX + 1];
	unsigned char bones_tmp[LIMDL_VERTEX_WEIGHTS_MAX + 1];

	/* Read header. */
	if (!liarc_reader_get_uint32 (reader, &count))
		return 0;

	/* Initialize empty weights. */
	for (i = 0 ; i < LIMDL_VERTEX_WEIGHTS_MAX + 1 ; i++)
	{
		weights_tmp[i] = 0.0f;
		bones_tmp[i] = 0;
	}

	/* Read weights. */
	/* We only save the most significant weights by doing in place insertion
	   sorting. Weights cannot be eliminated here because the bones using them
	   might become available only after a model merge. */
	for (i = 0 ; i < count ; i++)
	{
		if (!liarc_reader_get_uint32 (reader, &group) ||
		    !liarc_reader_get_float (reader, &weight))
			return 0;
		if (group >= self->weightgroups.count)
		{
			lisys_error_set (EINVAL, "weight group index out of bounds");
			return 0;
		}
		if (weight == 0.0f)
			continue;
		for (j = LIMDL_VERTEX_WEIGHTS_MAX ; j > 0 && weight > weights_tmp[j - 1] ; j--)
		{
			weights_tmp[j] = weights_tmp[j - 1];
			bones_tmp[j] = bones_tmp[j - 1];
		}
		bones_tmp[j] = group + 1;
		weights_tmp[j] = weight;
	}

	/* Normalize the weights. */
	weight = 0.0f;
	for (i = 0 ; i < LIMDL_VERTEX_WEIGHTS_MAX ; i++)
		weight += weights_tmp[i];
	if (weight > LIMAT_EPSILON)
	{
		for (i = 0 ; i < LIMDL_VERTEX_WEIGHTS_MAX ; i++)
			weights_tmp[i] /= weight;
	}
	else
	{
		weights_tmp[0] = 1.0f;
		bones_tmp[0] = 0;
	}

	/* Store the most significant weights to the vertex. */
	memcpy (vertex->weights, weights_tmp, LIMDL_VERTEX_WEIGHTS_MAX * sizeof (float));
	memcpy (vertex->bones, bones_tmp, LIMDL_VERTEX_WEIGHTS_MAX * sizeof (char));

	return 1;
}

static int private_write (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	if (!private_write_block (self, "lips/mdl", private_write_header, writer) ||
	    !private_write_block (self, "bou", private_write_bounds, writer) ||
	    !private_write_block (self, "mat", private_write_materials, writer) ||
	    !private_write_block (self, "ver", private_write_vertices, writer) ||
	    !private_write_block (self, "fac", private_write_faces, writer) ||
	    !private_write_block (self, "wei", private_write_weights, writer) ||
	    !private_write_block (self, "nod", private_write_nodes, writer) ||
	    !private_write_block (self, "ani", private_write_animations, writer) ||
	    !private_write_block (self, "hai", private_write_hairs, writer) ||
	    !private_write_block (self, "par", private_write_particles, writer) ||
	    !private_write_block (self, "sha", private_write_shapes, writer))
		return 0;
	return 1;
}

static int private_write_block (
	const LIMdlModel* self,
	const char*       name,
	LIMdlWriteFunc    func,
	LIArcWriter*      writer)
{
	int len;
	LIArcWriter* data;

	/* Write the block to a temporary buffer. */
	data = liarc_writer_new ();
	if (data == NULL)
		return 0;
	if (!func (self, data))
	{
		liarc_writer_free (data);
		return 0;
	}
	len = liarc_writer_get_length (data);
	if (!len)
	{
		liarc_writer_free (data);
		return 1;
	}

	/* Write the temporary to the main writer as a block. */
	if (!liarc_writer_append_string (writer, name) ||
	    !liarc_writer_append_nul (writer) ||
	    !liarc_writer_append_uint32 (writer, len) ||
	    !liarc_writer_append_raw (writer, liarc_writer_get_buffer (data), len))
	{
		liarc_writer_free (data);
		return 0;
	}
	liarc_writer_free (data);

	return 1;
}

static int private_write_animations (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	int j;
	LIMdlAnimation* animation;
	LIMatTransform* transform;

	/* Check if writing is needed. */
	if (!self->animations.count)
		return 1;

	/* Write animations. */
	if (!liarc_writer_append_uint32 (writer, self->animations.count))
		return 0;
	for (i = 0 ; i < self->animations.count ; i++)
	{
		animation = self->animations.array + i;
		if (!liarc_writer_append_string (writer, animation->name) ||
		    !liarc_writer_append_nul (writer) ||
		    !liarc_writer_append_uint32 (writer, animation->channels.count) ||
		    !liarc_writer_append_uint32 (writer, animation->length))
			return 0;
		for (j = 0 ; j < animation->channels.count ; j++)
		{
			if (!liarc_writer_append_string (writer, animation->channels.array[j]) ||
				!liarc_writer_append_nul (writer))
				return 0;
		}
		for (j = 0 ; j < animation->buffer.count ; j++)
		{
			transform = &animation->buffer.array[j].transform;
			if (!liarc_writer_append_float (writer, transform->position.x) ||
			    !liarc_writer_append_float (writer, transform->position.y) ||
			    !liarc_writer_append_float (writer, transform->position.z) ||
			    !liarc_writer_append_float (writer, transform->rotation.x) ||
			    !liarc_writer_append_float (writer, transform->rotation.y) ||
			    !liarc_writer_append_float (writer, transform->rotation.z) ||
			    !liarc_writer_append_float (writer, transform->rotation.w))
				return 0;
		}
	}

	return 1;
}

static int private_write_bounds (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	if (!liarc_writer_append_float (writer, self->bounds.min.x) ||
	    !liarc_writer_append_float (writer, self->bounds.min.y) ||
	    !liarc_writer_append_float (writer, self->bounds.min.z) ||
	    !liarc_writer_append_float (writer, self->bounds.max.x) ||
	    !liarc_writer_append_float (writer, self->bounds.max.y) ||
	    !liarc_writer_append_float (writer, self->bounds.max.z))
		return 0;
	return 1;
}

static int private_write_faces (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;

	/* Check if writing is needed. */
	if (!self->facegroups.count)
		return 1;

	if (!liarc_writer_append_uint32 (writer, self->facegroups.count))
		return 0;
	for (i = 0 ; i < self->facegroups.count ; i++)
	{
		if (!limdl_faces_write (self->facegroups.array + i, writer))
			return 0;
	}

	return 1;
}

static int private_write_hairs (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlHairs* hairs;

	/* Check if writing is needed. */
	if (!self->hairs.count)
		return 1;

	/* Write animations. */
	if (!liarc_writer_append_uint32 (writer, self->hairs.count))
		return 0;
	for (i = 0 ; i < self->hairs.count ; i++)
	{
		hairs = self->hairs.array + i;
		if (!limdl_hairs_write (hairs, writer))
			return 0;
	}

	return 1;
}

static int private_write_header (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	if (!liarc_writer_append_uint32 (writer, LIMDL_FORMAT_VERSION) ||
	    !liarc_writer_append_uint32 (writer, self->flags))
		return 0;
	return 1;
}

static int private_write_materials (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlMaterial* material;

	/* Check if writing is needed. */
	if (!self->materials.count)
		return 1;

	if (!liarc_writer_append_uint32 (writer, self->materials.count))
		return 0;
	for (i = 0 ; i < self->materials.count ; i++)
	{
		material = self->materials.array + i;
		if (!limdl_material_write (material, writer))
			return 0;
	}

	return 1;
}

static int private_write_nodes (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlNode* node;

	/* Check if writing is needed. */
	if (!self->nodes.count)
		return 1;

	/* Write nodes. */
	if (!liarc_writer_append_uint32 (writer, self->nodes.count))
		return 0;
	for (i = 0 ; i < self->nodes.count ; i++)
	{
		node = self->nodes.array[i];
		if (!limdl_node_write (node, writer))
			return 0;
	}

	return 1;
}

static int private_write_particles (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;

	/* Check if writing is needed. */
	if (!self->particlesystems.count)
		return 1;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->particlesystems.count))
		return 0;

	/* Write particle systems. */
	for (i = 0 ; i < self->particlesystems.count ; i++)
	{
		if (!limdl_particle_system_write (self->particlesystems.array + i, writer))
			return 0;
	}

	return 1;
}

static int private_write_shapes (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlShape* shape;

	/* Check if writing is needed. */
	if (!self->shapes.count)
		return 1;

	/* Write nodes. */
	if (!liarc_writer_append_uint32 (writer, self->shapes.count))
		return 0;
	for (i = 0 ; i < self->shapes.count ; i++)
	{
		shape = self->shapes.array + i;
		if (!limdl_shape_write (shape, writer))
			return 0;
	}

	return 1;
}

static int private_write_vertices (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	LIMdlVertex* vertex;

	/* Check if writing is needed. */
	if (!self->vertices.count)
		return 1;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->vertices.count))
		return 0;

	/* Write vertices. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		vertex = self->vertices.array + i;
		if (!liarc_writer_append_float (writer, vertex->texcoord[0]) ||
		    !liarc_writer_append_float (writer, vertex->texcoord[1]) ||
		    !liarc_writer_append_float (writer, vertex->normal.x) ||
		    !liarc_writer_append_float (writer, vertex->normal.y) ||
		    !liarc_writer_append_float (writer, vertex->normal.z) ||
		    !liarc_writer_append_float (writer, vertex->coord.x) ||
		    !liarc_writer_append_float (writer, vertex->coord.y) ||
		    !liarc_writer_append_float (writer, vertex->coord.z))
			return 0;
	}

	return 1;
}

static int private_write_weights (
	const LIMdlModel* self,
	LIArcWriter*      writer)
{
	int i;
	int j;
	int count;
	LIMdlVertex* vertex;
	LIMdlWeightGroup* group;

	/* Check if writing is needed. */
	if (!self->weightgroups.count)
		return 1;

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->weightgroups.count))
		return 0;

	/* Write weight groups. */
	for (i = 0 ; i < self->weightgroups.count ; i++)
	{
		group = self->weightgroups.array + i;
		liarc_writer_append_string (writer, group->name);
		liarc_writer_append_nul (writer);
		liarc_writer_append_string (writer, group->bone);
		liarc_writer_append_nul (writer);
	}

	/* Write header. */
	if (!liarc_writer_append_uint32 (writer, self->vertices.count))
		return 0;

	/* Write vertex weights. */
	for (i = 0 ; i < self->vertices.count ; i++)
	{
		vertex = self->vertices.array + i;
		for (count = 0 ; count < LIMDL_VERTEX_WEIGHTS_MAX ; count++)
		{
			if (!vertex->bones[count])
				break;
		}
		if (!liarc_writer_append_uint32 (writer, count))
			return 0;
		for (j = 0 ; j < count ; j++)
		{
			if (!liarc_writer_append_uint32 (writer, vertex->bones[j] - 1) ||
			    !liarc_writer_append_float (writer, vertex->weights[j]))
				return 0;
		}
	}

	return !writer->error;
}

/** @} */
/** @} */
