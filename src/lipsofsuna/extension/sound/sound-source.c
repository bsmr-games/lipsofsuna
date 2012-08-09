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

/**
 * \addtogroup LISnd Sound
 * @{
 * \addtogroup LISndSource Source
 * @{
 */

#include "lipsofsuna/system.h"
#ifndef LI_DISABLE_SOUND
#include "sound-private.h"

static int private_init_source (
	LISndSource* self);

static void private_free_source (
	LISndSource* self);

/*****************************************************************************/

/**
 * \brief Creates an empty sound source.
 * \param manager Sound manager.
 * \param stereo Nonzero to allow stereo.
 * \return New sound source or NULL.
 */
LISndSource* lisnd_source_new (
	LISndManager* manager,
	int           stereo)
{
	LISndSource* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LISndSource));
	if (self == NULL)
		return NULL;
	self->fade_factor = 0.0f;
	self->fade_value = 1.0f;
	self->looping = 0;
	self->offset = 0.0f;
	self->playing = 0;
	self->pitch = 1.0f;
	self->volume = 1.0f;
	self->stereo = stereo;
	self->manager = manager;

	/* Add to the dictionary. */
	if (!lialg_ptrdic_insert (manager->sources, self, self))
	{
		lisnd_source_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates an sound source and queues a sample.
 * \param manager Sound manager.
 * \param sample Sample to be queued.
 * \param stereo Nonzero to allow stereo.
 * \return New sound source or NULL.
 */
LISndSource* lisnd_source_new_with_sample (
	LISndManager* manager,
	LISndSample*  sample,
	int           stereo)
{
	LISndSource* self;

	/* Allocate self. */
	self = lisnd_source_new (manager, stereo);
	if (self == NULL)
		return NULL;

	/* Queue the sample. */
	lisnd_source_queue_sample (self, sample);

	return self;
}

/**
 * \brief Frees the sound source.
 * \param self Sound source.
 */
void lisnd_source_free (
	LISndSource* self)
{
	lialg_ptrdic_remove (self->manager->sources, self);
	private_free_source(self);
	lisys_free (self);
}

/**
 * \brief Unqueues samples already processed.
 *
 * You should call this function periodically, though not necessarily every
 * frame, for sources that make use of streaming to avoid the memory usage
 * growing too huge. If you don't queue samples or your sources are short
 * lived, you can omit calling this.
 *
 * \param self Sound source.
 * \param secs Seconds since last update.
 * \return The number of samples in the queue.
 */
int lisnd_source_update (
	LISndSource* self,
	float        secs)
{
	ALint num;
	ALuint buffer;

	/* Handle blocking samples. */
	/* Samples are loaded in separate threads so the sample may have not been
	   loaded yet. We need to pass until the sample has been loaded. */
	if (self->blocked_sample != NULL)
	{
		if (!self->blocked_sample->loaded)
			return 1;
		lisnd_source_queue_sample (self, self->blocked_sample);
	}
	if (!self->queued)
		return 0;

	/* Update fading. */
	if (self->fade_factor != 0.0f)
	{
		self->fade_value += secs * self->fade_factor;
		if (self->fade_value > 1.0f)
		{
			self->fade_value = 1.0f;
			self->fade_factor = 0.0f;
		}
		else if (self->fade_value < 0.0f)
		{
			self->fade_value = 0.0f;
			self->fade_factor = 0.0f;
		}
		alSourcef (self->source, AL_GAIN, self->volume * self->fade_value);
	}
	if (self->fade_value == 0.0f && self->fade_factor <= 0.0f)
		return 0;

	/* Unqueue finished buffers. */
	if (self->source)
	{
		alGetSourcei (self->source, AL_BUFFERS_PROCESSED, &num);
		self->queued -= num;
		while (num--)
			alSourceUnqueueBuffers (self->source, 1, &buffer);
	}
	if (!self->queued)
		private_free_source (self);

	return self->queued;
}

/**
 * \brief Queues a sample.
 * \param self Sound source.
 * \param sample Sample.
 */
void lisnd_source_queue_sample (
	LISndSource* self,
	LISndSample* sample)
{
	if (sample->loaded)
	{
		if (!private_init_source (self))
			return;
		if (self->stereo && sample->stereo)
			alSourceQueueBuffers (self->source, 1, sample->buffers + 1);
		else
			alSourceQueueBuffers (self->source, 1, sample->buffers);
		if (self->playing)
			alSourcePlay (self->source);
		self->blocked_sample = NULL;
		self->queued++;
	}
	else
		self->blocked_sample = sample;
}

/**
 * \brief Checks if the source has played all its samples.
 * \param self Sound source.
 * \return Nonzero if ended, zero if not.
 */
int lisnd_source_get_ended (
	LISndSource* self)
{
	return !self->queued && self->blocked_sample == NULL;
}

/**
 * \brief Sets the fading speed of the source.
 * \param self Sound source.
 * \param start Initial volume multiplier.
 * \param speed Fading amound per second.
 */
void lisnd_source_set_fading (
	LISndSource* self,
	float        start,
	float        speed)
{
	self->fade_value = start;
	self->fade_factor = speed;
	if (self->source)
		alSourcef (self->source, AL_GAIN, self->fade_value * self->volume);
}

/**
 * \brief Sets the looping setting of the source.
 * \param self Sound source.
 * \param looping Nonzero if should loop.
 */
void lisnd_source_set_looping (
	LISndSource* self,
	int          looping)
{
	self->looping = looping;
	if (self->source)
		alSourcei (self->source, AL_LOOPING, looping);
}

/**
 * \brief Gets the playback offset of the source.
 * \param self Sond source.
 * \return Offset in seconds.
 */
float lisnd_source_get_offset (
	LISndSource* self)
{
	float sec;

	if (self->source)
		alGetSourcef (self->source, AL_SEC_OFFSET, &sec);
	else
		sec = self->offset;

	return sec;
}

/**
 * \brief Sets the playback offset of the source.
 * \param self Sond source.
 * \param secs Offset in seconds.
 */
void lisnd_source_set_offset (
	LISndSource* self,
	float        secs)
{
	self->offset = secs;
	if (self->source)
		alSourcef (self->source, AL_SEC_OFFSET, secs);
}

/**
 * \brief Sets the pitch multiplier of the source.
 * \param self Sound source.
 * \param value Pitch multiplier.
 */
void lisnd_source_set_pitch (
	LISndSource* self,
	float        value)
{
	self->pitch = value;
	if (self->source)
		alSourcef (self->source, AL_PITCH, value);
}

/**
 * \brief Checks if the source is in playing state.
 * \param self Sound source.
 * \return Nonzero if playing, zero if stopped.
 */
int lisnd_source_get_playing (
	LISndSource* self)
{
	ALint state;

	if (self->playing)
		return 1;
	if (self->source)
	{
		alGetSourcei (self->source, AL_SOURCE_STATE, &state);
		return state == AL_PLAYING;
	}

	return 0;
}

/**
 * \brief Plays or stops the source.
 * \param self Sound source.
 * \param playing Nonzero for playing, zero for stopped.
 */
void lisnd_source_set_playing (
	LISndSource* self,
	int          playing)
{
	self->playing = playing;
	if (self->source)
	{
		if (playing)
			alSourcePlay (self->source);
		else
		{
			alGetSourcef (self->source, AL_SEC_OFFSET, &self->offset);
			alSourcePause (self->source);
		}
	}
}

/**
 * \brief Sets the position of the source.
 * \param self Sound source.
 * \param value Vector.
 */
void lisnd_source_set_position (
	LISndSource*       self,
	const LIMatVector* value)
{
	self->position = *value;
	if (self->source)
		alSource3f (self->source, AL_POSITION, -value->x, value->y, -value->z);
}

/**
 * \brief Checks if the source has stereo sound.
 * \param self Sound source.
 * \return Nonzero if stereo.
 */
int lisnd_source_get_stereo (
	LISndSource* self)
{
	return self->stereo;
}

/**
 * \brief Sets the velocity of the source.
 * \param self Sound source.
 * \param value Vector.
 */
void lisnd_source_set_velocity (
	LISndSource*       self,
	const LIMatVector* value)
{
	self->velocity = *value;
	if (self->source)
		alSource3f (self->source, AL_VELOCITY, value->x, value->y, value->z);
}

/**
 * \brief Sets the volume of the source.
 * \param self Sound source.
 * \param value Volume.
 */
void lisnd_source_set_volume (
	LISndSource* self,
	float        value)
{
	self->volume = value;
	if (self->source)
		alSourcef (self->source, AL_GAIN, self->fade_value * self->volume);
}

/*****************************************************************************/

static int private_init_source (
	LISndSource* self)
{
	if (self->source)
		return 1;
	alGenSources (1, &self->source);
	if (alGetError() != AL_NO_ERROR)
		return 0;
	alSourcef (self->source, AL_REFERENCE_DISTANCE, 1.0f);
	alSourcef (self->source, AL_MAX_DISTANCE, 100.0f);
	alSourcef (self->source, AL_GAIN, self->fade_value * self->volume);
	alSourcei (self->source, AL_LOOPING, self->looping);
	alSourcef (self->source, AL_SEC_OFFSET, self->offset);
	alSourcef (self->source, AL_PITCH, self->pitch);
	alSource3f (self->source, AL_POSITION, -self->position.x, self->position.y, -self->position.z);
	alSource3f (self->source, AL_VELOCITY, self->velocity.x, self->velocity.y, self->velocity.z);
	alSourcef (self->source, AL_GAIN, self->fade_value * self->volume);

	return 1;
}

static void private_free_source (
	LISndSource* self)
{
	if (self->source)
	{
		alSourceStop (self->source);
		alDeleteSources (1, &self->source);
		self->source = 0;
	}
}

#endif

/** @} */
/** @} */

