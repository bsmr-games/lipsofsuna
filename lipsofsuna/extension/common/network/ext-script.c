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
 * \addtogroup LIExtNetwork Network
 * @{
 */

#include <lipsofsuna/network.h>
#include "ext-module.h"
#include "ext-client.h"

/* @luadoc
 * module "Extension.Common.Network"
 * ---
 * -- Networking support.
 * -- @name Network
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Disconnects any client associated to the specified object.
 * --
 * -- Arguements:
 * -- object: Object. (required)
 * --
 * -- @param self Network class.
 * -- @param args Arguments.
 * function Network.disconnect(self)
 */
static void Network_disconnect (LIScrArgs* args)
{
	LIEngObject* object;
	LIExtClient* client;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	object = LIENG_OBJECT (args->self);
	client = liext_module_find_client_by_object (module, object->id);
	if (client != NULL)
		liext_client_free (client);
}

/* @luadoc
 * ---
 * -- Checks if the specified object has a client attached to it.
 * --
 * -- Arguments:
 * -- object: Object. (require)
 * --
 * -- @param self Network class.
 * -- @param args Arguments.
 * function Network.shutdown(self)
 */
static void Network_has_client (LIScrArgs* args)
{
	LIExtClient* client;
	LIExtModule* module;
	LIEngObject* object;
	LIScrData* data;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
		object = LIENG_OBJECT (data->data);
		client = liext_module_find_client_by_object (module, object->id);
		liscr_args_seti_bool (args, client != NULL);
	}
}

/* @luadoc
 * ---
 * -- Begins listening for clients.
 * --
 * -- Arguments:
 * -- port: Port to listen to.
 * -- udp: True for UDP.
 * --
 * -- @param self Network class.
 * -- @param args Arguments.
 * -- @return True on success.
 * function Network.host(self, args)
 */
static void Network_host (LIScrArgs* args)
{
	int port = 10101;
	int udp = 0;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liscr_args_gets_bool (args, "udp", &udp);
	liscr_args_gets_int (args, "port", &port);
	port = LIMAT_CLAMP (port, 1025, 32767);

	if (liext_module_host (module, udp, port))
		liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Connects to a server.
 * --
 * -- Arguments:
 * -- host: Server address.
 * -- login: Login name.
 * -- password: password.
 * -- port: Port to listen to.
 * -- udp: True for UDP.
 * --
 * -- @param self Network class.
 * -- @param args Arguments.
 * -- @return True on success.
 * function Network.join(self, args)
 */
static void Network_join (LIScrArgs* args)
{
	int port = 10101;
	int udp = 0;
	const char* addr = "localhost";
	const char* name = "Player";
	const char* pass = "password";
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liscr_args_gets_bool (args, "udp", &udp);
	liscr_args_gets_int (args, "port", &port);
	liscr_args_gets_string (args, "host", &addr);
	liscr_args_gets_string (args, "login", &addr);
	liscr_args_gets_string (args, "password", &addr);
	port = LIMAT_CLAMP (port, 1025, 32767);

	if (liext_module_join (module, udp, port, addr, name, pass))
		liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * ---
 * -- Sends a network packet to the client controlling the object.
 * --
 * -- Arguments:
 * -- object: Object. (required if hosting)
 * -- packet: Packet. (required)
 * -- reliable: Boolean.
 * --
 * -- @param self Network class.
 * -- @param args Arguments.
 * function Object.send(self, args)
 */
static void Network_send (LIScrArgs* args)
{
	int reliable = 1;
	LIEngObject* object = NULL;
	LIExtModule* module;
	LIScrData* data0;
	LIScrData* data1;
	LIScrPacket* packet;

	/* Get packet. */
	if (!liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data0))
		return;
	packet = data0->data;

	/* Get object if hosting. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	if (module->server_socket)
	{
		if (!liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data1))
			return;
		object = data1->data;
	}

	/* Send packet. */
	liscr_args_gets_bool (args, "reliable", &reliable);
	if (packet->writer != NULL)
	{
		if (reliable)
			liext_module_send (module, object, packet->writer, GRAPPLE_RELIABLE);
		else
			liext_module_send (module, object, packet->writer, 0);
	}
}

/* @luadoc
 * ---
 * -- Disconnects all client and closes the network connection.
 * --
 * -- @param self Network class.
 * function Network.shutdown(self)
 */
static void Network_shutdown (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
	liext_module_shutdown (module);
}

/* @luadoc
 * ---
 * -- Swaps the clients of the objects.
 * --
 * -- Switches the clients of the passed objects so that the client of the first
 * -- object controls the second object and vice versa. This is typically used when
 * -- you need to destroy the object of a player without disconnecting the client.
 * --
 * -- Argumens:
 * -- object: Object. (required)
 * -- other: Object. (required)
 * --
 * -- @param self Network class.
 * -- @param args Arguments.
 * function Object.swap_clients(self, args)
 */
static void Network_swap_clients (LIScrArgs* args)
{
	LIEngObject* object;
	LIExtClient* client0;
	LIExtClient* client1;
	LIExtModule* module;
	LIScrData* data0;
	LIScrData* data1;

	if (liscr_args_gets_data (args, "object", LISCR_SCRIPT_OBJECT, &data0) &&
	    liscr_args_gets_data (args, "other", LISCR_SCRIPT_OBJECT, &data1))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NETWORK);
		object = LIENG_OBJECT (data0->data);
		client0 = liext_module_find_client_by_object (module, object->id);
		object = LIENG_OBJECT (data1->data);
		client1 = liext_module_find_client_by_object (module, object->id);
		if (client0 != NULL)
			liext_client_swap (client0, client1);
		else if (client1 != NULL)
			liext_client_swap (client1, client0);
	}
}

void
liext_script_network (LIScrClass* self,
                      void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_NETWORK, data);
	liscr_class_insert_cfunc (self, "disconnect", Network_disconnect);
	liscr_class_insert_cfunc (self, "has_client", Network_has_client);
	liscr_class_insert_cfunc (self, "host", Network_host);
	liscr_class_insert_cfunc (self, "join", Network_join);
	liscr_class_insert_cfunc (self, "send", Network_send);
	liscr_class_insert_cfunc (self, "shutdown", Network_shutdown);
	liscr_class_insert_cfunc (self, "swap_clients", Network_swap_clients);
}

/** @} */
/** @} */
