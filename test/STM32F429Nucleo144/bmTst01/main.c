/** \file
*   \brief ThingRendevous's STM32F429Nucleo144 Bare Machine Test 01
*
* Test the virtual comm port.
* Blink the LED using the system clock tick handler.
* 
* This is the initial piece of code you should attempt to run.
* It helps you test your development system set up for the FRDM board.
* Everything from Eclipse, uP inits, simple IO usage, thru the download scheme.
*
* You'll use ext/STM32F429Nucleo144/Goodies for you F429 library.
*
* Just set up an app using this as the only file.  Build and run.
*
* This uses the CMSIS, ThingRendevous libraries, and directory system.
* It does not use ThingRendevous itself.
*
* It is a mash-up of 
* Projects/STM32F429ZI-Nucleo/Demonstration code.
* and
* Projects/STM32F429ZI-Nucleo/Examples/UART/UART_Printf/
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

volatile uint8_t BlinkSpeed;
volatile bool ChgdBlinkSpeed;

void __attribute__ ((interrupt)) SysTick_Handler(void)
{
    HAL_IncTick();
}

void __attribute__ ((interrupt)) EXTI15_10_IRQHandler(void)
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

int /* picky compilers */ main(void)
{
    main_init();

    while (1) {
        LED2_Blinky();
        Printfy();
    }

    return 0;
}
