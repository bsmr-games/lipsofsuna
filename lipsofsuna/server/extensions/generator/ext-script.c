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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvGenerator Generator
 * @{
 */

#include <lipsofsuna/script.h>
#include <lipsofsuna/server.h>
#include "ext-generator.h"
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Generator"
 * ---
 * -- Generate random dungeons.
 * -- @name Generator
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Saves the current world map.
 * --
 * -- @param self Generator class.
 * Generator.save(self)
 */
static void Generator_save (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	liser_server_save (module->server);
}

/*****************************************************************************/

void
liext_script_generator (LIScrClass* self,
                      void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_GENERATOR, data);
	liscr_class_insert_cfunc (self, "save", Generator_save);
}

/** @} */
/** @} */
/** @} */
