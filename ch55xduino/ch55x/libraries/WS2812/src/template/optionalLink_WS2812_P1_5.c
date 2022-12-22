//This file is generated by a script. 
/*
 * Copyright (c) 2020 by Deqing Sun <ds@thinkcreate.us> (c version for CH55x port)
 *               2022 by Stephen Erisman <github@serisman.com> (major rewrite)
 * WS2812 library for arduino CH55x.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "WS2812.h"
#include "WS2812_DELAY.h"

/*
 * F_CPU (MHz):                        @56   @24   @16   @12    @8
 *                                    ----- ----- ----- ----- -----
 * Instruction cycle time (nS):       17.86 41.66  62.5 83.33  125
 * Timing requirements (# ins. cycles):
 *   full bit period (min): (1.25 uS)   70    30    20    15   10
 *   bit '0' high (max):   (500 nS)     28    12     8     6    4
 *   bit '1' high (min):   (625 nS)     35    15    10     8    5
 *   data low (max):       (5 uS)      280   120    80    60   40
 *   reset low (min):      (6 uS, 250 uS for some newer chips)
 * Actual ins. cycles used: (assuming only 4 cycles needed for branching)
 *   DELAY (# loop iterations):         15     5     2     1    0
 *   full bit period: 13 + (DELAY*4)    73    33    21    17   13
 *   bit '0' high:    4                  4     4     4     4    4
 *   bit '0' low:     9 + (DELAY*4)     69    29    17    13    9
 *   bit '1' high:    6 + (DELAY*4)     66    26    14    10    6
 *   bit '1' low:     7                  7     7     7     7    7
 * Sources:
 *   https://cpldcpu.wordpress.com/2014/01/14/light_ws2812-library-v2-0-part-i-understanding-the-ws2812/
 *   https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
 */

void neopixel_show_long_P1_5(uint32_t dataAndLen) {
  //'dpl' (LSB),'dph','b' & 'acc'
  //DPTR is the array address, B is the low byte of length
#if F_CPU >= 8000000
  __asm__ ("    mov r3, b                           \n"

           ";save EA to R6                          \n"
           "    mov c,_EA                           \n"
           "    clr a                               \n"
           "    rlc a                               \n"
           "    mov r6, a                           \n"
           ";disable interrupt                      \n"
           "    clr _EA                             \n"

           "byteLoop$:                              \n" // [bytes, cycles]
           "    movx  a,@dptr                       \n" // [1,1]
           "    inc dptr                            \n" // [1,1]
           "    mov r2,#8                           \n" // [2,2]
           "bitLoop$:                               \n"
           "    rlc a                               \n" // [1,1]
           "    setb _P1_5                          \n" // [2,2]
           "    jc bit7High$                        \n" // [2,2/4|5]
           "    clr _P1_5                           \n" // [2,2]
           "bit7High$:                              \n"
#if WS2812_DELAY > 0
           "    mov r1,#"_STR(WS2812_DELAY)"        \n" // [2,2]  delay a bit
           "bitDelay$:                              \n"
           "    djnz r1,bitDelay$                   \n" // [2,2/4|5|6]
#endif
           "    clr _P1_5                           \n" // [2,2]
           "    djnz r2,bitLoop$                    \n" // [2,2/4|5|6]
           "    djnz r3,byteLoop$                   \n" // [2,2/4|5|6]

           ";restore EA from R6                     \n"
           "    mov a,r6                            \n"
           "    jz  skipRestoreEA_NP$               \n"
           "    setb  _EA                           \n"
           "skipRestoreEA_NP$:                      \n"
          );
          (void)dataAndLen;
#else
  #error WS2812 Requires at least 8MHz F_CPU clock
#endif
}
