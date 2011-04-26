/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenLight Light
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-light.h"
#include "render-private.h"

/**
 * \brief Creates a new light source.
 * \param scene Scene.
 * \param ambient Ambient color, array of 4 floats.
 * \param diffuse Diffuse color, array of 4 floats.
 * \param specular Specular color, array of 4 floats.
 * \param equation Attenuation equation, array of 3 floats.
 * \param cutoff Spot cutoff in radians.
 * \param exponent Spot expoent.
 * \param shadows Nonzero if the lamp casts shadows.
 * \return New light source or NULL.
 */
LIRenLight* liren_light_new (
	LIRenScene*  scene,
	const float* ambient,
	const float* diffuse,
	const float* specular,
	const float* equation,
	float        cutoff,
	float        exponent,
	int          shadows)
{
	LIRenLight* self;

	self = lisys_calloc (1, sizeof (LIRenLight));
	if (self == NULL)
		return NULL;
	self->scene = scene;

	self->v32 = liren_light32_new (scene->v32, ambient, diffuse,
		specular, equation, cutoff, exponent, shadows);
	if (self->v32 == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees a light source.
 * \param self Light source.
 */
void liren_light_free (
	LIRenLight* self)
{
	liren_light32_free (self->v32);
	lisys_free (self);
}

void liren_light_get_ambient (
	LIRenLight* self,
	float*      value)
{
	liren_light32_get_ambient (self->v32, value);
}

void liren_light_set_ambient (
	LIRenLight*  self,
	const float* value)
{
	liren_light32_set_ambient (self->v32, value);
}

void liren_light_get_diffuse (
	LIRenLight* self,
	float*      value)
{
	liren_light32_get_diffuse (self->v32, value);
}

void liren_light_set_diffuse (
	LIRenLight*  self,
	const float* value)
{
	liren_light32_set_diffuse (self->v32, value);
}

/**
 * \brief Checks if the light is registered.
 * \param self Light source.
 * \return Nonzero if registered.
 */
int liren_light_get_enabled (
	const LIRenLight* self)
{
	return liren_light32_get_enabled (self->v32);
}

void liren_light_get_equation (
	LIRenLight* self,
	float*      value)
{
	liren_light32_get_equation (self->v32, value);
}

void liren_light_set_equation (
	LIRenLight*  self,
	const float* value)
{
	liren_light32_set_equation (self->v32, value);
}

/**
 * \brief Gets the OpenGL position vector for the light.
 * \param self Light source.
 * \param value Return location for 4 floats.
 */
void liren_light_get_position (
	const LIRenLight* self,
	GLfloat*          value)
{
	liren_light32_get_position (self->v32, value);
}

/**
 * \brief Gets the priority of the light.
 * \param self Light source.
 * \return Priority value, higher means more important.
 */
float liren_light_get_priority (
	LIRenLight* self)
{
	return liren_light32_get_priority (self->v32);
}

/**
 * \brief Sets the priority of the light.
 * \param self Light source.
 * \param value Priority value, higher means more important.
 */
void liren_light_set_priority (
	LIRenLight* self,
	float       value)
{
	liren_light32_set_priority (self->v32, value);
}

LIRenScene* liren_light_get_scene (
	const LIRenLight* self)
{
	return self->scene;
}

/**
 * \brief Gets the shadow casting mode of the light.
 * \param self Light source.
 * \return Nonzero if shadow casting is allowed, zero if disabled.
 */
int liren_light_get_shadow (
	const LIRenLight* self)
{
	return liren_light32_get_shadow (self->v32);
}

/**
 * \brief Sets the shadow casting mode of the light.
 * \param self Light source.
 * \param value Nonzero to allow shadow casting, zero to disable.
 */
void liren_light_set_shadow (
	LIRenLight* self,
	int         value)
{
	liren_light32_set_shadow (self->v32, value);
}

float liren_light_get_shadow_far (
	const LIRenLight* self)
{
	return liren_light32_get_shadow_far (self->v32);
}

void liren_light_set_shadow_far (
	LIRenLight* self,
	float       value)
{
	liren_light32_set_shadow_far (self->v32, value);
}

float liren_light_get_shadow_near (
	const LIRenLight* self)
{
	return liren_light32_get_shadow_near (self->v32);
}

void liren_light_set_shadow_near (
	LIRenLight* self,
	float       value)
{
	liren_light32_set_shadow_near (self->v32, value);
}

void liren_light_get_specular (
	LIRenLight* self,
	float*      value)
{
	liren_light32_get_specular (self->v32, value);
}

void liren_light_set_specular (
	LIRenLight*  self,
	const float* value)
{
	liren_light32_set_specular (self->v32, value);
}

float liren_light_get_spot_cutoff (
	const LIRenLight* self)
{
	return liren_light32_get_spot_cutoff (self->v32);
}

void liren_light_set_spot_cutoff (
	LIRenLight* self,
	float       value)
{
	liren_light32_set_spot_cutoff (self->v32, value);
}

float liren_light_get_spot_exponent (
	const LIRenLight* self)
{
	return liren_light32_get_spot_exponent (self->v32);
}

void liren_light_set_spot_exponent (
	LIRenLight* self,
	float       value)
{
	liren_light32_set_spot_exponent (self->v32, value);
}

/**
 * \brief Gets the transformation of the light.
 * \param self Light source.
 * \param value Return value for the transformation.
 */
void liren_light_get_transform (
	LIRenLight*     self,
	LIMatTransform* value)
{
	liren_light32_get_transform (self->v32, value);
}

/**
 * \brief Sets the transformation of the light.
 *
 * This modifies the modelview matrix.
 *
 * \param self Light source.
 * \param transform Transformation.
 */
void liren_light_set_transform (
	LIRenLight*           self,
	const LIMatTransform* value)
{
	liren_light32_set_transform (self->v32, value);
}

/** @} */
/** @} */
