/** \file
*   \brief ThingRendevous's Binary ID Tree implementation
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "trB.h"
#include "trBidt.h"

/** Locates the root node based on the tree length.
*
*/
#define trBidt_rootNodeLocate_idx(a_tree_len) ( a_tree_len - 1U )

/** Finds an empty node in an tree.
*
* Brutarian linear search for an empty node from the beginning of the tree.
*
* Warning: the root node at the end better not be None or this may return it!
*
* \retval - index into the tree containing an empty node, the tree len if none.
*/
uint16_t trBidt_nodeEmptyFind(struct trBidt_Node_s *a_tree_p,
                                         /**< Points to the beginning of the tree. */
                              uint16_t a_tree_len
                             /**< The actual length of the tree; must be less than or equal to trBidt_Tree_len_max. */
    ) {
    uint16_t i;
    for (i = 0; (i < a_tree_len) && (a_tree_p[i].the_ID != trB_None); i++) {
    }
    return i;
}

/** Zaps a node, making it empty.
*
*/
#define trBidt_nodeEmptyZap(a_tree_p,idx) a_tree_p[ idx ].the_ID = a_tree_p[ idx ].child_left = a_tree_p[ idx ].child_right = trB_None

/** Inits all elements in an ID tree to none.
*
* \todo This works but should not be used.  Why?  All trees generated statically at compile-time.
*
* You need to call this at system start to init your ID/node/tree.
*/
void trBidt_init(struct trBidt_Node_s *a_tree_p,
                                         /**< Points to the beginning of the tree. */
                 uint16_t a_tree_len
                             /**< The actual length of the tree; must be less than trBidt_Tree_len_max. */
    ) {
    uint16_t currentnode;
    if (a_tree_len <= trBidt_Tree_len_max) {
        for (currentnode = 0; currentnode < a_tree_len; currentnode++) {
            trBidt_nodeEmptyZap(a_tree_p, currentnode);
        }
        // set the root node to any
        a_tree_p[trBidt_rootNodeLocate_idx(a_tree_len)].the_ID = trB_ID_Any;
    }
}

/** Private finder for an ID/key in a trBidt tree.  Note parameter differences with the public version.
*
* Also: this recurses; Watch your stack!
*
* \retval - If found, the index into the tree where found; otherwise trB_None.
*/
uint16_t trBidt_privateFind(uint16_t an_ID,
                         /**< The ID/key being sought.  Must not be 0 or trB_None.  */
                            struct trBidt_Node_s *a_tree_p,
                                         /**< Points to the beginning of the tree to be searched. */
                            uint16_t a_tree_len,
                              /**< Then length of the tree; the number of nodes+1.  */
                            uint16_t a_node_idx
                             /**< Index to the present node being examined.  Must be 0...max nodes-1 */
    ) {
    uint16_t ret_val = trB_None;        // assume the worst, prove otherwise
    if ((a_node_idx != trB_None) && (a_node_idx < a_tree_len)) {

        if (an_ID == a_tree_p[a_node_idx].the_ID) {
            ret_val = a_node_idx;
        } else if (an_ID < a_tree_p[a_node_idx].the_ID) {
            ret_val = trBidt_privateFind(an_ID, a_tree_p, a_tree_len, a_tree_p[a_node_idx].child_left);
        } else {
            ret_val = trBidt_privateFind(an_ID, a_tree_p, a_tree_len, a_tree_p[a_node_idx].child_right);
        }

    }
    return ret_val;
}

/** Finds the specified ID/key in a trBidt tree.
*
* This walks the tree until the ID is matched or the path has been traversed.
*
* \retval - If found, the index into the tree where found; otherwise trB_None.
*/
uint16_t trBidt_find(uint16_t an_ID,
                         /**< The ID/key being sought.  Must not be 0 or trB_None.  */
                     struct trBidt_Node_s * a_tree_p,
                                         /**< Points to the beginning of the tree to be searched. */
                     uint16_t a_tree_len
                             /**< Then length of the tree; the number of nodes+1.  */
    ) {
    uint16_t ret_val = trB_None;        // assume the worst, prove otherwise
    if (an_ID <= trB_TrUsedAppUsed_ID_boundary) {
        ret_val = trBidt_privateFind(an_ID, a_tree_p, a_tree_len, a_tree_p[a_tree_len - 1].child_left);
    } else {
        ret_val = trBidt_privateFind(an_ID, a_tree_p, a_tree_len, a_tree_p[a_tree_len - 1].child_right);
    }
    return ret_val;
}

/** Returns an ID/key in a trBidt tree given an index.
*
* \retval - If found, the ID for that index into the tree where found; otherwise trB_None.
*/
uint16_t trBidt_revfind(uint16_t an_idx, struct trBidt_Node_s * a_tree_p, uint16_t a_tree_len)
{
    uint16_t ret_val = trB_None;        // assume the worst, prove otherwise
    if (an_idx < (a_tree_len - 1)) {
        ret_val = a_tree_p[an_idx].the_ID;
    }
    return ret_val;
}

/** Inserts the specified ID/key into a trBidt tree.
*
* \todo Placeholder for future ThingRendevous implementation.
*
* Note where it was inserted is returned.  
* Use it to identify the co-incident index
* into an table of specific data structures.
*
* Note duplicate ID behavior.
* If the ID already exists, returns where it already exists.
*
* Does not operate in a critical section; you need to ensure that.
*
* \retval - If successfully inserted, the index into the tree where inserted; if it already exists, where it exists; otherwise trB_None means no place left to insert.
*/
uint16_t trBidt_insert(uint16_t an_ID,
                         /**< The ID/key being inserted. */
                       struct trBidt_Node_s * a_tree_p,
                                         /**< Points to the beginning of the tree to be searched. */
                       uint16_t a_tree_len,
                              /**< The actual length of the tree; must be less than trBidt_Tree_len_max. */
                       bool * is_duplicate_p
                              /**< False usually; true when a duplicate key is inserted, so wherePut_p will indicate the existing key. */
    ) {
    return trB_None;
}

/** Deletes the specified ID/key from a trBidt tree.
*
* \todo Placeholder for future ThingRendevous implementation.
*
* Note the node "deleted" is returns.
* Use it to identify the co-incident index
* into an table of specific data structures.
* You may want to make changes there, too.
* Also makes a handy hint for future inserts (woo hoo!).
* 
* Does not operate in a critical section; you need to ensure that.
*
* \retval - If successfully deleted, the index into the tree where we now have free node; otherwise trB_None means it wasn't found.
*/
uint16_t trBidt_delete(uint16_t an_ID,
                         /**< The ID/key being deleted. */
                       struct trBidt_Node_s * a_tree_p,
                                         /**< Points to the beginning of the tree to be searched. */
                       uint16_t a_tree_len
                             /**< The actual length of the tree; must be less than trBidt_Tree_len_max. */
    ) {
    return trB_None;
}

/** Rebalances the tree presented.
*
* \todo Placeholder for future ThingRendevous implementation.
*
* Binary ID trees are not balanced or sorted.  This rebalances a tree but does not sort.
*/
void trBidt_rebalance(struct trBidt_Node_s *a_tree_p,
                                         /**< Points to the beginning of the tree. */
                      uint16_t a_tree_len
                             /**< The actual length of the tree; must be less than trBidt_Tree_len_max. */
    ) {
}
