/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Tr Test 03 : Message Tests w/o Tasking
*
* The idea is to white-box test Tr internals,
* specifically Messages,
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

void Tst03_callResDump(uint16_t ret, uint16_t id, uint16_t fc, uint16_t s, uint16_t f, uint8_t * b, uint16_t bl)
{
    const char *const Test03_Rifcsf = "RetVal: %0#5hx  Me: %0#5hx  FlgCtr: %0#5hx  Source: %0#5hx  Flags: %0#5hx\n";
    printf(Test03_Rifcsf, ret, id, fc, s, f);
    trDump_byteDump(b, 8);
    trDump_stupidNl();
    if (bl > 8) {
        trDump_byteDump(&b[8], bl - 8);
        trDump_stupidNl();
    }
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

// AAAA produces messages, highest priority
// BBBB consumes messages, priority order 4 deep 8 bytes; mid priority, produces longer messages
// CCCC produces messages, lower priority
// DDDD consumes messages, fifo order 3 deep 16 bytes; lowest priority

// flags : messages
// 
//  0 : AAAA -> BBBB  |  A0
//  1 : CCCC -> BBBB  |  C0, A0
//  2 : BBBB <-       |  C0 {A0}
//  2 : BBBB -> DDDD  |  [A0 Bx]
//
//  3 : CCCC -> BBBB  |  C1, C0
//  4 : CCCC -> BBBB  |  C2, C1, C0
//  5 : AAAA -> BBBB  |  C2, C1, C0, A1
//  6 : AAAA -> BBBB  |  full A2 lost, C2, C1, C0, A1

//  7 : BBBB <-       |  C2, C1, C0 {A1}
//  7 : BBBB -> DDDD  |  [A1 Bx], [A0 Bx]
//  8 : BBBB <-       |  C2, C1 {C0}
//  8 : BBBB -> DDDD  |  [C0 Bx], [A1 Bx], [A0 Bx]
//  9 : BBBB <-       |  C2 {C1}
//  9 : BBBB -> DDDD  |  full [C1 Bx] lost, [C0 Bx], [A1 Bx], [A0 Bx]
//
// 10 : DDDD <-       |  [C0 Bx], [A1 Bx] {[A0 Bx]}
// 11 : DDDD <-       |  [C0 Bx] {[A1 Bx]}
// 12 : DDDD <-       |  {[C0 Bx]}
// 13 : DDDD <-       |  empty
//
// Note C2 is never extracted from BBBB's messages.

uint16_t Flags_Ctr;

#define trTask_Test02AAAA_ID 0xAAAA
#define trTask_Test02AAAA_NominalPriority 0x0101

#define trTask_Test02BBBB_ID 0xBBBB
#define trTask_Test02BBBB_NominalPriority 0x0102
#define trTask_Test02BBBB_MsgLen 4
#define trTask_Test02BBBB_MsgPayload 8
#define trTask_Test02BBBB_MsgOrdering 1

#define trTask_Test02CCCC_ID 0xCCCC
#define trTask_Test02CCCC_NominalPriority 0x0103

#define trTask_Test02DDDD_ID 0xDDDD
#define trTask_Test02DDDD_NominalPriority 0x0104
#define trTask_Test02DDDD_MsgLen 3
#define trTask_Test02DDDD_MsgPayload 16

uint8_t AAAA_Ctr;

void trTask_Test02AAAA(void *theTaskData_p)
{
    uint16_t ret;
    uint16_t i;
    uint8_t bf[8];

    const char *const Test00AAAA_SignOn = "...\nTest00AAAA   trAPI_Msg_giv\n";
    printf(Test00AAAA_SignOn);

    for (i = 0; i < 8; i++) {
        bf[i] = 0xA0 | AAAA_Ctr;
    }
    ret = trAPI_Msg_giv(trTask_Test02BBBB_ID, Flags_Ctr, &bf[0]);

    if (++AAAA_Ctr >= 16) {
        AAAA_Ctr = 0;
    }

    Tst03_callResDump(ret, trTask_Test02AAAA_ID, Flags_Ctr, trTask_Test02BBBB_ID, Flags_Ctr, &bf[0], 8);
}

uint8_t BBBB_Ctr;

void trTask_Test02BBBB(void *theTaskData_p)
{
    uint16_t ret;
    uint16_t i;
    uint8_t bf[16];
    uint16_t s;
    uint16_t f;

    const char *const Test00BBBB_SignOn = "...\nTest00BBBB   trAPI_Msg_tak & giv\n";
    printf(Test00BBBB_SignOn);

    ret = trAPI_Msg_tak(&s, &f, &bf[0]);
    Tst03_callResDump(ret, trTask_Test02BBBB_ID, Flags_Ctr, s, f, &bf[0], 8);

    for (i = 8; i < 16; i++) {
        bf[i] = 0xB0 | BBBB_Ctr;
    }

    if (++BBBB_Ctr >= 16) {
        BBBB_Ctr = 0;
    }

    ret = trAPI_Msg_giv(trTask_Test02DDDD_ID, Flags_Ctr, &bf[0]);
    Tst03_callResDump(ret, trTask_Test02BBBB_ID, Flags_Ctr, trTask_Test02DDDD_ID, Flags_Ctr, &bf[0], 16);
}

uint8_t CCCC_Ctr;

void trTask_Test02CCCC(void *theTaskData_p)
{
    uint16_t ret;
    uint8_t bf[8];
    uint16_t i;

    const char *const Test00CCCC_SignOn = "...\nTest00CCCC   trAPI_Msg_giv\n";
    printf(Test00CCCC_SignOn);

    for (i = 0; i < 8; i++) {
        bf[i] = 0xC0 | CCCC_Ctr;
    }
    ret = trAPI_Msg_giv(trTask_Test02BBBB_ID, Flags_Ctr, &bf[0]);

    if (++CCCC_Ctr >= 16) {
        CCCC_Ctr = 0;
    }

    Tst03_callResDump(ret, trTask_Test02CCCC_ID, Flags_Ctr, trTask_Test02BBBB_ID, Flags_Ctr, &bf[0], 8);
}

void trTask_Test02DDDD(void *theTaskData_p)
{
    uint16_t ret;
    uint8_t bf[16];
    uint16_t s;
    uint16_t f;

    const char *const Test00DDDD_SignOn = "...\nTest00DDDD   trAPI_Msg_tak\n";
    printf(Test00DDDD_SignOn);

    ret = trAPI_Msg_tak(&s, &f, &bf[0]);
    Tst03_callResDump(ret, trTask_Test02DDDD_ID, Flags_Ctr, s, f, &bf[0], 16);

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void TrTest03_AAAA(void)
{
    trDump_finageIdx(trDump_id2Idx(trTask_Test02AAAA_ID));
    trTask_Test02AAAA(NULL);
}

void TrTest03_BBBB(void)
{
    trDump_finageIdx(trDump_id2Idx(trTask_Test02BBBB_ID));
    trTask_Test02BBBB(NULL);
}

void TrTest03_CCCC(void)
{
    trDump_finageIdx(trDump_id2Idx(trTask_Test02CCCC_ID));
    trTask_Test02CCCC(NULL);
}

void TrTest03_DDDD(void)
{
    trDump_finageIdx(trDump_id2Idx(trTask_Test02DDDD_ID));
    trTask_Test02DDDD(NULL);
}

void TrTst03_actual(void)
{
    const char *const Test02_RunNComp = "---\nTrTest03_actual  Event\n";
    printf(Test02_RunNComp);

    TrTest03_AAAA();
    ++Flags_Ctr;
    TrTest03_CCCC();
    ++Flags_Ctr;
    TrTest03_BBBB();

    ++Flags_Ctr;
    TrTest03_CCCC();
    ++Flags_Ctr;
    TrTest03_CCCC();
    ++Flags_Ctr;
    TrTest03_AAAA();
    ++Flags_Ctr;
    TrTest03_AAAA();

    ++Flags_Ctr;
    TrTest03_BBBB();
    ++Flags_Ctr;
    TrTest03_BBBB();
    ++Flags_Ctr;
    TrTest03_BBBB();

    ++Flags_Ctr;
    TrTest03_DDDD();
    ++Flags_Ctr;
    TrTest03_DDDD();
    ++Flags_Ctr;
    TrTest03_DDDD();
    ++Flags_Ctr;
    TrTest03_DDDD();

    const char *const Test02_StopGoddammit = "---\nTrTest03_actual  Event  Stop, goddammit!\n===\n";
    printf(Test02_StopGoddammit);

    trDump_doSR();

}

void TrTst03_init(void)
{

    Flags_Ctr = 0;
    AAAA_Ctr = CCCC_Ctr = 0;

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void trTask_trTst03_init(void *theTaskData_p)
{
    while (1) {
        trAPI_Task_delay(trAPI_TaskID_self, 0xFFFFU);
    }
}

#define trTask_trTst03_init_ID 255

#define trTask_trTst03_action_ID 256
void trTask_trTst03_action(void *theTaskData_p)
{
    while (1) {
        switch (BlinkDelay) {
        case 1000:
            TrTst03_init();
            trDump_finageIdx(trDump_id2Idx(trTask_trTst03_action_ID));
            TrTst03_actual();
            trDump_finageIdx(trDump_id2Idx(trTask_trTst03_action_ID));
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

    BSP_LED_On(LED1);
}

int /* picky compilers */ main(void)
{
    main_init();

    trAPI_start();

    return 0;
}
