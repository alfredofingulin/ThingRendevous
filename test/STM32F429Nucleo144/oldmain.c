/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Tr Test 06 : Daisy Chain Events
*
* Tr internals were already white-boxed in the previous tests.
* Now, test basic, non-preempting task switching.
*           
* ~~ The init task shuts itself down.
*
* ~~ Three tasks, daisy-chaining an event.
*
* ~~ Tr's sys clock ticks are updated, but interrupt swapping is not done.
*
* ~~ trAPI_start() picks the highest priority task to run.
*
* ~~ All three tasks have the same priority.
*
* ~~ trTti_enter/leave are fully implemented.
*
* ~~ These are fully implemented:
*
* trTstc_ready_Resc_PrivSect_hdlr
* trTstc_susp_Resc_PrivSect_hdlr
* trTstc_completedInServ
* 
* ~~ Each task runs, signs on, toggles LED, does an active delay, tosses an event to the next task.
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

void Tst06_callResDump(void)
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// messag tests

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// message actual tests with init

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

#define trTask_Tst06_init_ID 255
void trTask_Tst06_init(void *theTaskData_p)
{
    while (1) {
        trAPI_Task_delay(trAPI_TaskID_self, 0xFFFFU);
    }
}

#define trTask_Tst06_512_ID 512
void trTask_Tst06_512_action(void *theTaskData_p)
{
    while (1) {
        const char *const Tst06_512_SignOn = "...\nTst06_512\n\r";
        printf(Tst06_512_SignOn);
        BSP_LED_Toggle(LED2);
        HAL_Delay(BlinkDelay);
        if (ChgBlinkDelay) {
            ChgBlinkDelay = false;
        }
        trAPI_Task_delay(trAPI_TaskID_self, 1U);
    }
}

#define trTask_Tst06_513_ID 513
void trTask_Tst06_513_action(void *theTaskData_p)
{
    while (1) {
        const char *const Tst06_513_SignOn = "...\nTst06_513\n\r";
        printf(Tst06_513_SignOn);
        BSP_LED_Toggle(LED2);
        HAL_Delay(BlinkDelay);
        if (ChgBlinkDelay) {
            ChgBlinkDelay = false;
        }
        trAPI_Task_delay(trAPI_TaskID_self, 1U);
    }
}

#define trTask_Tst06_514_ID 514
void trTask_Tst06_514_action(void *theTaskData_p)
{
    while (1) {
        const char *const Tst06_514_SignOn = "...\nTst06_514\n\r";
        printf(Tst06_514_SignOn);
        BSP_LED_Toggle(LED2);
        HAL_Delay(BlinkDelay);
        if (ChgBlinkDelay) {
            ChgBlinkDelay = false;
        }
        trAPI_Task_delay(trAPI_TaskID_self, 1U);
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
