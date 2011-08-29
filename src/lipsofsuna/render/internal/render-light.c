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
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenLight Light
 * @{
 */

#include "render-internal.h"
#include "render.h"
#include "../render21/render-private.h"
#include "../render32/render-private.h"

/**
 * \brief Creates a new light source.
 * \param render Renderer.
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
	LIRenRender* render,
	const float* ambient,
	const float* diffuse,
	const float* specular,
	const float* equation,
	float        cutoff,
	float        exponent,
	int          shadows)
{
	LIRenLight* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenLight));
	if (self == NULL)
		return 0;
	self->render = render;
	
	/* Choose a unique ID. */
	while (!self->id)
	{
		self->id = lialg_random_range (&render->random, 0x00000000, 0x0000FFFF);
		if (lialg_u32dic_find (render->lights, self->id))
			self->id = 0;
	}
	
	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->lights, self->id, self))
	{
		liren_light_free (self);
		return 0;
	}
	
	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_light32_new (render->v32, ambient, diffuse,
			specular, equation, cutoff, exponent, shadows);
		if (self->v32 == NULL)
		{
			liren_light_free (self);
			return 0;
		}
	}
	else
	{
		self->v21 = liren_light21_new (render->v21, ambient, diffuse,
			specular, equation, cutoff, exponent, shadows);
		if (self->v21 == NULL)
		{
			liren_light_free (self);
			return 0;
		}
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
	if (self->v32 != NULL)
		liren_light32_free (self->v32);
	if (self->v21 != NULL)
		liren_light21_free (self->v21);
	lisys_free (self);
}

void liren_light_set_ambient (
	LIRenLight*  self,
	const float* value)
{
	if (self->v32 != NULL)
		liren_light32_set_ambient (self->v32, value);
	else
		liren_light21_set_ambient (self->v21, value);
}

void liren_light_set_diffuse (
	LIRenLight*  self,
	const float* value)
{
	if (self->v32 != NULL)
		liren_light32_set_diffuse (self->v32, value);
	else
		liren_light21_set_diffuse (self->v21, value);
}

/**
 * \brief Enables or disables the light.
 * \param self Light source.
 * \param value Nonzero to enable.
 */
void liren_light_set_enabled (
	LIRenLight* self,
	int         value)
{
	if (self->v32 != NULL)
	{
		if (value == liren_light32_get_enabled (self->v32))
			return;
		if (value)
			liren_render32_insert_light (self->render->v32, self->v32);
		else
			liren_render32_remove_light (self->render->v32, self->v32);
	}
	else
	{
		if (value == liren_light21_get_enabled (self->v21))
			return;
		if (value)
			liren_render21_insert_light (self->render->v21, self->v21);
		else
			liren_render21_remove_light (self->render->v21, self->v21);
	}
}

void liren_light_set_equation (
	LIRenLight*  self,
	const float* value)
{
	if (self->v32 != NULL)
		liren_light32_set_equation (self->v32, value);
	else
		liren_light21_set_equation (self->v21, value);
}

/**
 * \brief Gets the priority of the light.
 * \param self Light source.
 * \return Priority value, higher means more important.
 */
void liren_light_set_priority (
	LIRenLight* self,
	float       value)
{
	if (self->v32 != NULL)
		return liren_light32_set_priority (self->v32, value);
	else
		return liren_light21_set_priority (self->v21, value);
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
	if (self->v32 != NULL)
		liren_light32_set_shadow (self->v32, value);
	else
		liren_light21_set_shadow (self->v21, value);
}

void liren_light_set_shadow_far (
	LIRenLight* self,
	float       value)
{
	if (self->v32 != NULL)
		liren_light32_set_shadow_far (self->v32, value);
	else
		liren_light21_set_shadow_far (self->v21, value);
}

void liren_light_set_shadow_near (
	LIRenLight* self,
	float       value)
{
	if (self->v32 != NULL)
		liren_light32_set_shadow_near (self->v32, value);
	else
		liren_light21_set_shadow_near (self->v21, value);
}

void liren_light_set_specular (
	LIRenLight*  self,
	const float* value)
{
	if (self->v32 != NULL)
		liren_light32_set_specular (self->v32, value);
	else
		liren_light21_set_specular (self->v21, value);
}

void liren_light_set_spot_cutoff (
	LIRenLight* self,
	float       value)
{
	if (self->v32 != NULL)
		liren_light32_set_spot_cutoff (self->v32, value);
	else
		liren_light21_set_spot_cutoff (self->v21, value);
}

void liren_light_set_spot_exponent (
	LIRenLight* self,
	float       value)
{
	if (self->v32 != NULL)
		liren_light32_set_spot_exponent (self->v32, value);
	else
		liren_light21_set_spot_exponent (self->v21, value);
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
	if (self->v32 != NULL)
		liren_light32_get_transform (self->v32, value);
	else
		liren_light21_get_transform (self->v21, value);
}

/**
 * \brief Sets the transformation of the light.
 * \param self Light source.
 * \param value Transformation.
 */
void liren_light_set_transform (
	LIRenLight*           self,
	const LIMatTransform* value)
{
	if (self->v32 != NULL)
		liren_light32_set_transform (self->v32, value);
	else
		liren_light21_set_transform (self->v21, value);
}

/** @} */
/** @} */
/** @} */
