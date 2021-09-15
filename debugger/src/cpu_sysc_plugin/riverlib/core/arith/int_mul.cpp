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
 *
 * Implemented algorithm provides 4 clocks per instruction
 */

#include "int_mul.h"
#include "api_core.h"

namespace debugger {

IntMul::IntMul(sc_module_name name_, bool async_reset) : sc_module(name_),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_ena("i_ena"),
    i_unsigned("i_unsigned"),
    i_hsu("i_hsu"),
    i_high("i_high"),
    i_rv32("i_rv32"),
    i_a1("i_a1"),
    i_a2("i_a2"),
    o_res("o_res"),
    o_valid("o_valid") {
    async_reset_ = async_reset;

    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_ena;
    sensitive << i_unsigned;
    sensitive << i_rv32;
    sensitive << i_high;
    sensitive << i_hsu;
    sensitive << i_a1;
    sensitive << i_a2;
    sensitive << r.result;
    sensitive << r.ena;
    sensitive << r.busy;

    SC_METHOD(registers);
    sensitive << i_nrst;
    sensitive << i_clk.pos();
};

void IntMul::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
#if 0
    vcd = sc_create_vcd_trace_file("intmul");
    vcd->set_time_unit(1, SC_PS);

    sc_trace(vcd, i_clk, "i_clk");
    sc_trace(vcd, i_nrst, "i_nrst");
    sc_trace(vcd, i_ena, "i_ena");
    sc_trace(vcd, i_unsigned, "i_unsigned");
    sc_trace(vcd, i_rv32, "i_rv32");
    sc_trace(vcd, i_high, "i_high");
    sc_trace(vcd, i_a1, "i_a1");
    sc_trace(vcd, i_a2, "i_a2");
#endif
    if (o_vcd) {
        sc_trace(o_vcd, i_a1, i_a1.name());
        sc_trace(o_vcd, i_a2, i_a2.name());
        sc_trace(o_vcd, i_ena, i_ena.name());
        sc_trace(o_vcd, i_unsigned, i_unsigned.name());
        sc_trace(o_vcd, i_high, i_high.name());
        sc_trace(o_vcd, i_hsu, i_hsu.name());
        sc_trace(o_vcd, o_res, o_res.name());
        sc_trace(o_vcd, o_valid, o_valid.name());

        std::string pn(name());
        sc_trace(o_vcd, r.ena, pn + ".r_ena");
        sc_trace(o_vcd, r.result, pn + ".r_result");
        sc_trace(o_vcd, r.high, pn + ".r_high");
        sc_trace(o_vcd, r.zero, pn + ".r_zero");
        sc_trace(o_vcd, r.inv, pn + ".r_inv");
        sc_trace(o_vcd, r.a1, pn + ".r_a1");
        sc_trace(o_vcd, r.a2, pn + ".r_a2");
    }
}

void IntMul::comb() {
    sc_uint<2> wb_mux_lvl0;
    Level0Type wb_lvl0;
    Level2Type wb_lvl2;
    Level4Type wb_lvl4;
    sc_biguint<128> wb_lvl5;
    sc_biguint<128> wb_res32;
    sc_uint<64> wb_res;
    sc_uint<64> vb_a1s;
    sc_uint<64> vb_a2s;
    bool v_a1s_nzero;
    bool v_a2s_nzero;

    v = r;

    v_a1s_nzero = i_a1.read()(62, 0).or_reduce();
    if (v_a1s_nzero && i_a1.read()[63]) {
        vb_a1s = ~i_a1.read() + 1;
    } else {
        vb_a1s = i_a1;
    }

    v_a2s_nzero = i_a2.read()(62, 0).or_reduce();
    if (v_a2s_nzero && i_a2.read()[63]) {
        vb_a2s = ~i_a2.read() + 1;
    } else {
        vb_a2s = i_a2;
    }

    v.ena = (r.ena.read() << 1) | (i_ena & !r.busy);

    if (i_ena.read()) {
        v.busy = 1;
        v.inv = 0;
        v.zero = 0;
        if (i_rv32.read()) {
            v.a1 = i_a1.read()(31, 0);
            if (!i_unsigned.read() && i_a1.read()[31]) {
                v.a1(63, 32) = ~0;
            }
            v.a2 = i_a2.read()(31, 0);
            if (!i_unsigned.read() && i_a2.read()[31]) {
                v.a2(63, 32) = ~0;
            }
        } else if (i_high.read() == 1) {
            if (i_hsu.read() == 1) {
                v.zero = !v_a1s_nzero || !i_a2.read().or_reduce();
                v.inv = i_a1.read()[63];
                v.a1 = vb_a1s;
                v.a2 = i_a2;
            } else if (i_unsigned.read() == 1) {
                v.a1 = i_a1;
                v.a2 = i_a2;
            } else {
                v.zero = !v_a1s_nzero || !v_a2s_nzero;
                v.inv = i_a1.read()[63] ^ i_a2.read()[63];
                v.a1 = vb_a1s;
                v.a2 = vb_a2s;
            }
        } else {
            v.a1 = i_a1;
            v.a2 = i_a2;
        }
        v.rv32 = i_rv32;
        v.unsign = i_unsigned;
        v.high = i_high;

        // Just for run-rime control (not for VHDL)
        v.a1_dbg = i_a1;
        v.a2_dbg = i_a2;
        v.reference_mul = compute_reference(i_unsigned.read(),
                                            i_rv32.read(),
                                            i_a1.read(),
                                            i_a2.read());
    }

    if (r.ena.read()[0]) {
        for (int i = 0; i < 32; i++) {
            wb_mux_lvl0 = r.a2(2*i + 1, 2*i);
            if (wb_mux_lvl0 == 0) {
                wb_lvl0.arr[i] = 0;
            } else if (wb_mux_lvl0 == 1) {
                wb_lvl0.arr[i] = sc_biguint<66>(r.a1);
            } else if (wb_mux_lvl0 == 2) {
                wb_lvl0.arr[i] = sc_biguint<66>(r.a1) << 1;
            } else {
                wb_lvl0.arr[i] = sc_biguint<66>(r.a1)
                              + (sc_biguint<66>(r.a1) << 1);
            }
        }

        for (int i = 0; i < 16; i++) {
            v_lvl1.arr[i] = (sc_biguint<69>(wb_lvl0.arr[2*i + 1]) << 2)
                          + sc_biguint<69>(wb_lvl0.arr[2*i]);
        }
    }

    if (r.ena.read()[1]) {
        for (int i = 0; i < 8; i++) {
            wb_lvl2.arr[i] = (sc_biguint<74>(r_lvl1.arr[2*i + 1]) << 4)
                       + sc_biguint<74>(r_lvl1.arr[2*i]);
        }

        for (int i = 0; i < 4; i++) {
            v_lvl3.arr[i] = (sc_biguint<83>(wb_lvl2.arr[2*i + 1]) << 8)
                          + sc_biguint<83>(wb_lvl2.arr[2*i]);
        }
    }

    if (r.ena.read()[2]) {
        v.busy = 0;
        for (int i = 0; i < 2; i++) {
            wb_lvl4.arr[i] = (sc_biguint<100>(r_lvl3.arr[2*i + 1]) << 16)
                           + sc_biguint<100>(r_lvl3.arr[2*i]);
        }

        wb_lvl5 = (sc_biguint<128>(wb_lvl4.arr[1]) << 32) 
                        + sc_biguint<128>(wb_lvl4.arr[0]);
        if (r.rv32.read()) {
            wb_res32(31, 0) = wb_lvl5(31, 0);
            if (r.unsign.read() || wb_lvl5[31] == 0) {
                wb_res32(127, 32) = 0;
            } else {
                wb_res32(127, 32) = ~0;
            }
            v.result = wb_res32;
        } else if (r.high.read() == 1) {
            if (r.zero.read() == 1) {
                v.result = 0;
            } else if (r.inv.read() == 1) {
                v.result = ~wb_lvl5;
            } else {
                v.result = wb_lvl5;
            }
        } else {
            v.result = wb_lvl5;
        }
    }

    wb_res = r.result.read()(63, 0);
    if (r.high.read()) {
        wb_res = r.result.read()(127, 64);
    }

    if (!async_reset_ && i_nrst.read() == 0) {
        R_RESET(v);
        for (int i = 0; i < 16; i++) {
            v_lvl1.arr[i] = 0;
        }
        for (int i = 0; i < 4; i++) {
            v_lvl3.arr[i] = 0;
        }
    }

    o_res = wb_res;
    o_valid = r.ena.read()[3];
}

void IntMul::registers() {
    // Debug purpose only"
    if (r.ena.read()[2]) {
        uint64_t t1 = v.result.read()(63,0).to_uint64();
        uint64_t t2 = r.reference_mul.to_uint64();
        if (t1 != t2) {
            char tstr[512];
            RISCV_sprintf(tstr, sizeof(tstr), 
                "IntMul error: unsigned=%d, rv32=%d, high=%d,  "
                "(%016" RV_PRI64 "x/%016" RV_PRI64 "x) => "
                "%016" RV_PRI64 "x != %016" RV_PRI64 "x\n",
                r.unsign.read(), r.rv32.read(), r.high.read(),
                r.a1_dbg.to_uint64(), r.a2_dbg.to_uint64(), t1, t2);
            cout << tstr;
            cout.flush();
        }
    }
    if (async_reset_ && i_nrst.read() == 0) {
        R_RESET(r);
        for (int i = 0; i < 16; i++) {
            r_lvl1.arr[i] = 0;
        }
        for (int i = 0; i < 4; i++) {
            r_lvl3.arr[i] = 0;
        }
    } else {
        r = v;
        r_lvl1 = v_lvl1;
        r_lvl3 = v_lvl3;
    }
}

uint64_t IntMul::compute_reference(bool unsign, bool rv32, uint64_t a1, uint64_t a2) {
    uint64_t ret;
    if (rv32) {
        if (unsign) {
            ret = (uint32_t)a1 * (uint32_t)a2;
        } else {
            ret = (uint64_t)((int64_t)((int32_t)a1 * (int32_t)a2));
        }
    } else {
        // The results are the same but just for clearence
        if (unsign) {
            ret = a1 * a2;
        } else {
            ret = (int64_t)a1 * (int64_t)a2;
        }
    }
    return ret;
}

}  // namespace debugger

