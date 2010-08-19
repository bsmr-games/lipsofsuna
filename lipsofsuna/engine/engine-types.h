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

#ifndef __ENGINE_TYPES_H__
#define __ENGINE_TYPES_H__

#include <lipsofsuna/system.h>

enum _LIEngMatrix
{
	LIENG_MATRIX_LOCAL,
	LIENG_MATRIX_WORLD,
};

enum
{
	LIENG_OBJECT_FLAG_DIRTY = 0x0001,
	LIENG_OBJECT_FLAG_DYNAMIC = 0x0002,
	LIENG_OBJECT_FLAG_INVISIBLE = 0x0008,
	LIENG_OBJECT_FLAG_SAVE = 0x0010,
	LIENG_OBJECT_FLAG_RELOAD = 0x8000
};

typedef struct _LIEngEngine LIEngEngine;
typedef enum _LIEngMatrix LIEngMatrix;
typedef struct _LIEngModel LIEngModel;
typedef struct _LIEngObject LIEngObject;
typedef struct _LIEngRender LIEngRender;
typedef struct _LIEngResources LIEngResources;
typedef struct _LIEngSector LIEngSector;
typedef struct _LIEngSelection LIEngSelection;

#endif
