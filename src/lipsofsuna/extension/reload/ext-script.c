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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtReload Reload
 * @{
 */

#include "ext-module.h"
#include "ext-reload.h"

static void Reload_get_enabled (LIScrArgs* args)
{
	LIExtReload* self;

	self = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RELOAD);
	liscr_args_seti_bool (args, liext_reload_get_enabled (self));
}
static void Reload_set_enabled (LIScrArgs* args)
{
	int value;
	LIExtReload* self;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		self = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_RELOAD);
		liext_reload_set_enabled (self, value);
	}
}

/*****************************************************************************/

void liext_script_reload (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RELOAD, "reload_get_enabled", Reload_get_enabled);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_RELOAD, "reload_set_enabled", Reload_set_enabled);
}

/** @} */
/** @} */
