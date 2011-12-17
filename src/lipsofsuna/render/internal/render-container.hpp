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

#ifndef __RENDER_INTERNAL_CONTAINER_HPP__
#define __RENDER_INTERNAL_CONTAINER_HPP__

#include "lipsofsuna/system.h"
#include "render-types.h"
#include <OgrePanelOverlayElement.h>

class LIRenContainer : public Ogre::PanelOverlayElement
{
public:
	LIRenContainer (const Ogre::String& name);
	virtual ~LIRenContainer ();
	virtual const Ogre::String& getTypeName () const;
protected:
	virtual ushort _notifyZOrder (ushort z);
	virtual ushort _notifyZOrderNonrecursive (ushort z);
	void get_children (std::vector<LIRenContainer*>& children);
private:
	static Ogre::String type_name;
};

#endif
