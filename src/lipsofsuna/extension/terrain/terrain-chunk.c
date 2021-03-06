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
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "terrain-chunk.h"

/**
 * \brief Creates a new terrain chunk.
 * \param size Chunk size along one axis in columns.
 * \return Terrain chunk, or NULL.
 */
LIExtTerrainChunk* liext_terrain_chunk_new (
	int size)
{
	LIExtTerrainChunk* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtTerrainChunk));
	if (self == NULL)
		return NULL;
	self->size = size;

	/* Allocate the chunk array. */
	self->columns = lisys_calloc (size * size, sizeof (LIExtTerrainColumn));
	if (self->columns == NULL)
	{
		liext_terrain_chunk_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the chunk.
 * \param self Terrain chunk.
 */
void liext_terrain_chunk_free (
	LIExtTerrainChunk* self)
{
	int i;

	if (self->columns != NULL)
	{
		for (i = 0 ; i < self->size * self->size ; i++)
			liext_terrain_column_clear (self->columns + i);
		lisys_free (self->columns);
	}
	if (self->model != NULL)
		limdl_model_free (self->model);
	lisys_free (self);
}

/**
 * \brief Draws a box of sticks at the given grid points.
 * \param self Terrain chunk.
 * \param grid_x1 Minimum X coordinate in grid units.
 * \param grid_z1 Minimum Z coordinate in grid units.
 * \param grid_x2 Maximum X coordinate in grid units.
 * \param grid_z2 Maximum Z coordinate in grid units.
 * \param world_y Y offset of the box in world units.
 * \param world_h Y height of the box in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Number of successful stick insertions.
 */
int liext_terrain_chunk_add_box (
	LIExtTerrainChunk*      self,
	int                     grid_x1,
	int                     grid_z1,
	int                     grid_x2,
	int                     grid_z2,
	float                   world_y,
	float                   world_h,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	int x;
	int z;
	int ret = 0;

	for (z = grid_z1 ; z <= grid_z2 ; z++)
	{
		for (x = grid_x1 ; x <= grid_x2 ; x++)
		{
			ret += liext_terrain_chunk_add_stick (self, x, z, world_y, world_h,
				material, filter_func, filter_data);
		}
	}

	return ret;
}

/**
 * \brief Draws a sloped box of sticks at the given grid points.
 * \param self Terrain chunk.
 * \param grid_x1 Minimum X coordinate in grid units.
 * \param grid_z1 Minimum Z coordinate in grid units.
 * \param grid_x2 Maximum X coordinate in grid units.
 * \param grid_z2 Maximum Z coordinate in grid units.
 * \param bot00 Y offset of the box in world units.
 * \param top00 Y height of the box in world units.
 * \param bot10 Y offset of the box in world units.
 * \param top10 Y height of the box in world units.
 * \param bot01 Y offset of the box in world units.
 * \param top01 Y height of the box in world units.
 * \param bot11 Y offset of the box in world units.
 * \param top11 Y height of the box in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Number of successful stick insertions.
 */
int liext_terrain_chunk_add_box_corners (
	LIExtTerrainChunk*      self,
	int                     grid_x1,
	int                     grid_z1,
	int                     grid_x2,
	int                     grid_z2,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	int x;
	int z;
	int sx;
	int sz;
	float b00, b10, b01, b11;
	float t00, t10, t01, t11;
	int ret = 0;

	sx = grid_x2 - grid_x1 + 1;
	sz = grid_z2 - grid_z1 + 1;
	for (z = 0 ; z < sz ; z++)
	{
		for (x = 0 ; x < sx ; x++)
		{
			b00 = limat_number_bilinear ((float) x    / sx, (float) z    / sz, bot00, bot10, bot01, bot11);
			b10 = limat_number_bilinear ((float)(x+1) / sx, (float) z    / sz, bot00, bot10, bot01, bot11);
			b01 = limat_number_bilinear ((float) x    / sx, (float)(z+1) / sz, bot00, bot10, bot01, bot11);
			b11 = limat_number_bilinear ((float)(x+1) / sx, (float)(z+1) / sz, bot00, bot10, bot01, bot11);
			t00 = limat_number_bilinear ((float) x    / sx, (float) z    / sz, top00, top10, top01, top11);
			t10 = limat_number_bilinear ((float)(x+1) / sx, (float) z    / sz, top00, top10, top01, top11);
			t01 = limat_number_bilinear ((float) x    / sx, (float)(z+1) / sz, top00, top10, top01, top11);
			t11 = limat_number_bilinear ((float)(x+1) / sx, (float)(z+1) / sz, top00, top10, top01, top11);
			ret += liext_terrain_chunk_add_stick_corners (self,
				grid_x1 + x, grid_z1 + z,
				b00, b10, b01, b11, t00, t10, t01, t11,
				material, filter_func, filter_data);
		}
	}

	return ret;
}

/**
 * \brief Draws a stick at the given grid point and Y offset.
 * \param self Terrain chunk.
 * \param column_x X coordinate of the column, in grid units within the chunk.
 * \param column_z Z coordinate of the column, in grid units within the chunk.
 * \param world_y Y offset of the stick in world units.
 * \param world_h Y height of the stick in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_chunk_add_stick (
	LIExtTerrainChunk*      self,
	int                     column_x,
	int                     column_z,
	float                   world_y,
	float                   world_h,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	const float slope[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	LIExtTerrainColumn* column;

	/* Add the stick to the column. */
	column = liext_terrain_chunk_get_column (self, column_x, column_z);
	if (!liext_terrain_column_add_stick (column, world_y, world_h, slope, slope, material, filter_func, filter_data))
		return 0;

	/* Update stamps. */
	/* The stamps of the neighbor columns are updated because face culling
	   may have changed due to the addition of the stick. */
	self->stamp++;
	if (column_x > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x - 1, column_z);
		column->stamp++;
	}
	if (column_x < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x + 1, column_z);
		column->stamp++;
	}
	if (column_z > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z - 1);
		column->stamp++;
	}
	if (column_z < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z + 1);
		column->stamp++;
	}

	return 1;
}

/**
 * \brief Draws a stick at the given grid point with the given vertex offsets.
 * \param self Terrain chunk.
 * \param column_x X coordinate of the column, in grid units within the chunk.
 * \param column_z Z coordinate of the column, in grid units within the chunk.
 * \param bot00 Bottom vertex Y coordinate, in world units.
 * \param bot10 Bottom vertex Y coordinate, in world units.
 * \param bot01 Bottom vertex Y coordinate, in world units.
 * \param bot11 Bottom vertex Y coordinate, in world units.
 * \param top00 Top vertex Y coordinate, in world units.
 * \param top10 Top vertex Y coordinate, in world units.
 * \param top01 Top vertex Y coordinate, in world units.
 * \param top11 Top vertex Y coordinate, in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_chunk_add_stick_corners (
	LIExtTerrainChunk*      self,
	int                     column_x,
	int                     column_z,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	LIExtTerrainColumn* column;

	/* Add the stick to the column. */
	column = liext_terrain_chunk_get_column (self, column_x, column_z);
	if (!liext_terrain_column_add_stick_corners (column, bot00, bot10, bot01, bot11, top00, top10, top01, top11, material, filter_func, filter_data))
		return 0;

	/* Update stamps. */
	/* The stamps of the neighbor columns are updated because face culling
	   may have changed due to the addition of the stick. */
	self->stamp++;
	if (column_x > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x - 1, column_z);
		column->stamp++;
	}
	if (column_x < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x + 1, column_z);
		column->stamp++;
	}
	if (column_z > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z - 1);
		column->stamp++;
	}
	if (column_z < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z + 1);
		column->stamp++;
	}

	return 1;
}

/**
 * \brief Builds the model of the chunk.
 * \param self Terrain chunk.
 * \param materials Terrain materials.
 * \param chunk_back Neighbour chunk used for culling.
 * \param chunk_front Neighbour chunk used for culling.
 * \param chunk_left Neighbour chunk used for culling.
 * \param chunk_right Neighbour chunk used for culling.
 * \param grid_size Grid size.
 * \param offset Offset of the chunk, in world units.
 * \return Nonzero on success, zero on failure.
 */
int liext_terrain_chunk_build_model (
	LIExtTerrainChunk*          self,
	const LIExtTerrainMaterial* materials,
	LIExtTerrainChunk*          chunk_back,
	LIExtTerrainChunk*          chunk_front,
	LIExtTerrainChunk*          chunk_left,
	LIExtTerrainChunk*          chunk_right,
	float                       grid_size,
	const LIMatVector*          offset)
{
	int i;
	int j;
	LIExtTerrainColumn* column;
	LIExtTerrainStick* sticks_back;
	LIExtTerrainStick* sticks_front;
	LIExtTerrainStick* sticks_left;
	LIExtTerrainStick* sticks_right;
	LIMatTransform transform;
	LIMatVector pos;
	LIMdlBuilder* builder;
	LIExtTerrainStick stick_infinite;
	LIExtTerrainColumn column_infinite;

	/* Check if changes are needed. */
	if (self->stamp == self->stamp_model)
		return 1;

	/* Create a very long column for culling. */
	liext_terrain_stick_clear (&stick_infinite);
	stick_infinite.height = 100000000000000000.0f;
	stick_infinite.material = 1;
	column_infinite.stamp = 0;
	column_infinite.stamp_model = 0;
	column_infinite.sticks = &stick_infinite;
	column_infinite.model = NULL;

	/* Build the column models. */
	for (j = 0 ; j < self->size ; j++)
	{
		for (i = 0 ; i < self->size ; i++)
		{
			/* Get the left neighbor sticks. */
			if (i > 0)
				column = self->columns + (i - 1) + j * self->size;
			else if (chunk_left != NULL)
				column = chunk_left->columns + (self->size - 1) + j * self->size;
			else
				column = &column_infinite;
			sticks_left = column->sticks;

			/* Get the right neighbor sticks. */
			if (i < self->size - 1)
				column = self->columns + (i + 1) + j * self->size;
			else if (chunk_right != NULL)
				column = chunk_right->columns + j * self->size;
			else
				column = &column_infinite;
			sticks_right = column->sticks;

			/* Get the front neighbor sticks. */
			if (j > 0)
				column = self->columns + i + (j - 1) * self->size;
			else if (chunk_front != NULL)
				column = chunk_front->columns + i + (self->size - 1) * self->size;
			else
				column = &column_infinite;
			sticks_front = column->sticks;

			/* Get the back neighbor sticks. */
			if (j < self->size - 1)
				column = self->columns + i + (j + 1) * self->size;
			else if (chunk_back != NULL)
				column = chunk_back->columns + i;
			else
				column = &column_infinite;
			sticks_back = column->sticks;

			/* Build the model. */
			pos = limat_vector_add (*offset, limat_vector_init (i * grid_size, 0.0f, j * grid_size));
			column = self->columns + i + j * self->size;
			if (!liext_terrain_column_build_model (column, materials,
				sticks_back, sticks_front, sticks_left, sticks_right, grid_size, &pos))
			{
				return 0;
			}
		}
	}

	/* Allocate the model. */
	if (self->model == NULL)
	{
		self->model = limdl_model_new ();
		if (self->model == NULL)
			return 0;
	}
	else
		limdl_model_clear (self->model);

	/* Allocate the model builder. */
	builder = limdl_builder_new (self->model);
	if (builder == NULL)
		return 0;

	/* Merge the models. */
	for (j = 0 ; j < self->size ; j++)
	{
		for (i = 0 ; i < self->size ; i++)
		{
			column = self->columns + i + j * self->size;
			if (column->model != NULL)
			{
				pos = limat_vector_add (*offset, limat_vector_init (i * grid_size, 0.0f, j * grid_size));
				transform = limat_transform_init (pos, limat_quaternion_identity ());
				limdl_builder_merge_model (builder, column->model, &transform);
			}
		}
	}

	/* Finish the build. */
	limdl_builder_finish (builder, 0);
	limdl_builder_free (builder);
	self->stamp_model = self->stamp;

	/* Sanity checks. */
	lisys_assert (self->model->materials.count <= 2);
	lisys_assert (self->model->lod.count == 1);
	lisys_assert (self->model->lod.array[0].face_groups.count <= 2);

	return 1;
}

/**
 * \brief Casts a sphere against the chunk and returns the hit fraction.
 * \param self Terrain chunk.
 * \param sphere_rel_cast_start Cast start position of the sphere, in grid units relative to the chunk origin.
 * \param sphere_rel_cast_end Cast end position of the sphere, in grid units relative to the chunk origin.
 * \param sphere_radius Sphere radius, in grid units.
 * \param result Return location for the hit fraction.
 * \return Nonzero if hit. Zero otherwise.
 */
int liext_terrain_chunk_cast_sphere (
	const LIExtTerrainChunk* self,
	const LIMatVector*       sphere_rel_cast_start,
	const LIMatVector*       sphere_rel_cast_end,
	float                    sphere_radius,
	LIExtTerrainCollision*   result)
{
	int i;
	int j;
	int min_x;
	int max_x;
	int min_z;
	int max_z;
	LIMatVector pos;
	LIMatVector cast1;
	LIMatVector cast2;
	LIExtTerrainColumn* column;
	LIExtTerrainCollision best;
	LIExtTerrainCollision frac;

	best.fraction = LIMAT_INFINITE;
	min_x = (int)(LIMAT_MIN (sphere_rel_cast_start->x, sphere_rel_cast_end->x) - sphere_radius);
	max_x = (int)(LIMAT_MAX (sphere_rel_cast_start->x, sphere_rel_cast_end->x) + sphere_radius);
	min_z = (int)(LIMAT_MIN (sphere_rel_cast_start->z, sphere_rel_cast_end->z) - sphere_radius);
	max_z = (int)(LIMAT_MAX (sphere_rel_cast_start->z, sphere_rel_cast_end->z) + sphere_radius);
	min_x = LIMAT_MAX (min_x, 0);
	max_x = LIMAT_MIN (max_x, self->size - 1);
	min_z = LIMAT_MAX (min_z, 0);
	max_z = LIMAT_MIN (max_z, self->size - 1);

	for (j = min_z ; j <= max_z ; j++)
	{
		for (i = min_x ; i <= max_x ; i++)
		{
			column = self->columns + i + j * self->size;
			pos = limat_vector_init (i, 0.0f, j);
			cast1 = limat_vector_subtract (*sphere_rel_cast_start, pos);
			cast2 = limat_vector_subtract (*sphere_rel_cast_end, pos);
			if (liext_terrain_column_cast_sphere (column, &cast1, &cast2, sphere_radius, &frac))
			{
				if (best.fraction > frac.fraction)
				{
					best = frac;
					best.x = i;
					best.z = j;
					best.point = limat_vector_add (best.point, pos);
				}
			}
		}
	}

	if (best.fraction > 1.0f)
		return 0;
	*result = best;
	return 1;
}

/**
 * \brief Clears the stick at the given grid point.
 * \param self Terrain chunk.
 * \param column_x X coordinate in grid units.
 * \param column_z Z coordinate in grid units.
 */
void liext_terrain_chunk_clear_column (
	LIExtTerrainChunk* self,
	int                column_x,
	int                column_z)
{
	LIExtTerrainColumn* column;

	/* Clear the column. */
	column = liext_terrain_chunk_get_column (self, column_x, column_z);
	if (column->sticks == NULL)
		return;
	liext_terrain_column_clear (column);

	/* Update stamps. */
	/* The stamps of the neighbor columns are updated because face culling
	   may have changed due to the removal of the sticks. */
	self->stamp++;
	column->stamp++;
	if (column_x > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x - 1, column_z);
		column->stamp++;
	}
	if (column_x < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x + 1, column_z);
		column->stamp++;
	}
	if (column_z > 0)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z - 1);
		column->stamp++;
	}
	if (column_z < self->size - 1)
	{
		column = liext_terrain_chunk_get_column (self, column_x, column_z + 1);
		column->stamp++;
	}
}

/**
 * \brief Clears the model of the chunk.
 * \param self Terrain chunk.
 */
void liext_terrain_chunk_clear_model (
	LIExtTerrainChunk* self)
{
	int i;

	for (i = 0 ; i < self->size * self->size ; i++)
	{
		liext_terrain_column_clear_model (self->columns + i);
	}
	if (self->model != NULL)
	{
		limdl_model_free (self->model);
		self->model = NULL;
		self->stamp_model = self->stamp - 1;
	}
}

/**
 * \brief Gets a column by the grid point within the chunk.
 * \param self Terrain chunk.
 * \param x X grid coordinate withing the chunk.
 * \param z Z grid coordinate withing the chunk.
 * \return Column.
 */
LIExtTerrainColumn* liext_terrain_chunk_get_column (
	LIExtTerrainChunk* self,
	int                x,
	int                z)
{
	return self->columns + x + z * self->size;
}

/**
 * \brief Gets the serialized data of the column.
 * \param self Terrain chunk.
 * \param column_x X coordinate in grid units.
 * \param column_z Z coordinate in grid units.
 * \param writer Archive writer.
 * \return Nonzero on success.
 */
int liext_terrain_chunk_get_column_data (
	LIExtTerrainChunk* self,
	int                column_x,
	int                column_z,
	LIArcWriter*       writer)
{
	LIExtTerrainColumn* column;

	column = liext_terrain_chunk_get_column (self, column_x, column_z);
	return liext_terrain_column_get_data (column, writer);
}

/**
 * \brief Sets the contents of the column from serialized data.
 * \param self Terrain chunk.
 * \param column_x X coordinate in grid units.
 * \param column_z Z coordinate in grid units.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_chunk_set_column_data (
	LIExtTerrainChunk* self,
	int                column_x,
	int                column_z,
	LIArcReader*       reader)
{
	LIExtTerrainColumn* column;

	column = liext_terrain_chunk_get_column (self, column_x, column_z);
	if (!liext_terrain_column_set_data (column, reader))
		return 0;
	self->stamp++;

	return 1;
}

/**
 * \brief Gets the serialized contents of the chunk.
 * \param self Terrain chunk.
 * \param writer Archive write.
 * \return Nonzero on success.
 */
int liext_terrain_chunk_get_data (
	LIExtTerrainChunk* self,
	LIArcWriter*       writer)
{
	int i;

	for (i = 0 ; i < self->size * self->size ; i++)
	{
		if (!liext_terrain_column_get_data (self->columns + i, writer))
			return 0;
	}

	return 1;
}

/**
 * \brief Sets the contents of the chunk from serialized data.
 * \param self Terrain chunk.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_chunk_set_data (
	LIExtTerrainChunk* self,
	LIArcReader*       reader)
{
	int i;

	self->stamp++;
	for (i = 0 ; i < self->size * self->size ; i++)
	{
		if (!liext_terrain_column_set_data (self->columns + i, reader))
			return 0;
		self->columns[i].stamp++;
	}

	return 1;
}

/**
 * \brief Gets the estimate of how much memory the chunk uses.
 * \param self Terrain chunk.
 * \return Memory consumption in bytes.
 */
int liext_terrain_chunk_get_memory_used (
	const LIExtTerrainChunk* self)
{
	int i;
	int total;

	total = sizeof (*self);
	for (i = 0 ; i < self->size * self->size ; i++)
		total += liext_terrain_column_get_memory_used (self->columns + i);
	if (self->model)
		total += limdl_model_get_memory (self->model);

	return total;
}

/** @} */
/** @} */
