/** \file
*   \brief ThingRendevous's Misc implementation
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

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "trB.h"
#include "trBidt.h"
#include "trKIDS.h"
#include "trTi.h"
#include "trAPIRet.h"
#include "trM.h"

/** Using KIDS common params, return the target/destination task index.
*
* \retval - the index for the targ/desti or trB_None.
*/
uint16_t trM_CP_TarDesT_idx_get(void *aKIDS_p
                       /**< Expected to be a KIDS pointer. */
    )
{
    uint16_t ret_val;
    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_destination_target_task == trB_ID_Self) {
        ret_val = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx;
    } else {
        ret_val = trBidt_find(trKIDS_ComParams_aKIDS_p_AntiAlias().cp_destination_target_task,
                              (struct trBidt_Node_s *)&trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne);
    }
    return ret_val;
}

/** Using KIDS common params, return the target/destination task ID.
*
* \retval - the ID for the targ/desti or trB_None.
*/
uint16_t trM_CP_TarDesT_ID_get(void *aKIDS_p
                       /**< Expected to be a KIDS pointer. */
    )
{
    uint16_t ret_val;
    if (trKIDS_ComParams_aKIDS_p_AntiAlias().cp_destination_target_task == trB_ID_Self) {
        ret_val = trBidt_revfind(trKIDS_ComParams_aKIDS_p_AntiAlias().cp_my_task_idx,
                                 (struct trBidt_Node_s *)&trTi_TaskIDTree[0], trTi_NumOfTasks_PlusOne);
    } else {
        ret_val = trKIDS_ComParams_aKIDS_p_AntiAlias().cp_destination_target_task;
    }
    return ret_val;
}

/** Copy from a buffer assuming wraparound is possible.
*/
void trM_wrapcpy_src(uint8_t * dest_p,  /**< Where to put the bytes copied. */
                     uint8_t * src_p,    /**< Where to find the bytes copied. */
                     uint16_t src_len,     /**< Length of the source buffer. */
                     uint16_t start_off,     /**< Where in the source to start copying from. */
                     uint16_t quant,     /**< How many bytes to copy. */
                     uint16_t * end_off_p    /**< After the copy, the offset into the source. */
    )
{
    uint16_t toend;
    uint16_t frombegin;

    if ((quant > src_len)
        || (start_off >= src_len)
        || (quant == 0)) {
        *end_off_p = start_off;
    } else {

        toend = src_len - start_off;

        if (toend >= quant) {
            memcpy(dest_p, &src_p[start_off], (size_t) quant);
            if (toend == quant) {
                *end_off_p = 0;
            } else {
                *end_off_p = start_off + quant;
            }
        } else {
            frombegin = quant - toend;
            memcpy(dest_p, &src_p[start_off], (size_t) toend);
            memcpy(&dest_p[toend], &src_p[0], (size_t) frombegin);
            *end_off_p = frombegin;
        }

    }

/*	uint16_t i;
	uint16_t j;
	uint16_t k;
	
	if ( ( quant > src_len ) 
		|| ( start_off >= src_len )
		|| ( quant == 0 ) )
	{
		*end_off_p = start_off;
	}
	else
	{
		
		j = 0;
		k = start_off;
		for ( i = 0; i < quant; i++ )
		{
			dest_p[j++] = src_p[k++];
			if ( k >= src_len )
			{
				k = 0;
			}
		}
		*end_off_p = k;
		
	}
*/
}

/** Copy from a buffer assuming wraparound is possible.
*/
void trM_wrapcpy_dest(uint8_t * dest_p,  /**< Where to put the bytes copied. */
                      uint16_t dest_len,    /**< Length of the destination buffer. */
                      uint16_t start_off,    /**< Where in the destination to start copying from. */
                      uint16_t quant,    /**< How many bytes to copy. */
                      uint16_t * end_off_p,   /**< After the copy, the offset into the destination. */
                      uint8_t * src_p   /**< Where to find the bytes copied. */
    )
{
    uint16_t toend;
    uint16_t frombegin;

    if ((quant > dest_len)
        || (start_off >= dest_len)
        || (quant == 0)) {
        *end_off_p = start_off;
    } else {

        toend = dest_len - start_off;

        if (toend >= quant) {
            memcpy(&dest_p[start_off], src_p, (size_t) quant);
            if (toend == quant) {
                *end_off_p = 0;
            } else {
                *end_off_p = start_off + quant;
            }
        } else {
            frombegin = quant - toend;
            memcpy(&dest_p[start_off], src_p, (size_t) toend);
            memcpy(&dest_p[0], &src_p[toend], (size_t) frombegin);
            *end_off_p = frombegin;
        }

    }

/*	uint16_t i;
	uint16_t j;
	uint16_t k;
	
	if ( ( quant > dest_len ) 
		|| ( start_off >= dest_len )
		|| ( quant == 0 ) )
	{
		*end_off_p = start_off;
	}
	else
	{
		
		j = start_off;
		k = 0;
		for ( i = 0; i < quant; i++ )
		{
			dest_p[j++] = src_p[k++];
			if ( j >= dest_len )
			{
				j = 0;
			}
		}
		*end_off_p = j;
		
	}
*/
}

// Note lack of initializer, as it should be.
