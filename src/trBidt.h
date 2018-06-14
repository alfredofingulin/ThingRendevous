/** \file
*   \brief ThingRendevous's Binary ID Tree interface
*
* This is <em> not the same as ThingKernel's tkBidt. </em>  
* ThingRendevous' implmentation is different.
* Expresses the scheme described in \link uDN02_IDs2Internals \endlink.
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

#ifndef trBidt_h
#define trBidt_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trB.h"

/** Trees can be indexed from 0...to this.
*
*/
#define trBidt_Tree_idx_last 0xFFFCU

/** Note maximum tree length is last + 1 and must be < uint16_t max.
*
*/
#define trBidt_Tree_len_max trBidt_Tree_idx_last+1U

/** Defines your tree's length based on the maximum number of IDs you'll need.
*
* Ensure that max_num_of_IDs+1 <= trBidt_Tree_len_max;
* this does not check for it.
*/
#define trBidt_MyTreeLen(max_num_of_IDs) ( max_num_of_IDs + 1U )

/** trBidt stores IDs in nodes.
*
* \li The nodes are collected into arrays.  
* \li The array is walked as a binary tree.
*
* Use macro trBidt_MyTreeLen() to size the tree
* based on the max_num_of_IDs you will expect.
*
* Indexes 0...(max_num_of_IDs - 1) store 
*
* \li an ID field 
* \li fields for the index to left (less than) and right (greater than) child nodes.  
*
* Any node where all fields are none
* indicates the node is empty and available for use.
* 
* A node where the ID field is not none indicates a valid node.
* If a child node exists, the child node index field will not be none.
* If no child node exists, then the child node index field will contain none.
*
* If the ID field is valid, you can, in turn, use the index for that ID, 0...(max_num_of_IDs - 1) 
* to index into your table of specific info,
* be it stream, messages, events, tasks, whatever,
* that corresponds to that ID.
*
* Index max_num_of_IDs helps find the "head" of the tree.
* For the head node aka index max_num_of_IDs:
*
* \li if all fields are none, the "head" is empty and the tree is completely empty
* \li if the ID is 0, and the left child node is not none, then the left child node will have any IDs reserved to ThingWare (values [0...254]).
* \li if the ID is 0, and the right child node is not none, then the right child node will have any IDs dedicated to the application (values [255...0xFFFE]).
* \li yes, it is possible to have both left and right heads (see http://www.imdb.com/title/tt0069372/ )
* 
* That last point is real important.  
* It allows trBidt to move the head node
* without a lot of complicated fix-up code.  
* Sorry to make your declaration
* so difficult, but that's the way it's gotta be.
*/
struct trBidt_Node_s {
    uint16_t the_ID;      /**< The "key" field; usually some kind of ID. */
    uint16_t child_left;      /**< The left child node field, any keys less than the node we're at, as an index into this tree. */
    uint16_t child_right;      /**< The right child node field, any keys greater than the node we're at, also an index into this tree. */
};

/** See trBidt_Node_s.  No different save that it's const.
*/
struct trBidt_Node_const_s {
    const uint16_t the_ID;      /**< Set trBidt_Node_s */
    const uint16_t child_left;      /**< Set trBidt_Node_s */
    const uint16_t child_right;      /**< Set trBidt_Node_s */
};

void trBidt_init(struct trBidt_Node_s *a_tree_p, uint16_t a_tree_len);

uint16_t trBidt_find(uint16_t an_ID, struct trBidt_Node_s *a_tree_p, uint16_t a_tree_len);

uint16_t trBidt_revfind(uint16_t an_idx, struct trBidt_Node_s *a_tree_p, uint16_t a_tree_len);

uint16_t trBidt_insert(uint16_t an_ID, struct trBidt_Node_s *a_tree_p, uint16_t a_tree_len, bool * is_duplicate_p);

uint16_t trBidt_delete(uint16_t an_ID, struct trBidt_Node_s *a_tree_p, uint16_t a_tree_len);

void trBidt_rebalance(struct trBidt_Node_s *a_tree_p, uint16_t a_tree_len);

#endif
