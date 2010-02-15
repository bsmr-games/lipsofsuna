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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtInventory Inventory 
 * @{
 */

#include <lipsofsuna/network.h>
#include "ext-module.h"
#include "ext-inventory.h"

#define LIEXT_INVENTORY_VERSION 0xFF

static int
private_send_close (LIExtInventory* self,
                    LIEngObject*    listener);

static int
private_send_open (LIExtInventory* self,
                   LIEngObject*    listener);

static int
private_send_object (LIExtInventory* self,
                     int             slot,
                     LIEngObject*    object);

static int
private_send_owner (LIExtInventory* self);

static int
private_send_remove (LIExtInventory* self,
                     int             slot);

/*****************************************************************************/

/**
 * \brief Creates a new inventory.
 *
 * \param module Extension module.
 * \return New inventory or NULL.
 */
LIExtInventory*
liext_inventory_new (LIExtModule* module)
{
	LIExtInventory* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtInventory));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Find unique ID. */
	while (!self->id)
	{
		self->id = lisys_randi (0x7FFFFFFF);
		if (lialg_u32dic_find (module->dictionary, self->id))
			self->id = 0;
	}

	/* Register self. */
	if (!lialg_u32dic_insert (module->dictionary, self->id, self))
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate listener dictionary. */
	self->listeners = lialg_u32dic_new ();
	if (self->listeners == NULL)
	{
		liext_inventory_free (self);
		return NULL;
	}

	/* Set default size. */
	if (!liext_inventory_set_size (self, 10))
	{
		liext_inventory_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the inventory.
 *
 * Objects are managed by the garbage collector so the stored objects
 * must not be freed here. The garbage collector is already doing it.
 *
 * \param self Inventory.
 */
void
liext_inventory_free (LIExtInventory* self)
{
	lialg_u32dic_remove (self->module->dictionary, self->id);
	if (self->listeners != NULL)
		lialg_u32dic_free (self->listeners);
	lisys_free (self->slots.array);
	lisys_free (self);
}

/**
 * \brief Finds a listener by id.
 *
 * \param self Inventory.
 * \param id Listener id.
 * \return Object or NULL.
 */
LIEngObject*
liext_inventory_find_listener (LIExtInventory* self,
                               int             id)
{
	return lialg_u32dic_find (self->listeners, id);
}

/**
 * \brief Adds a new inventory listener.
 *
 * \param self Inventory.
 * \param value Listener object.
 * \return Nonzero on success.
 */
int
liext_inventory_insert_listener (LIExtInventory* self,
                                 LIEngObject*    value)
{
	if (lialg_u32dic_find (self->listeners, value->id))
		return 1;
	if (!lialg_u32dic_insert (self->listeners, value->id, value))
		return 0;
	liscr_data_ref (value->script, self->script);
	private_send_open (self, value);

	return 1;
}

/**
 * \brief Removes an inventory listener.
 *
 * \param self Inventory.
 * \param value Listener object.
 */
void
liext_inventory_remove_listener (LIExtInventory* self,
                                 LIEngObject*    value)
{
	if (lialg_u32dic_find (self->listeners, value->id))
	{
		private_send_close (self, value);
		lialg_u32dic_remove (self->listeners, value->id);
		liscr_data_ref (value->script, self->script);
	}
}

/**
 * \brief Resets an inventory listener.
 *
 * This is called when the client associated to the listener has changed.
 * Causes the listener to be removed and readded so that the new inventory
 * ownership states get sent to the clients.
 *
 * \param self Inventory.
 * \param value Listener object.
 */
void
liext_inventory_reset_listener (LIExtInventory* self,
                                LIEngObject*    value)
{
	if (lialg_u32dic_find (self->listeners, value->id))
	{
		liext_inventory_remove_listener (self, value);
		liext_inventory_insert_listener (self, value);
	}
}

int
liext_inventory_get_id (const LIExtInventory* self)
{
	return self->id;
}

/**
 * \brief Gets an object in the specified slot.
 *
 * \param self Inventory.
 * \param slot Slot number.
 * \return Object or NULL.
 */
LIEngObject*
liext_inventory_get_object (LIExtInventory* self,
                            int             slot)
{
	if (slot < 0 || slot >= self->slots.count)
		return NULL;

	return self->slots.array[slot];
}

/**
 * \brief Sets an object in the specified slot.
 *
 * \param self Inventory.
 * \param slot Slot number.
 * \param object Object to set.
 * \return Nonzero on success.
 */
int
liext_inventory_set_object (LIExtInventory* self,
                            int             slot,
                            LIEngObject*    object)
{
	if (slot < 0 || slot >= self->slots.count)
		return 0;
	if (self->slots.array[slot] != NULL)
		liscr_data_unref (self->slots.array[slot]->script, self->script);
	self->slots.array[slot] = object;
	if (object != NULL)
	{
		liscr_data_ref (object->script, self->script);
		private_send_object (self, slot, object);
	}
	else
		private_send_remove (self, slot);

	return 1;
}

/**
 * \brief Gets an owner of the inventory.
 *
 * \param self Inventory.
 * \return Object or NULL.
 */
LIEngObject*
liext_inventory_get_owner (LIExtInventory* self)
{
	return self->owner;
}

/**
 * \brief Sets the ownert of the inventory.
 *
 * \param self Inventory.
 * \param value Object or NULL.
 * \return Nonzero on success.
 */
int
liext_inventory_set_owner (LIExtInventory* self,
                           LIEngObject*    value)
{
	LIExtInventory* old;

	if (self->owner == value)
		return 1;

	/* Clear old inventory. */
	if (value != NULL)
	{
		old = lialg_ptrdic_find (self->module->ptrdic, value);
		if (old != NULL)
		{
			lialg_ptrdic_remove (self->module->ptrdic, value);
			liscr_data_unref (old->owner->script, old->script);
			liscr_data_unref (old->script, old->owner->script);
			old->owner = NULL;
			private_send_owner (old);
		}
	}

	/* Set new owner. */
	if (self->owner != NULL)
	{
		liscr_data_unref (self->owner->script, self->script);
		liscr_data_unref (self->script, self->owner->script);
		lialg_ptrdic_remove (self->module->ptrdic, self->owner);
	}
	if (value != NULL)
	{
		liscr_data_ref (value->script, self->script);
		liscr_data_ref (self->script, value->script);
		lialg_ptrdic_insert (self->module->ptrdic, value, self);
	}
	self->owner = value;

	/* Inform scripts. */
	if (!private_send_owner (self))
		return 0;

	return 1;
}

/**
 * \brief Gets the number of items the inventory can hold.
 *
 * \param self Inventory.
 * \return Integer.
 */
int
liext_inventory_get_size (LIExtInventory* self)
{
	return self->slots.count;
}

/**
 * \brief Sets the number of items the inventory can hold.
 *
 * \param self Inventory.
 * \param value Integer.
 * \return Nonzero on success.
 */
int
liext_inventory_set_size (LIExtInventory* self,
                          int             value)
{
	int i;
	LIAlgU32dicIter iter;
	LIEngObject** tmp;

	/* Check for changes. */
	if (value == self->slots.count)
		return 1;

	/* Set new size. */
	if (value > self->slots.count)
	{
		tmp = lisys_realloc (self->slots.array, value * sizeof (LIEngObject*));
		if (tmp == NULL)
			return 0;
		self->slots.array = tmp;
		tmp += self->slots.count;
		memset (tmp, 0, (value - self->slots.count) * sizeof (LIEngObject*));
		self->slots.count = value;
	}
	else
	{
		for (i = value ; i < self->slots.count ; i++)
		{
			if (self->slots.array[i] != NULL)
				liscr_data_unref (self->slots.array[i]->script, self->script);
		}
		tmp = lisys_realloc (self->slots.array, value * sizeof (LIEngObject*));
		if (tmp != NULL || !value)
			self->slots.array = tmp;
		self->slots.count = value;
	}

	/* Update all listeners. */
	LIALG_U32DIC_FOREACH (iter, self->listeners)
	{
		private_send_close (self, iter.value);
		private_send_open (self, iter.value);
	}

	return 1;
}

/*****************************************************************************/

static int
private_send_close (LIExtInventory* self,
                    LIEngObject*    listener)
{
	LIScrScript* script = self->module->program->script;

	/* Check for callback. */
	liscr_pushdata (script->lua, self->script);
	lua_getfield (script->lua, -1, "user_removed_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}

	/* Invoke callback. */
	lua_pushvalue (script->lua, -2);
	lua_remove (script->lua, -3);
	liscr_pushdata (script->lua, listener->script);
	if (lua_pcall (script->lua, 2, 0, 0) != 0)
	{
		lisys_error_set (EINVAL, "Inventory.user_removed_cb: %s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}

	return 1;
}

static int
private_send_open (LIExtInventory* self,
                   LIEngObject*    listener)
{
	LIScrScript* script = self->module->program->script;

	/* Check for callback. */
	liscr_pushdata (script->lua, self->script);
	lua_getfield (script->lua, -1, "user_added_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}

	/* Invoke callback. */
	lua_pushvalue (script->lua, -2);
	lua_remove (script->lua, -3);
	liscr_pushdata (script->lua, listener->script);
	if (lua_pcall (script->lua, 2, 0, 0) != 0)
	{
		lisys_error_set (EINVAL, "Inventory.user_added_cb: %s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}

	return 1;
}

static int
private_send_object (LIExtInventory* self,
                     int             slot,
                     LIEngObject*    object)
{
	LIAlgU32dicIter iter;
	LIScrScript* script = self->module->program->script;

	LIALG_U32DIC_FOREACH (iter, self->listeners)
	{
		/* Check for callback. */
		liscr_pushdata (script->lua, self->script);
		lua_getfield (script->lua, -1, "item_added_cb");
		if (lua_type (script->lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (script->lua, 2);
			continue;
		}

		/* Invoke callback. */
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		liscr_pushdata (script->lua, LIENG_OBJECT (iter.value)->script);
		lua_pushnumber (script->lua, slot + 1);
		liscr_pushdata (script->lua, object->script);
		if (lua_pcall (script->lua, 4, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, "Inventory.item_added_cb: %s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
	}

	return 1;
}

static int
private_send_owner (LIExtInventory* self)
{
	LIAlgU32dicIter iter;
	LIScrScript* script = self->module->program->script;

	LIALG_U32DIC_FOREACH (iter, self->listeners)
	{
		/* Check for callback. */
		liscr_pushdata (script->lua, self->script);
		lua_getfield (script->lua, -1, "owner_changed_cb");
		if (lua_type (script->lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (script->lua, 2);
			continue;
		}

		/* Invoke callback. */
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		liscr_pushdata (script->lua, LIENG_OBJECT (iter.value)->script);
		if (self->owner != NULL)
			liscr_pushdata (script->lua, self->owner->script);
		if (lua_pcall (script->lua, 4, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, "Inventory.owner_changed_cb: %s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
	}

	return 1;
}

static int
private_send_remove (LIExtInventory* self,
                     int             slot)
{
	LIAlgU32dicIter iter;
	LIScrScript* script = self->module->program->script;

	LIALG_U32DIC_FOREACH (iter, self->listeners)
	{
		/* Check for callback. */
		liscr_pushdata (script->lua, self->script);
		lua_getfield (script->lua, -1, "item_removed_cb");
		if (lua_type (script->lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (script->lua, 2);
			continue;
		}

		/* Invoke callback. */
		lua_pushvalue (script->lua, -2);
		lua_remove (script->lua, -3);
		liscr_pushdata (script->lua, LIENG_OBJECT (iter.value)->script);
		lua_pushnumber (script->lua, slot + 1);
		if (lua_pcall (script->lua, 3, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, "Inventory.item_removed_cb: %s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}
	}

	return 1;
}

/** @} */
/** @} */
