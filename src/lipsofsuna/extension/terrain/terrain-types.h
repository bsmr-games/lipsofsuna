/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_TERRAIN_TYPES_H__
#define __EXT_TERRAIN_TYPES_H__

#define LIEXT_SCRIPT_TERRAIN "Terrain"
#define LIEXT_SCRIPT_TERRAIN_CHUNK "TerrainChunk"
#define LIEXT_STICK_EPSILON 0.01f
#define LIEXT_TERRAIN_MATERIAL_MAX 32
#define LIEXT_TERRAIN_SMOOTHING_LIMIT 0.1f

typedef struct _LIExtTerrain LIExtTerrain;
typedef struct _LIExtTerrainModule LIExtTerrainModule;

typedef struct _LIExtTerrainCollision LIExtTerrainCollision;
struct _LIExtTerrainCollision
{
	int x;
	int z;
	float fraction;
	LIMatVector point;
	LIMatVector normal;
};

#endif
