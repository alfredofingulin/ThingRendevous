/** \file
*   \brief ThingRendevous's Priority List implementation
*
* In reality, implements a linked list built by priority
* and ordered numerically from high to low (by priority from low to high).
* The head is the lowest priority item; the tail the highest priority item.
*
* \author Alfred O Fingulin FinguPublic@gmail.com
*/
/*
* MIT License
* 
* Copyright (c) 2017 Alfred O Fingulin  FinguPublic@gmail.com
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trB.h"
#include "trPl.h"

/** Move to the previous element in the list, if possible (a lower priority element).
*
* \retval true - there is a previous element and present key index contains the index value
* \retval false - we're at the beginning of the list and present key index is unchanged
*/
bool trPl_prev(uint16_t * present_key_idx_p,
                                      /**< Where we currently are in the list. */
               struct trPl_Lst_s *a_Lst_p
                                    /**< Where the list it. */
    )
{
    if (a_Lst_p[*present_key_idx_p].prev != trB_None) {
        *present_key_idx_p = a_Lst_p[*present_key_idx_p].prev;
        return true;
    } else {
        return false;
    }
}

/** Move to the next element in the list, if possible (a higher priority element).
*
* \retval true - there is a next element and present key index contains the index value
* \retval false - we're at the end of the list and present key index is unchanged
*/
bool trPl_next(uint16_t * present_key_idx_p,
                                      /**< Where we currently are in the list. */
               struct trPl_Lst_s * a_Lst_p
                                    /**< Where the list it. */
    )
{
    if (a_Lst_p[*present_key_idx_p].next != trB_None) {
        *present_key_idx_p = a_Lst_p[*present_key_idx_p].next;
        return true;
    } else {
        return false;
    }
}

/** Init the elements of a priority list to none.
*
*/
void trPl_init(struct trPl_Lst_s *a_Lst_p,
                                     /**< Where to find the priority list. */
               uint16_t a_Lst_len
                            /**< How many elements are in the priority list. */
    )
{
    uint16_t i;
    for (i = 0; i < a_Lst_len; i++) {
        a_Lst_p[i].pri = a_Lst_p[i].prev = a_Lst_p[i].next = trB_None;
    }
}

/** Init the root element holder to none.
*
*/
void trPl_initRootEle(struct trPl_RootEle_s *a_RootEle_p
                                            /**< Obvious. */
    )
{
    a_RootEle_p->head = a_RootEle_p->tail = trB_None;
}

/** Insert an element into the queue ordered by priority.
*
* Inserts the key/idx into the priority list
* after all items of higher or equal priority
* and before all items of lesser priority.
*
* It assumes the pri field of the element specifed by the key/idx
* has been filled in with the desired priority value.
*/
void trPl_insert(uint16_t the_key_idx,
                               /**< The key/index. */
                 struct trPl_Lst_s *a_Lst_p,
                                     /**< Which priority list to use. */
                 struct trPl_RootEle_s *the_RootEle_p
                                              /**< The root element (head & tail) for this list. */
    )
{
    uint16_t i;

    if ((the_RootEle_p->head & the_RootEle_p->tail) == trB_None) {
        // list is empty
        // this is the first item going in

        the_RootEle_p->head = the_RootEle_p->tail = the_key_idx;

    } else {
        // list is non-empty; will have to iterate

        i = the_RootEle_p->head;
        while ((a_Lst_p[the_key_idx].pri < a_Lst_p[i].pri)
               && trPl_next(&i, a_Lst_p)) {
        }

        if (a_Lst_p[the_key_idx].pri >= a_Lst_p[i].pri) {
            // element to insert has lower or equal priority to this one
            // insert the one presented before the one we're at

            a_Lst_p[the_key_idx].prev = a_Lst_p[i].prev;
            a_Lst_p[the_key_idx].next = i;
            if (a_Lst_p[i].prev != trB_None) {
                a_Lst_p[a_Lst_p[i].prev].next = the_key_idx;
            }
            a_Lst_p[i].prev = the_key_idx;
            if (a_Lst_p[the_key_idx].prev == trB_None) {
                the_RootEle_p->head = the_key_idx;
            }
        } else {
            // element to insert has higher priority than this one
            // insert the one presented after the one we're at

            a_Lst_p[the_key_idx].prev = i;
            a_Lst_p[the_key_idx].next = a_Lst_p[i].next;
            if (a_Lst_p[i].next != trB_None) {
                a_Lst_p[a_Lst_p[i].next].prev = the_key_idx;
            }
            a_Lst_p[i].next = the_key_idx;
            if (a_Lst_p[the_key_idx].next == trB_None) {
                the_RootEle_p->tail = the_key_idx;
            }
        }

    }
}

/** Removes the highest priority item in the list; reveals the key/idx.
*
* Use the priority getter to discover the priority.
*
* \retval - The key/index of the highest priority element, none for an empty list.
*/
uint16_t trPl_Hi_remove(struct trPl_Lst_s *a_Lst_p,
                                     /**< Which priority list to use. */
                        struct trPl_RootEle_s *the_RootEle_p
                                              /**< The root element (head & tail) for this list. */
    )
{
    uint16_t thekey_idx;

    thekey_idx = the_RootEle_p->tail;

    if (thekey_idx != trB_None) {
        // the queue is not empty
        // at least one item exists

        if (a_Lst_p[thekey_idx].prev != trB_None) {
            // if at least one more item exists

            // go to the prev element in the list
            // make the prev element's next element none
            // make the prev element the new tail
            // head takes care of itself

            a_Lst_p[a_Lst_p[thekey_idx].prev].next = trB_None;
            the_RootEle_p->tail = a_Lst_p[thekey_idx].prev;
        } else {
            // else prev is none, this is only item in queue

            // head and tail index to same nothing
            the_RootEle_p->tail = the_RootEle_p->head = trB_None;
        }

        // zap the present element (we've already got copies of what we need)

        a_Lst_p[thekey_idx].prev = a_Lst_p[thekey_idx].next = trB_None;
    }

    return thekey_idx;
}

/** Removes the lowest priority item in the list; reveals the key/idx.
*
* Use the priority getter to discover the priority.
*
* \retval - The key/index of the lowest priority element, none for an empty list.
*/
uint16_t trPl_Lo_remove(struct trPl_Lst_s * a_Lst_p,
                                     /**< Which priority list to use. */
                        struct trPl_RootEle_s * the_RootEle_p
                                              /**< The root element (head & tail) for this list. */
    )
{
    uint16_t thekey_idx;

    thekey_idx = the_RootEle_p->head;

    if (thekey_idx != trB_None) {
        // the queue is not empty
        // at least one item exists

        if (a_Lst_p[thekey_idx].next != trB_None) {
            // if at least one more item exists

            // go to the next element in the list
            // make the next element's prev element none
            // make the next element the new head
            // tail takes care of itself

            a_Lst_p[a_Lst_p[thekey_idx].next].prev = trB_None;
            the_RootEle_p->head = a_Lst_p[thekey_idx].next;
        } else {
            // else next is none, this is only item in queue

            // head and tail index to same nothing
            the_RootEle_p->head = the_RootEle_p->tail = trB_None;
        }

        // zap the present element (we've already got copies of what we need)

        a_Lst_p[thekey_idx].prev = a_Lst_p[thekey_idx].next = trB_None;
    }

    return thekey_idx;
}

/** Peeks at the highest priority item in the list without removing it; reveals the key/idx.
*
* Use the priority getter to discover the priority.
*
* \retval - The key/index of the highest priority element, none for an empty list.
*/
uint16_t trPl_Hi_peek(struct trPl_RootEle_s * the_RootEle_p
                                              /**< The root element (head & tail) for this list. */
    )
{
    return the_RootEle_p->tail;
}

/** Peeks at the lowest priority item in the list without removing it; reveals the key/idx.
*
* Use the priority getter to discover the priority.
*
* \retval - The key/index of the lowest priority element, none for an empty list.
*/
uint16_t trPl_Lo_peek(struct trPl_RootEle_s * the_RootEle_p
                                              /**< The root element (head & tail) for this list. */
    )
{
    return the_RootEle_p->head;
}

/** Deletes a priority list element from a list without changing its priority value.
*
*/
void trPl_delete(uint16_t the_key_idx,
                               /**< The key/index for the element to delete. */
                 struct trPl_Lst_s *a_Lst_p,
                                     /**< Which priority list to use. */
                 struct trPl_RootEle_s *the_RootEle_p
                                              /**< The root element (head & tail) for this list. */
    )
{
    if ((the_RootEle_p->head & the_RootEle_p->tail) != trB_None) {
        // list is non-empty

        if (a_Lst_p[the_key_idx].prev != trB_None) {
            a_Lst_p[a_Lst_p[the_key_idx].prev].next = a_Lst_p[the_key_idx].next;
        } else {
            the_RootEle_p->head = a_Lst_p[the_key_idx].next;
        }

        if (a_Lst_p[the_key_idx].next != trB_None) {
            a_Lst_p[a_Lst_p[the_key_idx].next].prev = a_Lst_p[the_key_idx].prev;
        } else {
            the_RootEle_p->tail = a_Lst_p[the_key_idx].prev;
        }

        a_Lst_p[the_key_idx].prev = a_Lst_p[the_key_idx].next = trB_None;
    }
}

/** Sets a priority in a priority list element; doesn't care if the element is part of a list or not.
*
* The priority list will not be resorted to reflect the new priority.
*/
void trPl_Pri_set(uint16_t the_key_idx,
                               /**< Where in the list. */
                  uint16_t thePri,
                          /**< The priority it will get. */
                  struct trPl_Lst_s *a_Lst_p
                                    /**< Where the list begins. */
    )
{
    a_Lst_p[the_key_idx].pri = thePri;
}

/** Gets a priority from a priority list element; doesn't care if the element is part of a list or not.
*
* \retval priority at that key/index.
*/
uint16_t trPl_Pri_get(uint16_t the_key_idx,
                               /**< Where in the list. */
                      struct trPl_Lst_s *a_Lst_p
                                    /**< Where the list begins. */
    )
{
    return a_Lst_p[the_key_idx].pri;
}

/** Make a priority list element free by setting the priority value to none.
*
* Does not change the prev/next links, so be careful to remove/delete 
* from a list first!
*/
void trPl_makeFree(uint16_t the_key_idx,
                               /**< Where in the list. */
                   struct trPl_Lst_s *a_Lst_p
                                    /**< Where the list begins. */
    )
{
    a_Lst_p[the_key_idx].pri = trB_None;
}

/** Find a free element in a priority list using a brutarian linear search.
*
* Looks for the first item where priority is none.
*
* \retval - The key/index of the free element.
*/
uint16_t trPl_findFree(struct trPl_Lst_s *a_Lst_p,
                                     /**< Which priority list to use. */
                       uint16_t a_Lst_len
                            /**< How many elements are in the priority list. */
    )
{
    uint16_t i;
    for (i = 0; (i < a_Lst_len) && (a_Lst_p[i].pri != trB_None); i++) {
    }
    return i;
}
