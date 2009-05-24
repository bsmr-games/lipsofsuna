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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlModel Model
 * @{
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include <string/lips-string.h>
#include "model-animation.h"
#include "model-bone.h"
#include "model-light.h"
#include "model-node.h"
#include "model-types.h"
#include "model-vertex.h"

#define LIMDL_FORMAT_VERSION 0xFFFFFFFA

typedef int limdlModelFlags;
typedef int limdlTextureFlags;
typedef int limdlTextureType;
typedef struct _limdlMaterial limdlMaterial;
typedef struct _limdlTexture limdlTexture;
typedef struct _limdlWeightGroup limdlWeightGroup;
typedef struct _limdlWeights limdlWeights;
typedef struct _limdlWeight limdlWeight;

enum
{
	LIMDL_MATERIAL_FLAG_BILLBOARD    = 0x01,
	LIMDL_MATERIAL_FLAG_COLLISION    = 0x02,
	LIMDL_MATERIAL_FLAG_CULLFACE     = 0x04,
	LIMDL_MATERIAL_FLAG_TRANSPARENCY = 0x08,
};

enum
{
	LIMDL_TEXTURE_FLAG_BILINEAR = 0x01,
	LIMDL_TEXTURE_FLAG_CLAMP    = 0x02,
	LIMDL_TEXTURE_FLAG_MIPMAP   = 0x04,
	LIMDL_TEXTURE_FLAG_REPEAT   = 0x08
};

enum
{
	LIMDL_TEXTURE_TYPE_EMPTY,
	LIMDL_TEXTURE_TYPE_ENVMAP,
	LIMDL_TEXTURE_TYPE_IMAGE
};

/*****************************************************************************/

struct _limdlModel
{
	/* Animation. */
	struct
	{
		int count;
		limdlAnimation* animations;
	} animation;

	/* Mesh. */
	int flags;
	limatAabb bounds;
	struct
	{
		int count;
		limdlMaterial* materials;
	} materials;
	struct
	{
		int count;
		limdlVertex* vertices;
		limdlWeights* weights;
	} vertex;
	struct
	{
		int count;
		limdlWeightGroup* weightgroups;
	} weightgroups;

	/* Hierarchy. */
	struct
	{
		int count;
		limdlNode** array;
	} nodes;
};

#ifdef __cplusplus
extern "C" {
#endif

limdlModel*
limdl_model_new ();

limdlModel*
limdl_model_new_from_data (liReader* reader);

limdlModel*
limdl_model_new_from_file (const char* path);

void
limdl_model_calculate_bounds (limdlModel* self);

void
limdl_model_free (limdlModel* self);

int
limdl_model_find_material (const limdlModel*    self,
                           const limdlMaterial* material);

limdlNode*
limdl_model_find_node (const limdlModel* self,
                       const char*       name);

int
limdl_model_insert_material (limdlModel*          self,
                             const limdlMaterial* material);

int
limdl_model_insert_node (limdlModel* self,
                         limdlNode*  node);

int
limdl_model_insert_triangle (limdlModel*         self,
                             int                 material,
                             const limdlVertex*  vertices,
                             const limdlWeights* weights);

int
limdl_model_write (const limdlModel* self,
                   liarcWriter*      writer);

int
limdl_model_write_file (const limdlModel* self,
                        const char*       path);

limdlAnimation*
limdl_model_get_animation (limdlModel* self,
                           const char* name);

int
limdl_model_get_index (const limdlModel*  self,
                       const limdlVertex* vertex);

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

struct _limdlMaterial
{
	int start;
	int end;
	int flags;
	char* shader;
	float emission;
	float shininess;
	float diffuse[4];
	float specular[4];
	struct
	{
		int count;
		limdlTexture* textures;
	} textures;
};

/*****************************************************************************/

struct _limdlTexture
{
	limdlTextureType type;
	limdlTextureFlags flags;
	int width;
	int height;
	char* string;
};

static inline int
limdl_texture_compare (const limdlTexture* self,
                       const limdlTexture* texture)
{
	if (self->type != texture->type ||
	    self->flags != texture->flags ||
	    self->width != texture->width ||
	    self->height != texture->height)
		return 0;
	if (strcmp (self->string, texture->string))
		return 0;

	return 1;
}

/*****************************************************************************/

struct _limdlWeightGroup
{
	char* name;
	char* bone;
	limdlNode* node;
};

struct _limdlWeights
{
	int count;
	limdlWeight* weights;
};

struct _limdlWeight
{
	int group;
	float weight;
};

#endif

/** @} */
/** @} */
