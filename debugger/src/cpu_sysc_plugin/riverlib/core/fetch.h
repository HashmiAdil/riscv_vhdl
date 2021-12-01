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

#ifndef __DEBUGGER_RIVERLIB_FETCH_H__
#define __DEBUGGER_RIVERLIB_FETCH_H__

#include <systemc.h>
#include "../river_cfg.h"

namespace debugger {

SC_MODULE(InstrFetch) {
    sc_in<bool> i_clk;
    sc_in<bool> i_nrst;
    sc_in<bool> i_pipeline_hold;
    sc_in<bool> i_mem_req_ready;
    sc_out<bool> o_mem_addr_valid;
    sc_out<sc_uint<CFG_CPU_ADDR_BITS>> o_mem_addr;
    sc_in<bool> i_mem_data_valid;
    sc_in<sc_uint<CFG_CPU_ADDR_BITS>> i_mem_data_addr;
    sc_in<sc_uint<32>> i_mem_data;
    sc_in<bool> i_mem_load_fault;
    sc_in<bool> i_mem_executable;
    sc_out<bool> o_mem_resp_ready;

    sc_in<bool> i_flush_pipeline;                   // reset pipeline and cache
    sc_in<bool> i_progbuf_ena;                      // executing from prog buffer
    sc_in<sc_uint<CFG_CPU_ADDR_BITS>> i_progbuf_pc;                // progbuf counter
    sc_in<sc_uint<32>> i_progbuf_instr;             // progbuf instruction
    sc_in<sc_uint<CFG_CPU_ADDR_BITS>> i_predict_npc;

    sc_out<bool> o_mem_req_fire;                    // used by branch predictor to form new npc value
    sc_out<bool> o_instr_load_fault;
    sc_out<bool> o_instr_executable;
    sc_out<bool> o_valid;
    sc_out<sc_uint<CFG_CPU_ADDR_BITS>> o_pc;
    sc_out<sc_uint<32>> o_instr;
    sc_out<bool> o_hold;                                // Hold due no response from icache yet

    void comb();
    void registers();

    SC_HAS_PROCESS(InstrFetch);

    InstrFetch(sc_module_name name_, bool async_reset);

    void generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd);

 private:
    struct RegistersType {
        sc_signal<bool> wait_resp;

        sc_signal<sc_uint<CFG_CPU_ADDR_BITS>> resp_address;
        sc_signal<sc_uint<32>> resp_data;
        sc_signal<bool> resp_valid;
        sc_signal<bool> instr_load_fault;
        sc_signal<bool> instr_executable;
    } v, r;

    void R_RESET(RegistersType &iv) {
        iv.wait_resp = 0;
        iv.resp_address = ~0ull;
        iv.resp_data = 0;
        iv.resp_valid = 0;
        iv.instr_load_fault = 0;
        iv.instr_executable = 0;
    }

    bool async_reset_;
};


}  // namespace debugger

#endif  // __DEBUGGER_RIVERLIB_FETCH_H__
