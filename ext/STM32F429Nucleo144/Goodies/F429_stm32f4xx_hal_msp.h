/** \file
*   \brief STM32F429ZI Nucleo 144 Polled virtual terminal interface; implementation in stm32f4xx_hal_msp
*
* Implements a polled virtual comm port on the Nucleo 144 board.
*
* It is a subset of the Projects/STM32F429ZI-Nucleo/Examples/UART/UART_Printf/ code.
*
* The implementation is in stm32f4xx_hal_msp,
* necessary because the use of "weak" typing
* means we have to force the linker to pull-in stm32f4xx_hal_msp.
*
* \author Alfred O Fingulin FinguPublic@gmail.com
*/
/*
* MIT License
* 
* Copyright (c) 2018 Alfred O Fingulin  FinguPublic@gmail.com
* 
* Permission is granted, free of charge, to any person obtaining a copy
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

#ifndef F429_stm32f4xx_hal_msp_h
#define F429_stm32f4xx_hal_msp_h

void F429_stm32f4xx_hal_msp_init(void);

#endif
