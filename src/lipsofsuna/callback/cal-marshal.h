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
 * \addtogroup lical Callback
 * @{
 * \addtogroup licalMarshal Marshal
 * @{
 */

#ifndef __CAL_MARSHAL_H__
#define __CAL_MARSHAL_H__

#include <stdarg.h>
#include <lipsofsuna/system.h>

typedef int (*licalMarshal)(void*, void*, va_list);

#ifdef __cplusplus
extern "C" {
#endif

LIAPICALL (int, lical_marshal_DATA, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_FLT, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_INT, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_INT_INT, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_INT_PTR, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_PTR, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_PTR_INT, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_PTR_PTR, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_PTR_PTR_INT, (void* call, void* data, va_list args));
LIAPICALL (int, lical_marshal_DATA_PTR_PTR_PTR, (void* call, void* data, va_list args));

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
