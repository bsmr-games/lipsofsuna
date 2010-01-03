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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#ifndef __CLIENT_SLOT_H__
#define __CLIENT_SLOT_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/render.h>

typedef struct _LIExtSlot LIExtSlot;
struct _LIExtSlot
{
	LICliClient* client;
	LIEngObject* object;
	LIEngConstraint* constraint;
};

LIExtSlot*
liext_slot_new (LICliClient* client,
                LIEngObject* object,
                const char*  node0,
                const char*  node1,
                int          model);

void
liext_slot_free (LIExtSlot* self);

#endif

/** @} */
/** @} */
/** @} */
