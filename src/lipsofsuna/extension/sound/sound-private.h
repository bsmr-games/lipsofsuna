/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __SOUND_PRIVATE_H__
#define __SOUND_PRIVATE_H__

#include <AL/al.h>
#include <AL/alc.h>
#include "lipsofsuna/algorithm.h"
#include "sound-manager.h"
#include "sound-system.h"
#include "sound-source.h"
#include "sound-sample.h"

struct _LISndManager
{
	LISndSystem* system;
	LIAlgStrdic* samples;
	LIAlgPtrdic* sources;
};

struct _LISndSystem
{
	ALCdevice* device;
	ALCcontext* context;
	LIMatVector listener_position;
	LIMatVector listener_velocity;
};

struct _LISndSource
{
	int playing;
	int stereo;
	float fade_factor;
	float fade_value;
	float looping;
	float offset;
	float pitch;
	float max_dist;
	float ref_dist;
	float rolloff;
	float volume;
	ALuint source;
	ALint queued;
	LIMatVector position;
	LIMatVector velocity;
	LISndManager* manager;
	LISndSample* blocked_sample;
};

struct _LISndSample
{
	int loaded;
	int stereo;
	char* file;
	ALuint buffers[2];
	LISysAsyncCall* worker;
};

#endif
