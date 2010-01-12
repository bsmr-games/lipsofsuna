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
 * \addtogroup liextsrvCreature Creature
 * @{
 */

#include <lipsofsuna/network.h>
#include "ext-creature.h"

static int
private_tick (LIExtCreature* self,
              float          secs);

/*****************************************************************************/

/**
 * \brief Creates a new creature logic.
 *
 * \param server Server.
 * \return Creature logic or NULL.
 */
LIExtCreature*
liext_creature_new (LISerServer* server)
{
	LIExtCreature* self;

	self = lisys_calloc (1, sizeof (LIExtCreature));
	if (self == NULL)
		return NULL;
	self->server = server;
	liext_creature_set_active (self, 1);

	return self;
}

/**
 * \brief Frees the creature logic.
 *
 * \param self Creature logic.
 */
void
liext_creature_free (LIExtCreature* self)
{
	if (self->active)
		lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

int
liext_creature_get_active (LIExtCreature* self)
{
	return self->active;
}

int
liext_creature_set_active (LIExtCreature* self,
                           int            value)
{
	if (self->active == value)
		return 1;
	if (value)
		lical_callbacks_insert (self->server->callbacks, self->server->engine, "tick", 0, private_tick, self, self->calls + 0);
	else
		lical_handle_release (self->calls + 0);
	self->active = value;

	return 1;
}

/**
 * \brief Sets the object controlled by the logic.
 *
 * References to objects are maintained automatically.
 *
 * \param self Creature logic.
 * \param object Object.
 * \return Nonzero on success.
 */
int
liext_creature_set_object (LIExtCreature* self,
                           LIEngObject*   object)
{
	int flags;

	if (self->object != NULL)
	{
		flags = lieng_object_get_flags (self->object);
		liphy_object_set_control_mode (self->object->physics, LIPHY_CONTROL_MODE_RIGID);
		lieng_object_set_flags (self->object, flags & ~LIENG_OBJECT_FLAG_DYNAMIC);
		liscr_data_unref (self->object->script, self->data);
	}
	if (object != NULL)
	{
		flags = lieng_object_get_flags (object);
		liphy_object_set_control_mode (object->physics, LIPHY_CONTROL_MODE_CHARACTER);
		lieng_object_set_flags (object, flags | LIENG_OBJECT_FLAG_DYNAMIC);
		liscr_data_ref (object->script, self->data);
	}
	self->object = object;

	return 1;
}

/*****************************************************************************/

static int
private_tick (LIExtCreature* self,
              float          secs)
{
	LIEngObject* object;
	LIPhyObject* physics;

	/* Get object data. */
	object = self->object;
	if (object == NULL)
		return 1;
	physics = object->physics;

	/* Apply controls. */
	liphy_object_set_movement (physics, 0.0f);
	if ((self->controls & LINET_CONTROL_MOVE_FRONT) &&
	   !(self->controls & LINET_CONTROL_MOVE_BACK))
		liphy_object_set_movement (physics, 1.0f);
	if ((self->controls & LINET_CONTROL_MOVE_BACK) &&
	   !(self->controls & LINET_CONTROL_MOVE_FRONT))
		liphy_object_set_movement (physics, -1.0f);

	return 1;
}

/** @} */
/** @} */
/** @} */
