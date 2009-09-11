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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxBuilder Builder
 * @{
 */

#include "voxel.h"
#include "voxel-builder.h"
#include "voxel-manager.h"
#include "voxel-material.h"
#include "voxel-private.h"
#include "voxel-sector.h"

#define PRIVATE_MISSING_MATERIAL 0xFFFFFFFF

static const int voxel_face_normals[6][3] =
{
	{ -1,  0,  0 },
	{  1,  0,  0 },
	{  0, -1,  0 },
	{  0,  1,  0 },
	{  0,  0, -1 },
	{  0,  0,  1 }
};

static int
private_build_tiles (livoxBuilder* self,
                     livoxBlock*   block,
                     int           bx,
                     int           by,
                     int           bz);

#ifndef LIVOX_DISABLE_GRAPHICS
static void
private_calculate_normal (livoxBuilder* self,
                          limatVector*  coord,
                          limatVector*  fnormal,
                          limatVector*  vnormal);

static int
private_insert_triangle (livoxBuilder* self,
                         int           id,
                         limatVector*  coords,
                         limatVector*  normals);
#endif

static int
private_insert_vertices (livoxBuilder* self,
                         int           material,
                         limatVector*  coords,
                         limatVector*  normals,
                         int           count);

/*****************************************************************************/

livoxBuilder*
livox_builder_new (livoxSector* sector)
{
	livoxBuilder* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (livoxBuilder));
	if (self == NULL)
		return NULL;
	self->sector = sector;

	/* Allocate rendering specific. */
#ifndef LIVOX_DISABLE_GRAPHICS
	if (sector->manager->render != NULL)
	{
		self->helpers.materials = lialg_u32dic_new ();
		if (self->helpers.materials == NULL)
		{
			livox_builder_free (self);
			return NULL;
		}
		self->helpers.normals = lialg_memdic_new ();
		if (self->helpers.normals == NULL)
		{
			livox_builder_free (self);
			return NULL;
		}
	}
#endif

	return self;
}

void
livox_builder_free (livoxBuilder* self)
{
	lialgMemdicIter iter;
	lialgU32dicIter iter1;
	livoxBuilderNormal* normal;
	livoxBuilderNormal* normal_next;
	livoxFaces* faces;

	if (self->helpers.normals != NULL)
	{
		LI_FOREACH_MEMDIC (iter, self->helpers.normals)
		{
			for (normal = iter.value ; normal != NULL ; normal = normal_next)
			{
				normal_next = normal->next;
				lisys_free (normal);
			}
		}
		lialg_memdic_free (self->helpers.normals);
	}
	if (self->helpers.materials != NULL)
	{
		LI_FOREACH_U32DIC (iter1, self->helpers.materials)
		{
			faces = iter1.value;
			lisys_free (faces->vertices.array);
			lisys_free (faces);
		}
		lialg_u32dic_free (self->helpers.materials);
	}
	if (self->helpers.shape != NULL)
		liphy_shape_free (self->helpers.shape);
	lisys_free (self->vertices.array);
	lisys_free (self);
}

int
livox_builder_build (livoxBuilder* self,
                     int           bx,
                     int           by,
                     int           bz)
{
#ifndef LIVOX_DISABLE_GRAPHICS
	int i;
	lialgU32dicIter iter;
	limatVector coord;
	limatVector normal0;
	limatVector normal1;
	livoxFaces* faces;
	livoxBuilderVertex* vertex;
#endif
	livoxBlock* block;

	/* Triangulate the block. */
	block = self->sector->blocks + LIVOX_BLOCK_INDEX (bx, by, bz);
	if (!private_build_tiles (self, block, bx, by, bz))
		return 0;

	/* Apply smooth normals and calculate bounding box. */
#ifndef LIVOX_DISABLE_GRAPHICS
	if (self->sector->manager->render != NULL && self->vertices.count)
	{
		LI_FOREACH_U32DIC (iter, self->helpers.materials)
		{
			faces = iter.value;
			vertex = faces->vertices.array;
			for (i = 0 ; i < faces->vertices.count ; i++)
			{
				coord = limat_vector_init (vertex->coord[0], vertex->coord[1], vertex->coord[2]);
				normal0 = limat_vector_init (vertex->normal[0], vertex->normal[1], vertex->normal[2]);
				private_calculate_normal (self, &coord, &normal0, &normal1);
				vertex->normal[0] = normal1.x;
				vertex->normal[1] = normal1.y;
				vertex->normal[2] = normal1.z;
				vertex++;
			}
		}
		self->aabb.min = limat_vector_init (
			self->sector->x * LIVOX_SECTOR_WIDTH + bx * LIVOX_BLOCK_WIDTH,
			self->sector->y * LIVOX_SECTOR_WIDTH + by * LIVOX_BLOCK_WIDTH,
			self->sector->z * LIVOX_SECTOR_WIDTH + bz * LIVOX_BLOCK_WIDTH);
		self->aabb.max = limat_vector_init (
			self->aabb.min.x + LIVOX_BLOCK_WIDTH,
			self->aabb.min.y + LIVOX_BLOCK_WIDTH,
			self->aabb.min.z + LIVOX_BLOCK_WIDTH);
	}
#endif

	return 1;
}

#ifndef LIVOX_DISABLE_GRAPHICS
lirndObject*
livox_builder_get_render (livoxBuilder* self)
{
	int i = 0;
	int count;
	lialgU32dicIter iter;
	lirndBuffer* buffers;
	lirndMaterial** materials;
	lirndObject* object;
	livoxFaces* faces;
	livoxMaterial* material;
	lirndApi* render;
	lirndFormat format =
	{
		sizeof (livoxBuilderVertex), 1,
		{ GL_FLOAT, GL_FLOAT, GL_FLOAT }, { 0, 0, 0 },
		GL_FLOAT, 2 * sizeof (float),
		GL_FLOAT, 5 * sizeof (float)
	};

	/* Allocate arrays. */
	render = self->sector->manager->renderapi;
	count = self->helpers.materials->size;
	buffers = lisys_calloc (count, sizeof (lirndBuffer));
	if (buffers == NULL)
		return NULL;
	materials = lisys_calloc (count, sizeof (lirndMaterial*));
	if (materials == NULL)
	{
		lisys_free (buffers);
		return NULL;
	}

	/* Allocate render data. */
	LI_FOREACH_U32DIC (iter, self->helpers.materials)
	{
		faces = iter.value;
		material = livox_manager_find_material (self->sector->manager, faces->material);
		if (material != NULL)
			materials[i] = render->lirnd_material_new_from_model (self->sector->manager->render, &material->model);
		else
			materials[i] = render->lirnd_material_new ();
		if (materials[i] == NULL)
			goto error;
		if (!render->lirnd_buffer_init (buffers + i, materials[i], &format,
		                                faces->vertices.array, faces->vertices.count))
		{
			render->lirnd_material_free (materials[i]);
			goto error;
		}
		i++;
	}

	/* Allocate render object. */
	object = render->lirnd_object_new_from_data (self->sector->manager->scene,
		0, &self->aabb, buffers, count, materials, count, NULL, 0);
	if (object == NULL)
		goto error;

	return object;

error:
	for (i-- ; i >= 0 ; i--)
	{
		render->lirnd_material_free (materials[i]);
		render->lirnd_buffer_free (buffers + i);
	}
	lisys_free (materials);
	lisys_free (buffers);

	return NULL;
}
#endif

liphyShape*
livox_builder_get_shape (livoxBuilder* self)
{
	liphyShape* shape;

	shape = self->helpers.shape;
	self->helpers.shape = NULL;

	return shape;
}

/*****************************************************************************/

static int
private_build_tiles (livoxBuilder* self,
                     livoxBlock*   block,
                     int           bx,
                     int           by,
                     int           bz)
{
	int i;
	int tx;
	int ty;
	int tz;
	int mat;
	limatVector blockoff;
	limatVector coords[6];
	limatVector normals[6];
	livoxVoxel tiles[LIVOX_TILES_PER_LINE + 2][LIVOX_TILES_PER_LINE + 2][LIVOX_TILES_PER_LINE + 2];
	limatVector verts[LIVOX_TILES_PER_LINE + 2][LIVOX_TILES_PER_LINE + 2][LIVOX_TILES_PER_LINE + 2];

	/* Get block offset. */
	blockoff = limat_vector_init (bx, by, bz);
	blockoff = limat_vector_multiply (blockoff, LIVOX_BLOCK_WIDTH);
	blockoff = limat_vector_add (blockoff, self->sector->origin);

	/* Get tiles. */
	/* FIXME: Fails at sector borders. */
	for (tz = -1 ; tz <= LIVOX_TILES_PER_LINE ; tz++)
	for (ty = -1 ; ty <= LIVOX_TILES_PER_LINE ; ty++)
	for (tx = -1 ; tx <= LIVOX_TILES_PER_LINE ; tx++)
	{
		int vx = LIVOX_TILES_PER_LINE * bx + tx;
		int vy = LIVOX_TILES_PER_LINE * by + ty;
		int vz = LIVOX_TILES_PER_LINE * bz + tz;
		if (vx >= 0 && vx < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE &&
			vy >= 0 && vy < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE &&
			vz >= 0 && vz < LIVOX_BLOCKS_PER_LINE * LIVOX_TILES_PER_LINE)
			tiles[tx + 1][ty + 1][tz + 1] = *livox_sector_get_voxel (self->sector, vx, vy, vz);
		else
			livox_voxel_init (&(tiles[tx + 1][ty + 1][tz + 1]), 0);
	}

	/* Get vertices. */
	for (tz = -1 ; tz <= LIVOX_TILES_PER_LINE ; tz++)
	for (ty = -1 ; ty <= LIVOX_TILES_PER_LINE ; ty++)
	for (tx = -1 ; tx <= LIVOX_TILES_PER_LINE ; tx++)
	{
		verts[tx + 1][ty + 1][tz + 1].x = blockoff.x +
			(tx + tiles[tx + 1][ty + 1][tz + 1].displacex / 127.0f) * LIVOX_TILE_WIDTH;
		verts[tx + 1][ty + 1][tz + 1].y = blockoff.y +
			(ty + tiles[tx + 1][ty + 1][tz + 1].displacey / 127.0f) * LIVOX_TILE_WIDTH;
		verts[tx + 1][ty + 1][tz + 1].z = blockoff.z +
			(tz + tiles[tx + 1][ty + 1][tz + 1].displacez / 127.0f) * LIVOX_TILE_WIDTH;
	}

	/* Generate faces. */
	for (tz = 1 ; tz <= LIVOX_TILES_PER_LINE ; tz++)
	for (ty = 1 ; ty <= LIVOX_TILES_PER_LINE ; ty++)
	for (tx = 1 ; tx <= LIVOX_TILES_PER_LINE ; tx++)
	{
		mat = livox_voxel_get_type (tiles[tx][ty][tz]);
		if (!mat)
			continue;

		/* Negative X. */
		if (!tiles[tx - 1][ty][tz].terrain)
		{
			coords[0] = verts[tx][ty][tz];
			coords[1] = verts[tx][ty + 1][tz + 1];
			coords[2] = verts[tx][ty][tz + 1];
			coords[3] = verts[tx][ty][tz];
			coords[4] = verts[tx][ty + 1][tz];
			coords[5] = verts[tx][ty + 1][tz + 1];
			for (i = 0 ; i < 6 ; i++)
				normals[i] = limat_vector_init (-1.0f, 0.0f, 0.0f);
			if (!private_insert_vertices (self, mat, coords, normals, 6))
				return 0;
		}

		/* Positive X. */
		if (!tiles[tx + 1][ty][tz].terrain)
		{
			coords[0] = verts[tx + 1][ty][tz];
			coords[1] = verts[tx + 1][ty][tz + 1];
			coords[2] = verts[tx + 1][ty + 1][tz + 1];
			coords[3] = verts[tx + 1][ty][tz];
			coords[4] = verts[tx + 1][ty + 1][tz + 1];
			coords[5] = verts[tx + 1][ty + 1][tz];
			for (i = 0 ; i < 6 ; i++)
				normals[i] = limat_vector_init (1.0f, 0.0f, 0.0f);
			if (!private_insert_vertices (self, mat, coords, normals, 6))
				return 0;
		}

		/* Negative Y. */
		if (!tiles[tx][ty - 1][tz].terrain)
		{
			coords[0] = verts[tx][ty][tz];
			coords[1] = verts[tx + 1][ty][tz + 1];
			coords[2] = verts[tx + 1][ty][tz];
			coords[3] = verts[tx][ty][tz];
			coords[4] = verts[tx][ty][tz + 1];
			coords[5] = verts[tx + 1][ty][tz + 1];
			for (i = 0 ; i < 6 ; i++)
				normals[i] = limat_vector_init (0.0f, -1.0f, 0.0f);
			if (!private_insert_vertices (self, mat, coords, normals, 6))
				return 0;
		}

		/* Positive Y. */
		if (!tiles[tx][ty + 1][tz].terrain)
		{
			coords[0] = verts[tx][ty + 1][tz];
			coords[1] = verts[tx + 1][ty + 1][tz];
			coords[2] = verts[tx + 1][ty + 1][tz + 1];
			coords[3] = verts[tx][ty + 1][tz];
			coords[4] = verts[tx + 1][ty + 1][tz + 1];
			coords[5] = verts[tx][ty + 1][tz + 1];
			for (i = 0 ; i < 6 ; i++)
				normals[i] = limat_vector_init (0.0f, 1.0f, 0.0f);
			if (!private_insert_vertices (self, mat, coords, normals, 6))
				return 0;
		}

		/* Negative Z. */
		if (!tiles[tx][ty][tz - 1].terrain)
		{
			coords[0] = verts[tx][ty][tz];
			coords[1] = verts[tx + 1][ty + 1][tz];
			coords[2] = verts[tx + 1][ty][tz];
			coords[3] = verts[tx][ty][tz];
			coords[4] = verts[tx][ty + 1][tz];
			coords[5] = verts[tx + 1][ty + 1][tz];
			for (i = 0 ; i < 6 ; i++)
				normals[i] = limat_vector_init (0.0f, 0.0f, -1.0f);
			if (!private_insert_vertices (self, mat, coords, normals, 6))
				return 0;
		}

		/* Positive Z. */
		if (!tiles[tx][ty][tz + 1].terrain)
		{
			coords[0] = verts[tx][ty][tz + 1];
			coords[1] = verts[tx + 1][ty][tz + 1];
			coords[2] = verts[tx + 1][ty + 1][tz + 1];
			coords[3] = verts[tx][ty][tz + 1];
			coords[4] = verts[tx + 1][ty + 1][tz + 1];
			coords[5] = verts[tx][ty + 1][tz + 1];
			for (i = 0 ; i < 6 ; i++)
				normals[i] = limat_vector_init (0.0f, 0.0f, 1.0f);
			if (!private_insert_vertices (self, mat, coords, normals, 6))
				return 0;
		}
	}
	if (!self->vertices.count)
		return 1;

	/* Create collision shape. */
	self->helpers.shape = liphy_shape_new_concave (self->sector->manager->physics,
		self->vertices.array, self->vertices.count);
	if (self->helpers.shape == NULL)
		return 0;

	return 1;
}

#ifndef LIVOX_DISABLE_GRAPHICS
static void
private_calculate_normal (livoxBuilder* self,
                          limatVector*  coord,
                          limatVector*  fnormal,
                          limatVector*  vnormal)
{
	int count;
	limatVector normal;
	livoxBuilderNormal* lookup;

	count = 1;
	normal = *fnormal;

	/* Find normal list. */
	lookup = lialg_memdic_find (self->helpers.normals, coord, sizeof (limatVector));
	assert (lookup != NULL);
	if (lookup == NULL)
	{
		*vnormal = *fnormal;
		return;
	}

	/* Calculate sum of suitable normals. */
	for ( ; lookup != NULL ; lookup = lookup->next)
	{
		if (limat_vector_dot (*fnormal, lookup->normal) > 0.1f)
		{
			normal = limat_vector_add (normal, lookup->normal);
			count++;
		}
	}

	/* Calculate average value. */
	normal = limat_vector_multiply (normal, 1.0f / count);
	*vnormal = normal;
}
#endif

#ifndef LIVOX_DISABLE_GRAPHICS
static int
private_insert_triangle (livoxBuilder* self,
                         int           id,
                         limatVector*  coords,
                         limatVector*  normals)
{
	int i;
	float scale;
	limatVector u;
	limatVector v;
	livoxFaces* faces;
	livoxMaterial* mat;
	livoxBuilderVertex* tmp;

	/* Check for existing. */
	mat = livox_manager_find_material (self->sector->manager, id);
	if (mat == NULL)
	{
		scale = 0.05f;
		id = -1;
	}
	else
		scale = mat->scale;
	faces = lialg_u32dic_find (self->helpers.materials, id);

	/* Create new material. */
	if (faces == NULL)
	{
		faces = lisys_calloc (1, sizeof (livoxFaces));
		if (faces == NULL)
			return 0;
		faces->material = id;
		if (!lialg_u32dic_insert (self->helpers.materials, id, faces))
		{
			lisys_free (faces);
			return 0;
		}
	}

	/* Calculate tangent and bitangent. */
	if (LI_ABS (normals->x) < LI_ABS (normals->y) && LI_ABS (normals->x) < LI_ABS (normals->z))
		u = limat_vector_init (1.0f, 0.0f, 0.0f);
	else if (LI_ABS (normals->y) < LI_ABS (normals->z))
		u = limat_vector_init (1.0f, 1.0f, 0.0f);
	else
		u = limat_vector_init (0.0f, 0.0f, 1.0f);
	v = limat_vector_cross (u, *normals);

	/* Reallocate vertices. */
	id = faces->vertices.count;
	tmp = realloc (faces->vertices.array, (id + 3) * sizeof (livoxBuilderVertex));
	if (tmp == NULL)
		return 0;
	faces->vertices.array = tmp;
	faces->vertices.count += 3;

	/* Insert vertices. */
	for (i = 0 ; i < 3 ; i++, id++)
	{
		faces->vertices.array[id].texcoord[0] = scale * limat_vector_dot (coords[i], u);
		faces->vertices.array[id].texcoord[1] = scale * limat_vector_dot (coords[i], v);
		faces->vertices.array[id].normal[0] = normals[i].x;
		faces->vertices.array[id].normal[1] = normals[i].y;
		faces->vertices.array[id].normal[2] = normals[i].z;
		faces->vertices.array[id].coord[0] = coords[i].x;
		faces->vertices.array[id].coord[1] = coords[i].y;
		faces->vertices.array[id].coord[2] = coords[i].z;
	}

	return 1;
}
#endif

static int
private_insert_vertices (livoxBuilder* self,
                         int           material,
                         limatVector*  coords,
                         limatVector*  normals,
                         int           count)
{
	int i;
	int j;
	limatVector coord;
	limatVector normal;
	limatVector* tmp;
	livoxBuilderNormal* lookup0;
	livoxBuilderNormal* lookup1;

	/* Insert collision vertices. */
	tmp = realloc (self->vertices.array, (self->vertices.count + count) * sizeof (limatVector));
	if (tmp == NULL)
		return 0;
	memcpy (tmp + self->vertices.count, coords, count * sizeof (limatVector));
	self->vertices.array = tmp;
	self->vertices.count += count;

	/* Insert model vertices. */
#ifndef LIVOX_DISABLE_GRAPHICS
	if (self->sector->manager->render != NULL)
	{
		for (i = 0 ; i < count ; i += 3)
		{
			/* Insert model triangle. */
			if (!private_insert_triangle (self, material, coords + i, normals + i))
				return 0;

			/* Create normal lookup. */
			normal = normals[i];
			for (j = 0 ; j < 3 ; j++)
			{
				coord = coords[i + j];
				lookup0 = lialg_memdic_find (self->helpers.normals, &coord, sizeof (limatVector));
				if (lookup0 != NULL)
				{
					lookup1 = lisys_malloc (sizeof (livoxBuilderNormal));
					if (lookup1 == NULL)
						continue;
					lookup1->next = lookup0->next;
					lookup1->normal = normal;
					lookup0->next = lookup1;
				}
				else
				{
					lookup1 = lisys_malloc (sizeof (livoxBuilderNormal));
					if (lookup1 == NULL)
						continue;
					lookup1->next = NULL;
					lookup1->normal = normal;
					if (!lialg_memdic_insert (self->helpers.normals, &coord, sizeof (limatVector), lookup1))
						lisys_free (lookup1);
				}
			}
		}
	}
#endif

	return 1;
}

/** @} */
/** @} */

