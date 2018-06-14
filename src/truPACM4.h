/** \file
*   \brief ThingRendevous's customized ARM Cortex m4 Context Handlers 
*
* Doxygen and Indent don't cope too well with this source code.
*
* This module implements the rendezvous scheme described in
* \link uDN06_RendEI \endlink , 
* \link uDN07_RendTI \endlink ,
* and \link uDN08_TskCntxt \endlink
* for mutual exclusion and actual context switching.
*
* It is the target for ARM Cortex M4 SVC and PendSV exception handlers
* allocated in startup_FRDMK20D50M.sx.
*
* This interface implements inline code
* coupling SVC and PendSV to the execption targets.
*
* Note the extensive use of dsb, data sync barrier instructions;
* ensures all data is fresh.  See
* https://stackoverflow.com/questions/15491751/real-life-use-cases-of-barriers-dsb-dmb-isb-in-arm
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

#ifndef truPACM4_h
#define truPACM4_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/** A template to marshalling an SVCall.  Embed your own modification to this.
*
* Lots of ARM peculiarities here.
*
* The exception expects r0..3 on that stack;
* that's how the exception gets the parameters.
*
* In turn, the exception returns a value in r0, and r1 & r2 & r3 if you want, on the stack.
*
* \param r*_param - Whatever you want, going into r0..3 respectivly, as uint32_t, but probably pointers.  You'll have to type cast to make things happen.
*
* \retval r0 into result - A uint32_t for you to act upon.  Some type coersion for you.
*
* Note that clobber list is unnecessary.
* That's 'cause r0..3 are considered scratch.
*/
static inline uint32_t __attribute__ ((always_inline)) truPACM4_SVCall_Template(uint32_t r0_param,
                        /**< Parameter/pointer coerced to uint32_t, goes in r0. */
                                                                                uint32_t r1_param,
                        /**< Parameter/pointer coerced to uint32_t, goes in r1. */
                                                                                uint32_t r2_param,
                        /**< Parameter/pointer coerced to uint32_t, goes in r2. */
                                                                                uint32_t r3_param
                       /**< Parameter/pointer coerced to uint32_t, goes in r3. */
    )
{
    uint32_t result = 0;

    __asm__ __volatile__(
        "ldr    r0, %1 \n\t"
        "ldr    r1, %2 \n\t"
        "ldr    r2, %3 \n\t" 
        "ldr    r3, %4 \n\t" 
        "dsb \n\t"
    "\n\t"
        "svc    $255 \n\t" 
    "\n\t"
        "str    r0, %0 \n\t"
    "\n\t"
        :"=m"(result)
        :"m"(r0_param), "m"(r1_param), "m"(r2_param), "m"(r3_param)
        : "r0", "r1", "r2", "r3"
        );
    return result;
}

/** A template to marshalling PendSV.  Embed your own modification to this.
*
* This is an assembler-packer _example_ for generating a Pend SV exception.
*
* Everything written about the SVCall applies here.
*/
static inline uint32_t __attribute__ ((always_inline)) truPACM4_PendSV_Template(uint32_t r0_param,
                        /**< Parameter/pointer coerced to uint32_t, goes in r0. */
                                                                                uint32_t r1_param,
                        /**< Parameter/pointer coerced to uint32_t, goes in r1. */
                                                                                uint32_t r2_param,
                        /**< Parameter/pointer coerced to uint32_t, goes in r2. */
                                                                                uint32_t r3_param
                       /**< Parameter/pointer coerced to uint32_t, goes in r3. */
    )
{
    uint32_t result;

    __asm__ __volatile__(
        "ldr    r0, %0 \n\t"
        "ldr    r1, %1 \n\t"
        "ldr    r2, %2 \n\t"
        "ldr    r3, %3 \n\t"
        "dsb \n\t"
        :
        :"m"(r0_param), "m"(r1_param), "m"(r2_param), "m"(r3_param)
        : "r0", "r1", "r2", "r3"
        );

    // this actually causes pendSV exception
    /** \todo board support package goes here SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk; */

    __asm__ __volatile__(
        "str    r0, %0 \n\t"
    "\n\t"
        :"=m"(result)
        :
        :
        );
    return result;
}

/** This marshalls SVCall to handle private sections.
*
* _Not an example._  Here 'cause no other place makes sense.
*
* Note data vs register is _not_ what you expect.
*/
static inline void __attribute__ ((always_inline)) truPACM4_Private_Section(
    void (*the_private_section_hdlr) (void *usually_aKIDS_p, void *more_stuff_p),
    void * actual_usually_aKIDS_p,
    void * actual_more_stuff_p
    )
{
    __asm__ __volatile__(
        "movw   r0, #23 \n\t"
        "ldr    r1, %0 \n\t"
        "ldr    r2, %1 \n\t"
        "ldr    r3, %2 \n\t"
        "dsb \n\t"
    "\n\t"
        "svc    $255 \n\t" 
        :
        :"m"(actual_usually_aKIDS_p), "m"(the_private_section_hdlr), "m"(actual_more_stuff_p)
        : "r0", "r1", "r2", "r3"
        );
}

void truPACM4_init(void);

#endif
