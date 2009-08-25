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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup liwdgCheck Check
 * @{
 */

#include "widget-check.h"
#include "widget-manager.h"

static int
private_init (liwdgCheck*   self,
              liwdgManager* manager);

static void
private_free (liwdgCheck* self);

static int
private_event (liwdgCheck* self,
               liwdgEvent* event);

static void
private_rebuild (liwdgCheck* self);

const liwdgClass liwdgCheckType =
{
	LIWDG_BASE_STATIC, &liwdgWidgetType, "Check", sizeof (liwdgCheck),
	(liwdgWidgetInitFunc) private_init,
	(liwdgWidgetFreeFunc) private_free,
	(liwdgWidgetEventFunc) private_event
};

/****************************************************************************/

liwdgWidget*
liwdg_check_new (liwdgManager* manager)
{
	return liwdg_widget_new (manager, &liwdgCheckType);
}

int
liwdg_check_get_active (liwdgCheck* self)
{
	return self->active;
}

void
liwdg_check_set_active (liwdgCheck* self,
                        int         active)
{
	self->active = active;
}

lifntFont*
liwdg_check_get_font (liwdgCheck* self)
{
	return self->font;
}

void
liwdg_check_set_font (liwdgCheck* self,
                      lifntFont*  font)
{
	self->font = font;
	private_rebuild (self);
}

const char*
liwdg_check_get_text (liwdgCheck* self)
{
	return self->string;
}

int
liwdg_check_set_text (liwdgCheck* self,
                      const char* text)
{
	char* tmp;

	tmp = listr_dup (text);
	if (tmp == NULL)
		return 0;
	lisys_free (self->string);
	self->string = tmp;
	private_rebuild (self);
	return 1;
}

/****************************************************************************/

static int
private_init (liwdgCheck*   self,
              liwdgManager* manager)
{
	if (!liwdg_widget_register_callback (LIWDG_WIDGET (self), LIWDG_CALLBACK_PRESSED, lical_marshal_DATA))
		return 0;
	liwdg_widget_set_focusable (LIWDG_WIDGET (self), 1);
	self->string = lisys_calloc (1, 1);
	if (self->string == NULL)
		return 0;
	self->font = liwdg_manager_find_font (manager, "default");
	self->text = lifnt_layout_new ();
	if (self->text == NULL)
	{
		lisys_free (self->string);
		return 0;
	}
	private_rebuild (self);

	return 1;
}

static void
private_free (liwdgCheck* self)
{
	lifnt_layout_free (self->text);
	lisys_free (self->string);
}

static int
private_event (liwdgCheck* self,
               liwdgEvent* event)
{
	int h;
	const char* mode;
	liwdgRect rect;
	liwdgStyle* style;

	switch (event->type)
	{
		case LIWDG_EVENT_TYPE_KEY_PRESS:
			if (event->key.keycode != SDLK_RETURN)
				return 1;
			self->active = !self->active;
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED);
			return 0;
		case LIWDG_EVENT_TYPE_BUTTON_PRESS:
			self->active = !self->active;
			lical_callbacks_call (LIWDG_WIDGET (self)->callbacks, LIWDG_CALLBACK_PRESSED);
			return 0;
		case LIWDG_EVENT_TYPE_RENDER:
			if (liwdg_widget_get_focus_keyboard (LIWDG_WIDGET (self)) && self->active)
				mode = "check-active-focus";
			else if (liwdg_widget_get_focus_keyboard (LIWDG_WIDGET (self)))
				mode = "check-focus";
			else if (self->active)
				mode = "check-active";
			else
				mode = "check";
			style = liwdg_widget_get_style (LIWDG_WIDGET (self), mode);
			liwdg_widget_get_style_allocation (LIWDG_WIDGET (self), mode, &rect);
			liwdg_widget_paint (LIWDG_WIDGET (self), mode, NULL);
			glColor4fv (style->color);
			h = lifnt_layout_get_height (self->text);
			lifnt_layout_render (self->text, rect.x, rect.y + (rect.height - h) / 2 - 2);
			return 1;
	}

	return liwdgWidgetType.event (LIWDG_WIDGET (self), event);
}

static void
private_rebuild (liwdgCheck* self)
{
	int h = 0;

	lifnt_layout_clear (self->text);
	if (self->font != NULL)
	{
		h = lifnt_font_get_height (self->font);
		lifnt_layout_append_string (self->text, self->font, self->string);
	}
	liwdg_widget_set_style_request (LIWDG_WIDGET (self),
		lifnt_layout_get_width (self->text), LI_MAX (
		lifnt_layout_get_height (self->text), h), "check");
}

/** @} */
/** @} */
