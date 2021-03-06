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

#include "terrain-stick.h"

static void private_clamp_vertices (
	LIExtTerrainStick* self,
	LIExtTerrainStick* next);

/*****************************************************************************/

/**
 * \brief Creates a new terrain stick.
 * \return Terrain stick, or NULL.
 */
LIExtTerrainStick* liext_terrain_stick_new (
	int   material,
	float height)
{
	LIExtTerrainStick* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtTerrainStick));
	if (self == NULL)
		return NULL;
	self->material = material;
	self->height = height;
	self->vertices[0][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[0][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);

	return self;
}

/**
 * \brief Frees the stick.
 * \param self Terrain stick.
 */
void liext_terrain_stick_free (
	LIExtTerrainStick* self)
{
	lisys_free (self);
}

/**
 * \brief Casts a sphere against the stick and returns the hit fraction.
 *
 * FIXME: Doesn't work yet.
 *
 * \param self Terrain stick.
 * \param bot00 Bottom surface Y offset.
 * \param bot10 Bottom surface Y offset.
 * \param bot01 Bottom surface Y offset.
 * \param bot11 Bottom surface Y offset.
 * \param top00 Top surface Y offset.
 * \param top10 Top surface Y offset.
 * \param top01 Top surface Y offset.
 * \param top11 Top surface Y offset.
 * \param sphere_rel_cast_start Cast start position of the sphere, in grid units relative to the column origin.
 * \param sphere_rel_cast_end Cast end position of the sphere, in grid units relative to the column origin.
 * \param sphere_radius Sphere radius, in grid units.
 * \param result Return location for the hit fraction.
 * \return Nonzero if hit. Zero otherwise.
 */
int liext_terrain_stick_cast_sphere (
	const LIExtTerrainStick* self,
	float                    bot00,
	float                    bot10,
	float                    bot01,
	float                    bot11,
	float                    top00,
	float                    top10,
	float                    top01,
	float                    top11,
	const LIMatVector*       sphere_rel_cast_start,
	const LIMatVector*       sphere_rel_cast_end,
	float                    sphere_radius,
	LIExtTerrainCollision*   result)
{
	float min;
	float max;
	LIExtTerrainCollision best;
	LIExtTerrainCollision frac;
	LIMatVector v;
	LIMatVector vtx[3];
	LIMatVector point;
	LIMatPlane plane;

	frac.x = 0;
	frac.z = 0;
	best.fraction = LIMAT_INFINITE;
	v = limat_vector_subtract (*sphere_rel_cast_end, *sphere_rel_cast_start);

	/* Left. */
	limat_plane_init (&plane, -1.0f, 0.0f, 0.0f, 0.0f);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		min = limat_mix (bot00, bot01, point.z);
		max = limat_mix (top00, top01, point.z);
		if (point.z >= 0 && point.z <= 1.0f && min <= point.y && point.y <= max)
		{
			/* Direct face hit. */
			best = frac;
			best.normal = limat_vector_init (-1.0f, 0.0f, 0.0f);
			best.point = limat_vector_init (0.0f, point.y, point.z);
		}
		else
		{
			/* Potential edge hit. */
			point.z = LIMAT_CLAMP (point.z, 0.0f, 1.0f);
			min = limat_mix (bot00, bot01, point.z);
			max = limat_mix (top00, top01, point.z);
			point.y = LIMAT_CLAMP (point.y, min, max);
			if (limat_intersect_point_cast_sphere (&point, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius, &frac.fraction))
			{
				if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
				{
					best = frac;
					best.normal = limat_vector_init (-1.0f, 0.0f, 0.0f);
					best.point = limat_vector_init (0.0f, point.y, point.z);
				}
			}
		}
	}

	/* Right. */
	limat_plane_init (&plane, 1.0f, 0.0f, 0.0f, 1.0f);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		min = limat_mix (bot10, bot11, point.z);
		max = limat_mix (top10, top11, point.z);
		if (point.z >= 0 && point.z <= 1.0f && min <= point.y && point.y <= max)
		{
			/* Direct face hit. */
			best = frac;
			best.normal = limat_vector_init (1.0f, 0.0f, 0.0f);
			best.point = limat_vector_init (1.0f, point.y, point.z);
		}
		else
		{
			/* Potential edge hit. */
			point.z = LIMAT_CLAMP (point.z, 0.0f, 1.0f);
			min = limat_mix (bot10, bot11, point.z);
			max = limat_mix (top10, top11, point.z);
			point.y = LIMAT_CLAMP (point.y, min, max);
			if (limat_intersect_point_cast_sphere (&point, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius, &frac.fraction))
			{
				if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
				{
					best = frac;
					best.normal = limat_vector_init (1.0f, 0.0f, 0.0f);
					best.point = limat_vector_init (1.0f, point.y, point.z);
				}
			}
		}
	}

	/* Front. */
	limat_plane_init (&plane, 0.0f, 0.0f, -1.0f, 0.0f);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		min = limat_mix (bot00, bot10, point.z);
		max = limat_mix (top00, top10, point.z);
		if (point.x >= 0 && point.x <= 1.0f && min <= point.y && point.y <= max)
		{
			/* Direct face hit. */
			best = frac;
			best.normal = limat_vector_init (0.0f, 0.0f, -1.0f);
			best.point = limat_vector_init (point.x, point.y, 0.0f);
		}
		else
		{
			/* Potential edge hit. */
			point.x = LIMAT_CLAMP (point.x, 0.0f, 1.0f);
			min = limat_mix (bot00, bot10, point.z);
			max = limat_mix (top00, top10, point.z);
			point.y = LIMAT_CLAMP (point.y, min, max);
			if (limat_intersect_point_cast_sphere (&point, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius, &frac.fraction))
			{
				if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
				{
					best = frac;
					best.normal = limat_vector_init (0.0f, 0.0f, -1.0f);
					best.point = limat_vector_init (point.x, point.y, 0.0f);
				}
			}
		}
	}

	/* Back. */
	limat_plane_init (&plane, 0.0f, 0.0f, 1.0f, 1.0f);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		min = limat_mix (bot01, bot11, point.z);
		max = limat_mix (top01, top11, point.z);
		if (point.x >= 0 && point.x <= 1.0f && min <= point.y && point.y <= max)
		{
			/* Direct face hit. */
			best = frac;
			best.normal = limat_vector_init (0.0f, 0.0f, 1.0f);
			best.point = limat_vector_init (point.x, point.y, 1.0f);
		}
		else
		{
			/* Potential edge hit. */
			point.x = LIMAT_CLAMP (point.x, 0.0f, 1.0f);
			min = limat_mix (bot01, bot11, point.z);
			max = limat_mix (top01, top11, point.z);
			point.y = LIMAT_CLAMP (point.y, min, max);
			if (limat_intersect_point_cast_sphere (&point, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius, &frac.fraction))
			{
				if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
				{
					best = frac;
					best.normal = limat_vector_init (0.0f, 0.0f, 1.0f);
					best.point = limat_vector_init (point.x, point.y, 1.0f);
				}
			}
		}
	}

	/* Bottom. */
	vtx[2] = limat_vector_init (0.0f, bot00, 0.0f);
	vtx[1] = limat_vector_init (0.0f, bot01, 1.0f);
	vtx[0] = limat_vector_init (1.0f, bot11, 1.0f);
	limat_plane_init_from_points (&plane, vtx + 0, vtx + 1, vtx + 2);
	lisys_assert (plane.y < 0.0f);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		if (point.x >= 0.0f && point.z >= 0.0f && point.z <= 1.0f && point.z >= point.x)
		{
			/* Direct face hit. */
			best = frac;
			limat_plane_get_normal (&plane, &best.normal);
			best.point = limat_vector_add (point, limat_vector_multiply (best.normal, -sphere_radius));
		}
		else
		{
			/* TODO: Potential edge hit. */
		}
	}
	vtx[2] = limat_vector_init (0.0f, bot00, 0.0f);
	vtx[1] = limat_vector_init (1.0f, bot11, 1.0f);
	vtx[0] = limat_vector_init (1.0f, bot10, 0.0f);
	limat_plane_init_from_points (&plane, vtx + 0, vtx + 1, vtx + 2);
	lisys_assert (plane.y < 0.0f);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		if (point.x >= 0.0f && point.z >= 0.0f && point.x <= 1.0f && point.x >= point.z)
		{
			/* Direct face hit. */
			best = frac;
			limat_plane_get_normal (&plane, &best.normal);
			best.point = limat_vector_add (point, limat_vector_multiply (best.normal, -sphere_radius));
		}
		else
		{
			/* TODO: Potential edge hit. */
		}
	}

	/* Top. */
	vtx[2] = limat_vector_init (0.0f, top00, 0.0f);
	vtx[1] = limat_vector_init (1.0f, top10, 0.0f);
	vtx[0] = limat_vector_init (1.0f, top11, 1.0f);
	limat_plane_init_from_points (&plane, vtx + 0, vtx + 1, vtx + 2);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	lisys_assert (plane.y > 0.0f);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		if (point.x >= 0.0f && point.z >= 0.0f && point.x <= 1.0f && point.x >= point.z)
		{
			/* Direct face hit. */
			best = frac;
			limat_plane_get_normal (&plane, &best.normal);
			best.point = limat_vector_add (point, limat_vector_multiply (best.normal, -sphere_radius));
		}
		else
		{
			/* TODO: Potential edge hit. */
		}
	}
	vtx[2] = limat_vector_init (0.0f, top00, 0.0f);
	vtx[1] = limat_vector_init (1.0f, top11, 1.0f);
	vtx[0] = limat_vector_init (0.0f, top10, 1.0f);
	limat_plane_init_from_points (&plane, vtx + 0, vtx + 1, vtx + 2);
	lisys_assert (plane.y > 0.0f);
	frac.fraction = limat_plane_cast_sphere (&plane, sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius);
	if (frac.fraction >= 0.0f && best.fraction > frac.fraction)
	{
		point = limat_vector_add (*sphere_rel_cast_start, limat_vector_multiply (v, frac.fraction));
		if (point.x >= 0.0f && point.z >= 0.0f && point.z <= 1.0f && point.z >= point.x)
		{
			/* Direct face hit. */
			best = frac;
			limat_plane_get_normal (&plane, &best.normal);
			best.point = limat_vector_add (point, limat_vector_multiply (best.normal, -sphere_radius));
		}
		else
		{
			/* TODO: Potential edge hit. */
		}
	}

	/* Check whether a collision occurred. */
	if (best.fraction > 1.0f || best.fraction == LIMAT_INFINITE)
		return 0;
	*result = best;
	return 1;
}

/**
 * \brief Clamps the vertices of the stick so that they fit below the next stick's vertices.
 * \param self Terrain stick.
 * \param next Terrain stick.
 */
void liext_terrain_stick_clamp_vertices (
	LIExtTerrainStick* self,
	LIExtTerrainStick* next)
{
	private_clamp_vertices (self, next);
}

/**
 * \brief Clamps the vertices of the stick so that they fit above the previous stick's vertices.
 * \param self Terrain stick.
 * \param prev Terrain stick.
 */
void liext_terrain_stick_clamp_vertices_bottom (
	LIExtTerrainStick* self,
	LIExtTerrainStick* prev)
{
	int x;
	int z;

	for (z = 0 ; z < 2 ; z++)
	{
		for (x = 0 ; x < 2 ; x++)
		{
			if (prev != NULL)
			{
				self->vertices[x][z].offset = LIMAT_MAX (
					self->vertices[x][z].offset,
					prev->vertices[x][z].offset - self->height);
			}
			else
			{
				self->vertices[x][z].offset = LIMAT_MAX (
					self->vertices[x][z].offset,
					-self->height);
			}
		}
	}
}

/**
 * \brief Clears the stick data.
 * \param self Terrain stick.
 */
void liext_terrain_stick_clear (
	LIExtTerrainStick* self)
{
	memset (self, 0, sizeof (LIExtTerrainStick));
	self->vertices[0][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[0][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
}

/**
 * \brief Copies the vertices from another stick.
 * \param self Terrain stick.
 * \param src Terrain stick.
 */
void liext_terrain_stick_copy_vertices (
	LIExtTerrainStick* self,
	LIExtTerrainStick* src)
{
	self->vertices[0][0] = src->vertices[0][0];
	self->vertices[1][0] = src->vertices[1][0];
	self->vertices[0][1] = src->vertices[0][1];
	self->vertices[1][1] = src->vertices[1][1];
}

/**
 * \brief Prevents the vertices of this and the next stick from crossing each other.
 *
 * If the vertices of the stick cross with vertices of the next stick, the
 * vertices of the next stick are pushed upwards until they do not cross.
 * The same operation may be applied sequentially to subsequent sticks
 * until no vertices cross anymore.
 *
 * \param self Terrain stick.
 * \param stick_first The first terrain stick of the column.
 */
void liext_terrain_stick_fix_vertices_downwards (
	LIExtTerrainStick* self,
	LIExtTerrainStick* stick_first)
{
	int x;
	int z;
	int prev_affected;
	LIExtTerrainStick* stick = self;
	LIExtTerrainStick* stick_prev;
	LIExtTerrainStick* tmp;

	/* The first stick is always valid. */
	if (stick == NULL)
		return;
	if (stick == stick_first)
		return;

	/* Find the previous stick. */
	for (tmp = stick_first ; tmp->next != stick ; tmp = tmp->next)
		{}
	stick_prev = tmp;

	/* Ensure that the vertices are above the start of the previous stick. */
	/* This case is not possible to fix properly since it would require
	   changing the height of the previous stick. That is too complicated to
	   handle so we will just clamp the offsets to the valid range. */
	for (z = 0 ; z < 2 ; z++)
	{
		for (x = 0 ; x < 2 ; x++)
		{
			if (stick->vertices[x][z].offset < -stick_prev->height)
				stick->vertices[x][z].offset = -stick_prev->height;
		}
	}

	/* Ensure that the vertices of the previous stick are below the stick's vertices. */
	/* Moving the vertices of the stick downwards can trigger this case. Since
	   the fix involves doing that as well, it is possible that multiple fixes
	   are needed along the chain. */
	while (1)
	{
		prev_affected = 0;
		for (z = 0 ; z < 2 ; z++)
		{
			for (x = 0 ; x < 2 ; x++)
			{
				if (stick_prev->vertices[x][z].offset > stick_prev->height + stick->vertices[x][z].offset)
				{
					stick_prev->vertices[x][z].offset = stick_prev->height + stick->vertices[x][z].offset;
					prev_affected = 1;
				}
			}
		}
		if (!prev_affected)
			return;
		if (stick_prev == stick_first)
			return;
		stick = stick_prev;
		for (tmp = stick_first ; tmp->next != stick_prev ; tmp = tmp->next)
			{}
		stick_prev = tmp;
	}
}

/**
 * \brief Prevents the vertices of this and the next stick from crossing each other.
 *
 * If the vertices of the stick cross with vertices of the next stick, the
 * vertices of the next stick are pushed upwards until they do not cross.
 * The same operation may be applied sequentially to subsequent sticks
 * until no vertices cross anymore.
 *
 * \param self Terrain stick.
 */
void liext_terrain_stick_fix_vertices_upwards (
	LIExtTerrainStick* self)
{
	int x;
	int z;
	int next_affected;
	LIExtTerrainStick* stick = self;

	/* The last stick is always valid. */
	if (stick == NULL)
		return;
	if (stick->next == NULL)
		return;

	/* Ensure that the vertices are below the end of the next stick. */
	/* This case is not possible to fix properly since it would require
	   changing the height of the next stick. That is too complicated to
	   handle so we will just clamp the offsets to the valid range. */
	for (z = 0 ; z < 2 ; z++)
	{
		for (x = 0 ; x < 2 ; x++)
		{
			if (stick->vertices[x][z].offset > stick->next->height)
				stick->vertices[x][z].offset = stick->next->height;
		}
	}

	/* Ensure that the vertices of the next stick are above the stick's vertices. */
	/* Moving the vertices of the stick upwards can trigger this case. Since
	   the fix involves doing that as well, it is possible that multiple fixes
	   are needed along the chain. */
	while (stick->next != NULL)
	{
		next_affected = 0;
		for (z = 0 ; z < 2 ; z++)
		{
			for (x = 0 ; x < 2 ; x++)
			{
				if (stick->next->vertices[x][z].offset < stick->vertices[x][z].offset - stick->next->height)
				{
					stick->next->vertices[x][z].offset = stick->vertices[x][z].offset - stick->next->height;
					next_affected = 1;
				}
			}
		}
		if (!next_affected)
			return;
		stick = stick->next;
	}
}

/**
 * \brief Moves the vertices of the stick.
 * \param self Terrain stick.
 * \param diff Amount to move.
 */
void liext_terrain_stick_move_vertices (
	LIExtTerrainStick* self,
	float              diff)
{
	if (self == NULL)
		return;
	self->vertices[0][0].offset += diff;
	self->vertices[1][0].offset += diff;
	self->vertices[0][1].offset += diff;
	self->vertices[1][1].offset += diff;
}

/**
 * \brief Resets the vertices to the flat orientation.
 * \param self Terrain stick.
 */
void liext_terrain_stick_reset_vertices (
	LIExtTerrainStick* self)
{
	memset (self->vertices, 0, sizeof (self->vertices));
	self->vertices[0][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][0].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[0][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
	self->vertices[1][1].normal = limat_vector_init (0.0f, 1.0f, 0.0f);
}

/**
 * \brief Subtracts a stick from this one.
 * \param self Terrain stick.
 * \param y Vertical offset of the subtracted stick, relative to the stick bottom.
 * \param h Height of the subtracted stick.
 * \param bot00 Bottom surface vertex offsets, relative to the bottom of the subtracted stick.
 * \param bot10 Bottom surface vertex offsets, relative to the bottom of the subtracted stick.
 * \param bot01 Bottom surface vertex offsets, relative to the bottom of the subtracted stick.
 * \param bot11 Bottom surface vertex offsets, relative to the bottom of the subtracted stick.
 * \param top00 Top surface vertex offsets, relative to the top of the subtracted stick.
 * \param top10 Top surface vertex offsets, relative to the top of the subtracted stick.
 * \param top01 Top surface vertex offsets, relative to the top of the subtracted stick.
 * \param top11 Top surface vertex offsets, relative to the top of the subtracted stick.
 * \return Number between 1-6, or 0 if ran out of memory.
 */
int liext_terrain_stick_subtract (
	LIExtTerrainStick* self,
	float              y,
	float              h,
	float              bot00,
	float              bot10,
	float              bot01,
	float              bot11,
	float              top00,
	float              top10,
	float              top01,
	float              top11)
{
	LIExtTerrainStick* stick;

	/* A) Is the stick completely below us?
	 *
	 * .....SSSSSSSSSS.....
	 * XXX??...............
	 */
	if (y + h <= 0.0f)
	{
		/* In this case, nothing is done. The vertices of the stick may still
		   overlap with us, but that should be handled by calling this
		   function for the previous stick. */
		return 1;
	}

	/* B) Is the stick completely above us?
	 *
	 * .....SSSSSSSSSS.....
	 * ...............??XXX
	 */
	if (y >= self->height)
	{
		/* In this case, only the vertices of the stick may change, but the
		   height and position remain the same. The vertices are clipped so
		   that they stay below the bottom of the subtracted stick, in the
		   manner similar to case E. */
		self->vertices[0][0].offset = LIMAT_MIN (self->vertices[0][0].offset, y + bot00 - self->height);
		self->vertices[1][0].offset = LIMAT_MIN (self->vertices[1][0].offset, y + bot10 - self->height);
		self->vertices[0][1].offset = LIMAT_MIN (self->vertices[0][1].offset, y + bot01 - self->height);
		self->vertices[1][1].offset = LIMAT_MIN (self->vertices[1][1].offset, y + bot11 - self->height);
		return 2;
	}

	/* C) Does the stick replace us completely?
	 *
	 * .....SSSSSSSSSS.....
	 * ..???XXXXXXXXXX???..
	 */
	if (y <= 0.0f && y + h >= self->height)
	{
		/* In this case, the stick would become degenerate since its height
		   becomes zero. We just indicate that with the return value. The
		   caller should decide what to do about degenerate sticks. */
		return 3;
	}

	/* D) Does the stick replace part of the bottom?
	 *
	 * .....SSSSSSSSSS.....
	 * ..???XXXXX..........
	 */
	if (y <= 0.0f)
	{
		/* In this case, the stick is shortened by the length of the
		   intersection. As we assume that this function has been called
		   for the previous stick, we also adjust the top surface so that
		   the vertices are above the vertices of the previous stick. */
		/* Note the the Y offset of this stick is considered to increase
		   the same amount as its height decreases, meaning that the vertices
		   technically remain at the same absolute positions. This means that
		   the original original absolute Y coordinates must be used when
		   fixing the offsets of the surface vertices. */
		self->vertices[0][0].offset = LIMAT_MAX (self->vertices[0][0].offset, y + h + top00 - self->height);
		self->vertices[1][0].offset = LIMAT_MAX (self->vertices[1][0].offset, y + h + top10 - self->height);
		self->vertices[0][1].offset = LIMAT_MAX (self->vertices[0][1].offset, y + h + top01 - self->height);
		self->vertices[1][1].offset = LIMAT_MAX (self->vertices[1][1].offset, y + h + top11 - self->height);
		self->height -= y + h;
		private_clamp_vertices (self, self->next);
		return 4;
	}

	/* E) Does the stick replace part of the top?
	 *
	 * .....SSSSSSSSSS.....
	 * ..........XXXXX???..
	 */
	if (y + h >= self->height)
	{
		/* In this case, the stick is shortened by the length of the
		   intersection. The vertices are first raised by the length of
		   the intersection so that they will be at the original absolute
		   Y offsets. Then, they are clamped to be below the bottom vertices
		   of the subtracted stick, similar to case B. */
		self->vertices[0][0].offset += self->height - y;
		self->vertices[1][0].offset += self->height - y;
		self->vertices[0][1].offset += self->height - y;
		self->vertices[1][1].offset += self->height - y;
		self->height = y;
		self->vertices[0][0].offset = LIMAT_MIN (self->vertices[0][0].offset, y + bot00 - self->height);
		self->vertices[1][0].offset = LIMAT_MIN (self->vertices[1][0].offset, y + bot10 - self->height);
		self->vertices[0][1].offset = LIMAT_MIN (self->vertices[0][1].offset, y + bot01 - self->height);
		self->vertices[1][1].offset = LIMAT_MIN (self->vertices[1][1].offset, y + bot11 - self->height);
		return 5;
	}

	/* F) Does the stick replace part of the middle?
	 *
	 * .....SSSSSSSSSS.....
	 * ........XXXX........
	 */
	if (y > 0.0f && y + h < self->height)
	{
		/* In this case, the stick is split in two parts so that the split
		   point lies at the start of the subtracted stick. The second part
		   of the stick is allocated and inserted after this stick. After
		   that, the vertices of this stick are modified similar to case E.
		   The vertices of the next stick are clamped so that they will be
		   above the top vertices of the subtracted stick. */
		stick = liext_terrain_stick_new (self->material, self->height - y - h);
		if (stick == NULL)
			return 0;
		stick->vertices[0][0].offset = LIMAT_MAX (self->vertices[0][0].offset, y + h + top00 - self->height);
		stick->vertices[1][0].offset = LIMAT_MAX (self->vertices[1][0].offset, y + h + top10 - self->height);
		stick->vertices[0][1].offset = LIMAT_MAX (self->vertices[0][1].offset, y + h + top01 - self->height);
		stick->vertices[1][1].offset = LIMAT_MAX (self->vertices[1][1].offset, y + h + top11 - self->height);
		private_clamp_vertices (stick, self->next);
		stick->next = self->next;
		self->next = stick;
		self->vertices[0][0].offset += self->height - y;
		self->vertices[1][0].offset += self->height - y;
		self->vertices[0][1].offset += self->height - y;
		self->vertices[1][1].offset += self->height - y;
		self->height = y;
		self->vertices[0][0].offset = LIMAT_MIN (self->vertices[0][0].offset, y + bot00 - self->height);
		self->vertices[1][0].offset = LIMAT_MIN (self->vertices[1][0].offset, y + bot10 - self->height);
		self->vertices[0][1].offset = LIMAT_MIN (self->vertices[0][1].offset, y + bot01 - self->height);
		self->vertices[1][1].offset = LIMAT_MIN (self->vertices[1][1].offset, y + bot11 - self->height);
		return 6;
	}

	/* Only the above six cases are possible. */
	lisys_assert (0);

	return 0;
}

/**
 * \brief Gets the serialized contents of the stick.
 * \param self Terrain stick.
 * \param writer Archive write.
 * \return Nonzero on success.
 */
int liext_terrain_stick_get_data (
	LIExtTerrainStick* self,
	LIArcWriter*       writer)
{
	return liarc_writer_append_uint32 (writer, self->material) &&
	       liarc_writer_append_float (writer, self->height) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].offset) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[0][0].normal.z) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].offset) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[1][0].normal.z) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].offset) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[0][1].normal.z) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].offset) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].splatting) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].normal.x) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].normal.y) &&
	       liarc_writer_append_float (writer, self->vertices[1][1].normal.z);
}

/**
 * \brief Sets the contents of the stick from serialized data.
 * \param self Terrain stick.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_stick_set_data (
	LIExtTerrainStick* self,
	LIArcReader*       reader)
{
	int i;
	int x;
	int z;
	uint32_t tmp1;
	float tmp2[21];

	/* Read the values. */
	if (!liarc_reader_get_uint32 (reader, &tmp1))
		return 0;
	for (i = 0 ; i < 21 ; i++)
	{
		if (!liarc_reader_get_float (reader, tmp2 + i))
			return 0;
	}

	/* Apply the values. */
	self->material = tmp1;
	self->height = tmp2[(i = 0)];
	for (z = 0 ; z < 2 ; z++)
	{
		for (x = 0 ; x < 2 ; x++)
		{
			self->vertices[x][z].offset = tmp2[++i];
			self->vertices[x][z].splatting = tmp2[++i];
			self->vertices[x][z].normal.x = tmp2[++i];
			self->vertices[x][z].normal.y = tmp2[++i];
			self->vertices[x][z].normal.z = tmp2[++i];
		}
	}

	return 1;
}

/**
 * \brief Gets the intersection type of the two sticks.
 * \param self Terrain stick.
 * \param y Vertical offset of the compared stick, relative to the bottom of self.
 * \param h Height of the compared stick.
 * \return Number between 1-6.
 */
int liext_terrain_stick_get_intersection_type (
	const LIExtTerrainStick* self,
	float                    y,
	float                    h)
{
	/* A) Is the stick completely below us?
	 * 
	 * .....SSSSSSSSSS.....
	 * XXX??...............
	 */
	if (y + h <= 0.0f)
		return 1;

	/* B) Is the stick completely above us?
	 * 
	 * .....SSSSSSSSSS.....
	 * ...............??XXX
	 */
	if (y >= self->height)
		return 2;

	/* C) Does the stick replace us completely?
	 * 
	 * .....SSSSSSSSSS.....
	 * ..???XXXXXXXXXX???..
	 */
	if (y <= 0.0f && y + h >= self->height)
		return 3;

	/* D) Does the stick replace part of the bottom?
	 * 
	 * .....SSSSSSSSSS.....
	 * ..???XXXXX..........
	 */
	if (y <= 0.0f)
		return 4;

	/* E) Does the stick replace part of the top?
	 * 
	 * .....SSSSSSSSSS.....
	 * ..........XXXXX???..
	 */
	if (y + h >= self->height)
		return 5;

	/* F) Does the stick replace part of the middle?
	 * 
	 * .....SSSSSSSSSS.....
	 * ........XXXX........
	 */
	if (y > 0.0f && y + h < self->height)
		return 6;

	/* Only the above six cases are possible. */
	lisys_assert (0);

	return 0;
}

/**
 * \brief Calculates the face normal of the stick.
 * \param self Terrain stick.
 * \param result Return location for the vector.
 */
void liext_terrain_stick_get_normal (
	const LIExtTerrainStick* self,
	LIMatVector*             result)
{
	LIMatVector v1;
	LIMatVector v2;
	LIMatVector v3;
	LIMatVector v4;
	LIMatVector n1;
	LIMatVector n2;

	v1 = limat_vector_init (1.0f, self->vertices[1][0].offset - self->vertices[0][0].offset, 0.0f);
	v2 = limat_vector_init (0.0f, self->vertices[0][1].offset - self->vertices[0][0].offset, 1.0f);
	v3 = limat_vector_init (-1.0f, self->vertices[0][1].offset - self->vertices[1][1].offset, 0.0f);
	v4 = limat_vector_init (0.0f, self->vertices[1][0].offset - self->vertices[1][1].offset, -1.0f);
	n1 = limat_vector_normalize (limat_vector_cross (v2, v1));
	n2 = limat_vector_normalize (limat_vector_cross (v4, v3));
	*result = limat_vector_normalize (limat_vector_add (n1, n2));
}

/**
 * \brief Calculates the face normal of the stick when one vertex is overridden.
 * \param self Terrain stick.
 * \param vertex_x Index of the overridden vertex.
 * \param vertex_y Index of the overridden vertex.
 * \param vertex_offset Overridden vertex offset.
 * \param result Return location for the vector.
 */
void liext_terrain_stick_get_normal_override (
	const LIExtTerrainStick* self,
	int                      vertex_x,
	int                      vertex_y,
	float                    vertex_offset,
	LIMatVector*             result)
{
	float offsets[2][2];
	LIMatVector v1;
	LIMatVector v2;
	LIMatVector v3;
	LIMatVector v4;
	LIMatVector n1;
	LIMatVector n2;

	offsets[0][0] = self->vertices[0][0].offset;
	offsets[1][0] = self->vertices[1][0].offset;
	offsets[0][1] = self->vertices[0][1].offset;
	offsets[1][1] = self->vertices[1][1].offset;
	offsets[vertex_x][vertex_y] = vertex_offset;
	v1 = limat_vector_init (1.0f, offsets[1][0] - offsets[0][0], 0.0f);
	v2 = limat_vector_init (0.0f, offsets[0][1] - offsets[0][0], 1.0f);
	v3 = limat_vector_init (-1.0f, offsets[1][0] - offsets[1][1], 0.0f);
	v4 = limat_vector_init (0.0f, offsets[0][1] - offsets[1][1], -1.0f);
	n1 = limat_vector_normalize (limat_vector_cross (v2, v1));
	n2 = limat_vector_normalize (limat_vector_cross (v4, v3));
	*result = limat_vector_multiply (limat_vector_add (n1, n2), 0.5f);
}

/**
 * \brief Sets the vertex offsets of the stick.
 * \param self Stick.
 * \param slope List of four floats.
 * \param offset Extra offset to add to the slope values.
 */
void liext_terrain_stick_set_vertices (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset)
{
	if (self == NULL)
		return;
	liext_terrain_stick_reset_vertices (self);
	self->vertices[0][0].offset = slope[0] + offset;
	self->vertices[1][0].offset = slope[1] + offset;
	self->vertices[0][1].offset = slope[2] + offset;
	self->vertices[1][1].offset = slope[3] + offset;
}

/**
 * \brief Sets the vertex offsets of the stick.
 *
 * The values chosen are the maximum of the current ones and those given in
 * the slope argument.
 *
 * \param self Stick.
 * \param slope List of four floats.
 * \param offset Extra offset to add to the slope values.
 */
void liext_terrain_stick_set_vertices_max (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset)
{
	if (self == NULL)
		return;
	self->vertices[0][0].offset = LIMAT_MAX (self->vertices[0][0].offset, slope[0] + offset);
	self->vertices[1][0].offset = LIMAT_MAX (self->vertices[1][0].offset, slope[1] + offset);
	self->vertices[0][1].offset = LIMAT_MAX (self->vertices[0][1].offset, slope[2] + offset);
	self->vertices[1][1].offset = LIMAT_MAX (self->vertices[1][1].offset, slope[3] + offset);
}

/**
 * \brief Sets the vertex offsets of the stick.
 *
 * The values chosen are the minimum of the current ones and those given in
 * the slope argument.
 *
 * \param self Stick.
 * \param slope List of four floats.
 * \param offset Extra offset to add to the slope values.
 */
void liext_terrain_stick_set_vertices_min (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset)
{
	if (self == NULL)
		return;
	self->vertices[0][0].offset = LIMAT_MIN (self->vertices[0][0].offset, slope[0] + offset);
	self->vertices[1][0].offset = LIMAT_MIN (self->vertices[1][0].offset, slope[1] + offset);
	self->vertices[0][1].offset = LIMAT_MIN (self->vertices[0][1].offset, slope[2] + offset);
	self->vertices[1][1].offset = LIMAT_MIN (self->vertices[1][1].offset, slope[3] + offset);
}

/*****************************************************************************/

static void private_clamp_vertices (
	LIExtTerrainStick* self,
	LIExtTerrainStick* next)
{
	int x;
	int z;

	if (next != NULL)
	{
		for (z = 0 ; z < 2 ; z++)
		{
			for (x = 0 ; x < 2 ; x++)
			{
				self->vertices[x][z].offset = LIMAT_MIN (
					self->vertices[x][z].offset,
					next->vertices[x][z].offset + next->height);
			}
		}
	}
}

/** @} */
/** @} */
