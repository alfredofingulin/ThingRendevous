/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Tr Test 07 : Emergency with Task Ping Pong
*
* Use Test 06 as a baseline.
*
* Test basic, preempting task switching.
*           
* ~~ The init task shuts itself down.
*
* ~~ One high priority task gets an event from a push button interrupt; it sends a panic message.
*
* ~~ Two tasks, each with event queue, same priority.  On event, announce, work LED for 5 seconds, bounce event to other task.
*
* ~~ Tr's sys clock ticks are updated.
*
* ~~ trTti_enter/leave are fully implemented.
*
* ~~ trTei_enter/leave are fully implemented (about damn time).
*
* ~~ These are fully implemented:
*
* trTstc_ready_Resc_PrivSect_hdlr
* trTstc_susp_Resc_PrivSect_hdlr
* trTstc_completedInServ
* 
* ~~ trAPI_start() picks the task.
*
* The goal is to make sure two tasks can back-n-forth 
* while a third one can notify of a panic.
*
* Testing Steps
*
* ~~ 1.  Turn off trTei_enter/leave, turn off panic task notification from interrupt.
*
* Result:  Panic task should sign on.  Everything works like test 06.
*
* ~~ 2.  Turn on trTei_enter/leave, turn off panic task notification from interrupt.
*
* Result: just like 1.
*
* ~~ 3.  Turn on panic task notification from interrupt.
*
* Result: Press the button and tasks should have a panic message.
*
* May need to regenerate trTi using trtgen.
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

void Tst07_callResDump(void)
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define trTask_Tst07_init_ID 255
// NominalPriority will be forced to 255
// StackSize will default to 1024

#define trTask_Tst07_panicJonny_ID 510
#define trTask_Tst07_panicJonny_NominalPriority 20000
#define trTask_Tst07_panicJonny_EvtLen 5
// StackSize will default to 1024

#define trTask_Tst07_workerBenton_ID 520
// NominalPriority will default to 32768
#define trTask_Tst07_workerBenton_EvtLen 5
// StackSize will default to 1024

#define trTask_Tst07_workerRace_ID 530
// NominalPriority will default to 32768
#define trTask_Tst07_workerRace_EvtLen 5
// StackSize will default to 1024

#define trTask_Tst07_idle_ID 599
#define trTask_Tst07_idle_NominalPriority 50000
// StackSize will default to 1024

#define FlagsIrrelevent 0
#define AnyEvtWillDo 1

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
        // switch (BlinkDelay) {
        // case 1000:
        // BlinkDelay = 500;
        // break;
        // case 500:
        // BlinkDelay = 100;
        // break;
        // case 100:
        // BlinkDelay = 50;
        // break;
        // case 50:
        // BlinkDelay = 1000;
        // break;
        // default:
        // BlinkDelay = 500;
        // break;
        // }
        trAPI_Evt_stf(trTask_Tst07_panicJonny_ID, FlagsIrrelevent, AnyEvtWillDo);
        ChgBlinkDelay = true;
    }
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void trTask_Tst07_init(void *theTaskData_p)
{
    const char *const Tst07_init_SignOn = "Tst07_init: Sign On\n";
    const char *const Tst07_init_post_ing = "Tst07_init: post to Benton\n";
    const char *const Tst07_init_SignOff = "Tst07_init: Sign Off\n";
    const char *const Tst07_init_BadRetVal = "Tst07_init: Bad ret val is %0#5hx\n";
    uint16_t r;
    printf(Tst07_init_SignOn);
    while (1) {
        printf(Tst07_init_post_ing);
        if ((r = trAPI_Evt_post(trTask_Tst07_workerBenton_ID, FlagsIrrelevent, AnyEvtWillDo))
            != trAPIRet_OK) {
            printf(Tst07_init_BadRetVal, r);
        }
        printf(Tst07_init_SignOff);
        trAPI_Task_delay(trAPI_TaskID_self, 0xFFFFU);
    }
}

void trTask_Tst07_panicJonny(void *theTaskData_p)
{
    const char *const Tst07_panicJonny_SignOn = "Tst07_panicJonny: Sign On\n";
    const char *const Tst07_panicJonny_pend_ing = "Tst07_panicJonny: pend-ing\n\n";
    const char *const Tst07_panicJonny_JustPanic_ing = "\n\nPanic, Jonny!  Just panic!\n";
    const char *const Tst07_panicJonny_BadRetVal = "Tst07_panicJonny: Bad ret val is %0#5hx\n";
    uint16_t s;
    uint16_t f;
    uint32_t e;
    uint16_t r;
    printf(Tst07_panicJonny_SignOn);
    while (1) {
        printf(Tst07_panicJonny_pend_ing);
        ChgBlinkDelay = false;
        if ((r = trAPI_Evt_pend(&s, &f, &e))
            != trAPIRet_OK) {
            printf(Tst07_panicJonny_BadRetVal, r);
        } else {
            printf(Tst07_panicJonny_JustPanic_ing);
        }
    }
}

void trTask_Tst07_workerBenton(void *theTaskData_p)
{
    const char *const Tst07_workerBenton_SignOn = "Tst07_workerBenton: Sign On\n";
    const char *const Tst07_workerBenton_pend_ing = "Tst07_workerBenton: pend-ing\n";
    const char *const Tst07_workerBenton_work_ing = "Tst07_workerBenton: work-ing\n";
    const char *const Tst07_workerBenton_post2Race = "Tst07_workerBenton: post to Race\n";
    const char *const Tst07_workerBenton_BadRetVal = "Tst07_workerBenton: Bad ret val is %0#5hx\n";
    int i;
    uint16_t s;
    uint16_t f;
    uint32_t e;
    uint16_t r;
    printf(Tst07_workerBenton_SignOn);
    while (1) {
        printf(Tst07_workerBenton_pend_ing);
        if ((r = trAPI_Evt_pend(&s, &f, &e))
            != trAPIRet_OK) {
            printf(Tst07_workerBenton_BadRetVal, r);
        } else {
            for (i = 0; i < 20; i++) {
                printf(Tst07_workerBenton_work_ing);
                HAL_Delay(250);
                BSP_LED_Toggle(LED2);
            }
            printf(Tst07_workerBenton_post2Race);
            if ((r = trAPI_Evt_post(trTask_Tst07_workerRace_ID, FlagsIrrelevent, AnyEvtWillDo))
                != trAPIRet_OK) {
                printf(Tst07_workerBenton_BadRetVal, r);
            }
        }
    }
}

void trTask_Tst07_workerRace(void *theTaskData_p)
{
    const char *const Tst07_workerRace_SignOn = "Tst07_workerRace: Sign On\n";
    const char *const Tst07_workerRace_pend_ing = "Tst07_workerRace: pend-ing\n";
    const char *const Tst07_workerRace_work_ing = "Tst07_workerRace: work-ing\n";
    const char *const Tst07_workerRace_post2Benton = "Tst07_workerRace: post to Benton\n";
    const char *const Tst07_workerRace_BadRetVal = "Tst07_workerRace: Bad ret val is %0#5hx\n";
    int i;
    uint16_t s;
    uint16_t f;
    uint32_t e;
    uint16_t r;
    printf(Tst07_workerRace_SignOn);
    while (1) {
        printf(Tst07_workerRace_pend_ing);
        if ((r = trAPI_Evt_pend(&s, &f, &e))
            != trAPIRet_OK) {
            printf(Tst07_workerRace_BadRetVal, r);
        } else {
            for (i = 0; i < 10; i++) {
                printf(Tst07_workerRace_work_ing);
                HAL_Delay(500);
                BSP_LED_Toggle(LED2);
            }
            printf(Tst07_workerRace_post2Benton);
            if ((r = trAPI_Evt_post(trTask_Tst07_workerBenton_ID, FlagsIrrelevent, AnyEvtWillDo))
                != trAPIRet_OK) {
                printf(Tst07_workerRace_BadRetVal, r);
            }
        }
    }
}

void trTask_Tst07_idle(void *theTaskData_p)
{
    const char *const Tst07_idle_SignOn = "Tst07_idle: Sign On\n";
    const char *const Tst07_idle_work_ing = "Tst07_idle: work-ing\n";
    int i;
    printf(Tst07_idle_SignOn);
    while (1) {
        for (i = 0; i < 5; i++) {
            printf(Tst07_idle_work_ing);
            HAL_Delay(1000);
            BSP_LED_Toggle(LED2);
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

    const char *const main_init_SignOff = "\n\nmain_init\n";
    printf(main_init_SignOff);
}

int /* picky compilers */ main(void)
{
    main_init();

    const char *const main_SignOn = "main; off to trAPI_start\n";
    printf(main_SignOn);

    trAPI_start();

    return 0;
}
