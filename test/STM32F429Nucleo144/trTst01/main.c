/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Tr Test 01 : Stream Tests w/o Tasking
*
* The idea is to white-box test Tr internals,
* specifically Stream,
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

void Tst01_callResDump(uint16_t Ret, uint16_t Tar, uint16_t L, uint16_t U, uint16_t A, uint16_t O)
{
    const char *const Tst01_TarLUAO =
        "RetVal: %0#5hx  Target: %0#5hx  Lower: %0#5hx  Upper: %0#5hx  Actual: %0#5hx  Avail: %0#5hx\n";
    printf(Tst01_TarLUAO, Ret, Tar, L, U, A, O);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// AAAA produces none, 2, 3, 4 bytes for BBBB
// BBBB consumes none,    3, 4, 5 bytes; stream is 24 bytes.
// CCCC produces none,       4, 5, 6, 7, 8 bytes for BBBB. 

// run 0
// AAAA 4 -> |  4 avail AO*4
// BBBB <- 4 |  0 avail {A0*}
// CCCC 8 -> |  8 avail C0*8
// run 1
// AAAA 4 -> | 12 avail A1*4, C0*8
// BBBB <- 5 |  7 avail A1*4, C0*3 {C0*5}
// CCCC 8 -> | 15 avail C1*8, A1*4, C0*3
// run 2
// AAAA 4 -> | 19 avail A2*4, C1*8, A1*4, C0*3
// BBBB <- 5 | 14 avail A2*4, C1*8, A1*2 {A1*2, C0*3}
// CCCC 8 -> | 22 avail C2*8, A2*4, C1*8, A1*2
// run 3
// AAAA 2 -> | 24 avail A3*2, C2*8, A2*4, C1*8, A1*2
// BBBB <- 5 | 19 avail A3*2, C2*8, A2*4, C1*5 {C1*3, A1*2}
// CCCC 5 -> | 24 avail C3*5, A3*2, C2*8, A2*4, C1*5
// run 4
// AAAA 0 -> | 24 avail C3*5, A3*2, C2*8, A2*4, C1*5
// BBBB <- 5 | 19 avail C3*5, A3*2, C2*8, A2*4 {C1*5}
// CCCC 5 -> | 24 avail C4*5, C3*5, A3*2, C2*8, A2*4
// run 5
// AAAA doesn't run
// BBBB <- 5 | 19 avail C4*5, C3*5, A3*2, C2*7 {C2*1, A2*4}
// CCCC doesn't run
// run 6
// AAAA doesn't run
// BBBB <- 5 | 14 avail C4*5, C3*5, A3*2, C2*2 {C2*5}
// CCCC doesn't run
// run 7
// AAAA doesn't run
// BBBB <- 5 |  9 avail C4*5, C3*4, {C3*1, A3*2, C2*2}
// CCCC doesn't run
// run 8
// AAAA doesn't run
// BBBB <- 5 |  4 avail C4*4 {C4*1, C3*4}
// CCCC doesn't run
// run 9
// AAAA doesn't run
// BBBB <- 4 |  0 avail {C4*4}
// CCCC doesn't run
// done

#define trTask_Tst01AAAA_ID 0xAAAA

#define trTask_Tst01BBBB_ID 0xBBBB
#define trTask_Tst01BBBB_StrmLen 24

#define trTask_Tst01CCCC_ID 0xCCCC

uint8_t AAAA_Ctr;

void trTask_Tst01AAAA(void *theTaskData_p)
{
    uint8_t buf[4];
    uint16_t i;
    uint16_t act;
    uint16_t avail;
    uint16_t ret;
    const char *const Tst01AAAA_SignOn = "...\nTst01AAAA   trAPI_Strm_put; 2 & 4\n";
    printf(Tst01AAAA_SignOn);
    for (i = 0; i < 4; i++) {
        buf[i] = 0xA0 | AAAA_Ctr;
    }
    if (++AAAA_Ctr >= 16) {
        AAAA_Ctr = 0;
    }
    ret = trAPI_Strm_put(trTask_Tst01BBBB_ID, 2, 4, &buf[0], &act, &avail);
    Tst01_callResDump(ret, trTask_Tst01BBBB_ID, 2, 4, act, avail);
    trDump_byteDump(buf, act);
}

void trTask_Tst01BBBB(void *theTaskData_p)
{
    uint8_t buf[5];
    uint16_t act;
    uint16_t avail;
    uint16_t ret;
    const char *const Tst01BBBB_SignOn = "...\nTst01BBBB   trAPI_Strm_get; 3 & 5\n";
    printf(Tst01BBBB_SignOn);
    ret = trAPI_Strm_get(3, 5, &buf[0], &act, &avail);

    Tst01_callResDump(ret, trTask_Tst01BBBB_ID, 3, 5, act, avail);
    trDump_byteDump(buf, act);
}

uint8_t CCCC_Ctr;

void trTask_Tst01CCCC(void *theTaskData_p)
{
    uint8_t buf[8];
    uint16_t i;
    uint16_t act;
    uint16_t avail;
    uint16_t ret;
    const char *const Tst01CCCC_SignOn = "...\nTst01CCCC   trAPI_Strm_put; 4 & 8\n";
    printf(Tst01CCCC_SignOn);
    for (i = 0; i < 8; i++) {
        buf[i] = 0xC0 | CCCC_Ctr;
    }
    if (++CCCC_Ctr >= 16) {
        CCCC_Ctr = 0;
    }
    ret = trAPI_Strm_put(trTask_Tst01BBBB_ID, 4, 8, &buf[0], &act, &avail);
    Tst01_callResDump(ret, trTask_Tst01BBBB_ID, 4, 8, act, avail);
    trDump_byteDump(buf, act);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

uint16_t Tst01_Run;

void trTst01_actual(void)
{
    if (Tst01_Run < 10) {
        const char *const Tst01_RunNComp = "---\ntrTst01_actual  Stream  Run %0#5hx .\n";
        printf(Tst01_RunNComp, Tst01_Run);

        if (Tst01_Run < 5) {
            trDump_finageIdx(trDump_id2Idx(trTask_Tst01AAAA_ID));
            trTask_Tst01AAAA(NULL);
            trDump_doStrm(trTask_Tst01BBBB_ID);
            trDump_finageIdx(trDump_id2Idx(trTask_Tst01BBBB_ID));
            trTask_Tst01BBBB(NULL);
            trDump_doStrm(trTask_Tst01BBBB_ID);
            trDump_finageIdx(trDump_id2Idx(trTask_Tst01CCCC_ID));
            trTask_Tst01CCCC(NULL);
            trDump_doStrm(trTask_Tst01BBBB_ID);
        } else {

            trDump_finageIdx(trDump_id2Idx(trTask_Tst01BBBB_ID));
            trTask_Tst01BBBB(NULL);
            trDump_doStrm(trTask_Tst01BBBB_ID);

        }

    } else {
        const char *const Tst01_StopGoddammit = "---\ntrTst01_actual  Stream  Stop, goddammit!\n===\n";
        printf(Tst01_StopGoddammit);

        trDump_doSR();

    }

    ++Tst01_Run;
}

void trTst01_init(void)
{

    AAAA_Ctr = CCCC_Ctr = 0;
    Tst01_Run = 0;

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

void trTask_trTst01_init(void *theTaskData_p)
{
    while (1) {
        trAPI_Task_delay(trAPI_TaskID_self, 0xFFFFU);
    }
}

#define trTask_trTst01_init_ID 255

#define trTask_trTst01_action_ID 256
void trTask_trTst01_action(void *theTaskData_p)
{
    while (1) {
        switch (BlinkDelay) {
        case 1000:
            trTst01_init();
            trDump_finageIdx(trDump_id2Idx(trTask_trTst01_action_ID));
            while (Tst01_Run <= 10) {
                trTst01_actual();
                trDump_finageIdx(trDump_id2Idx(trTask_trTst01_action_ID));
                BSP_LED_Toggle(LED2);
            }
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

    BSP_LED_On(LED1);
}

int /* picky compilers */ main(void)
{
    main_init();

    trAPI_start();

    return 0;
}
