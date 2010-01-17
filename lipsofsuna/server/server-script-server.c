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
 * \addtogroup liser Server
 * @{
 * \addtogroup liserscr Script
 * @{
 * \addtogroup liserscrServer Server
 * @{
 */

#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>

/* @luadoc
 * module "Core.Server.Server"
 * ---
 * -- Control the global server state.
 * -- @name Server
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Finds an object by ID.
 * --
 * -- Arguments:
 * -- id: Object ID. (required)
 * -- point: Center point for radius check.
 * -- radius: Maximum distance from center point.
 * --
 * -- @param self Server class.
 * -- @param args Arguments.
 * -- @return Object or nil.
 * function Server.find_object(self, args)
 */
static void Server_find_object (LIScrArgs* args)
{
	int id;
	float radius;
	LIEngObject* object;
	LIMatTransform transform;
	LIMatVector center;
	LISerServer* server;

	if (liscr_args_gets_int (args, "id", &id))
	{
		/* Find object. */
		server = liscr_class_get_userdata (args->clss, LISER_SCRIPT_SERVER);
		object = lieng_engine_find_object (server->engine, id);
		if (object == NULL)
			return;

		/* Optional radius check. */
		if (liscr_args_gets_vector (args, "point", &center) &&
		    liscr_args_gets_float (args, "radius", &radius))
		{
			if (!lieng_object_get_realized (object))
				return;
			lieng_object_get_transform (object, &transform);
			center = limat_vector_subtract (center, transform.position);
			if (limat_vector_get_length (center) > radius)
				return;
		}

		/* Return object. */
		liscr_args_seti_data (args, object->script);
	}
}

/* @luadoc
 * ---
 * -- Begins listening for clients.
 * --
 * -- Must be called before any other function.
 * --
 * -- Arguments:
 * -- port: Port to listen to.
 * -- udp: True for UDP.
 * --
 * -- @param self Server class.
 * -- @param args Arguments.
 * -- @return True on success.
 * function Server.host(self, args)
 */
static void Server_host (LIScrArgs* args)
{
	int port = 10101;
	int udp = 0;
	LISerServer* server;

	server = liscr_class_get_userdata (args->clss, LISER_SCRIPT_SERVER);
	liscr_args_gets_bool (args, "udp", &udp);
	liscr_args_gets_int (args, "port", &port);
	port = LIMAT_CLAMP (port, 1025, 32767);

	if (server->network == NULL)
	{
		server->network = liser_network_new (server, udp, port);
		if (server->network != NULL)
			return liscr_args_seti_bool (args, 1);
	}
}

/* @luadoc
 * ---
 * -- Finds all objects inside a sphere.
 * --
 * -- Arguments:
 * -- point: Center point. (required)
 * -- match: Match type. ("all"/"clients")
 * -- radius: Search radius.
 * --
 * -- @param self Server class.
 * -- @param args Arguments.
 * -- @return Array of matching objects.
 * function Server.nearby_object(self, args)
 */
static void Server_nearby_objects (LIScrArgs* args)
{
	int onlyclients = 0;
	float radius = 32.0f;
	const char* tmp;
	LIEngObjectIter iter;
	LIMatVector center;
	LISerServer* server;

	/* Check arguments. */
	server = liscr_class_get_userdata (args->clss, LISER_SCRIPT_SERVER);
	if (!liscr_args_gets_vector (args, "point", &center))
		return;
	liscr_args_gets_float (args, "radius", &radius);
	if (liscr_args_gets_string (args, "match", &tmp))
	{
		if (!strcmp (tmp, "clients"))
			onlyclients = 1;
	}
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);

	/* Find objects. */
	LIENG_FOREACH_OBJECT (iter, server->engine, &center, radius)
	{
		if (!onlyclients || LISER_OBJECT (iter.object)->client != NULL)
			liscr_args_seti_data (args, iter.object->script);
	}
}

/* @luadoc
 * ---
 * -- Request server save.
 * --
 * -- @param self Server class.
 * function Server.save(self)
 */
static void Server_save (LIScrArgs* args)
{
	LISerServer* server;

	server = liscr_class_get_userdata (args->clss, LISER_SCRIPT_SERVER);
#warning Saving not implemented.
//	lua_pushnumber (lua, liser_server_save (server));
}

/* @luadoc
 * ---
 * -- Request server shutdown.
 * --
 * -- @param self Server class.
 * function Server.shutdown(self)
 */
static void Server_shutdown (LIScrArgs* args)
{
	LISerServer* server;

	server = liscr_class_get_userdata (args->clss, LISER_SCRIPT_SERVER);
	limai_program_shutdown (server->program);
}

/* @luadoc
 * ---
 * -- Number of seconds the server has been running.
 * -- @name Server.time
 * -- @class table
 */
static void Server_getter_time (LIScrArgs* args)
{
	LISerServer* server;

	server = liscr_class_get_userdata (args->clss, LISER_SCRIPT_SERVER);
	liscr_args_seti_float (args, limai_program_get_time (server->program));
}

/*****************************************************************************/

void
liser_script_server (LIScrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LISER_SCRIPT_SERVER, data);
	liscr_class_insert_cfunc (self, "find_object", Server_find_object);
	liscr_class_insert_cfunc (self, "host", Server_host);
	liscr_class_insert_cfunc (self, "nearby_objects", Server_nearby_objects);
	liscr_class_insert_cfunc (self, "save", Server_save);
	liscr_class_insert_cfunc (self, "shutdown", Server_shutdown);
	liscr_class_insert_cvar (self, "time", Server_getter_time, NULL);
}

/** @} */
/** @} */
/** @} */
