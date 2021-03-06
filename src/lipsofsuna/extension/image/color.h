/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_IMAGE_COLOR_H__
#define __EXT_IMAGE_COLOR_H__

#include "lipsofsuna/system/system-types.h"

typedef struct _LIImgColor LIImgColor;
struct _LIImgColor
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

typedef struct _LIImgColorHSV LIImgColorHSV;
struct _LIImgColorHSV
{
	float h;
	float s;
	float v;
};

LIAPICALL (void, liimg_color_rgb_to_hsv, (
	const LIImgColor* self,
	LIImgColorHSV*    result));

LIAPICALL (void, liimg_color_hsv_to_rgb, (
	const LIImgColorHSV* self,
	LIImgColor*          result));

#endif
