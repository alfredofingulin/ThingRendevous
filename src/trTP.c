/** \file
*   \brief ThingRendevous's Task Private implementation
*
* Private types known only to Task and trTi code generator Trtgen.
*
* \author Alfred O Fingulin FinguPublic@gmail.com
*/
/*
* MIT License
* 
* Copyright (c) 2018, 2017 Alfred O Fingulin  FinguPublic@gmail.com
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
#include "trTi.h"
#include "trPl.h"
#include "trTP.h"

/** For a task index, serve up all pointers to info used to service the task.
*
* \retval true - task info exists for this task index
* \retval false - no task associated with this task.
*/
bool trTP_getp(uint16_t forTask_idx,   /**< Index, not the task ID, of the task of interest. */
               struct trTP_CI_s **ci_pp,
                                   /**< Where to put the pointer to the config info. */
               struct trTP_CB_s **cb_pp,
                                   /**< Where to put the pointer to a control block. */
               uint8_t ** sb_pp
                         /**< Where to put the pointer to a stack block. */
    )
{
    bool ret_val = false;

    if ((forTask_idx < trTi_NumOfTasks)
        && (trTi_TaskInfoList[forTask_idx].ib_t_p != NULL)) {
        *ci_pp = (struct trTP_CI_s *)trTi_TaskInfoList[forTask_idx].ib_t_p;
        *cb_pp = (struct trTP_CB_s *)trTi_TaskInfoList[forTask_idx].ib_t_p->ci_cb_p;
        *sb_pp = trTi_TaskInfoList[forTask_idx].ib_t_p->ci_sb_p;
        ret_val = true;
    }
    return ret_val;
}
