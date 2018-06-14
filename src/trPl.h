/** \file
*   \brief ThingRendevous's Priority List interface
*
* This is a generalized priority list
* as an array of priorities and index links within the list.
* The key and the index are one in the same.
* You must manage free list elements.
*
* The list is linked from head to tail 
* by lowest priority (priority value is a big number)
* to highest priority (priority value is a small number).
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

#ifndef trPl_h
#define trPl_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trB.h"

/** Priority list elements can be indexed from 0...to this.
*
*/
#define trPl_Lst_idx_last 0xFFFCU

/** Note maximum list length is last + 1 and must be < uint16_t max.
*
*/
#define trPl_Lst_len_max trPl_Lst_idx_last+1

/** trPl uses an array of linked-list elements.  Here's each array element..
*
* Indexes 0...(max_num_of_eles - 1) store 
*
* \li a priority field
* \li link to the previous element
* \li link to the next element
*
* The index and key are one in the same, and you can use 0...(max_num_of_eles - 1) 
* to index into your list of specific info,
* be it queue, mailbox, semphore, task. whatever.
* that corresponds to that ID.
*
* The priority field is obvious.  It is preserved and never cleared,
* even when the element is removed.  You can use this knowledge for
* your own purposes.  It's suggested that priority be none when an
* element is free.
*
* If a previous list element exists, the previous link index field will not be none.
* If the element is the head of the list, it will be none.
*
* If a next list element exists, the next link index field will not be none.
* If the element is the tail of the list, it will be none.
*
* If both previous and next are none, it may be the only element in the list.
*/
struct trPl_Lst_s {
    uint16_t pri;      /**< The priority of the element.  Duplicates are allowed. */
    uint16_t prev;      /**< Index link to the previous element in the list.  None if the head. */
    uint16_t next;      /**< Index link to the next element in the list.  None if the tail. */
};

/** trPl expects a seperate root element holding the list's head and tail.
*
* The head is the lowest priority item in the list (the priority value is a big number).
* 
* The tail is the highest priority item in the list (the priority value is a small number).
*/
struct trPl_RootEle_s {
    uint16_t head;      /**< Index to the head of the list.  None if list not init'd. */
    uint16_t tail;      /**< Index to the tail of the list.  None if list not init'd. */
};

void trPl_init(struct trPl_Lst_s *a_Lst_p, uint16_t a_Lst_len);

void trPl_initRootEle(struct trPl_RootEle_s *a_RootEle_p);

bool trPl_next(uint16_t * present_key_idx_p, struct trPl_Lst_s *a_Lst_p);

void trPl_insert(uint16_t the_key_idx, struct trPl_Lst_s *a_Lst_p, struct trPl_RootEle_s *the_RootEle_p);

uint16_t trPl_Hi_remove(struct trPl_Lst_s *a_Lst_p, struct trPl_RootEle_s *the_RootEle_p);
uint16_t trPl_Lo_remove(struct trPl_Lst_s *a_Lst_p, struct trPl_RootEle_s *the_RootEle_p);

uint16_t trPl_Hi_peek(struct trPl_RootEle_s *the_RootEle_p);
uint16_t trPl_Lo_peek(struct trPl_RootEle_s *the_RootEle_p);

void trPl_delete(uint16_t the_key_idx, struct trPl_Lst_s *a_Lst_p, struct trPl_RootEle_s *the_RootEle_p);

void trPl_Pri_set(uint16_t the_key_idx, uint16_t thePri, struct trPl_Lst_s *a_Lst_p);

uint16_t trPl_Pri_get(uint16_t the_key_idx, struct trPl_Lst_s *a_Lst_p);

void trPl_makeFree(uint16_t the_key_idx, struct trPl_Lst_s *a_Lst_p);

uint16_t trPl_findFree(struct trPl_Lst_s *a_Lst_p, uint16_t a_Lst_len);

/** Test the root element for empty condition.  It ain't a pointer!
*/
#define trPl_isEmpty(a_RootEle) ( ( ( a_RootEle.head & a_RootEle.tail) == trB_None ) ? true : false )

#endif
