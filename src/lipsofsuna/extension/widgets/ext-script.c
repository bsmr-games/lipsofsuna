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
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * --- Adds a font style.
 * -- @param clss Widgets class.
 * -- @param args Arguments.<ul>
 * --   <li>1,name: Font name.</li>
 * --   <li>2,file: Font file.</li>
 * --   <li>3,size: Font size.</li></ul>
 * function Widgets.add_font_style(clss, args)
 */
static void Widgets_add_font_style (LIScrArgs* args)
{
	int size = 16;
	const char* file;
	const char* name;
	LIExtModule* module;

	/* Arguments. */
	if (!liscr_args_geti_string (args, 0, &name) &&
	    !liscr_args_gets_string (args, "name", &name))
		return;
	if (!liscr_args_geti_string (args, 1, &file) &&
	    !liscr_args_gets_string (args, "file", &file))
		return;
	if (!liscr_args_geti_int (args, 2, &size))
	    liscr_args_gets_int (args, "size", &size);

	/* Load the font. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	if (!liwdg_styles_load_font (module->widgets->styles, name, file, size))
		lisys_error_report ();
}

/* @luadoc
 * --- Draws the user interface.
 * -- @param clss Widgets class.
 * function Widgets.draw(clss)
 */
static void Widgets_draw (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	liwdg_manager_render (module->widgets);
}

/* @luadoc
 * --- Finds a widget by screen position.
 * -- @param clss Widgets class.
 * -- @param args Arguments.<ul>
 * --   <li>1,point: Screen position.</li></ul>
 * function Widgets.find_widget(clss, args)
 */
static void Widgets_find_widget (LIScrArgs* args)
{
	int x;
	int y;
	LIExtModule* module;
	LIWdgWidget* widget;
	LIMatVector vector;
	LIScrData* data;

	if (!liscr_args_gets_vector (args, "point", &vector) &&
	    !liscr_args_geti_vector (args, 0, &vector))
	{
		SDL_GetMouseState (&x, &y);
		vector = limat_vector_init (x, y, 0.0f);
	}

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	widget = liwdg_manager_find_widget_by_point (module->widgets, (int) vector.x, (int) vector.y);
	if (widget == NULL)
		return;
	data = liwdg_widget_get_script (widget);
	if (data == NULL)
		return;
	liscr_args_seti_data (args, data);
}

/* @luadoc
 * --- Currently focused widget.
 * -- @name Widgets.focused_widget
 * -- @class table
 */
static void Widgets_get_focused_widget (LIScrArgs* args)
{
	int x;
	int y;
	LIExtModule* module;
	LIWdgWidget* widget;
	LIScrData* data;

	SDL_GetMouseState (&x, &y);
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_WIDGETS);
	widget = liwdg_manager_find_widget_by_point (module->widgets, x, y);
	if (widget == NULL)
		return;
	data = liwdg_widget_get_script (widget);
	if (data == NULL)
		return;
	liscr_args_seti_data (args, data);
}

/*****************************************************************************/

void liext_script_widgets (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_WIDGETS, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "add_font_style", Widgets_add_font_style);
	liscr_class_insert_cfunc (self, "draw", Widgets_draw);
	liscr_class_insert_cfunc (self, "find_widget", Widgets_find_widget);
	liscr_class_insert_cfunc (self, "get_focused_widget", Widgets_get_focused_widget);
}

/** @} */
/** @} */
