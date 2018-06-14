/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Tr Test 04 : Harmless System Clock
*
* Tr internals were already white-boxed in the previous tests.
* Now, ensure that system clock is essentially harmless when nothing is happening
* and see if everything really works
* as expected.
* Uses trDump & stdio to report Tr internals.
* Also blinks the blue LED.
*
* It has tasks; these are stepped-thru manually;
* no actual task swapping is done.
*
* This assumes an stubbed/incomplete Tr:
*
* ~~ trAPI_start() goes directly to the main task
*
* ~~ trSR_RunningTask_idx is always set to the task idx by this program
*
* Use the finagle function in trDump.
*
* ~~ trTti_enter() is always "call direct"  result = 1
*
* Make the SVC handler always call direct.
*
* ~~ trTti_leave() never swaps tasks (or isn't called)
*
* Make the SVC handler always return direct.
*
* ~~ Explicit Interrupts don't.
*
* Always go back to the interrupted task.
*
* ~~ everything in trTstc is stubbed to do nothing.
* The following need stubbing:
*
* trTstc_ready_Resc_PrivSect_hdlr  DONE
* trTstc_susp_Resc_PrivSect_hdlr  DONE
* trTstc_completedInServ  DONE
*           
* ~~ The init task shuts itself down, but never runs.
*
* ~~ Tr's sys clock ticks are updated, but interrupt swapping is not done.
*
* Because explicit interrupts don't.
*
* There are three parts.
*
* ~~ 1.  Make sure the go flag isn't.
*
* Try a simple loop in test code; make sure trSClk doesn't.
* 
* ~~ 2.  Set the go flag, comment out trSClk private section handler.
*
* Repeat simple loop; make sure trSCLk doesn't.
*
* ~~ 3.  Uncomment trSClk private section handler.
*
* trSClk still shouldn't, 'cause there's nothing on the delay list.
*
* ~~ 4.  Uncomment trTstc_* stuff above.
*
* trSClk still shouldn't, 'cause there's nothing on the delay list.
*
* By the time this is done, we've verified
* when there's no tasks on a delay, then 
* trSClk doesn't update task lists.
*
* trStart tap go flag DONE
* trSClk private section  DONE
* trSClk full private section  DONE
* trTstc back to normal DONE
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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void trDump_byteDump(uint8_t * b_p, uint16_t cnt);
uint16_t trDump_id2Idx(uint16_t id);
void trDump_finageIdx(uint16_t idx);
void trDump_doSR(void);
void trDump_doT(uint16_t id);
void trDump_doStrm(uint16_t id);
void trDump_doMsg(uint16_t id);
void trDump_doEvt(uint16_t id);

void Tst04_callResDump(void)
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

trAPIEI_ForAndBy(SysTick_Handler, SysTick_Handler_actual)
void SysTick_Handler_actual(void)
{
    HAL_IncTick();
    trAPI_SClk_tick();
}

trAPIEI_ForAndBy(EXTI15_10_IRQHandler, EXTI15_10_IRQHandler_actual)
void EXTI15_10_IRQHandler_actual(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_BUTTON_PIN);
}

volatile uint16_t BlinkDelay;
volatile bool ChgBlinkDelay;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (!ChgBlinkDelay) {
        switch (BlinkDelay) {
        case 1000:
            BlinkDelay = 500;
            break;
        case 500:
            BlinkDelay = 100;
            break;
        case 100:
            BlinkDelay = 50;
            break;
        case 50:
            BlinkDelay = 1000;
            break;
        default:
            BlinkDelay = 500;
            break;
        }
        ChgBlinkDelay = true;
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define trTask_Tst04_init_ID 255
void trTask_Tst04_init(void *theTaskData_p)
{
    while (1) {
        trAPI_Task_delay(trAPI_TaskID_self, 0xFFFFU);
    }
}

#define trTask_Tst04_512_action_ID 512
void trTask_Tst04_512_action(void *theTaskData_p)
{
    const char *const Tst04_512_SignOn = "...\nTst04_512\n";
    while (1) {
        switch (BlinkDelay) {
        case 1000:
            printf(Tst04_512_SignOn);
            HAL_Delay(BlinkDelay);
            BSP_LED_Toggle(LED2);
            BlinkDelay = 500;
            ChgBlinkDelay = true;
            break;
        case 500:
        case 100:
        case 50:
            HAL_Delay(BlinkDelay);
            BSP_LED_Toggle(LED2);
            break;
        default:
            ;
            break;
        }
        if (ChgBlinkDelay) {
            ChgBlinkDelay = false;
        }
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
    BlinkDelay = 500;
    ChgBlinkDelay = false;

    BSP_LED_On(LED1);
}

int /* picky compilers */ main(void)
{
    main_init();

    trAPI_start();

    return 0;
}
