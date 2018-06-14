/** \file
*   \brief ThingRendevous's System Clock implementation
*
* Here are the KIDS function handlers.
* Relies on externs populated in trTi by generation from Trtgen.
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
#include "trKIDS.h"
#include "trAPIRet.h"
#include "trTi.h"
#include "trSR.h"
#include "trTP.h"
#include "trPl.h"
#include "truPACM4.h"
#include "trTstc.h"
#include "trSClk.h"

/** Private section to modify delayed tasks.
*/
void trSClk_PrivSect_hdlr(void *notused, void *notused1)
{
    uint16_t taskofinterest;
    bool moreentriesexist;

    // why yes, the logic here is obtuse
    //
    // if we delete whatever we're at, it screws up the iterator
    // hence all the side-stepping

    if ((moreentriesexist = !trPl_isEmpty(trSR_TimeoutTasks_pl))) {

        taskofinterest = trSR_TimeoutTasks_pl.head;

        while (moreentriesexist) {

            if (((trTi_TaskInfoList[taskofinterest].ib_t_p->ci_cb_p->cb_task_delay_remaining) > 0)
                && ((--(trTi_TaskInfoList[taskofinterest].ib_t_p->ci_cb_p->cb_task_delay_remaining) == 0))
                ) {
                trTstc_ready_Timo(taskofinterest);
            }

            moreentriesexist = trPl_next(&taskofinterest, trTi_TaskTimeout_pe);

        }

    }
}

/** Somewhat obvious; incremented with every clock tick.
*/
volatile uint16_t trSClk_AccumulatedTicks;

/** Inc ticks, walk thru the task delay count list, make ready those that are zero.
*/
void trSClk_sclk(void *aKIDS_p)
{
    if (trSR_Flags_going()) {

        ++trSClk_AccumulatedTicks;

        truPACM4_Private_Section(trSClk_PrivSect_hdlr, NULL, NULL);

    }
    (trKIDS_SClk_Tick_aKIDS_p()->cp).cp_ret_val = trAPIRet_OK;
}

/** SClk Private inits.
*
*/
void trSClk_init(void)
{
    trSClk_AccumulatedTicks = 0;
}
