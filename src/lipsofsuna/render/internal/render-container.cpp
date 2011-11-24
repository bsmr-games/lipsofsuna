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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenContainer Container
 * @{
 */

#include "render-internal.h"
#include "render-container.hpp"

Ogre::String LIRenContainer::type_name ("LIRenContainer");

LIRenContainer::LIRenContainer (const Ogre::String& name) : Ogre::PanelOverlayElement (name)
{
}

LIRenContainer::~LIRenContainer ()
{
}

const Ogre::String& LIRenContainer::getTypeName () const
{
	return type_name;
}

ushort LIRenContainer::_notifyZOrder (ushort z)
{
	std::vector<LIRenContainer*> children;
	children.push_back (this);

	/* Sort children using the breadth first search algorithm. */
	for (size_t count = children.size () ; count ; count = children.size ())
	{
		for (size_t i = 0 ; i < count ; i++)
		{
			LIRenContainer* child = children[i];
			z = child->_notifyZOrderNonrecursive (z);
			child->get_children (children);
		}
		children.erase (children.begin (), children.begin () + count);
	}

	return z;
}

ushort LIRenContainer::_notifyZOrderNonrecursive (ushort z)
{
	OverlayElement::_notifyZOrder (z);

	ChildIterator it = getChildIterator();
	while (it.hasMoreElements ())
	{
		Ogre::OverlayElement* child = it.getNext ();
		if (child->getTypeName () != type_name)
			z = child->_notifyZOrder (z);
	}

	return z + 1;
}

void LIRenContainer::get_children (std::vector<LIRenContainer*>& children)
{
	ChildIterator it = getChildIterator ();
	while (it.hasMoreElements ())
	{
		Ogre::OverlayElement* child = it.getNext ();
		if (child->getTypeName () == type_name)
			children.push_back ((LIRenContainer*) child);
	}
}

/** @} */
/** @} */
/** @} */
