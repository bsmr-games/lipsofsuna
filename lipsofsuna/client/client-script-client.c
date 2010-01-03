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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliscr Script
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/client.h>

/* @luadoc
 * module "Core.Client.Client"
 * ---
 * -- Access and manipulate the state of the client.
 * -- @name Client
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Cycles widget focus.
 * --
 * -- Arguments:
 * -- @param backward: True if should cycle backward.
 * --
 * -- @param self Client class.
 * -- @param args Arguments.
 * function Client.cycle_focus(self, args)
 */
static void Client_cycle_focus (LIScrArgs* args)
{
	int prev = 0;
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_focus (client->widgets, !prev);
}

/* @luadoc
 * ---
 * -- Cycles window focus.
 * --
 * -- Arguments:
 * -- @param backward: True if should cycle backward.
 * --
 * -- @param self Client class.
 * -- @param args Arguments.
 * function Client.cycle_focus(self, args)
 */
static void Client_cycle_window_focus (LIScrArgs* args)
{
	int prev = 0;
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_gets_bool (args, "backward", &prev);
	liwdg_manager_cycle_window_focus (client->widgets, !prev);
}

/* @luadoc
 * ---
 * -- Finds an object by ID.
 * --
 * -- Arguments:
 * -- id: Object ID.
 * --
 * -- @param self Client class.
 * -- @param args Arguments.
 * -- @return Object or nil.
 * function Client.find_object(self, args)
 */
static void Client_find_object (LIScrArgs* args)
{
	int id;
	LIEngObject* object;
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	if (!liscr_args_gets_int (args, "id", &id))
		return;
	object = lieng_engine_find_object (client->engine, id);
	if (object != NULL)
		liscr_args_seti_data (args, object->script);
}

/* @luadoc
 * ---
 * -- Launches a server and joins it.
 * --
 * -- If a server has already been launched, it is terminated.
 * --
 * -- Arguments:
 * -- login: Login name.
 * -- password: Password.
 * --
 * -- @param self Client class.
 * -- @param args Arguments.
 * -- @return True on success.
 * function Client.host(self, args)
 */
static void Client_host (LIScrArgs* args)
{
	const char* name = NULL;
	const char* pass = NULL;
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_gets_string (args, "login", &name);
	liscr_args_gets_string (args, "password", &pass);
	if (!licli_client_host (client))
	{
		lisys_error_report ();
		return;
	}
	if (!licli_client_connect (client, name, pass))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Joins a server.
 * --
 * -- Arguments:
 * -- login: Login name.
 * -- password: Password.
 * --
 * -- @param self Client class.
 * -- @param args Arguments.
 * -- @return True on success.
 * function Client.join(self, args)
 */
static void Client_join (LIScrArgs* args)
{
	const char* name = NULL;
	const char* pass = NULL;
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_gets_string (args, "login", &name);
	liscr_args_gets_string (args, "password", &pass);
	if (!licli_client_connect (client, name, pass))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Sends a network packet to the server.
 * --
 * -- Arguments:
 * -- packet: Packet.
 * -- reliable: True for reliable.
 * --
 * -- @param self Client class.
 * -- @param args Arguments.
 * function Client.send(self, args)
 */
static void Client_send (LIScrArgs* args)
{
	int reliable = 1;
	LICliClient* client;
	LIScrData* data;
	LIScrPacket* packet;

	if (liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
		packet = data->data;
		if (packet->writer != NULL)
		{
			liscr_args_gets_bool (args, "reliable", &reliable);
			if (reliable)
				licli_client_send (client, packet->writer, GRAPPLE_RELIABLE);
			else
				licli_client_send (client, packet->writer, 0);
		}
	}
}

/* @luadoc
 * ---
 * -- Gets the current cursor position.
 * -- @name Client.cursor_pos
 * -- @class table
 */
static void Client_getter_cursor_pos (LIScrArgs* args)
{
	int x;
	int y;
	LICliClient* client;
	LIMatVector tmp;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	client->video.SDL_GetMouseState (&x, &y);
	tmp = limat_vector_init (x, client->widgets->height - y - 1, 0.0f);
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * ---
 * -- Movement mode flag.
 * -- @name Client.moving
 * -- @class table
 */
static void Client_getter_moving (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_bool (args, licli_client_get_moving (client));
}
static void Client_setter_moving (LIScrArgs* args)
{
	int value;
	LICliClient* client;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
		licli_client_set_moving (client, value);
	}
}

/* @luadoc
 * ---
 * -- Root widget.
 * -- @name Client.root
 * -- @class table
 */
static void Client_setter_root (LIScrArgs* args)
{
	LICliClient* client;
	LIScrData* data;
	LIWdgWidget* window;

	/* Detach old root widget. */
	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	window = liwdg_manager_get_root (client->widgets);
	if (window != NULL)
	{
		liscr_data_unref (liwdg_widget_get_userdata (window), NULL);
		liwdg_widget_set_visible (window, 0);
	}
	liwdg_manager_set_root (client->widgets, NULL);

	/* Set new root window. */
	if (liscr_args_geti_data (args, 0, LICLI_SCRIPT_WIDGET, &data))
	{
		window = data->data;
		if (window->parent != NULL || window->state != LIWDG_WIDGET_STATE_DETACHED)
			return;
		liwdg_manager_set_root (client->widgets, data->data);
		liscr_data_ref (data, NULL);
	}
}

/* @luadoc
 * ---
 * -- Main window title.
 * -- @name Client.title
 * -- @class table
 */
static void Client_setter_title (LIScrArgs* args)
{
	const char* value;
	LICliClient* client;

	if (liscr_args_geti_string (args, 0, &value))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
		client->video.SDL_WM_SetCaption (value, value);
	}
}

/*****************************************************************************/

void
licli_script_client (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_CLIENT, data);
	liscr_class_insert_cfunc (self, "cycle_focus", Client_cycle_focus);
	liscr_class_insert_cfunc (self, "cycle_window_focus", Client_cycle_window_focus);
	liscr_class_insert_cfunc (self, "find_object", Client_find_object);
	liscr_class_insert_cfunc (self, "host", Client_host);
	liscr_class_insert_cfunc (self, "join", Client_join);
	liscr_class_insert_cfunc (self, "send", Client_send);
	liscr_class_insert_mvar (self, "cursor_pos", Client_getter_cursor_pos, NULL);
	liscr_class_insert_mvar (self, "moving", Client_getter_moving, Client_setter_moving);
	liscr_class_insert_mvar (self, "root", NULL, Client_setter_root);
	liscr_class_insert_mvar (self, "title", NULL, Client_setter_title);
}

/** @} */
/** @} */

