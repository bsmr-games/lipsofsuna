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
 * \addtogroup LIThr Thread
 * @{
 * \addtogroup LIThrMutex Mutex
 * @{
 */

#include <pthread.h>
#include <lipsofsuna/system.h>
#include "thread-mutex.h"

struct _LIThrMutex
{
	pthread_mutex_t mutex;
};

LIThrMutex* lithr_mutex_new ()
{
	LIThrMutex* self;

	self = lisys_calloc (1, sizeof (LIThrMutex));
	if (self == NULL)
		return NULL;
	pthread_mutex_init (&self->mutex, NULL);

	return self;
}

void lithr_mutex_free (
	LIThrMutex* self)
{
	pthread_mutex_destroy (&self->mutex);
	lisys_free (self);
}

void lithr_mutex_lock (
	LIThrMutex* self)
{
	pthread_mutex_lock (&self->mutex);
}

void lithr_mutex_unlock (
	LIThrMutex* self)
{
	pthread_mutex_unlock (&self->mutex);
}

/** @} */
/** @} */
