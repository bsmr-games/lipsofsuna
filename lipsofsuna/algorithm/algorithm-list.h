/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lialg Algorithm
 * @{
 * \addtogroup lialgList List
 * @{
 */

#ifndef __ALGORITHM_LIST_H__
#define __ALGORITHM_LIST_H__

#include <system/lips-system.h>

/**
 * \brief A doubly linked list class.
 */
typedef struct _lialgList lialgList;
struct _lialgList
{
	lialgList* next;
	lialgList* prev;
	void* data;
};

/**
 * \brief Used for list enumeration.
 */
typedef void (*lialgListForeach)(void*);

/**
 * \brief Used for list sorting.
 */
typedef int (*lialgListCompare)(const void*, const void*);

/**
 * \brief Frees the list.
 *
 * \param self Linked list.
 */
static inline void
lialg_list_free (lialgList* self)
{
	lialgList* tmp;
	lialgList* next;

	for (tmp = self ; tmp != NULL ; tmp = next)
	{
		next = tmp->next;
		lisys_free (tmp);
	}
}

/**
 * \brief Iterates throught the list to get its length.
 *
 * \param self Linked list.
 * \return Length of the list.
 */
static inline int
lialg_list_get_length (lialgList* self)
{
	int len = 0;
	lialgList* tmp;

	for (tmp = self ; tmp != NULL ; tmp = tmp->next)
		len++;
	return len;
}

/**
 * \brief Calls the provided item for each data in the list.
 *
 * \param self Linked list.
 * \param call Function to call with the data.
 */
static inline void
lialg_list_foreach (lialgList**      self,
                    lialgListForeach call)
{
	lialgList* tmp;

	for (tmp = *self ; tmp != NULL ; tmp = tmp->next)
		call (tmp->data);
}

/**
 * \brief Prepends a node to the list.
 *
 * \param self Linked list.
 * \param data Data to store in the node.
 * \return Nonzero on success.
 */
static inline int
lialg_list_append (lialgList** self,
                   void*       data)
{
	lialgList* tmp;
	lialgList* last;

	tmp = (lialgList*) lisys_malloc (sizeof (lialgList));
	if (tmp == NULL)
		return 0;
	if (*self != NULL)
	{
		for (last = *self ; last->next != NULL ; last = last->next);
		tmp->prev = last;
		tmp->next = NULL;
		tmp->data = data;
		last->next = tmp;
	}
	else
	{
		tmp->prev = NULL;
		tmp->next = NULL;
		tmp->data = data;
		*self = tmp;
	}
	return 1;
}


/**
 * \brief Appends a readily allocated node to the list.
 *
 * The node must have been allocated with malloc and friends if you
 * intend to free the list with #lialg_list_free.
 *
 * \param self Linked list.
 * \param node Linked list node.
 */
static inline void
lialg_list_append_node (lialgList** self,
                        lialgList*  node)
{
	lialgList* last;

	if (*self != NULL)
	{
		for (last = *self ; last->next != NULL ; last = last->next);
		node->prev = last;
		node->next = NULL;
		last->next = node;
	}
	else
	{
		node->prev = NULL;
		node->next = NULL;
		*self = node;
	}
}

/**
 * \brief Prepends a node to the list.
 *
 * \param self Linked list.
 * \param data Data to store in the node.
 * \return Nonzero on success.
 */
static inline int
lialg_list_prepend (lialgList** self,
                    void*       data)
{
	lialgList* tmp;

	tmp = (lialgList*) lisys_malloc (sizeof (lialgList));
	if (tmp == NULL)
		return 0;
	tmp->prev = NULL;
	tmp->next = *self;
	tmp->data = data;
	if (*self != NULL)
		(*self)->prev = tmp;
	*self = tmp;
	return 1;
}

/**
 * \brief Prepends an already allocated node to the list.
 *
 * The node must have been allocated with malloc and friends if you
 * intend to free the list with #lialg_list_free. Otherwise use
 * #lialg_list_detach_node to remove the node from list without freeing it.
 *
 * \param self Linked list.
 * \param node Linked list node.
 */
static inline void
lialg_list_prepend_node (lialgList** self,
                         lialgList*  node)
{
	node->prev = NULL;
	node->next = *self;
	if (*self != NULL)
		(*self)->prev = node;
	*self = node;
}

/**
 * \brief Prepends a node to the list with sorting.
 *
 * \param self Linked list.
 * \param data Data to store in the node.
 * \param cmp Sorting function.
 * \return Nonzero on success.
 */
static inline int
lialg_list_prepend_sorted (lialgList**      self,
                           void*            data,
                           lialgListCompare cmp)
{
	lialgList* tmp;
	lialgList* ptr;

	/* Allocate memory. */
	tmp = (lialgList*) lisys_malloc (sizeof (lialgList));
	if (tmp == NULL)
		return 0;
	tmp->data = data;

	/* Handle empty lists. */
	if (*self == NULL)
	{
		tmp->next = NULL;
		tmp->prev = NULL;
		*self = tmp;
		return 1;
	}

	/* Handle prepending. */
	if (cmp (data, (*self)->data) <= 0)
	{
		tmp->prev = NULL;
		tmp->next = *self;
		(*self)->prev = tmp;
		*self = tmp;
		return 1;
	}

	/* Search for the spot. */
	for (ptr = *self ; ptr->next != NULL ; ptr = ptr->next)
	{
		if (cmp (data, ptr->next->data) <= 0)
			break;
	}

	/* Insert to the list. */
	tmp->prev = ptr;
	tmp->next = ptr->next;
	if (ptr->next != NULL)
		ptr->next->prev = tmp;
	ptr->next = tmp;
	return 1;
}

/**
 * \brief Removes a node from the list without freeing it.
 *
 * \param self Linked list.
 * \param node Node to detach.
 */
static inline void
lialg_list_detach_node (lialgList** self,
                        lialgList*  node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	if (*self == node)
		*self = node->next;
}

/**
 * \brief Removes a node from the list.
 *
 * \param self Linked list.
 * \param node Node to remove.
 */
static inline void
lialg_list_remove (lialgList** self,
                   lialgList*  node)
{
	if (node->prev != NULL)
		node->prev->next = node->next;
	if (node->next != NULL)
		node->next->prev = node->prev;
	if (*self == node)
		*self = node->next;
	lisys_free (node);
}

/** @} */
/** @} */

#endif
