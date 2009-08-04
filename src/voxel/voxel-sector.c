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
 * \addtogroup livoxSector Sector
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "voxel-builder.h"
#include "voxel-manager.h"
#include "voxel-sector.h"
#include "voxel-private.h"

#define LIVOX_ERASE_SHIFT (0.25f * LIVOX_TILE_WIDTH)
#define LIVOX_TILES_PER_SECLINE (LIVOX_TILES_PER_LINE * LIVOX_BLOCKS_PER_LINE)

static int
private_build_block (livoxSector* self,
                     int          x,
                     int          y,
                     int          z);

/*****************************************************************************/

/**
 * \brief Creates a new sector.
 *
 * \param engine Engine.
 * \param id Sector number.
 * \return New sector or NULL.
 */
livoxSector*
livox_sector_new (livoxManager* manager,
                  uint32_t      id)
{
	livoxSector* self;

	/* Allocate self. */
	self = calloc (1, sizeof (livoxSector));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->manager = manager;
	self->x = id % LIVOX_SECTORS_PER_LINE;
	self->y = id / LIVOX_SECTORS_PER_LINE % LIVOX_SECTORS_PER_LINE;
	self->z = id / LIVOX_SECTORS_PER_LINE / LIVOX_SECTORS_PER_LINE;
	self->origin.x = LIVOX_SECTOR_WIDTH * self->x;
	self->origin.y = LIVOX_SECTOR_WIDTH * self->y;
	self->origin.z = LIVOX_SECTOR_WIDTH * self->z;

	/* Insert to manager. */
	if (!lialg_u32dic_insert (manager->sectors, id, self))
	{
		free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the sector.
 *
 * \param self Sector.
 */
void
livox_sector_free (livoxSector* self)
{
	int i;

	/* Free blocks. */
	for (i = 0 ; i < LIVOX_BLOCKS_PER_SECTOR ; i++)
		livox_block_free (self->blocks + i, self->manager);

	free (self);
}

int
livox_sector_build_block (livoxSector* self,
                          int          x,
                          int          y,
                          int          z)
{
	return private_build_block (self, x, y, z);
}

/**
 * \brief Erases terrain inside the box.
 *
 * \param self Sector.
 * \param box Bounding box relative to the origin of the sector.
 */
void
livox_sector_erase_aabb (livoxSector*     self,
                         const limatAabb* box)
{
	int i;
	int x;
	int y;
	int z;
	limatAabb block;
	limatAabb child;

	for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
	{
		block.min = limat_vector_init (
			x * LIVOX_BLOCK_WIDTH,
			y * LIVOX_BLOCK_WIDTH,
			z * LIVOX_BLOCK_WIDTH);
		block.max = limat_vector_init (
			(x + 1) * LIVOX_BLOCK_WIDTH,
			(y + 1) * LIVOX_BLOCK_WIDTH,
			(z + 1) * LIVOX_BLOCK_WIDTH);
		if (limat_aabb_intersects_aabb (box, &block))
		{
			child.min = limat_vector_subtract (box->min, block.min);
			child.max = limat_vector_subtract (box->max, block.min);
			if (livox_block_erase_aabb (self->blocks + i, &child))
				self->dirty = 1;
		}
	}
}

/**
 * \brief Erases the voxel fragment closest to the passed point.
 *
 * \param self Sector.
 * \param point Point in sector space.
 * \return Nonzero on success, zero if the nearby voxels were all empty.
 */
int
livox_sector_erase_point (livoxSector*       self,
                          const limatVector* point)
{
	int x, y, z;
	int xb, yb, zb;
	int xc, yc, zc;
	int xt, yt, zt;
	int best;
	int bestx;
	int besty;
	int bestz;
	int mask;
	int type;
	int shape;
	float d0;
	float d1;
	float dist0;
	float dist1;
	limatVector off;
	livoxVoxel voxel;

	/* Find hit voxel. */
	x = (int)(point->x / LIVOX_TILE_WIDTH);
	y = (int)(point->y / LIVOX_TILE_WIDTH);
	z = (int)(point->z / LIVOX_TILE_WIDTH);
	if (x < 0 || x >= LIVOX_TILES_PER_SECLINE ||
	    y < 0 || y >= LIVOX_TILES_PER_SECLINE ||
	    z < 0 || z >= LIVOX_TILES_PER_SECLINE)
		return 0;

	/* Find closest deletable corner. */
	/* We try to favor the voxel that was hit by using two distance checks,
	   one for the closest corner and another for the closest voxel. */
	best = 0;
	bestx = x;
	besty = y;
	bestz = z;
	dist0 = 10.0E10f;
	dist1 = 10.0E10f;
	for (zt = LI_MAX (0, z - 1) ; zt <= z + 1 && zt < LIVOX_TILES_PER_SECLINE ; zt++)
	for (yt = LI_MAX (0, y - 1) ; yt <= y + 1 && yt < LIVOX_TILES_PER_SECLINE ; yt++)
	for (xt = LI_MAX (0, x - 1) ; xt <= x + 1 && xt < LIVOX_TILES_PER_SECLINE ; xt++)
	{
		/* Get voxel information. */
		voxel = livox_sector_get_voxel (self, xt, yt, zt);
		shape = livox_voxel_get_shape (voxel);

		/* Get distance to voxel center. */
		off.x = (xt + 0.5f) * LIVOX_TILE_WIDTH;
		off.y = (yt + 0.5f) * LIVOX_TILE_WIDTH;
		off.z = (zt + 0.5f) * LIVOX_TILE_WIDTH;
		d1 = limat_vector_get_length (limat_vector_subtract (*point, off));

		/* Check if empty. */
		if (!shape)
			continue;

		/* Find closest corner. */
		mask = 1;
		for (zc = 0 ; zc < 2 ; zc++)
		for (yc = 0 ; yc < 2 ; yc++)
		for (xc = 0 ; xc < 2 ; xc++, mask <<= 1)
		{
			if (shape & mask)
			{
				off.x = (xt + xc) * LIVOX_TILE_WIDTH;
				off.y = (yt + yc) * LIVOX_TILE_WIDTH;
				off.z = (zt + zc) * LIVOX_TILE_WIDTH;
				d0 = limat_vector_get_length (limat_vector_subtract (*point, off));
				if (!best || d0 < dist0 || (d0 == dist0 && d1 < dist1))
				{
					best = mask;
					bestx = xt;
					besty = yt;
					bestz = zt;
					dist0 = d0;
					dist1 = d1;
				}
			}
		}
	}
	if (!best)
		return 0;

	/* Get voxel information. */
	voxel = livox_sector_get_voxel (self, bestx, besty, bestz);
	type = livox_voxel_get_type (voxel);
	shape = livox_voxel_get_shape (voxel);

	/* Clear the found corner. */
	xb = x / LIVOX_TILES_PER_LINE;
	yb = y / LIVOX_TILES_PER_LINE;
	zb = z / LIVOX_TILES_PER_LINE;
	voxel = livox_voxel_init (shape & (~best), type);
	livox_sector_set_voxel (self, bestx, besty, bestz, voxel);
	self->blocks[LIVOX_BLOCK_INDEX (xb, yb, zb)].stamp++;
	self->dirty = 1;

	return 1;
}

/**
 * \brief Erases terrain inside the sphere.
 *
 * \param self Sector.
 * \param center Center of the sphere relative to the origin of the sector.
 * \param radius Radius of the sphere.
 */
void
livox_sector_erase_sphere (livoxSector*       self,
                           const limatVector* center,
                           float              radius)
{
	int i;
	int x;
	int y;
	int z;
	float r;
	limatVector block;
	limatVector dist;

	r = radius + LIVOX_BLOCK_WIDTH;
	for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
	{
		block = limat_vector_init (
			(x + 0.5f) * LIVOX_BLOCK_WIDTH,
			(y + 0.5f) * LIVOX_BLOCK_WIDTH,
			(z + 0.5f) * LIVOX_BLOCK_WIDTH);
		dist = limat_vector_subtract (*center, block);
		if (limat_vector_dot (dist, dist) < r * r)
		{
			block = limat_vector_init (
				x * LIVOX_BLOCK_WIDTH,
				y * LIVOX_BLOCK_WIDTH,
				z * LIVOX_BLOCK_WIDTH);
			block = limat_vector_subtract (*center, block);
			if (livox_block_erase_sphere (self->blocks + i, &block, radius))
				self->dirty = 1;
		}
	}
}

/**
 * \brief Fills the sector with the given terrain type.
 *
 * \param self Sector.
 * \param terrain Terrain type.
 */
void
livox_sector_fill (livoxSector* self,
                   livoxVoxel   terrain)
{
	int x;
	int y;
	int z;
	int i = 0;

	for (z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
	{
		for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
		{
			for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++)
			{
				livox_block_fill (self->blocks + i, self->manager, terrain);
				i++;
			}
		}
	}
	self->dirty = 1;
}

/**
 * \brief Fills a sphere with the given terrain type.
 *
 * \param self Sector.
 * \param box Bounding box relative to the origin of the sector.
 * \param terrain Terrain type.
 */
void
livox_sector_fill_aabb (livoxSector*     self,
                        const limatAabb* box,
                        livoxVoxel       terrain)
{
	int i;
	int x;
	int y;
	int z;
	limatAabb block;
	limatAabb child;

	for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
	{
		block.min = limat_vector_init (
			x * LIVOX_BLOCK_WIDTH,
			y * LIVOX_BLOCK_WIDTH,
			z * LIVOX_BLOCK_WIDTH);
		block.max = limat_vector_init (
			(x + 1) * LIVOX_BLOCK_WIDTH,
			(y + 1) * LIVOX_BLOCK_WIDTH,
			(z + 1) * LIVOX_BLOCK_WIDTH);
		if (limat_aabb_intersects_aabb (box, &block))
		{
			child.min = limat_vector_subtract (box->min, block.min);
			child.max = limat_vector_subtract (box->max, block.min);
			if (livox_block_fill_aabb (self->blocks + i, &child, terrain))
				self->dirty = 1;
		}
	}
}

/**
 * \brief Fills the voxel fragment closest to the passed point.
 *
 * \param self Sector.
 * \param point Point in sector space.
 * \param terrain Terrain type.
 * \return Nonzero on success, zero if the nearby voxels were all empty.
 */
int
livox_sector_fill_point (livoxSector*       self,
                         const limatVector* point,
                         int                terrain)
{
	int x, y, z;
	int xb, yb, zb;
	int xc, yc, zc;
	int xt, yt, zt;
	int best;
	int bestx;
	int besty;
	int bestz;
	int mask;
	int shape;
	float d0;
	float d1;
	float dist0;
	float dist1;
	limatVector off;
	livoxVoxel voxel;

	/* Find hit voxel. */
	x = (int)(point->x / LIVOX_TILE_WIDTH);
	y = (int)(point->y / LIVOX_TILE_WIDTH);
	z = (int)(point->z / LIVOX_TILE_WIDTH);
	if (x < 0 || x >= LIVOX_TILES_PER_SECLINE ||
	    y < 0 || y >= LIVOX_TILES_PER_SECLINE ||
	    z < 0 || z >= LIVOX_TILES_PER_SECLINE)
		return 0;

	/* Find closest deletable corner. */
	/* We try to favor the voxel that was hit by using two distance checks,
	   one for the closest corner and another for the closest voxel. */
	best = 0;
	bestx = x;
	besty = y;
	bestz = z;
	dist0 = 10.0E10f;
	dist1 = 10.0E10f;
	for (zt = LI_MAX (0, z - 1) ; zt <= z + 1 && zt < LIVOX_TILES_PER_SECLINE ; zt++)
	for (yt = LI_MAX (0, y - 1) ; yt <= y + 1 && yt < LIVOX_TILES_PER_SECLINE ; yt++)
	for (xt = LI_MAX (0, x - 1) ; xt <= x + 1 && xt < LIVOX_TILES_PER_SECLINE ; xt++)
	{
		/* Get voxel information. */
		voxel = livox_sector_get_voxel (self, xt, yt, zt);
		shape = livox_voxel_get_shape (voxel);

		/* Get distance to voxel center. */
		off.x = (xt + 0.5f) * LIVOX_TILE_WIDTH;
		off.y = (yt + 0.5f) * LIVOX_TILE_WIDTH;
		off.z = (zt + 0.5f) * LIVOX_TILE_WIDTH;
		d1 = limat_vector_get_length (limat_vector_subtract (*point, off));

		/* Check if not empty. */
		if (shape)
			continue;

		/* Find closest empty corner. */
		mask = 1;
		for (zc = 0 ; zc < 2 ; zc++)
		for (yc = 0 ; yc < 2 ; yc++)
		for (xc = 0 ; xc < 2 ; xc++, mask <<= 1)
		{
			if (!(shape & mask))
			{
				off.x = (xt + xc) * LIVOX_TILE_WIDTH;
				off.y = (yt + yc) * LIVOX_TILE_WIDTH;
				off.z = (zt + zc) * LIVOX_TILE_WIDTH;
				d0 = limat_vector_get_length (limat_vector_subtract (*point, off));
				if (!best || d0 < dist0 || (d0 == dist0 && d1 < dist1))
				{
					best = mask;
					bestx = xt;
					besty = yt;
					bestz = zt;
					dist0 = d0;
					dist1 = d1;
				}
			}
		}
	}
	if (!best)
		return 0;

	/* Get voxel information. */
	voxel = livox_sector_get_voxel (self, bestx, besty, bestz);
	shape = livox_voxel_get_shape (voxel);

	/* Clear the found corner. */
	xb = x / LIVOX_TILES_PER_LINE;
	yb = y / LIVOX_TILES_PER_LINE;
	zb = z / LIVOX_TILES_PER_LINE;
	voxel = livox_voxel_init (shape | best, terrain);
	livox_sector_set_voxel (self, bestx, besty, bestz, voxel);
	self->blocks[LIVOX_BLOCK_INDEX (xb, yb, zb)].stamp++;
	self->dirty = 1;

	return 1;
}

/**
 * \brief Fills a sphere with the given terrain type.
 *
 * \param self Sector.
 * \param center Center of the sphere relative to the origin of the sector.
 * \param radius Radius of the sphere.
 * \param terrain Terrain type.
 */
void
livox_sector_fill_sphere (livoxSector*       self,
                          const limatVector* center,
                          float              radius,
                          livoxVoxel         terrain)
{
	int i;
	int x;
	int y;
	int z;
	float r;
	limatVector block;
	limatVector dist;

	r = radius + LIVOX_BLOCK_WIDTH;
	for (i = z = 0 ; z < LIVOX_BLOCKS_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_BLOCKS_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_BLOCKS_PER_LINE ; x++, i++)
	{
		block = limat_vector_init (
			(x + 0.5f) * LIVOX_BLOCK_WIDTH,
			(y + 0.5f) * LIVOX_BLOCK_WIDTH,
			(z + 0.5f) * LIVOX_BLOCK_WIDTH);
		dist = limat_vector_subtract (*center, block);
		if (limat_vector_dot (dist, dist) < r * r)
		{
			block = limat_vector_init (
				x * LIVOX_BLOCK_WIDTH,
				y * LIVOX_BLOCK_WIDTH,
				z * LIVOX_BLOCK_WIDTH);
			block = limat_vector_subtract (*center, block);
			if (livox_block_fill_sphere (self->blocks + i, &block, radius, terrain))
				self->dirty = 1;
		}
	}
}

/**
 * \brief Reads terrain data from the database.
 *
 * \param self Sector.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
livox_sector_read (livoxSector* self,
                   liarcSql*    sql)
{
	int i;
	int id;
	int size;
	const char* query;
	const void* bytes;
	liReader* reader;
	livoxBlock* block;
	sqlite3_stmt* statement;

	/* Prepare statement. */
	id = LIVOX_SECTOR_INDEX (self->x, self->y, self->z);
	query = "SELECT data FROM voxel_sectors WHERE id=?;";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		return 0;
	}
	if (sqlite3_bind_int (statement, 1, id) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Execute statement. */
	if (sqlite3_step (statement) != SQLITE_ROW)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}

	/* Read columns. */
	bytes = sqlite3_column_blob (statement, 0);
	size = sqlite3_column_bytes (statement, 0);
	reader = li_reader_new (bytes, size);
	if (reader == NULL)
	{
		sqlite3_finalize (statement);
		return 0;
	}

	/* Deserialize terrain. */
	for (i = 0 ; i < LIVOX_BLOCKS_PER_SECTOR ; i++)
	{
		block = livox_sector_get_block (self, i);
		if (!livox_block_read (block, self->manager, reader))
		{
			sqlite3_finalize (statement);
			return 0;
		}
	}
	sqlite3_finalize (statement);

	/* Force rebuild. */
	self->dirty = 1;

	return 1;
}

/**
 * \brief Called once per tick to update the status of the sector.
 *
 * \param self Sector.
 * \param secs Number of seconds since the last update.
 */
void
livox_sector_update (livoxSector* self,
                     float        secs)
{
}

/**
 * \brief Writes the terrain data of the sector to the database.
 *
 * \param self Sector.
 * \param sql Database.
 * \return Nonzero on success.
 */
int
livox_sector_write (livoxSector* self,
                    liarcSql*    sql)
{
	int i;
	int id;
	int col = 1;
	const char* query;
	sqlite3_stmt* statement;
	liarcWriter* writer;
	livoxBlock* block;

	id = LIVOX_SECTOR_INDEX (self->x, self->y, self->z);

	/* Don't save empty sectors. */
	if (livox_sector_get_empty (self))
	{
		query = "DELETE FROM voxel_sectors WHERE id=?;";
		if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
			return 0;
		}
		if (sqlite3_bind_int (statement, col++, id) != SQLITE_OK)
		{
			lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		if (sqlite3_step (statement) != SQLITE_DONE)
		{
			lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
			sqlite3_finalize (statement);
			return 0;
		}
		sqlite3_finalize (statement);
		return 1;
	}

	/* Serialize terrain. */
	writer = liarc_writer_new ();
	if (writer == NULL)
		return 0;
	for (i = 0 ; i < LIVOX_BLOCKS_PER_SECTOR ; i++)
	{
		block = livox_sector_get_block (self, i);
		livox_block_write (block, writer);
	}

	/* Prepare statement. */
	query = "INSERT OR REPLACE INTO voxel_sectors (id,data) VALUES (?,?);";
	if (sqlite3_prepare_v2 (sql, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (sql));
		liarc_writer_free (writer);
		return 0;
	}

	/* Bind values. */
	if (sqlite3_bind_int (statement, col++, id) != SQLITE_OK ||
	    sqlite3_bind_blob (statement, col++, writer->memory.buffer, writer->memory.length, SQLITE_TRANSIENT) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		liarc_writer_free (writer);
		return 0;
	}
	liarc_writer_free (writer);

	/* Write values. */
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (sql));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

/**
 * \brief Gets a voxel block.
 *
 * \param self Sector.
 * \param index Block index.
 * \return Block.
 */
livoxBlock*
livox_sector_get_block (livoxSector* self,
                        int          index)
{
	assert (index >= 0);
	assert (index < LIVOX_BLOCKS_PER_SECTOR);

	return self->blocks + index;
}

/**
 * \brief Gets the bounding box of the sector.
 *
 * \param self Sector.
 * \param result Return location for the bounding box.
 */
void
livox_sector_get_bounds (const livoxSector* self,
                         limatAabb*         result)
{
	limatVector min;
	limatVector max;

	min = self->origin;
	max = limat_vector_init (LIVOX_SECTOR_WIDTH, LIVOX_SECTOR_WIDTH, LIVOX_SECTOR_WIDTH);
	max = limat_vector_add (min, max);
	limat_aabb_init_from_points (result, &min, &max);
}

/**
 * \brief Returns nonzero if the sector is dirty.
 *
 * \param self Sector.
 * \return Nonzero if dirty.
 */
int
livox_sector_get_dirty (const livoxSector* self)
{
	return self->dirty;
}

/**
 * \brief Returns nonzero if the sector is dirty.
 *
 * \param self Sector.
 * \return Nonzero if dirty.
 */
void
livox_sector_set_dirty (livoxSector* self,
                        int          value)
{
	self->dirty = value;
}

/**
 * \brief Checks if the sector contains no terrain.
 *
 * \param self Sector.
 * \return Nonzero if the sector is empty.
 */
int
livox_sector_get_empty (const livoxSector* self)
{
	int i;

	for (i = 0 ; i < LIVOX_BLOCKS_PER_SECTOR ; i++)
	{
		if (self->blocks[i].type != LIVOX_BLOCK_TYPE_FULL ||
		    self->blocks[i].full.terrain != 0)
			return 0;
	}

	return 1;
}

/**
 * \brief Gets the offset of the sector in the world in voxels.
 *
 * \param self Sector.
 * \param x Return location for the offset.
 * \param t Return location for the offset.
 * \param z Return location for the offset.
 */
void
livox_sector_get_offset (const livoxSector* self,
                         int*               x,
                         int*               y,
                         int*               z)
{
	*x = self->x;
	*y = self->y;
	*z = self->z;
}

/**
 * \brief Gets the origin of the sector.
 *
 * \param self Sector.
 * \param result Return location for the position vector.
 */
void
livox_sector_get_origin (const livoxSector* self,
                         limatVector*       result)
{
	*result = self->origin;
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \return Terrain type or zero.
 */
livoxVoxel
livox_sector_get_voxel (livoxSector* sector,
                        int          x,
                        int          y,
                        int          z)
{
	livoxBlock* block;
	int bx = x / LIVOX_TILES_PER_LINE;
	int by = y / LIVOX_TILES_PER_LINE;
	int bz = z / LIVOX_TILES_PER_LINE;
	int tx = x % LIVOX_TILES_PER_LINE;
	int ty = y % LIVOX_TILES_PER_LINE;
	int tz = z % LIVOX_TILES_PER_LINE;

	if (x < 0 || y < 0 || z < 0 ||
	    bx < 0 || bx >= LIVOX_BLOCKS_PER_LINE ||
	    by < 0 || by >= LIVOX_BLOCKS_PER_LINE ||
	    bz < 0 || bz >= LIVOX_BLOCKS_PER_LINE)
		return 0;
	block = sector->blocks + LIVOX_BLOCK_INDEX (bx, by, bz);

	return livox_block_get_voxel (block, tx, ty, tz);
}

/**
 * \brief Sets the terrain type of a voxel.
 *
 * \param self Block.
 * \param x Offset of the voxel within the sector.
 * \param y Offset of the voxel within the sector.
 * \param z Offset of the voxel within the sector.
 * \param terrain Terrain type.
 * \return Nonzero if a voxel was modified.
 */
int
livox_sector_set_voxel (livoxSector* self,
                        int          x,
                        int          y,
                        int          z,
                        livoxVoxel    terrain)
{
	int ret;
	int bx = x / LIVOX_BLOCKS_PER_LINE;
	int by = y / LIVOX_BLOCKS_PER_LINE;
	int bz = z / LIVOX_BLOCKS_PER_LINE;
	int tx = x % LIVOX_BLOCKS_PER_LINE;
	int ty = y % LIVOX_BLOCKS_PER_LINE;
	int tz = z % LIVOX_BLOCKS_PER_LINE;
	livoxBlock* block;

	if (x < 0 || y < 0 || z < 0 ||
	    bx < 0 || bx >= LIVOX_BLOCKS_PER_LINE || 
	    by < 0 || by >= LIVOX_BLOCKS_PER_LINE || 
	    bz < 0 || bz >= LIVOX_BLOCKS_PER_LINE)
	{
		assert (0);
		return 0;
	}
	block = self->blocks + LIVOX_BLOCK_INDEX (bx, by, bz);
	ret = livox_block_set_voxel (block, tx, ty, tz, terrain);

	return ret;
}

/*****************************************************************************/

static int
private_build_block (livoxSector* self,
                     int          x,
                     int          y,
                     int          z)
{
	livoxBlock* block;
	livoxBuilder* builder;

	/* Optimize block. */
	block = self->blocks + LIVOX_BLOCK_INDEX (x, y, z);
	livox_block_optimize (block);

	/* Generate mesh. */
	builder = livox_builder_new (self);
	if (builder == NULL)
		return 0;
	if (!livox_builder_build (builder, x, y, z))
	{
		livox_builder_free (builder);
		return 0;
	}

	/* Replace physics data. */
	if (block->physics != NULL)
		liphy_object_free (block->physics);
	if (block->shape != NULL)
		liphy_shape_free (block->shape);
	block->physics = NULL;
	block->shape = livox_builder_get_shape (builder);
	if (block->shape != NULL)
	{
		block->physics = liphy_object_new (self->manager->physics, block->shape,
			LIPHY_SHAPE_MODE_CONCAVE, LIPHY_CONTROL_MODE_STATIC);
		if (block->physics != NULL)
		{
			liphy_object_set_collision_group (block->physics, LIPHY_GROUP_TILES);
			liphy_object_set_collision_mask (block->physics, ~(LIPHY_GROUP_TILES | LIPHY_GROUP_STATICS));
			liphy_object_set_realized (block->physics, 1);
		}
	}

	/* Replace render data. */
#ifndef LIVOX_DISABLE_GRAPHICS
	if (self->manager->render != NULL)
	{
		if (block->render != NULL)
			self->manager->renderapi->lirnd_object_free (block->render);
		block->render = livox_builder_get_render (builder);
		if (block->render == NULL)
		{
			livox_builder_free (builder);
			return 0;
		}
		self->manager->renderapi->lirnd_object_set_realized (block->render, 1);
	}
#endif

	livox_builder_free (builder);

	return 1;
}

/** @} */
/** @} */
