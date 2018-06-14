/** \file
*   \brief ThingRendevous's Stream Private interface
*
* Private types known only to Stream and trTi code generator Trtgen.
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

#ifndef trStrmP_h
#define trStrmP_h

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "trPl.h"

/** Stream Data Block; really just an array of bytes.
*
* - Located in .bss.
* - An array of these forms a task's event queue.
* - Trtgen names this trTi_Strm_DB_SomeName_ID_Idx[ WhateverTheLengthIs ].
*/

/** Stream Control Block
*
* - Located in .bss.
* - one exists for every task that has a steam
* - Trtgen names this trTi_Strm_CB_SomeName_ID_Idx
*/
struct trStrmP_CB_s {
    uint16_t cb_len_used;      /**< Bytes in the stream at this moment. */
    uint16_t cb_offset_read;      /**< Zero-based offset in the stream where data will be read from. */
    uint16_t cb_offset_writ;      /**< Zero-based offset in the stream where data will be written to. */
};

/** Stream Config Info
*
* - Located in .txt
* - one exists for every task that has a stream.
* - Trtgen names this trTi_Strm_CI_SomeName_ID_Idx.
*/
struct trStrmP_CI_s {
    const uint16_t ci_StrmLen;      /**< Number of number possible. */
    struct trStrmP_CB_s *const ci_cb_p;       /**< Points to the stream control block in RAM. */
    uint8_t *const ci_db_p;       /**< Points to the stream byte array in RAM. */
};

bool trStrmP_getp(uint16_t forTask_idx, struct trStrmP_CI_s **ci_pp, struct trStrmP_CB_s **cb_pp, uint8_t ** db_pp);

/** Stream private Get Info
*
* Expected by the private Get Private Section handler
*/
struct trStrmP_GetInfo_s {
    uint16_t ret_val;  /**< Return value. */
    struct trStrmP_CI_s *ci_p;
    struct trStrmP_CB_s *cb_p;
    uint8_t *db_p;
};

#define trStrmP_GetInfo_p() ((struct trStrmP_GetInfo_s *)getInfo_s_p)

void trStrmP_get_PrivSect_hdlr(void *aKIDS_p, void *getInfo_s_p);

/** Stream private Xover Get Info
*
* Expected by the private Xover Get Private Section handler
*/
struct trStrmP_XoverGetInfo_s {
    bool ret_val;  /**< Not quite the same return value. */
    struct trStrmP_CI_s *ci_p;
    struct trStrmP_CB_s *cb_p;
    uint8_t *db_p;
    uint16_t target_task_idx;
};

#define trStrmP_XoverGetInfo_p() ((struct trStrmP_XoverGetInfo_s *)xoverGetInfo_s_p)

void trStrmP_xoverGet_PrivSect_hdlr(void *aKIDS_p, void *xoverGetInfo_s_p);

/** Strm private Put Info
*
* Expected by the private Put Private Section handler
*/
struct trStrmP_PutInfo_s {
    uint16_t ret_val;  /**< Return value. */
    struct trStrmP_CI_s *ci_p;
    struct trStrmP_CB_s *cb_p;
    uint8_t *db_p;
};

#define trStrmP_PutInfo_p() ((struct trStrmP_PutInfo_s *)putInfo_s_p)

void trStrmP_put_PrivSect_hdlr(void *aKIDS_p, void *putInfo_s_p);

#endif
