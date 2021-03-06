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

#ifndef __RENDER_INTERNAL_ATTACHMENT_ENTITY_HPP__
#define __RENDER_INTERNAL_ATTACHMENT_ENTITY_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/model.h"
#include "render-attachment.hpp"
#include <map>
#include <vector>
#include <OgreEntity.h>
#include <OgreResource.h>

struct LIRenTextureReplace
{
	Ogre::String name;
	Ogre::TexturePtr texture;
};

class LIRenAttachmentEntity : public LIRenAttachment
{
public:
	LIRenAttachmentEntity (LIRenObject* object, LIRenModel* model);
	virtual ~LIRenAttachmentEntity ();
	virtual LIRenModelData* get_model () const;
	virtual bool has_model (LIRenModel* model);
	virtual bool is_loaded () const;
	virtual void remove_model (LIRenModel* model);
	virtual void replace_texture (const char* name, Ogre::TexturePtr& texture);
	virtual void update (float secs);
	virtual void update_pose (LIMdlPoseSkeleton* skeleton);
	virtual void update_settings ();
protected:
	void clear ();
	bool create_skeleton ();
	void replace_texture_now (const Ogre::String& name, Ogre::TexturePtr& texture);
protected:
	bool loaded;
	bool loading_mesh;
	bool loading_deps;
	LIMdlPoseBuffer* pose_buffer;
	LIRenModel* model;
	Ogre::Entity* entity;
	Ogre::MeshPtr mesh;
	Ogre::SkeletonPtr skeleton;
	std::vector<Ogre::ResourcePtr> resources;
	std::vector<LIRenTextureReplace> queued_texture_replaces;
	std::map<Ogre::String, Ogre::String> applied_texture_replaces;
};

#endif
