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
 * \addtogroup liextcliSpeech Speech
 * @{
 */

#ifndef __EXT_SPEECH_H__
#define __EXT_SPEECH_H__

#include "ext-module.h"

#define LIEXT_SPEECH_TIMEOUT 10.0f

typedef struct _LIExtSpeech LIExtSpeech;
struct _LIExtSpeech
{
	float timer;
	float alpha;
	LIFntLayout* text;
};

LIExtSpeech*
liext_speech_new (LICliClient* client,
                  const char*  text);

void
liext_speech_free (LIExtSpeech* self);

#endif

/** @} */
/** @} */
/** @} */
