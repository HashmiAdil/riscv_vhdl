/*
 *  Copyright 2021 Sergey Khabarov, sergeykhbr@gmail.com
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include <inttypes.h>
#include <string.h>
#include <axi_maps.h>
#include "fw_api.h"

static const char TEST_MTIMER_NAME[8] = "mtimer";

void test_mtimer(void) {
    pnp_map *pnp = (pnp_map *)ADDR_BUS0_XSLV_PNP;
    clint_map *clint = (clint_map *)ADDR_BUS0_XSLV_CLINT;
    uint64_t mtie = 0x1ull << 7;

    asm("csrc mie, %0" : :"r"(mtie));         // MTIE=0: disable mtimer interrupt

    clint->mtime = 0ull;
    uint64_t t1 = clint->mtime;
    uint64_t t2 = clint->mtime;
    if (t2 <= t1) {
        printf_uart("FAIL: mtimer not clocking: %d, %d\r\n", t1, t2);
        return;
    }

    fw_enable_m_interrupts();

    pnp->fwdbg1 = 0;
    clint->mtimecmp[fw_get_cpuid()] = clint->mtime + 3000;

    asm("csrs mie, %0" : :"r"(mtie));         // MTIE=1: enable mtimer interrupt

    while (pnp->fwdbg1 == 0) {}               // should be update int irq handler

    asm("csrc mie, %0" : :"r"(mtie));         // MTIE=0: disable mtimer interrupt

    printf_uart("MTIMER . . . . .%s", "PASS\r\n");
}

