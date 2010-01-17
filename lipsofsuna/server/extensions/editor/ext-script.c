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
 * \addtogroup LIExtEditor Editor
 * @{
 */

#include <lipsofsuna/main.h>
#include <lipsofsuna/server.h>
#include "ext-module.h"

/* @luadoc
 * module "Extension.Server.Editor"
 * ---
 * -- Allows dynamic editing of the game.
 * -- @name Editor
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Saves the current world map.
 * --
 * -- @param self Editor class.
 * function Editor.save(self)
 */
static void Editor_save (LIScrArgs* args)
{
	LIExtModule* module;
	LISerServer* server;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_EDITOR);
	server = limai_program_find_component (module->program, "server");
	if (server != NULL)
		liser_server_save (server);
}

/*****************************************************************************/

void
liext_script_editor (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EDITOR, data);
	liscr_class_insert_cfunc (self, "save", Editor_save);
}

/** @} */
/** @} */
