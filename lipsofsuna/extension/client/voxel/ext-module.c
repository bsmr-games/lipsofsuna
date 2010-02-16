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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#include <lipsofsuna/client.h>
#include "ext-module.h"

static int
private_block_free (LIExtModule*      self,
                    LIVoxUpdateEvent* event);

static int
private_block_load (LIExtModule*      self,
                    LIVoxUpdateEvent* event);

static int
private_packet (LIExtModule* self,
                LICliClient* client,
                LIArcReader* reader);

static int
private_tick (LIExtModule* self,
              float        secs);

static int
private_voxel_assign (LIExtModule* self,
                      LIArcReader* reader);

static int
private_voxel_diff (LIExtModule* self,
                    LIArcReader* reader);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Voxel",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->client = limai_program_find_component (program, "client");

	/* Allocate voxel manager. */
	self->voxels = livox_manager_new (program->callbacks, program->sectors);
	if (self->voxels == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register voxel component. */
	if (!limai_program_insert_component (program, "voxel", self->voxels))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Allocate blocks. */
	self->blocks = lialg_memdic_new ();
	if (self->blocks == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "packet", 1, private_packet, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", 1, private_tick, self, self->calls + 1) ||
	    !lical_callbacks_insert (self->voxels->callbacks, self->voxels, "block-free", 1, private_block_free, self, self->calls + 2) ||
	    !lical_callbacks_insert (self->voxels->callbacks, self->voxels, "block-load", 1, private_block_load, self, self->calls + 3))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register class. */
	liscr_script_create_class (program->script, "Voxel", liext_script_voxel, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	LIAlgMemdicIter iter;

	/* Free callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Free blocks. */
	if (self->blocks != NULL)
	{
		LIALG_MEMDIC_FOREACH (iter, self->blocks)
			liext_block_free (iter.value);
		lialg_memdic_free (self->blocks);
	}

	/* Unregister voxel component. */
	if (self->voxels != NULL)
		limai_program_remove_component (self->client->program, "voxel");

	/* Free resources. */
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);
	lisys_free (self);
}

int
liext_module_build_all (LIExtModule* self)
{
	int ret = 1;
	LIAlgMemdicIter iter;
	LIExtBlock* block;
	LIVoxBlockAddr* addr;

	LIALG_MEMDIC_FOREACH (iter, self->blocks)
	{
		addr = iter.key;
		block = iter.value;
		ret &= liext_module_build_block (self,
			addr->sector[0], addr->sector[1], addr->sector[2],
			addr->block[0], addr->block[1], addr->block[2]);
	}

	return ret;
}

int
liext_module_build_block (LIExtModule* self,
                          int          sx,
                          int          sy,
                          int          sz,
                          int          bx,
                          int          by,
                          int          bz)
{
	LIExtBlock* eblock;
	LIVoxBlock* vblock;
	LIVoxBlockAddr addr;
	LIVoxSector* vsector;

	/* Find sector. */
	vsector = lialg_sectors_data_offset (self->voxels->sectors, "voxel", sx, sy, sz, 0);
	if (vsector == NULL)
		return 1;

	/* Find block. */
	vblock = livox_sector_get_block (vsector, LIVOX_BLOCK_INDEX (bx, by, bz));
	addr.sector[0] = sx;
	addr.sector[1] = sy;
	addr.sector[2] = sz;
	addr.block[0] = bx;
	addr.block[1] = by;
	addr.block[2] = bz;
	eblock = lialg_memdic_find (self->blocks, &addr, sizeof (addr));
	if (eblock == NULL)
	{
		eblock = liext_block_new (self->client);
		if (eblock == NULL)
			return 0;
		if (!lialg_memdic_insert (self->blocks, &addr, sizeof (addr), eblock))
		{
			liext_block_free (eblock);
			return 0;
		}
	}

	/* Build block. */
	if (!liext_block_build (eblock, self, vblock, &addr))
	{
		lialg_memdic_remove (self->blocks, &addr, sizeof (addr));
		liext_block_free (eblock);
		return 0;
	}

	return 1;
}

void
liext_module_clear_all (LIExtModule* self)
{
	LIAlgMemdicIter iter;
	LIExtBlock* block;

	LIALG_MEMDIC_FOREACH (iter, self->blocks)
	{
		block = iter.value;
		liext_block_clear (block);
	}
}

/*****************************************************************************/

static int
private_block_free (LIExtModule*      self,
                    LIVoxUpdateEvent* event)
{
	LIExtBlock* eblock;
	LIVoxBlockAddr addr;

	addr.sector[0] = event->sector[0];
	addr.sector[1] = event->sector[1];
	addr.sector[2] = event->sector[2];
	addr.block[0] = event->block[0];
	addr.block[1] = event->block[1];
	addr.block[2] = event->block[2];
	eblock = lialg_memdic_find (self->blocks, &addr, sizeof (addr));
	if (eblock != NULL)
	{
		lialg_memdic_remove (self->blocks, &addr, sizeof (addr));
		liext_block_free (eblock);
	}

	return 1;
}

static int
private_block_load (LIExtModule*      self,
                    LIVoxUpdateEvent* event)
{
	int sx = event->sector[0];
	int sy = event->sector[1];
	int sz = event->sector[2];
	int bx = event->block[0];
	int by = event->block[1];
	int bz = event->block[2];

	liext_module_build_block (self, sx, sy, sz, bx, by, bz);

	return 1;
}

static int
private_packet (LIExtModule* self,
                LICliClient* client,
                LIArcReader* reader)
{
	uint8_t type;

	reader->pos = 0;
	if (!liarc_reader_get_uint8 (reader, &type))
		return 1;
	switch (type)
	{
		case LIEXT_VOXEL_PACKET_ASSIGN:
			private_voxel_assign (self, reader);
			break;
		case LIEXT_VOXEL_PACKET_DIFF:
			private_voxel_diff (self, reader);
			break;
	}

	return 1;
}

static int
private_tick (LIExtModule* self,
              float        secs)
{
	livox_manager_update (self->voxels, secs);

	return 1;
}

static int
private_voxel_assign (LIExtModule* self,
                      LIArcReader* reader)
{
	LIVoxMaterial* material;

	while (!liarc_reader_check_end (reader))
	{
		material = livox_material_new_from_stream (reader);
		if (material == NULL)
			return 0;
		if (!livox_manager_insert_material (self->voxels, material))
			livox_material_free (material);
	}

	return 1;
}

static int
private_voxel_diff (LIExtModule* self,
                    LIArcReader* reader)
{
	uint8_t sectorx;
	uint8_t sectory;
	uint8_t sectorz;
	uint16_t blockid;
	LIVoxBlock* block;
	LIVoxSector* sector;

	while (!liarc_reader_check_end (reader))
	{
		/* Read block offset. */
		if (!liarc_reader_get_uint8 (reader, &sectorx) ||
		    !liarc_reader_get_uint8 (reader, &sectory) ||
		    !liarc_reader_get_uint8 (reader, &sectorz) ||
		    !liarc_reader_get_uint16 (reader, &blockid))
			return 0;
		if (blockid >= LIVOX_BLOCKS_PER_SECTOR)
			return 0;

		/* Find or create sector. */
		sector = lialg_sectors_data_offset (self->voxels->sectors, "voxel", sectorx, sectory, sectorz, 1);
		if (sector == NULL)
			return 0;

		/* Read block data. */
		block = livox_sector_get_block (sector, blockid);
		if (!livox_block_read (block, self->voxels, reader))
			return 0;
		if (livox_block_get_dirty (block))
			livox_sector_set_dirty (sector, 1);
	}

	return 1;
}

/** @} */
/** @} */
/** @} */
