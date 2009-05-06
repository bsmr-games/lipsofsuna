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
 * \addtogroup limat Math
 * @{
 * \addtogroup limatVector Vector
 * @{
 */

#ifndef __MATH_VECTOR_H__
#define __MATH_VECTOR_H__

#include <math.h>
#include "math-generic.h"

/**
 * \brief A three-dimensional vector type.
 */
typedef struct _limatVector limatVector;
struct _limatVector
{
	float x;
	float y;
	float z;
};

/**
 * \brief Returns a vector with the given components.
 *
 * \param x Float.
 * \param y Float.
 * \param z Float.
 * \return Vector.
 */
static inline limatVector
limat_vector_init (float x,
                   float y,
                   float z)
{
	limatVector result = { x, y, z };
	return result;
}

/**
 * \brief Gets the opposite vector.
 *
 * \param self Vector.
 * \return Vector.
 */
static inline limatVector
limat_vector_invert (limatVector self)
{
	limatVector result = { -self.x, -self.y, -self.z };
	return result;
}

/**
 * \brief Gets the length of the vector.
 *
 * \param self Vector.
 * \return Float.
 */
static inline float
limat_vector_get_length (limatVector self)
{
	return sqrt (self.x * self.x + self.y * self.y + self.z * self.z);
}

/**
 * \brief Adds a vector to another.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Vector.
 */
static inline limatVector
limat_vector_add (limatVector self,
                  limatVector vector)
{
	limatVector result;

	result.x = self.x + vector.x;
	result.y = self.y + vector.y;
	result.z = self.z + vector.z;
	return result;
}

/**
 * \brief Subtracts a vector from another.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Vector.
 */
static inline limatVector
limat_vector_subtract (limatVector self,
                       limatVector vector)
{
	limatVector result;

	result.x = self.x - vector.x;
	result.y = self.y - vector.y;
	result.z = self.z - vector.z;
	return result;
}

/**
 * \brief Multiplies the vector by a scalar.
 *
 * \param self Vector.
 * \param scalar Vector.
 * \return Vector.
 */
static inline limatVector
limat_vector_multiply (limatVector self,
                       float       scalar)
{
	limatVector result;

	result.x = self.x * scalar;
	result.y = self.y * scalar;
	result.z = self.z * scalar;
	return result;
}

/**
 * \brief Normalizes the vector.
 *
 * \param self Vector.
 * \return Vector.
 */
static inline limatVector
limat_vector_normalize (limatVector self)
{
	limatVector result;
	float len = limat_vector_get_length (self);

	if (len < LI_MATH_EPSILON)
	{
		result.x = 0.0f;
		result.y = 0.0f;
		result.z = 0.0f;
	}
	else
	{
		result.x = self.x / len;
		result.y = self.y / len;
		result.z = self.z / len;
	}
	return result;
}

/**
 * \brief Calculates the scalar product of two vectors.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Scalar.
 */
static inline float
limat_vector_dot (limatVector self,
                  limatVector vector)
{
	return self.x * vector.x + self.y * vector.y + self.z * vector.z;
}

/**
 * \brief Calculates the cross product of two vectors.
 *
 * \param self Vector.
 * \param vector Vector.
 * \return Vector.
 */
static inline limatVector
limat_vector_cross (limatVector self,
                    limatVector vector)
{
	limatVector result;

	result.x =  (self.y * vector.z - self.z * vector.y);
	result.y = -(self.x * vector.z - self.z * vector.x);
	result.z =  (self.x * vector.y - self.y * vector.x);
	return result;
}

/**
 * \brief Performs linear vector interpolation.
 *
 * \param self First interpolated vector.
 * \param vector Second interpolated vector.
 * \param weight Interpolating scalar.
 * \return Vector.
 */
static inline limatVector
limat_vector_lerp (limatVector self,
                   limatVector vector,
                   float       weight)
{
	float a = weight;
	float b = 1.0f - weight;
	limatVector result;

	result.x = a * self.x + b * vector.x;
	result.y = a * self.y + b * vector.y;
	result.z = a * self.z + b * vector.z;

	return result;
}

#endif

/** @} */
/** @} */
