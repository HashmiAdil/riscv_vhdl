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

#include "api_core.h"
#include "fpu_top.h"

namespace debugger {

FpuTop::FpuTop(sc_module_name name_, bool async_reset) : sc_module(name_),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_ena("i_ena"),
    i_ivec("i_ivec"),
    i_a("i_a"),
    i_b("i_b"),
    o_res("o_res"),
    o_ex_invalidop("o_ex_invalidop"),
    o_ex_divbyzero("o_ex_divbyzero"),
    o_ex_overflow("o_ex_overflow"),
    o_ex_underflow("o_ex_underflow"),
    o_ex_inexact("o_ex_inexact"),
    o_valid("o_valid"),
    fadd_d0("fadd_d0", async_reset),
    fdiv_d0("fdiv_d0", async_reset),
    fmul_d0("fmul_d0", async_reset),
    d2l_d0("d2l_d0", async_reset),
    l2d_d0("l2d_d0", async_reset) {
    async_reset_ = async_reset;

    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_ena;
    sensitive << i_ivec;
    sensitive << i_a;
    sensitive << i_b;
    sensitive << r.ivec;
    sensitive << r.busy;
    sensitive << r.ready;
    sensitive << r.a;
    sensitive << r.b;
    sensitive << r.result;
    sensitive << r.ex_invalidop;
    sensitive << r.ex_divbyzero;
    sensitive << r.ex_overflow;
    sensitive << r.ex_underflow;
    sensitive << r.ex_inexact;
    sensitive << r.ena_fadd;
    sensitive << r.ena_fdiv;
    sensitive << r.ena_fmul;
    sensitive << r.ena_d2l;
    sensitive << r.ena_l2d;
    sensitive << r.ena_w32;
    sensitive << wb_res_fadd;
    sensitive << w_valid_fadd;
    sensitive << w_illegalop_fadd;
    sensitive << w_overflow_fadd;
    sensitive << w_busy_fadd;
    sensitive << wb_res_fdiv;
    sensitive << w_valid_fdiv;
    sensitive << w_illegalop_fdiv;
    sensitive << w_divbyzero_fdiv;
    sensitive << w_overflow_fdiv;
    sensitive << w_underflow_fdiv;
    sensitive << w_busy_fdiv;
    sensitive << wb_res_fmul;
    sensitive << w_valid_fmul;
    sensitive << w_illegalop_fmul;
    sensitive << w_overflow_fmul;
    sensitive << w_busy_fmul;
    sensitive << wb_res_d2l;
    sensitive << w_valid_d2l;
    sensitive << w_overflow_d2l;
    sensitive << w_underflow_d2l;
    sensitive << w_busy_d2l;
    sensitive << wb_res_l2d;
    sensitive << w_valid_l2d;
    sensitive << w_busy_l2d;

    SC_METHOD(registers);
    sensitive << i_nrst;
    sensitive << i_clk.pos();

    fadd_d0.i_clk(i_clk);
    fadd_d0.i_nrst(i_nrst);
    fadd_d0.i_ena(r.ena_fadd);
    fadd_d0.i_add(w_fadd_d);
    fadd_d0.i_sub(w_fsub_d);
    fadd_d0.i_eq(w_feq_d);
    fadd_d0.i_lt(w_flt_d);
    fadd_d0.i_le(w_fle_d);
    fadd_d0.i_max(w_fmax_d);
    fadd_d0.i_min(w_fmin_d);
    fadd_d0.i_a(r.a);
    fadd_d0.i_b(r.b);
    fadd_d0.o_res(wb_res_fadd);
    fadd_d0.o_illegal_op(w_illegalop_fadd);
    fadd_d0.o_overflow(w_overflow_fadd);
    fadd_d0.o_valid(w_valid_fadd);
    fadd_d0.o_busy(w_busy_fadd);

    fdiv_d0.i_clk(i_clk);
    fdiv_d0.i_nrst(i_nrst);
    fdiv_d0.i_ena(r.ena_fdiv);
    fdiv_d0.i_a(r.a);
    fdiv_d0.i_b(r.b);
    fdiv_d0.o_res(wb_res_fdiv);
    fdiv_d0.o_illegal_op(w_illegalop_fdiv);
    fdiv_d0.o_divbyzero(w_divbyzero_fdiv);
    fdiv_d0.o_overflow(w_overflow_fdiv);
    fdiv_d0.o_underflow(w_underflow_fdiv);
    fdiv_d0.o_valid(w_valid_fdiv);
    fdiv_d0.o_busy(w_busy_fdiv);

    fmul_d0.i_clk(i_clk);
    fmul_d0.i_nrst(i_nrst);
    fmul_d0.i_ena(r.ena_fmul);
    fmul_d0.i_a(r.a);
    fmul_d0.i_b(r.b);
    fmul_d0.o_res(wb_res_fmul);
    fmul_d0.o_illegal_op(w_illegalop_fmul);
    fmul_d0.o_overflow(w_overflow_fmul);
    fmul_d0.o_valid(w_valid_fmul);
    fmul_d0.o_busy(w_busy_fmul);

    d2l_d0.i_clk(i_clk);
    d2l_d0.i_nrst(i_nrst);
    d2l_d0.i_ena(r.ena_d2l);
    d2l_d0.i_signed(w_fcvt_signed);
    d2l_d0.i_w32(r.ena_w32);
    d2l_d0.i_a(r.a);
    d2l_d0.o_res(wb_res_d2l);
    d2l_d0.o_overflow(w_overflow_d2l);
    d2l_d0.o_underflow(w_underflow_d2l);
    d2l_d0.o_valid(w_valid_d2l);
    d2l_d0.o_busy(w_busy_d2l);

    l2d_d0.i_clk(i_clk);
    l2d_d0.i_nrst(i_nrst);
    l2d_d0.i_ena(r.ena_l2d);
    l2d_d0.i_signed(w_fcvt_signed);
    l2d_d0.i_w32(r.ena_w32);
    l2d_d0.i_a(r.a);
    l2d_d0.o_res(wb_res_l2d);
    l2d_d0.o_valid(w_valid_l2d);
    l2d_d0.o_busy(w_busy_l2d);
};

void FpuTop::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    if (o_vcd) {
        sc_trace(o_vcd, i_ena, i_ena.name());
        sc_trace(o_vcd, i_a, i_a.name());
        sc_trace(o_vcd, i_b, i_b.name());
        sc_trace(o_vcd, i_ivec, i_ivec.name());
        sc_trace(o_vcd, o_res, o_res.name());
        sc_trace(o_vcd, o_valid, o_valid.name());

        std::string pn(name());
        sc_trace(o_vcd, r.result, pn + ".r_result");
        sc_trace(o_vcd, r.ena_fadd, pn + ".r_ena_fadd");
        sc_trace(o_vcd, r.ena_fdiv, pn + ".r_ena_fdiv");
        sc_trace(o_vcd, r.ena_fmul, pn + ".r_ena_fmul");
        sc_trace(o_vcd, r.ena_d2l, pn + ".r_ena_d2l");
        sc_trace(o_vcd, r.ena_l2d, pn + ".r_ena_l2d");
        sc_trace(o_vcd, r.ivec, pn + ".r_ivec");
    }
    fadd_d0.generateVCD(i_vcd, o_vcd);
    fdiv_d0.generateVCD(i_vcd, o_vcd);
    fmul_d0.generateVCD(i_vcd, o_vcd);
    d2l_d0.generateVCD(i_vcd, o_vcd);
    l2d_d0.generateVCD(i_vcd, o_vcd);
}

void FpuTop::comb() {
    sc_bv<Instr_FPU_Total> iv;
    v = r;

    iv = i_ivec.read();
    v.ena_fadd = 0;
    v.ena_fdiv = 0;
    v.ena_fmul = 0;
    v.ena_d2l = 0;
    v.ena_l2d = 0;
    v.ready = 0;
    if (i_ena.read() == 1 && r.busy.read() == 0) {
        v.busy = 1;
        v.a = i_a.read();
        v.b = i_b.read();
        v.ivec = i_ivec.read();
        v.ex_invalidop = 0;
        v.ex_divbyzero = 0;
        v.ex_overflow = 0;
        v.ex_underflow = 0;
        v.ex_inexact = 0;

        v.ena_fadd = (iv[Instr_FADD_D - Instr_FADD_D]
                    | iv[Instr_FSUB_D - Instr_FADD_D]
                    | iv[Instr_FLE_D - Instr_FADD_D]
                    | iv[Instr_FLT_D - Instr_FADD_D]
                    | iv[Instr_FEQ_D - Instr_FADD_D]
                    | iv[Instr_FMAX_D - Instr_FADD_D]
                    | iv[Instr_FMIN_D - Instr_FADD_D]).to_bool();
        v.ena_fdiv = iv[Instr_FDIV_D - Instr_FADD_D].to_bool();
        v.ena_fmul = iv[Instr_FMUL_D - Instr_FADD_D].to_bool();
        v.ena_d2l = (iv[Instr_FCVT_LU_D - Instr_FADD_D]
                    | iv[Instr_FCVT_L_D - Instr_FADD_D]
                    | iv[Instr_FCVT_WU_D - Instr_FADD_D]
                    | iv[Instr_FCVT_W_D - Instr_FADD_D]).to_bool();
        v.ena_l2d = (iv[Instr_FCVT_D_LU - Instr_FADD_D]
                    | iv[Instr_FCVT_D_L - Instr_FADD_D]
                    | iv[Instr_FCVT_D_WU - Instr_FADD_D]
                    | iv[Instr_FCVT_D_W - Instr_FADD_D]).to_bool();

        v.ena_w32 = (iv[Instr_FCVT_WU_D - Instr_FADD_D]
                    | iv[Instr_FCVT_W_D - Instr_FADD_D]
                    | iv[Instr_FCVT_D_WU - Instr_FADD_D]
                    | iv[Instr_FCVT_D_W - Instr_FADD_D]).to_bool();
    }

    if (r.busy.read() == 1 && (r.ivec.read()[Instr_FMOV_X_D - Instr_FADD_D]
                        | r.ivec.read()[Instr_FMOV_D_X - Instr_FADD_D]) == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = r.a;
    } else if (w_valid_fadd == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_fadd;
        v.ex_invalidop = w_illegalop_fadd;
        v.ex_overflow = w_overflow_fadd;
    } else if (w_valid_fdiv == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_fdiv;
        v.ex_invalidop = w_illegalop_fdiv;
        v.ex_divbyzero = w_divbyzero_fdiv;
        v.ex_overflow = w_overflow_fdiv;
        v.ex_underflow = w_underflow_fdiv;
    } else if (w_valid_fmul == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_fmul;
        v.ex_invalidop = w_illegalop_fmul;
        v.ex_overflow = w_overflow_fmul;
    } else if (w_valid_d2l == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_d2l;
        v.ex_overflow = w_overflow_d2l;
        v.ex_underflow = w_underflow_d2l;
    } else if (w_valid_l2d == 1) {
        v.busy = 0;
        v.ready = 1;
        v.result = wb_res_l2d;
    }

    if (!async_reset_ && i_nrst.read() == 0) {
        R_RESET(v);
    }

    w_fadd_d = r.ivec.read()[Instr_FADD_D - Instr_FADD_D].to_bool();
    w_fsub_d = r.ivec.read()[Instr_FSUB_D - Instr_FADD_D].to_bool();
    w_feq_d = r.ivec.read()[Instr_FEQ_D - Instr_FADD_D].to_bool();
    w_flt_d = r.ivec.read()[Instr_FLT_D - Instr_FADD_D].to_bool();
    w_fle_d = r.ivec.read()[Instr_FLE_D - Instr_FADD_D].to_bool();
    w_fmax_d = r.ivec.read()[Instr_FMAX_D - Instr_FADD_D].to_bool();
    w_fmin_d = r.ivec.read()[Instr_FMIN_D - Instr_FADD_D].to_bool();
    w_fcvt_signed = (r.ivec.read()[Instr_FCVT_L_D - Instr_FADD_D] |
                     r.ivec.read()[Instr_FCVT_D_L - Instr_FADD_D] |
                     r.ivec.read()[Instr_FCVT_W_D - Instr_FADD_D] |
                     r.ivec.read()[Instr_FCVT_D_W - Instr_FADD_D]).to_bool();

    o_res = r.result;
    o_ex_invalidop = r.ex_invalidop;
    o_ex_divbyzero = r.ex_divbyzero;
    o_ex_overflow = r.ex_overflow;
    o_ex_underflow = r.ex_underflow;
    o_ex_inexact = r.ex_inexact;
    o_valid = r.ready;
}

void FpuTop::registers() {
    if (async_reset_ && i_nrst.read() == 0) {
        R_RESET(r);
    } else {
        r = v;
    }
}

}  // namespace debugger

