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
 * @details    Vivado synthesizer (2016.2) doesn't support shift
 *             from dynamic value, so implement this mux.
 */

#ifndef __DEBUGGER_RIVERLIB_RSHIFT_H__
#define __DEBUGGER_RIVERLIB_RSHIFT_H__

#include <systemc.h>
#include "../../river_cfg.h"

namespace debugger {

SC_MODULE(Shifter) {
    sc_in<bool> i_clk;
    sc_in<bool> i_nrst;
    sc_in<sc_uint<4>> i_mode;             // operation type: [0]0=rv64;1=rv32;[1]=sll;[2]=srl;[3]=sra
    sc_in<sc_uint<RISCV_ARCH>> i_a1;      // Operand 1
    sc_in<sc_uint<6>> i_a2;               // Shift bits number
    sc_out<sc_uint<RISCV_ARCH>> o_res;

    void comb();
    void registers();

    SC_HAS_PROCESS(Shifter);

    Shifter(sc_module_name name_, bool async_reset);

    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);

private:
    struct RegistersType {
        sc_signal<sc_uint<RISCV_ARCH>> res;
    } v, r;

    void R_RESET(RegistersType &iv) {
        iv.res = 0;
    }

    bool async_reset_;
};

}  // namespace debugger

#endif  // __DEBUGGER_RIVERLIB_RSHIFT_H__
