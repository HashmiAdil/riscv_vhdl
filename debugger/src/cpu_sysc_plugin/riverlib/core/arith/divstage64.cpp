/*
 *  Copyright 2019 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "divstage64.h"

namespace debugger {

divstage64::divstage64(sc_module_name name_) : sc_module(name_),
    i_divident("i_divident"),
    i_divisor("i_divisor"),
    o_resid("o_resid"),
    o_bits("o_bits") {

    SC_METHOD(comb);
    sensitive << i_divident;
    sensitive << i_divisor;
};

void divstage64::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    if (o_vcd) {
        sc_trace(o_vcd, o_bits, o_bits.name());
        sc_trace(o_vcd, o_resid, o_resid.name());
        sc_trace(o_vcd, i_divident, i_divident.name());
        sc_trace(o_vcd, i_divisor, i_divisor.name());

        std::string pn(name());
        sc_trace(o_vcd, wb_divx1, pn + ".wb_divx1");
        sc_trace(o_vcd, wb_divx2, pn + ".wb_divx2");
        sc_trace(o_vcd, wb_divx3, pn + ".wb_divx3");
        sc_trace(o_vcd, wb_divx4, pn + ".wb_divx4");
        sc_trace(o_vcd, wb_divx5, pn + ".wb_divx5");
        sc_trace(o_vcd, wb_divx6, pn + ".wb_divx6");
        sc_trace(o_vcd, wb_divx7, pn + ".wb_divx7");
        sc_trace(o_vcd, wb_divx8, pn + ".wb_divx8");
        sc_trace(o_vcd, wb_divx9, pn + ".wb_divx9");
        sc_trace(o_vcd, wb_divx10, pn + ".wb_divx10");
        sc_trace(o_vcd, wb_divx11, pn + ".wb_divx11");
        sc_trace(o_vcd, wb_divx12, pn + ".wb_divx12");
        sc_trace(o_vcd, wb_divx13, pn + ".wb_divx13");
        sc_trace(o_vcd, wb_divx14, pn + ".wb_divx14");
        sc_trace(o_vcd, wb_divx15, pn + ".wb_divx15");
        sc_trace(o_vcd, wb_thresh[7], pn + ".wb_thresh7");
        sc_trace(o_vcd, wb_thresh[6], pn + ".wb_thresh6");
        sc_trace(o_vcd, wb_thresh[5], pn + ".wb_thresh5");
        sc_trace(o_vcd, wb_thresh[4], pn + ".wb_thresh4");
        sc_trace(o_vcd, wb_dif[3], pn + ".wb_dif3");
    }
}

void divstage64::comb() {
    sc_uint<4> wb_bits;

    wb_divident = i_divident.read().to_uint64();
    wb_divisor = i_divisor.read();

    wb_divx1(63, 0) = wb_divisor(63, 0);
    wb_divx1[64] = wb_divisor(123, 64).or_reduce();

    wb_divx2[0] = 0;
    wb_divx2(63, 1) = wb_divisor(62, 0);
    wb_divx2[64] = wb_divisor(123, 63).or_reduce();

    wb_divx3(64, 0) = (0, wb_divx2(63, 0)) + (0, wb_divx1(63, 0));
    wb_divx3[64] = wb_divx3[64] || wb_divx2[64];

    wb_divx4(1, 0) = 0;
    wb_divx4(63, 2) = wb_divisor(61, 0);
    wb_divx4[64] = wb_divisor(123, 62).or_reduce();

    wb_divx5(64, 0) = (0, wb_divx4(63, 0)) + (0, wb_divx1(63, 0));
    wb_divx5[64] = wb_divx5[64] || wb_divx4[64];

    wb_divx6[0] = 0;
    wb_divx6(63, 1) = wb_divx3(62, 0);
    wb_divx6[64] = wb_divx3[64] || wb_divx3[63];

    wb_divx8(2, 0) = 0;
    wb_divx8(63, 3) = wb_divisor(60, 0);
    wb_divx8[64] = wb_divisor(123, 61).or_reduce();

    // 7 = 8 - 1
    wb_divx7(64, 0) = wb_divx8(64, 0) - (0, wb_divx1(63, 0));
    wb_divx7[64] = wb_divx7[64] || wb_divisor(123, 62).or_reduce();

    // 9 = 8 + 1
    wb_divx9(64, 0) = (0, wb_divx8(63, 0)) + (0, wb_divx1(63, 0));
    wb_divx9[64] = wb_divx9[64] || wb_divisor(123, 61).or_reduce();

    // 10 = 8 + 2
    wb_divx10(64, 0) = (0, wb_divx8(63, 0)) + (0, wb_divx2(63, 0));
    wb_divx10[64] = wb_divx10[64] || wb_divisor(123, 61).or_reduce();

    // 11 = 8 + 3
    wb_divx11(64, 0) = (0, wb_divx8(63, 0)) + (0, wb_divx3(63, 0));
    wb_divx11[64] = wb_divx10[64] || wb_divisor(123, 61).or_reduce();

    // 12 = 3 << 2
    wb_divx12(1, 0) = 0;
    wb_divx12(63, 2) = wb_divx3(61, 0);
    wb_divx12[64] = wb_divx3[64] || wb_divx3[63] || wb_divx3[62];

    // 16 = divisor << 4
    wb_divx16(3, 0) = 0;
    wb_divx16(63, 4) = wb_divisor(59, 0);
    wb_divx16[64] = wb_divisor(123, 60).or_reduce();

    // 13 = 16 - 3
    wb_divx13(64, 0) = wb_divx16(64, 0) - (0, wb_divx3(63, 0));
    wb_divx13[64] = wb_divx13[64] || wb_divisor(123, 61).or_reduce();

    // 14 = 7 << 1
    wb_divx14[0] = 0;
    wb_divx14(63, 1) = wb_divx7(62, 0);
    wb_divx14[64] = wb_divx7[64] || wb_divx7[63];

    // 15 = 16 - 1
    wb_divx15(64, 0) = wb_divx16(64, 0) - (0, wb_divx1(63, 0));
    wb_divx15[64] = wb_divx15[64] || wb_divisor(123, 61).or_reduce();

    wb_thresh[15] = (0, wb_divident) - (0, wb_divx15);
    wb_thresh[14] = (0, wb_divident) - (0, wb_divx14);
    wb_thresh[13] = (0, wb_divident) - (0, wb_divx13);
    wb_thresh[12] = (0, wb_divident) - (0, wb_divx12);
    wb_thresh[11] = (0, wb_divident) - (0, wb_divx11);
    wb_thresh[10] = (0, wb_divident) - (0, wb_divx10);
    wb_thresh[9] = (0, wb_divident) - (0, wb_divx9);
    wb_thresh[8] = (0, wb_divident) - (0, wb_divx8);
    wb_thresh[7] = (0, wb_divident) - (0, wb_divx7);
    wb_thresh[6] = (0, wb_divident) - (0, wb_divx6);
    wb_thresh[5] = (0, wb_divident) - (0, wb_divx5);
    wb_thresh[4] = (0, wb_divident) - (0, wb_divx4);
    wb_thresh[3] = (0, wb_divident) - (0, wb_divx3);
    wb_thresh[2] = (0, wb_divident) - (0, wb_divx2);
    wb_thresh[1] = (0, wb_divident) - (0, wb_divx1);
    wb_thresh[0] = (0, wb_divident);

    if (wb_thresh[15][65] == 0) {
        wb_bits = 15;
        wb_dif = wb_thresh[15].range(63, 0).to_uint64();
    } else if (wb_thresh[14][65] == 0) {
        wb_bits = 14;
        wb_dif = wb_thresh[14].range(63, 0).to_uint64();
    } else if (wb_thresh[13][65] == 0) {
        wb_bits = 13;
        wb_dif = wb_thresh[13].range(63, 0).to_uint64();
    } else if (wb_thresh[12][65] == 0) {
        wb_bits = 12;
        wb_dif = wb_thresh[12].range(63, 0).to_uint64();
    } else if (wb_thresh[11][65] == 0) {
        wb_bits = 11;
        wb_dif = wb_thresh[11].range(63, 0).to_uint64();
    } else if (wb_thresh[10][65] == 0) {
        wb_bits = 10;
        wb_dif = wb_thresh[10].range(63, 0).to_uint64();
    } else if (wb_thresh[9][65] == 0) {
        wb_bits = 9;
        wb_dif = wb_thresh[9].range(63, 0).to_uint64();
    } else if (wb_thresh[8][65] == 0) {
        wb_bits = 8;
        wb_dif = wb_thresh[8].range(63, 0).to_uint64();
    } else if (wb_thresh[7][65] == 0) {
        wb_bits = 7;
        wb_dif = wb_thresh[7].range(63, 0).to_uint64();
    } else if (wb_thresh[6][65] == 0) {
        wb_bits = 6;
        wb_dif = wb_thresh[6].range(63, 0).to_uint64();
    } else if (wb_thresh[5][65] == 0) {
        wb_bits = 5;
        wb_dif = wb_thresh[5].range(63, 0).to_uint64();
    } else if (wb_thresh[4][65] == 0) {
        wb_bits = 4;
        wb_dif = wb_thresh[4].range(63, 0).to_uint64();
    } else if (wb_thresh[3][65] == 0) {
        wb_bits = 3;
        wb_dif = wb_thresh[3].range(63, 0).to_uint64();
    } else if (wb_thresh[2][65] == 0) {
        wb_bits = 2;
        wb_dif = wb_thresh[2].range(63, 0).to_uint64();
    } else if (wb_thresh[1][65] == 0) {
        wb_bits = 1;
        wb_dif = wb_thresh[1].range(63, 0).to_uint64();
    } else {
        wb_bits = 0;
        wb_dif = wb_thresh[0].range(63, 0).to_uint64();
    }

    o_bits = wb_bits;
    o_resid = wb_dif;
}

}  // namespace debugger

