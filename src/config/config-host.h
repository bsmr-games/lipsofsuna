/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup licfg Config
 * @{
 * \addtogroup licfgHost Host
 * @{
 */

#ifndef __CONFIG_HOST_H__
#define __CONFIG_HOST_H__

typedef struct _licfgHost licfgHost;
struct _licfgHost
{
	char* host;
	int port;
	int udp;
};

licfgHost*
licfg_host_new (const char* dir);

void
licfg_host_free (licfgHost* self);

#endif

/** @} */
/** @} */

