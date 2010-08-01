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

#include "ext-module.h"
#include "ext-client.h"

#define CLIENT_REFRESH_RADIUS 10.0f

static int private_init (
	LIExtModule* self);

static int private_accept (
	grapple_user user,
	LIExtModule* self);

static int private_login (
	const char*  login,
	const char*  password,
	LIExtModule* self);

static int private_connect (
	LIExtModule*     self,
	grapple_message* message);

static int private_disconnect (
	LIExtModule*     self,
	grapple_message* message);

static int private_message_client (
	LIExtModule*     self,
	grapple_message* message);

static int private_message_server (
	LIExtModule*     self,
	grapple_message* message);

static int private_refused (
	LIExtModule*     self,
	grapple_message* message);

static int private_rename (
	LIExtModule*     self,
	grapple_message* message);

static int private_update (
	LIExtModule* self,
	float        secs);

static int private_shutdown (
	LIExtModule* self);

/*****************************************************************************/

LIMaiExtensionInfo liext_network_info =
{
	LIMAI_EXTENSION_VERSION, "Network",
	liext_network_new,
	liext_network_free
};

LIExtModule* liext_network_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Connect callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program, "program-shutdown", 0, private_shutdown, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_update, self, self->calls + 1))
	{
		liext_network_free (self);
		return NULL;
	}

	/* Initialize self. */
	if (!private_init (self))
	{
		liext_network_free (self);
		return NULL;
	}

	return self;
}

void liext_network_free (
	LIExtModule* self)
{
	LIAlgU32dicIter iter0;
	LIAlgStrdicIter iter1;

	if (self->clients != NULL)
	{
		lisys_assert (self->clients->size == 0);
		LIALG_U32DIC_FOREACH (iter0, self->clients)
			liext_client_free (iter0.value);
		lialg_u32dic_free (self->clients);
	}
	if (self->passwords != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->passwords)
			lisys_free (iter1.value);
		lialg_strdic_free (self->passwords);
	}
	if (self->client_socket != 0)
		grapple_client_destroy (self->client_socket);
	if (self->server_socket != 0)
		grapple_server_destroy (self->server_socket);
	pthread_mutex_destroy (&self->mutex);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

int liext_network_host (
	LIExtModule* self,
	int          udp,
	int          port)
{
	grapple_error error;

	/* Close old socket. */
	if (self->client_socket || self->server_socket)
		liext_network_shutdown (self);

	/* Create new socket. */
	self->server_socket = grapple_server_init ("Lips of Suna", LINET_PROTOCOL_VERSION);
	if (!self->server_socket)
	{
		lisys_error_set (EINVAL, "host: creating host socket failed");
		return 0;
	}
	grapple_server_notified_set (self->server_socket, GRAPPLE_NOTIFY_STATE_OFF);
	grapple_server_port_set (self->server_socket, port);
	grapple_server_protocol_set (self->server_socket, udp? GRAPPLE_PROTOCOL_UDP : GRAPPLE_PROTOCOL_TCP);
	grapple_server_session_set (self->server_socket, "");
	grapple_server_namepolicy_set (self->server_socket, GRAPPLE_NAMEPOLICY_REQUIRED);
	grapple_server_connectionhandler_set (self->server_socket, (grapple_connection_callback) private_accept, self);
	grapple_server_passwordhandler_set (self->server_socket, (grapple_password_callback) private_login, self);

	/* Wait for connections. */
	if (grapple_server_start (self->server_socket) != GRAPPLE_OK)
	{
		error = grapple_server_error_get (self->server_socket);
		lisys_error_set (EINVAL, "host: %s", grapple_error_text (error));
		grapple_server_destroy (self->server_socket);
		self->server_socket = 0;
		return 0;
	}

	return 1;
}

int liext_network_join (
	LIExtModule* self,
	int          udp,
	int          port,
	const char*  addr,
	const char*  name,
	const char*  pass)
{
	grapple_error error;

	/* Close old socket. */
	if (self->client_socket || self->server_socket)
		liext_network_shutdown (self);

	/* Create new socket. */
	self->client_socket = grapple_client_init ("Lips of Suna", LINET_PROTOCOL_VERSION);
	if (!self->client_socket)
		return 0;
	grapple_client_address_set (self->client_socket, addr);
	grapple_client_port_set (self->client_socket, port);
	if (udp)
		grapple_client_protocol_set (self->client_socket, GRAPPLE_PROTOCOL_UDP);
	else
		grapple_client_protocol_set (self->client_socket, GRAPPLE_PROTOCOL_TCP);
	grapple_client_name_set (self->client_socket, name);
	grapple_client_password_set (self->client_socket, pass);

	/* Connect to the host. */
	if (grapple_client_start (self->client_socket, 0) != GRAPPLE_OK)
	{
		error = grapple_client_error_get (self->client_socket);
		lisys_error_set (EINVAL, "connect: %s", grapple_error_text (error));
		grapple_client_destroy (self->client_socket);
		self->client_socket = 0;
		return 0;
	}

	return 1;
}

void liext_network_update (
	LIExtModule* self,
	float        secs)
{
	int ret;
	grapple_message* message;

	/* Handle client socket. */
	if (self->client_socket)
	{
		while (grapple_client_messages_waiting (self->client_socket))
		{
			message = grapple_client_message_pull (self->client_socket);
			switch (message->type)
			{
				case GRAPPLE_MSG_NEW_USER_ME:
				case GRAPPLE_MSG_USER_NAME:
				case GRAPPLE_MSG_SESSION_NAME:
					ret = 1;
					break;
				case GRAPPLE_MSG_USER_MSG:
					ret = private_message_client (self, message);
					break;
				case GRAPPLE_MSG_CONNECTION_REFUSED:
					ret = private_refused (self, message);
					break;
				case GRAPPLE_MSG_SERVER_DISCONNECTED:
					lisys_error_set (EINVAL, "disconnected");
					ret = 0;
					break;
				default:
					ret = 1;
					break;
			}
			grapple_message_dispose (message);
			if (!ret)
			{
				lisys_error_report ();
				liext_network_shutdown (self);
			}
		}
	}

	/* Handle server socket. */
	if (self->server_socket)
	{
		if (grapple_server_messages_waiting (self->server_socket))
		{
			message = grapple_server_message_pull (self->server_socket);
			switch (message->type)
			{
				case GRAPPLE_MSG_NEW_USER:
					private_connect (self, message);
					break;
				case GRAPPLE_MSG_USER_NAME:
					private_rename (self, message);
					break;
				case GRAPPLE_MSG_USER_MSG:
					private_message_server (self, message);
					break;
				case GRAPPLE_MSG_USER_DISCONNECTED:
					private_disconnect (self, message);
					break;
				default:
					break;
			}
			grapple_message_dispose (message);
		}
	}
}

/**
 * \brief Find a client by network user.
 *
 * \param self Network.
 * \param user Network user.
 * \return Client or NULL.
 */
LIExtClient* liext_network_find_client (
	LIExtModule* self,
	grapple_user user)
{
	return lialg_u32dic_find (self->clients, user);
}

int liext_network_send (
	LIExtModule* self,
	grapple_user user,
	LIArcWriter* writer,
	int          flags)
{
	LIExtClient* client;

	if (self->client_socket)
	{
		grapple_client_send (self->client_socket, GRAPPLE_SERVER, flags,
			liarc_writer_get_buffer (writer),
			liarc_writer_get_length (writer));
		return 1;
	}
	if (self->server_socket)
	{
		client = liext_network_find_client (self, user);
		if (client == NULL)
			return 0;
		liext_client_send (client, writer, flags);
		return 1;
	}

	return 0;
}

void liext_network_shutdown (
	LIExtModule* self)
{
	LIAlgU32dicIter iter0;
	LIAlgStrdicIter iter1;

	/* Disconnect and free clients. */
	LIALG_U32DIC_FOREACH (iter0, self->clients)
		liext_client_free (iter0.value);
	lialg_u32dic_clear (self->clients);

	/* Clear pending authorization requests. */
	LIALG_STRDIC_FOREACH (iter1, self->passwords)
		lisys_free (iter1.value);
	lialg_strdic_clear (self->passwords);

	/* Destroy socket. */
	if (self->client_socket)
	{
		grapple_client_destroy (self->client_socket);
		self->client_socket = 0;
	}
	if (self->server_socket)
	{
		grapple_server_destroy (self->server_socket);
		self->server_socket = 0;
	}
}

int liext_network_get_closed (
	LIExtModule* self)
{
	if (self->server_socket)
		return grapple_server_closed_get (self->server_socket);
	return 1;
}

void liext_network_set_closed (
	LIExtModule* self,
	int          value)
{
	if (self->server_socket)
		grapple_server_closed_set (self->server_socket, value);
}

int liext_network_get_connected (
	LIExtModule* self)
{
	return self->client_socket != 0 || self->server_socket != 0;
}

/*****************************************************************************/

static int private_init (
	LIExtModule* self)
{
	pthread_mutex_init (&self->mutex, NULL);

	/* Allocate client list. */
	self->clients = lialg_u32dic_new ();
	if (self->clients == NULL)
		return 0;
	self->passwords = lialg_strdic_new ();
	if (self->passwords == NULL)
		return 0;

	/* Register classes. */
	liscr_script_create_class (self->program->script, "Network", liext_script_network, self);

	return 1;
}

/**
 * \brief Handles newly accepted connections.
 *
 * This function is called by Grapple from one of its worker threads.
 * The main game thread is running in the background so we need to lock
 * any data we use.
 *
 * \param user Grapple user.
 * \param self Network.
 * \return Nonzero if the user is allowed to enter.
 */
static int private_accept (
	grapple_user user,
	LIExtModule* self)
{
	int ret = 1;
	char* address;

	/* Get client address. */
	address = grapple_server_client_address_get (self->server_socket, user);
	if (address == NULL)
		return 0;

	/* TODO: Check if banned. */
	lisys_free (address);
	return ret;
}

/**
 * \brief Handles users who send in their login request.
 *
 * This function is called by Grapple from one of its worker threads.
 * The main game thread is running in the background so we need to lock
 * any data we use.
 *
 * \param login Login name.
 * \param password User password.
 * \param self Network.
 * \return Nonzero if the user is allowed to enter.
 */
static int private_login (
	const char*   login,
	const char*   password,
	LIExtModule* self)
{
	int ret = 1;

	/* Sanity checks. */
	if (login == NULL || password == NULL || !strlen (login) || !strlen (password))
		return 0;

	/* TODO: Check for account. */

	return ret;
}

static int private_connect (
	LIExtModule*     self,
	grapple_message* message)
{
	char* pass;
	LIExtClient* client;

	/* Get temporarily stored password. */
	pthread_mutex_lock (&self->mutex);
	pass = lialg_strdic_find (self->passwords, message->NEW_USER.name);
	if (pass != NULL)
		lialg_strdic_remove (self->passwords, message->NEW_USER.name);
	pthread_mutex_unlock (&self->mutex);

	/* Create client. */
	client = liext_client_new (self, message->NEW_USER.id);
	if (client == NULL)
	{
		grapple_server_disconnect_client (self->server_socket, message->NEW_USER.id);
		liscr_script_set_gc (self->program->script, 1);
		lisys_free (pass);
		return 0;
	}
	if (!lialg_u32dic_insert (self->clients, message->NEW_USER.id, client))
	{
		liext_client_free (client);
		liscr_script_set_gc (self->program->script, 1);
		lisys_free (pass);
		return 0;
	}

	/* Emit a login event. */
	limai_program_event (self->program, "login",
		"client", LISCR_TYPE_INT, client->net,
		"login", LISCR_TYPE_STRING, message->NEW_USER.name,
		"password", LISCR_TYPE_STRING, pass, NULL);
	lisys_free (pass);
	liscr_script_set_gc (self->program->script, 1);

	return 1;
}

static int private_disconnect (
	LIExtModule*     self,
	grapple_message* message)
{
	LIExtClient* client;

	/* Get the client. */
	client = liext_network_find_client (self, message->USER_DISCONNECTED.id);
	if (client == NULL)
		return 0;
	liext_client_free (client);

	return 1;
}

static int private_message_client (
	LIExtModule*     self,
	grapple_message* message)
{
	int len;
	const uint8_t* data;
	LIArcReader* reader;
	LIScrData* packet;

	/* Check for valid length. */
	len = message->USER_MSG.length;
	data = message->USER_MSG.data;
	if (len < 1)
	{
		lisys_error_set (EINVAL, "invalid packet size");
		return 0;
	}

	/* Create packet reader. */
	reader = liarc_reader_new ((char*) data, len);
	if (reader == NULL)
		return 0;
	reader->pos = 1;

	/* Emit an event. */
	packet = liscr_packet_new_readable (self->program->script, reader);
	if (packet != NULL)
	{
		limai_program_event (self->program, "packet",
			"message", LISCR_TYPE_INT, data[0],
			"packet", LISCR_SCRIPT_PACKET, packet, NULL);
		liscr_data_unref (packet, NULL);
	}
	liarc_reader_free (reader);

	return 1;
}

static int private_message_server (
	LIExtModule*     self,
	grapple_message* message)
{
	LIArcReader* reader;
	LIExtClient* client;
	LIScrData* packet;

	/* Get the client. */
	client = liext_network_find_client (self, message->USER_MSG.id);
	if (client == NULL)
		return 0;

	/* Create packet reader. */
	if (message->USER_MSG.length < 1)
		return 0;
	reader = liarc_reader_new (message->USER_MSG.data, message->USER_MSG.length);
	if (reader == NULL)
		return 0;
	reader->pos = 1;

	/* Emit an event. */
	packet = liscr_packet_new_readable (self->program->script, reader);
	if (packet != NULL)
	{
		limai_program_event (self->program, "packet",
			"message", LISCR_TYPE_INT, ((uint8_t*) message->USER_MSG.data)[0],
			"client", LISCR_TYPE_INT, (int) client->net,
			"packet", LISCR_SCRIPT_PACKET, packet, NULL);
		liscr_data_unref (packet, NULL);
	}
	liarc_reader_free (reader);

	return 1;
}

static int private_refused (
	LIExtModule*     self,
	grapple_message* message)
{
	switch (message->CONNECTION_REFUSED.reason)
	{
		case GRAPPLE_NOCONN_VERSION_MISMATCH:
			lisys_error_set (LISYS_ERROR_VERSION, "incompatible server version");
			break;
		case GRAPPLE_NOCONN_SERVER_FULL:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "server is full");
			break;
		case GRAPPLE_NOCONN_SERVER_CLOSED:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "server is closed");
			break;
		case GRAPPLE_NOCONN_PASSWORD_MISMATCH:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "invalid password");
			break;
		case GRAPPLE_NOCONN_NAME_NOT_UNIQUE:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "account already in use");
			break;
		default:
			lisys_error_set (LISYS_ERROR_UNKNOWN, "unknown error %d", message->CONNECTION_REFUSED.reason);
			break;
	}
	return 0;
}

static int private_rename (
	LIExtModule*     self,
	grapple_message* message)
{
	LIExtClient* client;

	/* Get the client. */
	client = liext_network_find_client (self, message->USER_NAME.id);
	if (client == NULL)
		return 0;

	/* Not allowed. */
	return 0;
}

static int private_update (
	LIExtModule* self,
	float        secs)
{
	liext_network_update (self, secs);

	return 1;
}

static int private_shutdown (
	LIExtModule* self)
{
	/* The program is shutting down so we need to disconnect the clients before
	   the script gets deleted. Since client logouts emit events, there must not
	   be any clients left after this callback. */
	liext_network_shutdown (self);

	return 1;
}

/** @} */
/** @} */
