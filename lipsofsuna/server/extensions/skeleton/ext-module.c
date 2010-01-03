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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvSkeleton Skeleton
 * @{
 */

#include <lipsofsuna/server.h>
#include "ext-skeleton.h"
#include "ext-module.h"

LISerExtensionInfo liextInfo =
{
	LISER_EXTENSION_VERSION, "Skeleton",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LISerServer* server)
{
	LIExtModule* self;

	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->server = server;
	self->skeleton = liext_skeleton_new (server);
	if (self->skeleton == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	liscr_script_create_class (server->script, "Skeleton", liext_script_skeleton, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	/* FIXME: Remove the class here. */
	liext_skeleton_free (self->skeleton);
	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
