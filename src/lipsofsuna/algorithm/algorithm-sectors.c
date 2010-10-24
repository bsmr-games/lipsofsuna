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
 * \addtogroup LIAlg Algorithm
 * @{
 * \addtogroup LIAlgSectors Sectors
 * @{
 */

#include <lipsofsuna/system.h>
#include "algorithm-range.h"
#include "algorithm-range-iter.h"
#include "algorithm-sectors.h"

typedef struct _LIAlgSectorsContent LIAlgSectorsContent;
struct _LIAlgSectorsContent
{
	LIAlgSectorFreeFunc free;
	LIAlgSectorLoadFunc load;
	void* data;
};

static void
private_free_sector (LIAlgSectors* self,
                     LIAlgSector*  sector);

/*****************************************************************************/

LIAlgSectors*
lialg_sectors_new (int   count,
                   float width)
{
	LIAlgSectors* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIAlgSectors));
	if (self == NULL)
		return NULL;
	self->count = count;
	self->width = width;

	/* Allocate sectors. */
	self->sectors = lialg_u32dic_new ();
	if (self->sectors == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate content types. */
	self->content = lialg_strdic_new ();
	if (self->sectors == NULL)
	{
		lialg_u32dic_free (self->sectors);
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
lialg_sectors_free (LIAlgSectors* self)
{
	lisys_assert (self->content->size == 0);

	lialg_sectors_clear (self);
	lialg_strdic_free (self->content);
	lialg_u32dic_free (self->sectors);
	lisys_free (self);
}

void
lialg_sectors_clear (LIAlgSectors* self)
{
	LIAlgU32dicIter iter;

	LIALG_U32DIC_FOREACH (iter, self->sectors)
		private_free_sector (self, iter.value);
	lialg_u32dic_clear (self->sectors);
}

void*
lialg_sectors_data_index (LIAlgSectors* self,
                          const char*   name,
                          int           index,
                          int           create)
{
	LIAlgSector* sector;

	sector = lialg_sectors_sector_index (self, index, create);
	if (sector == NULL)
		return NULL;

	return lialg_strdic_find (sector->content, name);
}

void*
lialg_sectors_data_offset (LIAlgSectors* self,
                           const char*   name,
                           int           x,
                           int           y,
                           int           z,
                           int           create)
{
	LIAlgSector* sector;

	sector = lialg_sectors_sector_offset (self, x, y, z, create);
	if (sector == NULL)
		return NULL;

	return lialg_strdic_find (sector->content, name);
}

void*
lialg_sectors_data_point (LIAlgSectors*      self,
                          const char*        name,
                          const LIMatVector* point,
                          int                create)
{
	LIAlgSector* sector;

	sector = lialg_sectors_sector_point (self, point, create);
	if (sector == NULL)
		return NULL;

	return lialg_strdic_find (sector->content, name);
}

LIAlgSector*
lialg_sectors_sector_index (LIAlgSectors* self,
                            int           index,
                            int           create)
{
	void* data;
	LIAlgSector* sector;
	LIAlgSectorsContent* content;
	LIAlgStrdicIter iter;

	/* Try existing. */
	sector = lialg_u32dic_find (self->sectors, index);
	if (sector != NULL)
		return sector;
	if (!create)
		return NULL;

	/* Allocate sector. */
	sector = lisys_calloc (1, sizeof (LIAlgSector));
	if (sector == NULL)
		return NULL;
	sector->index = index;
	sector->stamp = lisys_time (NULL);
	sector->manager = self;
	sector->content = lialg_strdic_new ();
	if (sector->content == NULL)
	{
		lisys_free (sector);
		return NULL;
	}

	/* Calculate position. */
	lialg_sectors_index_to_offset (self, index, &sector->x, &sector->y, &sector->z);
	sector->position = limat_vector_init (sector->x, sector->y, sector->z);
	sector->position = limat_vector_multiply (sector->position, self->width);

	/* Add to dictionary. */
	if (!lialg_u32dic_insert (self->sectors, index, sector))
	{
		private_free_sector (self, sector);
		return NULL;
	}

	/* Create content. */
	LIALG_STRDIC_FOREACH (iter, self->content)
	{
		content = iter.value;
		data = content->load (sector);
		if (data != NULL)
		{
			if (!lialg_strdic_insert (sector->content, iter.key, data))
				content->free (data);
		}
	}

	/* Invoke callbacks. */
	if (self->sector_load_callback.callback != NULL)
		self->sector_load_callback.callback (self->sector_load_callback.userdata, sector);

	return sector;
}

LIAlgSector*
lialg_sectors_sector_offset (LIAlgSectors* self,
                             int           x,
                             int           y,
                             int           z,
                             int           create)
{
	return lialg_sectors_sector_index (self,
		lialg_sectors_offset_to_index (self, x, y, z), create);
}

LIAlgSector*
lialg_sectors_sector_point (LIAlgSectors*      self,
                            const LIMatVector* point,
                            int                create)
{
	return lialg_sectors_sector_index (self,
		lialg_sectors_point_to_index (self, point), create);
}

void
lialg_sectors_index_to_offset (LIAlgSectors* self,
                               int           index,
                               int*          x,
                               int*          y,
                               int*          z)
{
	*x = index % self->count;
	*y = index / self->count % self->count;
	*z = index / self->count / self->count % self->count;
}

void
lialg_sectors_index_to_offset_static (int  count,
                                      int  index,
                                      int* x,
                                      int* y,
                                      int* z)
{
	*x = index % count;
	*y = index / count % count;
	*z = index / count / count % count;
}

int
lialg_sectors_insert_content (LIAlgSectors*       self,
                              const char*         name,
                              void*               data,
                              LIAlgSectorFreeFunc free,
                              LIAlgSectorLoadFunc load)
{
	void* data_;
	LIAlgSector* sector;
	LIAlgSectorsContent* content;
	LIAlgU32dicIter iter;

	lisys_assert (!lialg_strdic_find (self->content, name));

	/* Allocate content. */
	content = lisys_calloc (1, sizeof (LIAlgSectorsContent));
	if (content == NULL)
		return 0;
	content->data = data;
	content->free = free;
	content->load = load;

	/* Insert to dictionary. */
	if (!lialg_strdic_insert (self->content, name, content))
	{
		lisys_free (content);
		return 0;
	}

	/* Allocate sector data. */
	LIALG_U32DIC_FOREACH (iter, self->sectors)
	{
		sector = iter.value;
		data_ = content->load (sector);
		if (data != NULL)
		{
			if (!lialg_strdic_insert (sector->content, name, data_))
				content->free (data_);
		}
	}

	return 1;
}

int
lialg_sectors_offset_to_index (LIAlgSectors* self,
                               int           x,
                               int           y,
                               int           z)
{
	return x + (y + z * self->count) * self->count;
}

int
lialg_sectors_offset_to_index_static (int count,
                                      int x,
                                      int y,
                                      int z)
{
	return x + (y + z * count) * count;
}

int
lialg_sectors_point_to_index (LIAlgSectors*      self,
                              const LIMatVector* point)
{
	int x;
	int y;
	int z;

	x = LIMAT_CLAMP ((int)(point->x / self->width), 0, self->count - 1);
	y = LIMAT_CLAMP ((int)(point->y / self->width), 0, self->count - 1);
	z = LIMAT_CLAMP ((int)(point->z / self->width), 0, self->count - 1);

	return x + (y + z * self->count) * self->count;
}

int
lialg_sectors_point_to_index_static (int                count,
                                     float              width,
                                     const LIMatVector* point)
{
	int x;
	int y;
	int z;

	x = LIMAT_CLAMP ((int)(point->x / width), 0, count - 1);
	y = LIMAT_CLAMP ((int)(point->y / width), 0, count - 1);
	z = LIMAT_CLAMP ((int)(point->z / width), 0, count - 1);

	return x + (y + z * count) * count;
}

void
lialg_sectors_refresh_point (LIAlgSectors*      self,
                             const LIMatVector* point,
                             float              radius)
{
	time_t stamp;
	LIAlgRange range;
	LIAlgRangeIter iter;
	LIAlgSector* sector;

	/* Block recursion here or the newly loaded sectors may try to load more
	 * sectors when creating objects. That could lead to ugly conflicts due
	 * to the incomplete sector loads in progress in earlier recursion levels.
	 */
	if (!self->loading)
	{
		self->loading = 1;
		stamp = time (NULL);
		range = lialg_range_new_from_sphere (point, radius, self->width);
		range = lialg_range_clamp (range, 0, self->count - 1);
		LIALG_RANGE_FOREACH (iter, range)
		{
			sector = lialg_sectors_sector_index (self, iter.index, 1);
			sector->stamp = stamp;
		}
		self->loading = 0;
	}
}

void
lialg_sectors_remove (LIAlgSectors* self,
                      int           index)
{
	LIAlgSector* sector;

	sector = lialg_u32dic_find (self->sectors, index);
	if (sector != NULL)
	{
		private_free_sector (self, sector);
		lialg_u32dic_remove (self->sectors, index);
	}
}

void
lialg_sectors_remove_content (LIAlgSectors* self,
                              const char*   name)
{
	void* data;
	LIAlgSector* sector;
	LIAlgSectorsContent* content;
	LIAlgU32dicIter iter;

	/* Find content. */
	content = lialg_strdic_find (self->content, name);
	if (content == NULL)
		return;

	/* Free sector data. */
	LIALG_U32DIC_FOREACH (iter, self->sectors)
	{
		sector = iter.value;
		data = lialg_strdic_find (sector->content, name);
		if (data != NULL)
			content->free (data);
	}

	/* Free content. */
	lialg_strdic_remove (self->content, name);
	lisys_free (content);
}

void
lialg_sectors_update (LIAlgSectors* self,
                      float         secs)
{
}

void*
lialg_sectors_get_userdata (LIAlgSectors* self,
                            const char*   name)
{
	LIAlgSectorsContent* content;

	content = lialg_strdic_find (self->content, name);
	if (content == NULL)
		return NULL;

	return content->data;
}

/*****************************************************************************/

static void
private_free_sector (LIAlgSectors* self,
                     LIAlgSector*  sector)
{
	void* data;
	LIAlgSectorsContent* content;
	LIAlgStrdicIter iter;

	/* Invoke callbacks. */
	if (self->sector_free_callback.callback != NULL)
		self->sector_free_callback.callback (self->sector_load_callback.userdata, sector);

	/* Free sector data. */
	LIALG_STRDIC_FOREACH (iter, self->content)
	{
		content = iter.value;
		data = lialg_strdic_find (sector->content, iter.key);
		if (data != NULL)
			content->free (data);
	}
	lialg_strdic_free (sector->content);
	lisys_free (sector);
}

/** @} **/
/** @} **/

