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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgWidget Widget
 * @{
 */

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-group.h"
#include "widget-window.h"

static int
private_new (LIWdgWidget*      self,
             const LIWdgClass* clss,
             LIWdgManager*     manager);

static int
private_init (LIWdgWidget*  self,
              LIWdgManager* manager);

static void
private_free (LIWdgWidget* self);

static int
private_event (LIWdgWidget* self,
               LIWdgEvent*  event);

static void
private_paint_scaled (LIWdgWidget* self,
                      LIWdgRect*   rect,
                      LIWdgStyle*  style_);

static void
private_paint_tiled (LIWdgWidget* self,
                     LIWdgRect*   rect,
                     LIWdgStyle*  style_);

static void
private_rebuild_style (LIWdgWidget* self);

/*****************************************************************************/

const LIWdgClass*
liwdg_widget_widget ()
{
	static const LIWdgClass clss =
	{
		NULL, "Widget", sizeof (LIWdgWidget),
		(LIWdgWidgetInitFunc) private_init,
		(LIWdgWidgetFreeFunc) private_free,
		(LIWdgWidgetEventFunc) private_event
	};
	return &clss;
}

LIWdgWidget*
liwdg_widget_new (LIWdgManager*     manager,
                  const LIWdgClass* clss)
{
	LIWdgWidget* self;

	self = lisys_calloc (1, clss->size);
	if (self == NULL)
		return 0;
	self->manager = manager;
	self->type = clss;
	if (!private_new (self, clss, manager))
		return 0;

	return self;
}

/**
 * \brief Frees the widget.
 *
 * This function can be called for all inherited classes.
 *
 * \param self Widget.
 */
void
liwdg_widget_free (LIWdgWidget* self)
{
	const LIWdgClass* ptr;

	lical_callbacks_call (self->manager->callbacks, self->manager, "widget-free", lical_marshal_DATA_PTR, self);
	for (ptr = self->type ; ptr != NULL ; ptr = liwdg_class_get_base (ptr))
	{
		if (ptr->free != NULL)
			ptr->free (self);
	}
	lisys_free (self);
}

/**
 * \brief Unparents the widget and removes from the dialog list.
 *
 * If the widget is a child widget of another widget, it is removed from the
 * parent container. If it is the root widget of the widget manager or a dialog,
 * it is removed from the manager.
 *
 * \param self Widget.
 * \return Nonzero if was detached from something.
 */
int
liwdg_widget_detach (LIWdgWidget* self)
{
	int changed = 0;

	switch (self->state)
	{
		/* Remove from parent container. */
		case LIWDG_WIDGET_STATE_DETACHED:
			if (self->parent != NULL)
			{
				lisys_assert (liwdg_widget_typeis (self->parent, liwdg_widget_container ()));
				liwdg_container_detach_child (LIWDG_CONTAINER (self->parent), self);
				changed = 1;
			}
			break;

		/* Remove from popup stack. */
		case LIWDG_WIDGET_STATE_POPUP:
			liwdg_manager_remove_popup (self->manager, self);
			changed = 1;
			break;

		/* Remove from manager root. */
		case LIWDG_WIDGET_STATE_ROOT:
			lisys_assert (liwdg_manager_get_root (self->manager) == self);
			liwdg_manager_set_root (self->manager, NULL);
			changed = 1;
			break;

		/* Remove from window stack. */
		case LIWDG_WIDGET_STATE_WINDOW:
			liwdg_manager_remove_window (self->manager, self);
			changed = 1;
			break;
	}
	self->state = LIWDG_WIDGET_STATE_DETACHED;

	return changed;
}

void
liwdg_widget_draw (LIWdgWidget* self)
{
	LIWdgEvent event;

	if (self->visible)
	{
		event.type = LIWDG_EVENT_TYPE_RENDER;
		self->type->event (self, &event);
	}
}

/**
 * \brief Handles an event.
 *
 * \param self Widget.
 * \param event Event.
 * \return Nonzero if passed through unhandled, zero if absorbed by the widget.
 */
int
liwdg_widget_event (LIWdgWidget* self,
                    LIWdgEvent*  event)
{
	return self->type->event (self, event);
}

int
liwdg_widget_insert_callback (LIWdgWidget* self,
                              const char*  type,
                              void*        func,
                              void*        data)
{
	return lical_callbacks_insert (self->manager->callbacks, self, type, 0, func, data, NULL);
}

int
liwdg_widget_insert_callback_full (LIWdgWidget* self,
                                   const char*  type,
                                   int          priority,
                                   void*        func,
                                   void*        data,
                                   LICalHandle* handle)
{
	return lical_callbacks_insert (self->manager->callbacks, self, type, priority, func, data, handle);
}

void
liwdg_widget_move (LIWdgWidget* self,
                   int          x,
                   int          y)
{
	liwdg_widget_set_allocation (self, x, y,
		self->allocation.width,
		self->allocation.height);
}

/**
 * \brief Paints widget graphics.
 *
 * \param self Widget.
 * \param rect Rectangle or NULL for the allocation of the widget.
 */
void
liwdg_widget_paint (LIWdgWidget* self,
                    LIWdgRect*   rect)
{
	LIWdgStyle* style_;

	/* Get style. */
	if (rect == NULL)
		rect = &self->allocation;
	style_ = self->style;
	if (style_->texture == NULL)
		return;

	/* Paint the style. */
	if (style_->scale)
		private_paint_scaled (self, rect, style_);
	else
		private_paint_tiled (self, rect, style_);
}

/**
 * \brief Translates coordinates from screen space to widget space.
 *
 * Coordinate translation is needed when widgets are inside a scrollable viewport.
 * For example, if a widget wishes to check if screen space pointer coordinates
 * are inside a specific area of the widget, it needs to translate the coordinates
 * first with this function.
 *
 * \param self Widget.
 * \param screenx Coordinates in screen space.
 * \param screeny Coordinates in screen space.
 * \param widgetx Coordinates in widget space.
 * \param widgety Coordinates in widget space.
 */
void
liwdg_widget_translate_coords (LIWdgWidget* self,
                               int          screenx,
                               int          screeny,
                               int*         widgetx,
                               int*         widgety)
{
	*widgetx = screenx;
	*widgety = screeny;
	if (self->parent != NULL)
		liwdg_widget_translate_coords (self->parent, screenx, screeny, widgetx, widgety);
	if (liwdg_widget_typeis (self, liwdg_widget_container ()))
		liwdg_container_translate_coords (LIWDG_CONTAINER (self), screenx, screeny, widgetx, widgety);
}

/**
 * \brief Checks if the widget implements the given class.
 *
 * \param self Widget.
 * \param clss Class.
 * \return Nonzero if implements.
 */
int
liwdg_widget_typeis (const LIWdgWidget* self,
                     const LIWdgClass*  clss)
{
	const LIWdgClass* ptr;

	for (ptr = self->type ; ptr != NULL ; ptr = liwdg_class_get_base (ptr))
	{
		if (clss == ptr)
			return 1;
	}

	return 0;
}

/**
 * \brief Calls the type specific update handler of the widget.
 *
 * \param self Widget.
 * \param secs Seconds since last update.
 */
void
liwdg_widget_update (LIWdgWidget* self,
                     float        secs)
{
	LIWdgEvent event;

	event.type = LIWDG_EVENT_TYPE_UPDATE;
	event.update.secs = secs;
	self->type->event (self, &event);
}

void
liwdg_widget_get_allocation (LIWdgWidget* self,
                             LIWdgRect*   allocation)
{
	*allocation = self->allocation;
}

void
liwdg_widget_set_allocation (LIWdgWidget* self,
                             int          x,
                             int          y,
                             int          w,
                             int          h)
{
	LIWdgEvent event;

	if (self->allocation.x != x ||
	    self->allocation.y != y ||
	    self->allocation.width != w ||
	    self->allocation.height != h)
	{
		self->allocation.x = x;
		self->allocation.y = y;
		self->allocation.width = w;
		self->allocation.height = h;
		event.type = LIWDG_EVENT_TYPE_ALLOCATION;
		liwdg_widget_event (self, &event);
	}
}

/**
 * \brief Gets the font provided by the style of the widget.
 *
 * \param self Widget.
 * \return Font or NULL.
 */
LIFntFont*
liwdg_widget_get_font (const LIWdgWidget* self)
{
	if (self->style == NULL)
		return NULL;
	return liwdg_manager_find_font (self->manager, self->style->font);
}

/**
 * \brief Gets the rectangle of the contents of the widget.
 *
 * The content rectangle is the allocation of the widget minus style paddings.
 *
 * \param self Widget.
 * \param allocation Return location for a rectangle.
 */
void
liwdg_widget_get_content (LIWdgWidget* self,
                          LIWdgRect*   allocation)
{
	allocation->x = self->allocation.x + self->style->pad[1];
	allocation->y = self->allocation.y + self->style->pad[0];
	allocation->width = self->allocation.width - self->style->pad[1] - self->style->pad[2];
	allocation->height = self->allocation.height - self->style->pad[0] - self->style->pad[3];
}

int
liwdg_widget_get_focusable (LIWdgWidget* self)
{
	return self->focusable;
}

void
liwdg_widget_set_focusable (LIWdgWidget* self,
                            int          focusable)
{
	self->focusable = focusable;
}

/**
 * \brief Gets the focus state of the widget.
 *
 * \param self Widget.
 * \return Nonzero if the widget has focus.
 */
int
liwdg_widget_get_focused (LIWdgWidget* self)
{
	return (liwdg_manager_get_focus (self->manager) == self);
}

/**
 * \brief Gives focus to the widget.
 *
 * \param self Widget.
 */
void
liwdg_widget_set_focused (LIWdgWidget* self)
{
	if (liwdg_manager_get_focus (self->manager) != self &&
	    liwdg_widget_get_visible (self))
		liwdg_manager_set_focus (self->manager, self);
}

int
liwdg_widget_get_grab (const LIWdgWidget* self)
{
	return self->manager->widgets.grab == self;
}

void
liwdg_widget_set_grab (LIWdgWidget* self,
                       int          value)
{
	int cx;
	int cy;

	if (value)
	{
		cx = self->manager->width / 2;
		cy = self->manager->height / 2;
		self->manager->video.SDL_ShowCursor (SDL_DISABLE);
		self->manager->video.SDL_WarpMouse (cx, cy);
		self->manager->video.SDL_WM_GrabInput (SDL_GRAB_OFF);
		self->manager->widgets.grab = self;
	}
	else
	{
		self->manager->video.SDL_ShowCursor (SDL_ENABLE);
		self->manager->video.SDL_WM_GrabInput (SDL_GRAB_OFF);
		self->manager->widgets.grab = NULL;
	}
}

/**
 * \brief Gets the full size request of the widget.
 *
 * Returns the larger of the user and hard size requests, combined with the
 * style paddings of the widget.
 *
 * \param self Widget.
 * \param request Return location for the size.
 */
void
liwdg_widget_get_request (LIWdgWidget* self,
                          LIWdgSize*   request)
{
	/* Get ordinary request. */
	request->width = self->hardrequest.width;
	request->height = self->hardrequest.height;
	if (self->userrequest.width != -1)
		request->width = LIMAT_MAX (request->width, self->userrequest.width);
	if (self->userrequest.height != -1)
		request->height = LIMAT_MAX (request->height, self->userrequest.height);

	/* Add style paddings. */
	request->width += self->style->pad[1] + self->style->pad[2];
	request->height += self->style->pad[0] + self->style->pad[3];
}

/**
 * \brief Sets or unsets the user overridden size request of the widget.
 *
 * \param self Widget.
 * \param w Width or -1 to unset.
 * \param h Height or -1 to unset.
 */
void
liwdg_widget_set_request (LIWdgWidget* self,
                          int          w,
                          int          h)
{
	if (self->userrequest.width != w ||
	    self->userrequest.height != h)
	{
		self->userrequest.width = w;
		self->userrequest.height = h;
		if (self->parent != NULL)
			liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	}
}

/**
 * \brief Sets the internal size request of the widget.
 *
 * \param self Widget.
 * \param w Width.
 * \param h Height.
 */
void
liwdg_widget_set_request_internal (LIWdgWidget* self,
                                   int          w,
                                   int          h)
{
	if (self->hardrequest.width != w ||
	    self->hardrequest.height != h)
	{
		self->hardrequest.width = w;
		self->hardrequest.height = h;
		if (self->parent != NULL)
			liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	}
}

LIWdgWidget*
liwdg_widget_get_root (LIWdgWidget* self)
{
	LIWdgWidget* widget;

	for (widget = self ; widget->parent != NULL ; widget = widget->parent) { }
	return widget;
}

void
liwdg_widget_set_state (LIWdgWidget* self,
                        const char*  state)
{
	char* tmp;

	/* Early exit. */
	if ((self->state_name == NULL && state == NULL) ||
	    (self->state_name != NULL && state != NULL && !strcmp (state, self->state_name)))
		return;

	/* Store state name. */
	if (state != NULL)
	{
		tmp = listr_dup (state);
		if (tmp != NULL)
		{
			lisys_free (self->state_name);
			self->state_name = tmp;
		}
	}
	else
	{
		lisys_free (self->state_name);
		self->state_name = NULL;
	}

	/* Rebuild style. */
	private_rebuild_style (self);
}

LIWdgStyle*
liwdg_widget_get_style (LIWdgWidget* self)
{
	return self->style;
}

void
liwdg_widget_set_style (LIWdgWidget* self,
                        const char*  style)
{
	char* tmp;

	/* Early exit. */
	if ((self->style_name == NULL && style == NULL) ||
	    (self->style_name != NULL && style != NULL && !strcmp (style, self->style_name)))
		return;

	/* Store style name. */
	if (style != NULL)
	{
		tmp = listr_dup (style);
		if (tmp != NULL)
		{
			lisys_free (self->style_name);
			self->style_name = tmp;
		}
	}
	else
	{
		lisys_free (self->style_name);
		self->style_name = NULL;
	}

	/* Rebuild style. */
	private_rebuild_style (self);
}

void*
liwdg_widget_get_userdata (LIWdgWidget* self)
{
	return self->userdata;
}

void
liwdg_widget_set_userdata (LIWdgWidget* self,
                           void*        value)
{
	self->userdata = value;
}

int
liwdg_widget_get_visible (LIWdgWidget* self)
{
	return self->visible;
}

void
liwdg_widget_set_visible (LIWdgWidget* self,
                          int          visible)
{
	self->visible = (visible != 0);
	if (self->parent != NULL)
		liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	if (self->state == LIWDG_WIDGET_STATE_POPUP)
		liwdg_manager_remove_popup (self->manager, self);
}

/*****************************************************************************/

static int
private_new (LIWdgWidget*      self,
             const LIWdgClass* clss,
             LIWdgManager*     manager)
{
	const LIWdgClass* base;

	/* Initialization. */
	base = liwdg_class_get_base (clss);
	if (base != NULL)
	{
		if (!private_new (self, base, manager))
			return 0;
	}
	else
	{
		lisys_assert (clss == liwdg_widget_widget ());
	}
	if (clss->init == NULL)
		return 1;
	if (clss->init (self, manager))
		return 1;

	/* Error recovery. */
	if (base != NULL)
		self->type = base;
	else
		self->type = liwdg_widget_widget ();
	liwdg_widget_free (self);

	return 0;
}

static int
private_init (LIWdgWidget*  self,
              LIWdgManager* manager)
{
	self->style = &self->manager->styles->fallback;
	self->userrequest.width = -1;
	self->userrequest.height = -1;
	self->manager = manager;
	self->visible = 1;
	return 1;
}

static void
private_free (LIWdgWidget* self)
{
	lisys_free (self->style_name);
	lisys_free (self->state_name);
}

static int
private_event (LIWdgWidget* self,
               LIWdgEvent*  event)
{
	if (event->type == LIWDG_EVENT_TYPE_CLOSE)
	{
		lisys_assert (self->state == LIWDG_WIDGET_STATE_POPUP);
		liwdg_manager_remove_popup (self->manager, self);
		return 1;
	}

	return 1;
}

static void
private_paint_scaled (LIWdgWidget* self,
                      LIWdgRect*   rect,
                      LIWdgStyle*  style_)
{
	int px;
	int py;
	int pw;
	int ph;
	float center;
	float size;
	float xs;
	float ys;
	float tx[2];
	float ty[2];

	/* Calculate texture coordinates. */
	tx[0] = (float)(style_->x) / style_->texture->width;
	tx[1] = (float)(style_->x + style_->w[1]) / style_->texture->width;
	ty[0] = (float)(style_->y) / style_->texture->height;
	ty[1] = (float)(style_->y + style_->h[1]) / style_->texture->height;

	/* Calculate pixels per texture unit. */
	xs = tx[1] - tx[0];
	ys = ty[1] - ty[0];
	if (xs < LIMAT_EPSILON || ys < LIMAT_EPSILON)
		return;
	xs = rect->width / xs;
	ys = rect->height / ys;

	/* Scale and translate to fill widget area. */
	if (ty[1] - ty[0] >= rect->height / xs)
	{
		center = 0.5f * (ty[0] + ty[1]);
		size = ty[1] - ty[0];
		ty[0] = center - 0.5f * rect->height / xs;
		ty[1] = center + 0.5f * rect->height / xs;
	}
	else
	{
		center = 0.5f * (tx[0] + tx[1]);
		size = tx[1] - tx[0];
		tx[0] = center - 0.5f * rect->width / ys;
		tx[1] = center + 0.5f * rect->width / ys;
	}

	/* Bind texture. */
	glBindTexture (GL_TEXTURE_2D, style_->texture->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glColor3f (1.0f, 1.0f, 1.0f);

	/* Paint fill. */
	px = rect->x;
	py = rect->y;
	pw = rect->width;
	ph = rect->height;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[0], ty[0]); glVertex2f (px     , py);
	glTexCoord2f (tx[1], ty[0]); glVertex2f (px + pw, py);
	glTexCoord2f (tx[0], ty[1]); glVertex2f (px     , py + ph);
	glTexCoord2f (tx[1], ty[1]); glVertex2f (px + pw, py + ph);
	glEnd ();
}

static void
private_paint_tiled (LIWdgWidget* self,
                     LIWdgRect*   rect,
                     LIWdgStyle*  style_)
{
	int px;
	int py;
	float fw;
	float fh;
	float fu;
	float fv;
	float w[3];
	float h[3];
	float tx[4];
	float ty[4];

	/* Calculate repeat counts. */
	w[0] = style_->w[0];
	w[1] = LIMAT_MAX (1, style_->w[1]);
	w[2] = style_->w[2];
	h[0] = style_->h[0];
	h[1] = LIMAT_MAX (1, style_->h[1]);
	h[2] = style_->h[2];

	/* Calculate texture coordinates. */
	tx[0] = (float)(style_->x) / style_->texture->width;
	tx[1] = (float)(style_->x + style_->w[0]) / style_->texture->width;
	tx[2] = (float)(style_->x + style_->w[0] + style_->w[1]) / style_->texture->width;
	tx[3] = (float)(style_->x + style_->w[0] + style_->w[1] + style_->w[2]) / style_->texture->width;
	ty[0] = (float)(style_->y) / style_->texture->height;
	ty[1] = (float)(style_->y + style_->h[0]) / style_->texture->height;
	ty[2] = (float)(style_->y + style_->h[0] + style_->h[1]) / style_->texture->height;
	ty[3] = (float)(style_->y + style_->h[0] + style_->h[1] + style_->h[2]) / style_->texture->height;

	/* Bind texture. */
	glBindTexture (GL_TEXTURE_2D, style_->texture->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glColor3f (1.0f, 1.0f, 1.0f);

	/* Paint corners. */
	px = rect->x;
	py = rect->y;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[0], ty[0]); glVertex2f (px       , py);
	glTexCoord2f (tx[1], ty[0]); glVertex2f (px + w[0], py);
	glTexCoord2f (tx[0], ty[1]); glVertex2f (px       , py + h[0]);
	glTexCoord2f (tx[1], ty[1]); glVertex2f (px + w[0], py + h[0]);
	glEnd ();
	px = rect->x + rect->width - w[2] - 1;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[2], ty[0]); glVertex2f (px       , py);
	glTexCoord2f (tx[3], ty[0]); glVertex2f (px + w[2], py);
	glTexCoord2f (tx[2], ty[1]); glVertex2f (px       , py + h[0]);
	glTexCoord2f (tx[3], ty[1]); glVertex2f (px + w[2], py + h[0]);
	glEnd ();
	py = rect->y + rect->height - h[2] - 1;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[2], ty[2]); glVertex2f (px       , py);
	glTexCoord2f (tx[3], ty[2]); glVertex2f (px + w[2], py);
	glTexCoord2f (tx[2], ty[3]); glVertex2f (px       , py + h[2]);
	glTexCoord2f (tx[3], ty[3]); glVertex2f (px + w[2], py + h[2]);
	glEnd ();
	px = rect->x;
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tx[0], ty[2]); glVertex2f (px       , py);
	glTexCoord2f (tx[1], ty[2]); glVertex2f (px + w[0], py);
	glTexCoord2f (tx[0], ty[3]); glVertex2f (px       , py + h[2]);
	glTexCoord2f (tx[1], ty[3]); glVertex2f (px + w[0], py + h[2]);
	glEnd ();

	/* Paint horizontal borders. */
	for (px = rect->x + w[0] ; px < rect->x + rect->width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], rect->x + rect->width - px - w[2] - 1);
		fu = tx[1] + (tx[2] - tx[1]) * fw / w[1];
		py = rect->y;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[1], ty[0]); glVertex2f (px     , py);
		glTexCoord2f (fu   , ty[0]); glVertex2f (px + fw, py);
		glTexCoord2f (tx[1], ty[1]); glVertex2f (px     , py + h[0]);
		glTexCoord2f (fu   , ty[1]); glVertex2f (px + fw, py + h[0]);
		glEnd ();
		py = rect->y + rect->height - h[2] - 1;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[1], ty[2]); glVertex2f (px     , py);
		glTexCoord2f (fu   , ty[2]); glVertex2f (px + fw, py);
		glTexCoord2f (tx[1], ty[3]); glVertex2f (px     , py + h[2]);
		glTexCoord2f (fu   , ty[3]); glVertex2f (px + fw, py + h[2]);
		glEnd ();
	}

	/* Paint vertical borders. */
	for (py = rect->y + h[0] ; py < rect->y + rect->height - h[2] ; py += h[1])
	{
		fh = LIMAT_MIN (h[1], rect->y + rect->height - py - h[2] - 1);
		fv = ty[1] + (ty[2] - ty[1]) * fh / h[1];
		px = rect->x;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[0], ty[1]); glVertex2f (px       , py);
		glTexCoord2f (tx[1], ty[1]); glVertex2f (px + w[0], py);
		glTexCoord2f (tx[0], fv   ); glVertex2f (px       , py + fh);
		glTexCoord2f (tx[1], fv   ); glVertex2f (px + w[0], py + fh);
		glEnd ();
		px = rect->x + rect->width - w[2] - 1;
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[2], ty[1]); glVertex2f (px       , py);
		glTexCoord2f (tx[3], ty[1]); glVertex2f (px + w[2], py);
		glTexCoord2f (tx[2], fv   ); glVertex2f (px       , py + fh);
		glTexCoord2f (tx[3], fv   ); glVertex2f (px + w[2], py + fh);
		glEnd ();
	}

	/* Paint fill. */
	for (py = rect->y + h[0] ; py < rect->y + rect->height - h[2] ; py += h[1])
	for (px = rect->x + w[0] ; px < rect->x + rect->width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], rect->x + rect->width - px - w[2] - 1);
		fh = LIMAT_MIN (h[1], rect->y + rect->height - py - h[2] - 1);
		fu = tx[1] + (tx[2] - tx[1]) * fw / w[1];
		fv = ty[1] + (ty[2] - ty[1]) * fh / h[1];
		glBegin (GL_TRIANGLE_STRIP);
		glTexCoord2f (tx[1], ty[1]); glVertex2f (px     , py);
		glTexCoord2f (fu   , ty[1]); glVertex2f (px + fw, py);
		glTexCoord2f (tx[1], fv   ); glVertex2f (px     , py + fh);
		glTexCoord2f (fu   , fv   ); glVertex2f (px + fw, py + fh);
		glEnd ();
	}
}

static void
private_rebuild_style (LIWdgWidget* self)
{
	int len0;
	int len1;
	char* full;
	LIWdgEvent event;
	LIWdgStyle* style = NULL;

	/* Find style. */
	if (self->style_name != NULL)
	{
		if (self->state_name != NULL)
		{
			len0 = strlen (self->style_name);
			len1 = strlen (self->state_name);
			full = lisys_calloc (len0 + len1 + 2, sizeof (char));
			if (full != NULL)
			{
				strcpy (full, self->style_name);
				strcpy (full + len0 + 1, self->state_name);
				full[len0] = ':';
				style = lialg_strdic_find (self->manager->styles->subimgs, full);
				lisys_free (full);
			}
		}
		else
			style = lialg_strdic_find (self->manager->styles->subimgs, self->style_name);
	}
	if (style == NULL)
		style = &self->manager->styles->fallback;

	/* Set new style and request. */
	if (self->style != style)
	{
		self->style = style;
		event.type = LIWDG_EVENT_TYPE_STYLE;
		self->type->event (self, &event);
		if (self->parent != NULL)
			liwdg_container_child_request (LIWDG_CONTAINER (self->parent), self);
	}
}

/** @} */
/** @} */
