/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Tr Test 02 : Event Tests w/o Tasking
*
* The idea is to white-box test Tr internals,
* specifically Events,
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
* ~~ trAPI_start() goes directly to the main task  DONE
*
* ~~ trSR_RunningTask_idx is always set to the task idx by this program  DONE, ...i hope...
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
* ~~ everything in trTstc is stubbed to do nothing.
* The following need stubbing:
*
* trTstc_ready_Resc_PrivSect_hdlr  DONE
* trTstc_susp_Resc_PrivSect_hdlr  DONE
* trTstc_completedInServ  DONE
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

void Tst02_callResDump(uint16_t ret, uint16_t id, uint16_t fc, uint16_t s, uint16_t f, uint32_t e)
{
    const char *const Tst02_Rifcsfe =
        "RetVal: %0#5hx  Me: %0#5hx  FlgCtr: %0#5hx  Source: %0#5hx  Flags: %0#5hx  Event: %0#10lx\n";
    printf(Tst02_Rifcsfe, ret, id, fc, s, f, e);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// AAAA produces events, highest priority
// BBBB consumes events, mid priority, len 5
// CCCC produces events, lower priority

// flags : events
// 
//  0 : AAAA 1 -> |  A0
//  1 : CCCC 1 -> |  C0, A0
//  2 : BBBB <- 1 |  C0 {A0}
// 
//  3 : CCCC 1 -> |  C1, C0
//  4 : CCCC 1 -> |  C2, C1, C0
//  5 : AAAA 1 -> |  C2, C1, C0, A1
//  6 : BBBB <- 1 |  C2, C1, C0 {A1}
//  7 : BBBB <- 1 |  C2, C1 {C0}
// 
//  8 : AAAA 1 -> |  C2, C1, A2
//  9 : AAAA 1 -> |  C2, C1, A3, A2
// 10 : BBBB <- 1 |  C2, C1, A3 {A2}
// 11 : BBBB <- 1 |  C2, C1 {A3}
// 12 : BBBB <- 1 |  C2 {C1}
//
// 13 : CCCC 1 -> |  C3, C2
// 14 : AAAA 1 -> |  C3, C2, A4
// 15 : AAAA 1 -> |  C3, C2, A5, A4
// 16 : AAAA 1 -> |  C3, C2, A6, A5, A4
// 17 : CCCC 1 -> |  full C3, C2, A6, A5, A4
//
// and 5* BBBB should pull off events in the order listed.
// Last 22: BBBB should fail to find anything.

uint16_t Flags_Ctr;

#define trTask_Tst02AAAA_ID 0xAAAA
#define trTask_Tst02AAAA_NominalPriority 0x0201

#define trTask_Tst02BBBB_ID 0xBBBB
#define trTask_Tst02BBBB_EvtLen 5
#define trTask_Tst02BBBB_NominalPriority 0x0202

#define trTask_Tst02CCCC_ID 0xCCCC
#define trTask_Tst02CCCC_NominalPriority 0x0203

uint8_t AAAA_Ctr;

void trTask_Tst02AAAA(void *theTaskData_p)
{
    uint16_t ret;
    uint32_t e;
    const char *const Test00AAAA_SignOn = "...\nTest00AAAA   trAPI_Evt_stf\n";
    printf(Test00AAAA_SignOn);
    e = 0x00A0U | (unsigned long)(AAAA_Ctr);
    if (++AAAA_Ctr >= 16) {
        AAAA_Ctr = 0;
    }
    ret = trAPI_Evt_stf(trTask_Tst02BBBB_ID, Flags_Ctr, e);
    Tst02_callResDump(ret, trTask_Tst02AAAA_ID, Flags_Ctr, trTask_Tst02AAAA_ID, Flags_Ctr, e);
}

void trTask_Tst02BBBB(void *theTaskData_p)
{
    uint16_t ret;
    uint16_t s;
    uint16_t f;
    uint32_t e;
    const char *const Test00BBBB_SignOn = "...\nTest00BBBB   trAPI_Evt_grb\n";
    printf(Test00BBBB_SignOn);
    ret = trAPI_Evt_grb(&s, &f, &e);
    Tst02_callResDump(ret, trTask_Tst02BBBB_ID, Flags_Ctr, s, f, e);
}

uint8_t CCCC_Ctr;

void trTask_Tst02CCCC(void *theTaskData_p)
{
    uint16_t ret;
    uint32_t e;
    const char *const Test00CCCC_SignOn = "...\nTest00CCCC   trAPI_Evt_stf\n";
    printf(Test00CCCC_SignOn);
    e = 0x00C0U | (unsigned long)(CCCC_Ctr);
    if (++CCCC_Ctr >= 16) {
        CCCC_Ctr = 0;
    }
    ret = trAPI_Evt_stf(trTask_Tst02BBBB_ID, Flags_Ctr, e);
    Tst02_callResDump(ret, trTask_Tst02CCCC_ID, Flags_Ctr, trTask_Tst02CCCC_ID, Flags_Ctr, e);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void TrTst02_AAAA(void)
{
    trDump_finageIdx(trDump_id2Idx(trTask_Tst02AAAA_ID));
    trTask_Tst02AAAA(NULL);
//      trDump_doEvt( trTask_Tst02BBBB_ID );
}

void TrTst02_BBBB(void)
{
    trDump_finageIdx(trDump_id2Idx(trTask_Tst02BBBB_ID));
    trTask_Tst02BBBB(NULL);
//      trDump_doEvt( trTask_Tst02BBBB_ID );
}

void TrTst02_CCCC(void)
{
    trDump_finageIdx(trDump_id2Idx(trTask_Tst02CCCC_ID));
    trTask_Tst02CCCC(NULL);
//      trDump_doEvt( trTask_Tst02BBBB_ID );
}

uint16_t Tst02_Run;

void TrTst02_actual(void)
{
    const char *const Tst02_RunNComp = "---\nTrTst02_actual  Event\n";
    printf(Tst02_RunNComp);

    TrTst02_AAAA();
    ++Flags_Ctr;
    TrTst02_CCCC();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;

    TrTst02_CCCC();
    ++Flags_Ctr;
    TrTst02_CCCC();
    ++Flags_Ctr;
    TrTst02_AAAA();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;

    TrTst02_AAAA();
    ++Flags_Ctr;
    TrTst02_AAAA();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;

    TrTst02_CCCC();
    ++Flags_Ctr;
    TrTst02_AAAA();
    ++Flags_Ctr;
    TrTst02_AAAA();
    ++Flags_Ctr;
    TrTst02_AAAA();
    ++Flags_Ctr;
    TrTst02_CCCC();
    ++Flags_Ctr;

    TrTst02_BBBB();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;
    TrTst02_BBBB();
    ++Flags_Ctr;

    TrTst02_BBBB();
    ++Flags_Ctr;

    const char *const Tst02_StopGoddammit = "---\nTrTst02_actual  Event  Stop, goddammit!\n===\n";
    printf(Tst02_StopGoddammit);

    trDump_doSR();

}

void TrTst02_init(void)
{

    Flags_Ctr = 0;
    AAAA_Ctr = CCCC_Ctr = 0;
    Tst02_Run = 0;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

volatile uint16_t BlinkDelay;
volatile bool ChgBlinkDelay;

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

void trTask_trTst02_init(void *theTaskData_p)
{
    while (1) {
        trAPI_Task_delay(trAPI_TaskID_self, 0xFFFFU);
    }
}

#define trTask_trTst02_init_ID 255

#define trTask_trTst02_action_ID 256
void trTask_trTst02_action(void *theTaskData_p)
{
    while (1) {
        switch (BlinkDelay) {
        case 1000:
            TrTst02_init();
            trDump_finageIdx(trDump_id2Idx(trTask_trTst02_action_ID));
            TrTst02_actual();
            trDump_finageIdx(trDump_id2Idx(trTask_trTst02_action_ID));
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
