/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Tr Test 00
*
* This is test is an extention of the the Bare Machine Test 01 (not zero-zero!).
*
* It works the same as Bare Machine Test 01.
* Test the virtual comm port.
* Blink the LED using the system clock tick handler.
*
* ~~ BUT: uses a "hobbled" version of Tr.
*
* In truPACM4:
*
* \li Tr Itself enter/leave is suppressed; does nothing
* \li Explicit Itself enter/leave is suppressed; does nothing
* \li Private Sections can work, but won't be used.
*
* In short: No task switching.
*
* For interrupts:
*
* \li trAPIEI services are used
* \li but in consequence of truPACM4, no task switches occur.
* 
* At start up:
*
* \li trStart does work, or not, but only the one task defined here is used
*
* That one task is just a perpetual loop, just like main the bare machine.
*
* ~~ SO: What the hell is this testing?
*
* \li trtGen creates trTi file for this app
* \li trStart can set up, init, create a process stack, and launch a task
* \li interrupts can run on there own stack
* \li the whole trAPIEI chain can recognize no task switch is needed.
*
* ~~ Suggested Steps
*
* trStart:
*
* \li call trTst00_action direct
* \li stub out all asm code except xpsrinit
*
* trAPIEI:
*
* \li stub out EI enter/exit traps & compares
*
* truPACM4:
*
* \li stub out all Task Itself & Explicit interrupt handling
* \li leave private section in
*
* Run a test: Should work.  This is bare machine operation,
* with a layer of interrupt handling BS.
*
* ~~ PASS.
*
* trStart:
*
* \li call trTst00_action direct
* \li remove assembly code stub outs, one by one
*
* Run a test each time: Should work.
* Piece-wise trStart "doesn't screw up" testing.
* And we're running on two stacks.
*
* ~~ uncomment interrupt disable/enable PASS
* ~~ uncomment stack switch PASS
*
* trAPIEI:
*
* \li remove stub out EI enter/exit traps & compares
*
* Run a test each time: Should work.
* Piece-wise verify that interrupt wrapper can handoff to 
* SVC entry point and get back.
*
* ~~ stub out trap, stub nulls for trap ret vals; verifies ret val works w/o trap PASS
* ~~ uncomment traps, leave null stub in place; GOTO BELOW
*
* truPACM4:
*
* \li precondition is test above
* \li remove stub out handler
*
* Piece-wise check-out handler to actual_handler handoff
* 
* ~~ stub out sp selection, call to actual handler, return checks,
*    with trAPIEI in state above; PASS
* ~~ remove stub outs, call actual handler returning "do nothing" values; PASS
* ~~ GOTO BELOW
*
* trAPIEI:
*
* ~~ remove null stub; verify trap return val is correct PASS
*
* Leave truPACM4 alone; modifications carry over to future testing.
*
* Ditto trStart.
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
#include <string.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_nucleo_144.h"
#include "F429_SystemClock_Config.h"
#include "F429_stm32f4xx_hal_msp.h"
#include <stdio.h>
#include "trAPIEI.h"
#include "trAPI.h"

volatile uint8_t BlinkSpeed;
volatile bool ChgdBlinkSpeed;

trAPIEI_ForAndBy(SysTick_Handler, SysTick_Handler_actual)
void SysTick_Handler_actual(void)
{
    HAL_IncTick();
}

trAPIEI_ForAndBy(EXTI15_10_IRQHandler, EXTI15_10_IRQHandler_actual)
void EXTI15_10_IRQHandler_actual(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (!ChgdBlinkSpeed) {
        if (BlinkSpeed >= 2) {
            BlinkSpeed = 0;
        } else {
            BlinkSpeed++;
        }
        ChgdBlinkSpeed = true;
    }
}

void LED2_Blinky(void)
{
    /* Test on blink speed */
    if (BlinkSpeed == 0) {
        BSP_LED_Toggle(LED2);
        /* Wait for 500ms */
        HAL_Delay(500);
    } else if (BlinkSpeed == 1) {
        BSP_LED_Toggle(LED2);
        /* Wait for 100ms */
        HAL_Delay(100);
    } else if (BlinkSpeed >= 2) {
        BSP_LED_Toggle(LED2);
        /* Wait for 50ms */
        HAL_Delay(50);
    }
}

const char *const trPrintfy_by = "BlinkSpeed is %0#3hx \n\r";

void Printfy(void)
{
    if (ChgdBlinkSpeed) {
        printf(trPrintfy_by, BlinkSpeed);
        ChgdBlinkSpeed = false;
    }
}

void main_init(void)
{
    /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, instruction and Data caches
       - Configure the Systick to generate an interrupt each 1 msec
       - Set NVIC Group Priority to 4
       - Global MSP (MCU Support Package) initialization
     */
    HAL_Init();

    /* Configure the system clock to 180 MHz */
    SystemClock_Config();

    /* Configure LEDs on Nucleo */
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);

    /* Configure the User Button in EXTI Mode */
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    F429_stm32f4xx_hal_msp_init();      /* to pull in the hal_msp additions */

    /* Initiate BlinkSpeed variable */
    BlinkSpeed = 0;
    ChgdBlinkSpeed = false;

    BSP_LED_On(LED1);
}

void trTask_trTst00_init(void *theTaskData_p)
{
    while (1) {
        trAPI_Task_delay(trAPI_TaskID_self, 0xFFFFU);
    }
}

#define trTask_trTst00_init_ID 255

void trTask_trTst00_action(void *theTaskData_p)
{
    while (1) {
        LED2_Blinky();
        Printfy();
    }
}

#define trTask_trTst00_action_ID 256

int /* picky compilers */ main(void)
{
    main_init();

    trAPI_start();

    return 0;
}
