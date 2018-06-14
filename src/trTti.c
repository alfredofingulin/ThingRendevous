/** \file
*   \brief ThingRendevous's Tr Task Itself implementation, customized for ARM Cortex M4 
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
#include "trKhf.h"
#include "truPACM4.h"
#include "trTti.h"

/** Enter (maybe) the Tr Task Itself
*
* \retval 0: Kernel request executed by Tr Task Itself; no further action needed.
* \retval non-zero: Call the Tr Task Itself directly.
*/
uint16_t trTti_enter(void *aKIDS_p
                       /**< Where to find the kernel request. */
    )
{
    uint32_t result = 1;
    if (!trKhf_runOnCaller(aKIDS_p)) {
        __asm__ __volatile__(
            "movw   r0, #19 \n\t"
            "ldr    r1, %1 \n\t"
            "dsb \n\t"
        "\n\t"
            "svc    $255 \n\t" 
        "\n\t"
            "str    r0, %0 \n\t"
        "\n\t"
            :"=m"(result)
            : "m"(aKIDS_p)
            : "r0", "r1"
            );
    }
    return ((uint16_t) result);
}

/** Private Leave (maybe) the Tr Task Itself
*
*/
void trTti_leave(void *aKIDS_p
                       /**< Where to find the kernel request. */
    )
{
    if (!trKhf_runOnCaller(aKIDS_p)) {
        __asm__ __volatile__(
            "movw   r0, #20 \n\t"
            "ldr    r1, %0 \n\t"
            "dsb \n\t"
        "\n\t"
            "svc    $255 \n\t" 
        "\n\t"
            : 
            : "m"(aKIDS_p)
            : "r0", "r1"
            );
    }
}

/** Tr Task Itself
*/
void trTask_Tti(void *theTaskData_p
                             /**< Expected to be a KIDS pointer. */
    )
{
    ((((struct trKIDS_ComParams_AntiAlias_s *)theTaskData_p)->cp).cp_kernel_func_hndlr) (theTaskData_p);
    trTti_leave(theTaskData_p);
}

/** Tr's Task Itself init has nothing to do.
*
*/
void trTti_init(void)
{
}
