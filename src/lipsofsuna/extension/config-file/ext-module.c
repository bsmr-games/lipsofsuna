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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtConfigFile ConfigFile
 * @{
 */

#include "ext-module.h"

LIMaiExtensionInfo liext_config_file_info =
{
	LIMAI_EXTENSION_VERSION, "ConfigFile",
	liext_config_file_new,
	liext_config_file_free,
	liext_config_file_get_memstat
};

LIExtModule* liext_config_file_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_CONFIG_FILE, self);
	liext_script_config_file (program->script);

	return self;
}

void liext_config_file_free (
	LIExtModule* self)
{
	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_config_file_get_memstat (
	LIExtModule*  self,
	LIMaiMemstat* stat)
{
/* TODO: memstat */
}

/** @} */
/** @} */
