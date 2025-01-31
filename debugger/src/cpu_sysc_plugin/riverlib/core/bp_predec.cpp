/*
 *  Copyright 2018 Sergey Khabarov, sergeykhbr@gmail.com
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

#include "bp_predec.h"

namespace debugger {

BpPreDecoder::BpPreDecoder(sc_module_name name_) :
    sc_module(name_),
    i_c_valid("i_c_valid"),
    i_addr("i_addr"),
    i_data("i_data"),
    i_ra("i_ra"),
    o_jmp("o_jmp"),
    o_pc("o_pc"),
    o_npc("o_npc") {

    SC_METHOD(comb);
    sensitive << i_addr;
    sensitive << i_data;
    sensitive << i_ra;
};

void BpPreDecoder::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    if (o_vcd) {
        sc_trace(o_vcd, i_c_valid, i_c_valid.name());
        sc_trace(o_vcd, i_addr, i_addr.name());
        sc_trace(o_vcd, i_data, i_data.name());
        sc_trace(o_vcd, i_ra, i_ra.name());
        sc_trace(o_vcd, o_jmp, o_jmp.name());
        sc_trace(o_vcd, o_pc, o_pc.name());
        sc_trace(o_vcd, o_npc, o_npc.name());
    }
}

void BpPreDecoder::comb() {
    sc_uint<32> vb_tmp;
    sc_uint<CFG_CPU_ADDR_BITS> vb_npc;
    sc_uint<CFG_CPU_ADDR_BITS> vb_pc;
    sc_uint<CFG_CPU_ADDR_BITS> vb_jal_off;
    sc_uint<CFG_CPU_ADDR_BITS> vb_jal_addr;
    sc_uint<CFG_CPU_ADDR_BITS> vb_branch_off;
    sc_uint<CFG_CPU_ADDR_BITS> vb_branch_addr;
    sc_uint<CFG_CPU_ADDR_BITS> vb_c_j_off;
    sc_uint<CFG_CPU_ADDR_BITS> vb_c_j_addr;

    vb_pc = i_addr.read();
    vb_tmp = i_data.read();

    // Unconditional jump "J"
    if (vb_tmp[31]) {
        vb_jal_off(CFG_CPU_ADDR_BITS-1, 20) = ~0;
    } else {
        vb_jal_off(CFG_CPU_ADDR_BITS-1, 20) = 0;
    }
    vb_jal_off(19, 12) = vb_tmp(19, 12);
    vb_jal_off[11] = vb_tmp[20];
    vb_jal_off(10, 1) = vb_tmp(30, 21);
    vb_jal_off[0] = 0;
    vb_jal_addr = vb_pc + vb_jal_off;

    v_jal = 0;
    if (vb_tmp.range(6, 0) == 0x6F) {
        v_jal = 1;
    }

    // Conditional branches "BEQ", "BNE", "BLT", "BGE", BLTU", "BGEU"
    // Only negative offset leads to predicted jumps
    if (vb_tmp[31]) {
        vb_branch_off(CFG_CPU_ADDR_BITS-1, 12) = ~0;
    } else {
        vb_branch_off(CFG_CPU_ADDR_BITS-1, 12) = 0;
    }
    vb_branch_off[11] = vb_tmp[7];
    vb_branch_off(10, 5) = vb_tmp(30, 25);
    vb_branch_off(4, 1) = vb_tmp(11, 8);
    vb_branch_off[0] = 0;
    vb_branch_addr = vb_pc + vb_branch_off;

    v_branch = 0;
    if (vb_tmp.range(6, 0) == 0x63 && vb_tmp[31] == 1) {
        v_branch = 1;
    }

    // Check Compressed "C_J" unconditional jump
    if (vb_tmp[12]) {
        vb_c_j_off(CFG_CPU_ADDR_BITS-1, 11) = ~0;
    } else {
        vb_c_j_off(CFG_CPU_ADDR_BITS-1, 11) = 0;
    }
    vb_c_j_off[10] = vb_tmp[8];
    vb_c_j_off(9, 8) = vb_tmp(10, 9);
    vb_c_j_off[7] = vb_tmp[6];
    vb_c_j_off[6] = vb_tmp[7];
    vb_c_j_off[5] = vb_tmp[2];
    vb_c_j_off[4] = vb_tmp[11];
    vb_c_j_off(3, 1) = vb_tmp(5, 3);
    vb_c_j_off[0] = 0;
    vb_c_j_addr = vb_pc + vb_c_j_off;

    v_c_j = 0;
    if (vb_tmp.range(15, 13) == 0x5 && vb_tmp.range(1, 0) == 0x1) {
        v_c_j = i_c_valid;
    }

    // Compressed RET pseudo-instruction
    v_c_ret = 0;
    if (vb_tmp.range(15, 0) == 0x8082) {
        v_c_ret = i_c_valid;
    }

    if (v_jal == 1) {
        vb_npc = vb_jal_addr;
    } else if (v_branch == 1) {
        vb_npc = vb_branch_addr;
    } else if (v_c_j == 1) {
        vb_npc = vb_c_j_addr;
    } else if (v_c_ret == 1) {
        vb_npc = i_ra.read();
    } else {
        vb_npc = vb_pc + 4;
    }


    o_jmp = v_jal | v_branch | v_c_j | v_c_ret;
    o_pc = vb_pc;
    o_npc = vb_npc;
}


}  // namespace debugger

