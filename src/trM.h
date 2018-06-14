/** \file
*   \brief ThingRendevous's Misc interface 
*
* Stuff that just doesn't fit elsewhere.
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

#ifndef trM_h
#define trM_h

#include "trKIDS.h"
#include "trBidt.h"

/** Serve up the source task index for the KIDS.  
*
* Since this was filled in at the API level from the current task,
* then it must be good.
*
* - Return type is uint16_t.
*/
#define trM_CP_Source_idx_get(aKIDS_p) ( trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx )

/** Serve up the source task ID from the KIDS.  
*
* Since the idx was filled in at the API level from the current task,
* then the ID found must be good.
*
* - param is a KIDS ptr
* - param is a Bidt node tree ptr (see trBidt)
* - param is the Bidt length (see trBidt)
* - Return type is uint16_t.
*/
#define trM_CP_Source_ID_get(aKIDS_p,a_tree_p,a_tree_len) ( trBidt_revfind( trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx, (struct trBidt_Node_s *)a_tree_p, a_tree_len ) )

uint16_t trM_CP_TarDesT_idx_get(void *aKIDS_p
                       /**< Expected to be a KIDS pointer. */
    );

uint16_t trM_CP_TarDesT_ID_get(void *aKIDS_p
                       /**< Expected to be a KIDS pointer. */
    );

void trM_wrapcpy_src(uint8_t * dest_p,
                     uint8_t * src_p, uint16_t src_len, uint16_t start_off, uint16_t quant, uint16_t * end_off_p);

void trM_wrapcpy_dest(uint8_t * dest_p,
                      uint16_t dest_len, uint16_t start_off, uint16_t quant, uint16_t * end_off_p, uint8_t * src_p);

// Note lack of initializer, as it should be.

#endif
