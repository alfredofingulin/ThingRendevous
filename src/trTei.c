/** \file
*   \brief ThingRendevous's Tr Explicit Interrupt implementation, customized for ARM Cortex M4 
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
#include "trTei.h"

/** Enter (maybe) Tr Explicit Interrupt
*
* This is not used by ARM Cortex M4 machines.  See trAPIEI and truPACM4.
*/
uint16_t trTei_enter(void *aKIDS_p)
{
    return 0;
}

/** Leave (maybe) Tr Explicit Interrupt
*
* This is not used by ARM Cortex M4 machines.  See trAPIEI and truPACM4.
*/
void trTei_leave(void *aKIDS_p)
{
}

/** Tr Explicity Interrupt "Task"
*
* This is not used by ARM Cortex M4 machines.  See trAPIEI and truPACM4.
*/
void trTask_Tei(void *theTaskData_p
                             /**< This is not used. */
    )
{
}

/** Tr's Explicit Interrupt init
*
*/
void trTei_init(void)
{
}
