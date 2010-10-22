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
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysFile File
 * @{
 */

#ifndef __SYSTEM_FILE_H__
#define __SYSTEM_FILE_H__

#include <stdio.h>
#include "system-compiler.h"

enum
{
	LISYS_ACCESS_EXISTS = 0x08,
	LISYS_ACCESS_READ = 0x01,
	LISYS_ACCESS_WRITE = 0x02,
	LISYS_ACCESS_EXEC = 0x04,
};

enum
{
	LISYS_STAT_BLOCK,
	LISYS_STAT_CHAR,
	LISYS_STAT_DIRECTORY,
	LISYS_STAT_FILE,
	LISYS_STAT_LINK
};

typedef struct _LISysMmap LISysMmap;
typedef struct _LISysStat LISysStat;
struct _LISysStat
{
	int uid;
	int gid;
	int mode;
	int size;
	int type;
	long mtime;
	long rdev;
};

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (int, lisys_access, (
	const char* path,
	int         mode));

LIAPICALL (LISysMmap*, lisys_mmap_open, (
	const char* path));

LIAPICALL (void, lisys_mmap_free, (
	LISysMmap* self));

LIAPICALL (void*, lisys_mmap_get_buffer, (
	LISysMmap* self));

LIAPICALL (int, lisys_mmap_get_size, (
	LISysMmap* self));

LIAPICALL (char*, lisys_readlink, (
	const char* path));

LIAPICALL (int, lisys_stat, (
	const char* path,
	LISysStat*  result));

LIAPICALL (int, lisys_lstat, (
	const char* path,
	LISysStat*  result));

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
