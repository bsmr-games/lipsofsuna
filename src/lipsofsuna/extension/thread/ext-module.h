/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include "lipsofsuna/extension.h"

#define LIEXT_SCRIPT_THREAD "Thread"

typedef struct _LIExtThread LIExtThread;
struct _LIExtThread
{
	char* code;
	char* file;
	LIMaiProgram* program;
	LIScrData* script;
	LISysThread* thread;
};

LIExtThread* liext_thread_inst_new (
	LIMaiProgram* program,
	lua_State*    lua,
	const char*   file,
	const char*   code,
	const char*   args);

void liext_thread_inst_free (
	LIExtThread* self);

int liext_thread_inst_get_done (
	LIExtThread* self);

void liext_thread_inst_set_quit (
	LIExtThread* self);

/*****************************************************************************/

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LIMaiProgram* program;
};

LIExtModule* liext_thread_new (
	LIMaiProgram* program);

void liext_thread_free (
	LIExtModule* self);

/*****************************************************************************/

void liext_script_thread (
	LIScrScript* self);

#endif
