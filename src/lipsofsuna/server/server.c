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
 * \addtogroup LISer Server
 * @{
 * \addtogroup LISerServer Server
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "server.h"
#include "server-callbacks.h"

/**
 * \brief Creates a new server instance.
 * \param path Data directory root.
 * \param name Mod name.
 * \param args Arguments passed to the program.
 * \return New server or NULL.
 */
LISerServer* liser_server_new (
	const char* path,
	const char* name,
	const char* args)
{
	LISerServer* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISerServer));
	if (self == NULL)
		return NULL;

	/* Create program. */
	self->program = limai_program_new (path, name, args);
	if (self->program == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->sectors = self->program->sectors;
	self->callbacks = self->program->callbacks;
	self->engine = self->program->engine;
	self->paths = self->program->paths;
	self->script = self->program->script;

	/* Initialize server component. */
	if (!limai_program_insert_component (self->program, "server", self))
	{
		limai_program_free (self->program);
		return NULL;
	}
	if (!liser_server_init_callbacks_client (self))
	{
		liser_server_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the server.
 *
 * \param self Server.
 */
void
liser_server_free (LISerServer* self)
{
	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, "server-free", lical_marshal_DATA);

	/* Free program. */
	if (self->program != NULL)
	{
		limai_program_remove_component (self->program, "server");
		limai_program_free (self->program);
	}

	lisys_free (self);
}

/**
 * \brief Runs the server script.
 * \param self Server.
 * \return Nonzero on success.
 */
int liser_server_main (
	LISerServer* self)
{
	return limai_program_execute_script (self->program, "main.lua");
}

/** @} */
/** @} */
