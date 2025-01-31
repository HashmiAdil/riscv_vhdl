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

#include "csr.h"

namespace debugger {

bool dbg_e_valid = 0;

CsrRegs::CsrRegs(sc_module_name name_, uint32_t hartid, bool async_reset)
    : sc_module(name_),
    i_clk("i_clk"),
    i_nrst("i_nrst"),
    i_sp("i_sp"),
    i_req_valid("i_req_valid"),
    o_req_ready("o_req_ready"),
    i_req_type("i_req_type"),
    i_req_addr("i_req_addr"),
    i_req_data("i_req_data"),
    o_resp_valid("o_resp_valid"),
    i_resp_ready("i_resp_ready"),
    o_resp_data("o_resp_data"),
    o_resp_exception("o_resp_exception"),
    i_e_halted("i_e_halted"),
    i_e_pc("i_e_pc"),
    i_e_instr("i_e_instr"),
    i_msip("i_msip"),
    i_mtip("i_mtip"),
    i_meip("i_meip"),
    i_seip("i_seip"),
    o_irq_software("o_irq_software"),
    o_irq_timer("o_irq_timer"),
    o_irq_external("o_irq_external"),
    i_e_valid("i_e_valid"),
    o_executed_cnt("o_executed_cnt"),
    o_step("o_step"),
    i_dbg_progbuf_ena("i_dbg_progbuf_ena"),
    o_progbuf_end("o_progbuf_end"),
    o_progbuf_error("o_progbuf_error"),
    o_flushi_ena("o_flushi_ena"),
    o_flushi_addr("o_flushi_addr"),
    o_mpu_region_we("o_mpu_region_we"),
    o_mpu_region_idx("o_mpu_region_idx"),
    o_mpu_region_addr("o_mpu_region_addr"),
    o_mpu_region_mask("o_mpu_region_mask"),
    o_mpu_region_flags("o_mpu_region_flags") {
    hartid_ = hartid;
    async_reset_ = async_reset;

    SC_METHOD(comb);
    sensitive << i_nrst;
    sensitive << i_sp;
    sensitive << i_req_valid;
    sensitive << i_req_type;
    sensitive << i_req_addr;
    sensitive << i_req_data;
    sensitive << i_resp_ready;
    sensitive << i_e_halted;
    sensitive << i_e_pc;
    sensitive << i_e_instr;
    sensitive << i_msip;
    sensitive << i_mtip;
    sensitive << i_meip;
    sensitive << i_seip;
    sensitive << i_e_valid;
    sensitive << i_dbg_progbuf_ena;
    sensitive << r.state;
    sensitive << r.cmd_type;
    sensitive << r.cmd_addr;
    sensitive << r.cmd_data;
    sensitive << r.cmd_exception;
    sensitive << r.progbuf_end;
    sensitive << r.progbuf_err;
    sensitive << r.mtvec;
    sensitive << r.mtvec_mode;
    sensitive << r.mscratch;
    sensitive << r.mstackovr;
    sensitive << r.mstackund;
    sensitive << r.mtval;
    sensitive << r.mode;
    sensitive << r.uie;
    sensitive << r.mie;
    sensitive << r.mpie;
    sensitive << r.mpp;
    sensitive << r.mepc;
    sensitive << r.uepc;
    sensitive << r.usie;
    sensitive << r.ssie;
    sensitive << r.msie;
    sensitive << r.utie;
    sensitive << r.stie;
    sensitive << r.mtie;
    sensitive << r.ueie;
    sensitive << r.seie;
    sensitive << r.meie;
    sensitive << r.usip;
    sensitive << r.ssip;
    sensitive << r.msip;
    sensitive << r.utip;
    sensitive << r.stip;
    sensitive << r.mtip;
    sensitive << r.ueip;
    sensitive << r.seip;
    sensitive << r.meip;
    sensitive << r.ex_fpu_invalidop;
    sensitive << r.ex_fpu_divbyzero;
    sensitive << r.ex_fpu_overflow;
    sensitive << r.ex_fpu_underflow;
    sensitive << r.ex_fpu_inexact;
    sensitive << r.trap_irq;
    sensitive << r.trap_cause;
    sensitive << r.trap_addr;
    sensitive << r.timer;
    sensitive << r.cycle_cnt;
    sensitive << r.executed_cnt;
    sensitive << r.dscratch0;
    sensitive << r.dscratch1;
    sensitive << r.dpc;
    sensitive << r.halt_cause;
    sensitive << r.dcsr_ebreakm;
    sensitive << r.dcsr_stopcount;
    sensitive << r.dcsr_stoptimer;
    sensitive << r.dcsr_step;
    sensitive << r.dcsr_stepie;
    sensitive << r.stepping_mode_cnt;
    sensitive << r.ins_per_step;

    SC_METHOD(registers);
    sensitive << i_nrst;
    sensitive << i_clk.pos();
};

void CsrRegs::generateVCD(sc_trace_file *i_vcd, sc_trace_file *o_vcd) {
    if (o_vcd) {
        sc_trace(o_vcd, i_sp, i_sp.name());
        sc_trace(o_vcd, i_req_valid, i_req_valid.name());
        sc_trace(o_vcd, o_req_ready, o_req_ready.name());
        sc_trace(o_vcd, i_req_type, i_req_type.name());
        sc_trace(o_vcd, i_req_addr, i_req_addr.name());
        sc_trace(o_vcd, i_req_data, i_req_data.name());
        sc_trace(o_vcd, o_resp_valid, o_resp_valid.name());
        sc_trace(o_vcd, i_resp_ready, i_resp_ready.name());
        sc_trace(o_vcd, o_resp_data, o_resp_data.name());
        sc_trace(o_vcd, o_resp_exception, o_resp_exception.name());
        sc_trace(o_vcd, i_e_halted, i_e_halted.name());
        sc_trace(o_vcd, i_e_pc, i_e_pc.name());
        sc_trace(o_vcd, i_e_instr, i_e_instr.name());
        sc_trace(o_vcd, i_msip, i_msip.name());
        sc_trace(o_vcd, i_mtip, i_mtip.name());
        sc_trace(o_vcd, i_meip, i_meip.name());
        sc_trace(o_vcd, i_seip, i_seip.name());
        sc_trace(o_vcd, i_dbg_progbuf_ena, i_dbg_progbuf_ena.name());
        sc_trace(o_vcd, o_irq_software, o_irq_software.name());
        sc_trace(o_vcd, o_irq_timer, o_irq_timer.name());
        sc_trace(o_vcd, o_irq_external, o_irq_external.name());
        sc_trace(o_vcd, o_executed_cnt, o_executed_cnt.name());
        sc_trace(o_vcd, o_step, o_step.name());
        sc_trace(o_vcd, o_progbuf_end, o_progbuf_end.name());
        sc_trace(o_vcd, o_progbuf_error, o_progbuf_error.name());
        sc_trace(o_vcd, o_flushi_ena, o_flushi_ena.name());
        sc_trace(o_vcd, o_flushi_addr, o_flushi_addr.name());

        std::string pn(name());
        sc_trace(o_vcd, r.state, pn + ".r_state");
        sc_trace(o_vcd, r.mode, pn + ".r_mode");
        sc_trace(o_vcd, r.mie, pn + ".r_mie");
        sc_trace(o_vcd, r.mepc, pn + ".r_mepc");
        sc_trace(o_vcd, r.mtval, pn + ".r_mtval");
        sc_trace(o_vcd, r.mtvec, pn + ".r_mtvec");
        sc_trace(o_vcd, r.mtvec_mode, pn + ".r_mtvec_mode");
        sc_trace(o_vcd, r.meip, pn + ".r_meip");
        sc_trace(o_vcd, r.mtip, pn + ".r_mtip");
        sc_trace(o_vcd, r.msip, pn + ".r_msip");
        sc_trace(o_vcd, r.trap_cause, pn + ".r_trap_cause");
        sc_trace(o_vcd, r.dpc, pn + ".r_dpc");
        sc_trace(o_vcd, r.dcsr_ebreakm, pn + ".r_dcsr_ebreakm");
        sc_trace(o_vcd, r.halt_cause, pn + ".r_halt_cause");
        sc_trace(o_vcd, r.dcsr_step, pn + ".r_step");
        sc_trace(o_vcd, r.stepping_mode_cnt, pn + ".r_stepping_mode_cnt");
        sc_trace(o_vcd, r.ins_per_step, pn + ".r_ins_per_step");
        sc_trace(o_vcd, r.flushi_ena, pn + ".r_flushi_ena");
        sc_trace(o_vcd, r.flushi_addr, pn + ".r_flushi_addr");
    }
}

void CsrRegs::comb() {
    bool v_sw_irq;
    bool v_tmr_irq;
    bool v_ext_irq;
    bool w_trap_valid;
    bool v_trap_irq;
    sc_uint<5> wb_trap_cause;
    sc_uint<RISCV_ARCH> vb_mtval;   // additional exception information
    bool w_mstackovr;
    bool w_mstackund;
    bool v_csr_rena;
    bool v_csr_wena;
    bool v_csr_trapreturn;
    sc_uint<RISCV_ARCH> vb_rdata;
    bool v_req_halt;
    bool v_req_resume;
    bool v_req_progbuf;
    bool v_req_ready;
    bool v_resp_valid;

    v = r;

    dbg_e_valid = i_e_valid.read();     // used in RtlWrapper to count executed instructions

    vb_rdata = 0;
    v_req_halt = 0;
    v_req_resume = 0;
    v_req_progbuf = 0;
    v.flushi_ena = 0;
    v.flushi_addr = 0;
    v_req_ready = 0;
    v_resp_valid = 0;
    v_csr_rena = 0;
    v_csr_wena = 0;
    v_csr_trapreturn = 0;
    w_trap_valid = 0;
    v_trap_irq = 0;
    wb_trap_cause = 0;
    vb_mtval = 0;

    v.mpu_we = 0;

    switch (r.state.read()) {
    case State_Idle:
        v.progbuf_end = 0;
        v.progbuf_err = 0;
        v_req_ready = 1;
        if (i_req_valid) {
            v.cmd_type = i_req_type;
            v.cmd_addr = i_req_addr;
            v.cmd_data = i_req_data;
            v.cmd_exception = 0;
            if (i_req_type.read()[CsrReq_ExceptionBit]) {
                v.state = State_Exception;
            } else if (i_req_type.read()[CsrReq_BreakpointBit]) {
                v.state = State_Breakpoint;
            } else if (i_req_type.read()[CsrReq_HaltBit]) {
                v.state = State_Halt;
            } else if (i_req_type.read()[CsrReq_ResumeBit]) {
                v.state = State_Resume;
            } else if (i_req_type.read()[CsrReq_InterruptBit]) {
                v.state = State_Interrupt;
            } else if (i_req_type.read()[CsrReq_TrapReturnBit]) {
                v.state = State_TrapReturn;
            } else if (i_req_type.read()[CsrReq_WfiBit]) {
                v.state = State_Wfi;
            } else {
                v.state = State_RW;
            }
        }
        break;
    case State_Exception:
        v.state = State_Response;
        w_trap_valid = 1;
        vb_mtval = r.cmd_data;
        wb_trap_cause = r.cmd_addr.read()(4, 0);
        v.cmd_data = r.mtvec.read() & ~0x3ull;
        if (i_dbg_progbuf_ena.read() == 1) {
            v.progbuf_err = 1;
            v.progbuf_end = 1;
            v.cmd_exception = 1;
        }
        if (r.cmd_addr.read() == EXCEPTION_CallFromUmode) {
            wb_trap_cause = r.cmd_addr.read() + r.mode.read().to_uint();
        }
        break;
    case State_Breakpoint:  // software breakpoint
        v.state = State_Response;
        if (i_dbg_progbuf_ena.read() == 1) {
            // do not modify halt cause in debug mode
            v.progbuf_end = 1;
            v.cmd_data = ~0ull;     // signal to executor to switch into Debug Mode and halt
        } else if (r.dcsr_ebreakm) {
            v.halt_cause = HALT_CAUSE_EBREAK;
            v.dpc = r.cmd_data;
            v.cmd_data = ~0ull;     // signal to executor to switch into Debug Mode and halt
        } else {
            w_trap_valid = 1;
            wb_trap_cause = r.cmd_addr.read()(4, 0);
            vb_mtval = i_e_pc;
            v.cmd_data = r.mtvec.read() & ~0x3ull;   // Jump to exception handler
        }
        break;
    case State_Halt:
        v.state = State_Response;
        v.halt_cause = r.cmd_addr.read()(2, 0); // Halt Request or Step done
        v.dpc = i_e_pc.read();
        break;
    case State_Resume:
        v.state = State_Response;
        if (i_dbg_progbuf_ena.read() == 1) {
            v.cmd_data = 0;
        } else {
            v.cmd_data = r.dpc;
        }
        break;
    case State_Interrupt:
        v.state = State_Response;
        w_trap_valid = 1;
        wb_trap_cause = 4*r.cmd_addr.read() + PRV_M;//r.mode.read().to_int();
        v_trap_irq = 1;
        if (r.mtvec_mode.read() == 1) {
            // vectorized
            v.cmd_data = (r.mtvec.read() & ~0x3ull) + 4*wb_trap_cause;
        } else {
            v.cmd_data = r.mtvec.read() & ~0x3ull;
        }
        break;
    case State_TrapReturn:
        v.state = State_Response;
        v_csr_trapreturn = 1;
        if (r.cmd_addr.read() == CSR_mepc) {
            v.cmd_data = r.mepc;
        } else if (r.cmd_addr.read() == CSR_hepc) {
            v.cmd_data = 0;
        } else if (r.cmd_addr.read() == CSR_sepc) {
            v.cmd_data = 0;
        } else if (r.cmd_addr.read() == CSR_uepc) {
            v.cmd_data = r.uepc;
        } else {
            v.cmd_data = 0;
        }
        break;
    case State_RW:
        v.state = State_Response;
        v_csr_rena = r.cmd_type.read()[CsrReq_ReadBit];
        v_csr_wena = r.cmd_type.read()[CsrReq_WriteBit];
        if (r.mode.read() < r.cmd_addr.read()(9,8)) {
            // Not enough priv to access this register
            v.cmd_exception = 1;
        }
        // All operation into CSR implemented through the Read-Modify-Write
        // we cannot generate exception on write access into read-only regs
        break;
    case State_Wfi:
        v.state = State_Response;
        v.cmd_data = 0;     // no error, valid for all mdoes
        break;
    case State_Response:
        v_resp_valid = 1;
        if (i_resp_ready) {
            v.state = State_Idle;
        }
        break;
    default:;
    }

    switch (r.cmd_addr.read()) {
    case CSR_fflags:
        vb_rdata[0] = r.ex_fpu_inexact;
        vb_rdata[1] = r.ex_fpu_underflow;
        vb_rdata[2] = r.ex_fpu_overflow;
        vb_rdata[3] = r.ex_fpu_divbyzero;
        vb_rdata[4] = r.ex_fpu_invalidop;
        break;
    case CSR_frm:
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata(2, 0) = 0x4;  // Round mode: round to Nearest (RMM)
        }
        break;
    case CSR_fcsr:
        vb_rdata[0] = r.ex_fpu_inexact;
        vb_rdata[1] = r.ex_fpu_underflow;
        vb_rdata[2] = r.ex_fpu_overflow;
        vb_rdata[3] = r.ex_fpu_divbyzero;
        vb_rdata[4] = r.ex_fpu_invalidop;
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata(7, 5) = 0x4;  // Round mode: round to Nearest (RMM)
        }
        break; 
    case CSR_misa:
        /** Base[XLEN-1:XLEN-2]
            *      1 = 32
            *      2 = 64
            *      3 = 128
            */
        vb_rdata(RISCV_ARCH-1, RISCV_ARCH-2) = 2;
        /** BitCharacterDescription
            * 0  A Atomic extension
            * 1  B Tentatively reserved for Bit operations extension
            * 2  C Compressed extension
            * 3  D Double-precision Foating-point extension
            * 4  E RV32E base ISA (embedded)
            * 5  F Single-precision Foating-point extension
            * 6  G Additional standard extensions present
            * 7  H Hypervisor mode implemented
            * 8  I RV32I/64I/128I base ISA
            * 9  J Reserved
            * 10 K Reserved
            * 11 L Tentatively reserved for Decimal Floating-Point extension
            * 12 M Integer Multiply/Divide extension
            * 13 N User-level interrupts supported
            * 14 O Reserved
            * 15 P Tentatively reserved for Packed-SIMD extension
            * 16 Q Quad-precision Foating-point extension
            * 17 R Reserved
            * 18 S Supervisor mode implemented
            * 19 T Tentatively reserved for Transactional Memory extension
            * 20 U User mode implemented
            * 21 V Tentatively reserved for Vector extension
            * 22 W Reserved
            * 23 X Non-standard extensions present
            * 24 Y Reserved
            * 25 Z Reserve
            */
        vb_rdata['A' - 'A'] = 1;
        vb_rdata['I' - 'A'] = 1;
        vb_rdata['M' - 'A'] = 1;
        vb_rdata['U' - 'A'] = 1;
        vb_rdata['C' - 'A'] = 1;
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata['D' - 'A'] = 1;
        }
        break;
    case CSR_mvendorid:
        vb_rdata = CFG_VENDOR_ID;
        break;
    case CSR_marchid:
        break;
    case CSR_mimplementationid:
        vb_rdata = CFG_IMPLEMENTATION_ID;
        break;
    case CSR_mhartid:
        vb_rdata(63, 0) = hartid_;
        break;
    case CSR_uepc:// - User mode program counter
        vb_rdata = r.uepc;
        if (v_csr_wena) {
            v.uepc = r.cmd_data.read()(CFG_CPU_ADDR_BITS-1, 0);
        }
        break;
    case CSR_mstatus:// - Machine mode status register
        vb_rdata[0] = r.uie;
        vb_rdata[3] = r.mie;
        vb_rdata[7] = r.mpie;
        vb_rdata(12, 11) = r.mpp;
        if (CFG_HW_FPU_ENABLE) {
            vb_rdata(14, 13) = 0x1;   // FS field: Initial state
        }
        vb_rdata(33, 32) = 0x2;       // UXL: User mode supported 64-bits
        if (v_csr_wena) {
            v.uie = r.cmd_data.read()[0];
            v.mie = r.cmd_data.read()[3];
            v.mpie = r.cmd_data.read()[7];
            v.mpp = r.cmd_data.read()(12, 11);
        }
        break;
    case CSR_medeleg:// - Machine exception delegation
        break;
    case CSR_mideleg:// - Machine interrupt delegation
        break;
    case CSR_mie:// - Machine interrupt enable bit
        vb_rdata[0] = r.usie;   // user software interrupt
        vb_rdata[1] = r.ssie;   // super-user software interrupt
        vb_rdata[3] = r.msie;   // machine software interrupt
        vb_rdata[4] = r.utie;   // user timer interrupt
        vb_rdata[5] = r.stie;   // super-user timer interrupt
        vb_rdata[7] = r.mtie;   // machine timer interrupt
        vb_rdata[8] = r.ueie;   // user external interrupt
        vb_rdata[9] = r.seie;   // super-user external interrupt
        vb_rdata[11] = r.meie;  // machine external interrupt
        if (v_csr_wena) {
            v.usie = r.cmd_data.read()[0];
            v.ssie = r.cmd_data.read()[1];
            v.msie = r.cmd_data.read()[3];
            v.utie = r.cmd_data.read()[4];
            v.stie = r.cmd_data.read()[5];
            v.mtie = r.cmd_data.read()[7];
            v.ueie = r.cmd_data.read()[8];
            v.seie = r.cmd_data.read()[9];
            v.meie = r.cmd_data.read()[11];
        }
        break;
    case CSR_mtvec:
        vb_rdata = r.mtvec;
        vb_rdata(1, 0) = r.mtvec_mode;
        if (v_csr_wena) {
            v.mtvec = r.cmd_data.read()(RISCV_ARCH-1, 2) << 2;
            v.mtvec_mode = r.cmd_data.read()(1, 0);
        }
        break;
    case CSR_mscratch:// - Machine scratch register
        vb_rdata = r.mscratch;
        if (v_csr_wena) {
            v.mscratch = r.cmd_data.read();
        }
        break;
    case CSR_mepc:// - Machine program counter
        vb_rdata = r.mepc;
        if (v_csr_wena) {
            v.mepc = r.cmd_data.read()(CFG_CPU_ADDR_BITS-1, 0);
        }
        break;
    case CSR_mcause:// - Machine trap cause
        vb_rdata = 0;
        vb_rdata[63] = r.trap_irq;
        vb_rdata(4, 0) = r.trap_cause;
        break;
    case CSR_mtval:// - Machine bad address or instruction
        vb_rdata = r.mtval;
        if (v_csr_wena) {
            v.mtval = r.cmd_data.read();
        }
        break;
    case CSR_mip:// - Machine interrupt pending
        vb_rdata[0] = r.usip;   // user software pending bit
        vb_rdata[1] = r.ssip;   // super-user software pending bit
        vb_rdata[3] = r.msip;   // machine software pending bit
        vb_rdata[4] = r.utip;   // user timer pending bit
        vb_rdata[5] = r.stip;   // super-user timer pending bit
        vb_rdata[7] = r.mtip;   // RO: machine timer pending bit
        vb_rdata[8] = r.ueip;   // user external pending bit
        vb_rdata[9] = r.seip;   // super-user external pending bit
        vb_rdata[11] = r.meip;  // RO: machine external pending bit (cleared by writing into mtimecmp)
        break;
    case CSR_cycle:
        vb_rdata = r.cycle_cnt;
        break;
    case CSR_time:
        vb_rdata = r.timer;
        break;
    case CSR_insret:
        vb_rdata = r.executed_cnt;
        break;
    case CSR_mstackovr:// - Machine Stack Overflow
        vb_rdata = r.mstackovr;
        if (v_csr_wena) {
            v.mstackovr = r.cmd_data.read()(CFG_CPU_ADDR_BITS-1, 0);
        }
        break;
    case CSR_mstackund:// - Machine Stack Underflow
        vb_rdata = r.mstackund;
        if (v_csr_wena) {
            v.mstackund = r.cmd_data.read()(CFG_CPU_ADDR_BITS-1, 0);
        }
        break;
    case CSR_mpu_addr:  // [WO] MPU address
        if (v_csr_wena) {
            v.mpu_addr = r.cmd_data.read()(CFG_CPU_ADDR_BITS-1, 0);
        }
        break;
    case CSR_mpu_mask:  // [WO] MPU mask
        if (v_csr_wena) {
            v.mpu_mask = r.cmd_data.read()(CFG_CPU_ADDR_BITS-1, 0);
        }
        break;
    case CSR_mpu_ctrl:  // [WO] MPU flags and write ena
        vb_rdata = CFG_MPU_TBL_SIZE << 8;
        if (v_csr_wena) {
            v.mpu_idx = r.cmd_data.read()(8+CFG_MPU_TBL_WIDTH-1, 8);
            v.mpu_flags = r.cmd_data.read()(CFG_MPU_FL_TOTAL-1, 0);
            v.mpu_we = 1;
        }
        break;
    case CSR_flushi:
        if (v_csr_wena) {
            v.flushi_ena = 1;
            v.flushi_addr = r.cmd_data.read()(CFG_CPU_ADDR_BITS-1, 0);
        }
        break;
    case CSR_dcsr:
        vb_rdata(31, 28) = 4;                   // xdebugver: 4=External debug supported
        vb_rdata[15] = r.dcsr_ebreakm;
        vb_rdata[11] = r.dcsr_stepie;           // interrupt dis/ena during step
        vb_rdata[10] = r.dcsr_stopcount;        // don't increment any counter
        vb_rdata[9] = r.dcsr_stoptimer;         // don't increment timer
        vb_rdata(8, 6) = r.halt_cause;
        vb_rdata[2] = r.dcsr_step;
        vb_rdata(1, 0) = 3;                     // prv: privilege in debug mode: 3=machine
        if (v_csr_wena) {
            v.dcsr_ebreakm = r.cmd_data.read()[15];
            v.dcsr_stepie = r.cmd_data.read()[11];
            v.dcsr_stopcount = r.cmd_data.read()[10];
            v.dcsr_stoptimer = r.cmd_data.read()[9];
            v.dcsr_step = r.cmd_data.read()[2];
        }
        break;
    case CSR_dpc:
        // Upon entry into debug mode DPC must contains:
        //       cause        |   Address
        // -------------------|----------------
        // ebreak             |  Address of ebreak instruction
        // single step        |  Address of next instruction to be executed
        // trigger (HW BREAK) |  if timing=0, cause isntruction, if timing=1 enxt instruction
        // halt request       |  next instruction
        //
        if (i_e_halted) {
            vb_rdata = r.dpc.read();
        } else {
            // make visible current pc for the debugger even in running state
            vb_rdata = i_e_pc.read();
        }
        if (v_csr_wena) {
            v.dpc = r.cmd_data.read();
        }
        break;
    case CSR_dscratch0:
        vb_rdata = r.dscratch0;
        if (v_csr_wena) {
            v.dscratch0 = r.cmd_data.read();
        }
        break;
    case CSR_dscratch1:
        vb_rdata = r.dscratch1;
        if (v_csr_wena) {
            v.dscratch1 = r.cmd_data.read();
        }
        break;
    default:;
        // Not implemented CSR:
        if (r.state.read() == State_RW) {
            v.cmd_exception = 1;
        }
    }

    if (v_csr_rena) {
        v.cmd_data = vb_rdata;
    }


    if (v_csr_trapreturn) {
        if (r.mode.read() == PRV_M && r.cmd_addr.read() == CSR_mepc) {
            v.mie = r.mpie;
            v.mpie = 1;
            v.mode = r.mpp;
            v.mpp = PRV_U;
        } else if (r.mode.read() == PRV_U && r.cmd_addr.read() == CSR_uepc) {
            v.mie = r.mpie;
            v.mpie = 0;     // Interrupts in a user mode actually not supported
            v.mode = r.mpp;
            v.mpp = PRV_U;
        } else {
            v.cmd_exception = 1;
        }
    }

    if (w_trap_valid) {
        v.mie = 0;
        v.mpp = r.mode;
        v.mepc = i_e_pc.read();     // current latched instruction not executed overwritten by exception/interrupt
        v.mtval = vb_mtval;         // additional information for hwbreakpoint, memaccess faults and illegal opcodes
        v.trap_cause = wb_trap_cause;
        v.trap_irq = v_trap_irq;
        v.mode = PRV_M;
        switch (r.mode.read()) {
        case PRV_U:
            v.mpie = r.uie;
            break;
        case PRV_M:
            v.mpie = r.mie;
            break;
        default:;
        }
    }

    v.msip = i_msip.read();
    v_sw_irq = r.msip.read() && r.msie.read() && r.mie.read() && (!r.dcsr_step || r.dcsr_stepie);

    v.mtip = i_mtip.read();
    v_tmr_irq = r.mtip.read() && r.mtie.read() && r.mie.read() && (!r.dcsr_step || r.dcsr_stepie);

    v.meip = i_meip.read();
    v.seip = i_seip.read();
    v_ext_irq = r.meip.read() && r.meie.read() && r.mie.read() && (!r.dcsr_step || r.dcsr_stepie);



    w_mstackovr = 0;
    if (r.mstackovr.read().or_reduce() && 
        (i_sp.read()(CFG_CPU_ADDR_BITS-1, 0) < r.mstackovr.read())) {
        w_mstackovr = 1;
        v.mstackovr = 0;
    }

    w_mstackund = 0;
    if (r.mstackund.read().or_reduce() &&
        (i_sp.read()(CFG_CPU_ADDR_BITS-1, 0) > r.mstackund.read())) {
        w_mstackund = 1;
        v.mstackund = 0;
    }

#if 0
    if (i_fpu_valid.read()) {
        v.ex_fpu_invalidop = i_ex_fpu_invalidop.read();
        v.ex_fpu_divbyzero = i_ex_fpu_divbyzero.read();
        v.ex_fpu_overflow = i_ex_fpu_overflow.read();
        v.ex_fpu_underflow = i_ex_fpu_underflow.read();
        v.ex_fpu_inexact = i_ex_fpu_inexact.read();
    }

#endif


    if (i_e_halted.read() == 0 || !r.dcsr_stopcount) {
        v.cycle_cnt = r.cycle_cnt.read() + 1;
    }
    if ((i_e_valid && !r.dcsr_stopcount)
        || i_e_valid.read() && !(i_dbg_progbuf_ena && r.dcsr_stopcount)) {
        v.executed_cnt = r.executed_cnt.read() + 1;
    }
    if (!((i_e_halted || i_dbg_progbuf_ena) && r.dcsr_stoptimer)) {
        v.timer = r.timer.read() + 1;
    }

    if (!async_reset_ && !i_nrst.read()) {
        R_RESET(v);
    }

    o_req_ready = v_req_ready;
    o_resp_valid = v_resp_valid;
    o_resp_data = r.cmd_data;
    o_resp_exception = r.cmd_exception;
    o_progbuf_end = r.progbuf_end & i_resp_ready.read();
    o_progbuf_error = r.progbuf_err & i_resp_ready.read();

    o_irq_software = v_sw_irq;
    o_irq_timer = v_tmr_irq;
    o_irq_external = v_ext_irq;
    o_stack_overflow = w_mstackovr;
    o_stack_underflow = w_mstackund;

    o_executed_cnt = r.executed_cnt;
    o_mpu_region_we = r.mpu_we;
    o_mpu_region_idx = r.mpu_idx;
    o_mpu_region_addr = r.mpu_addr;
    o_mpu_region_mask = r.mpu_mask;
    o_mpu_region_flags = r.mpu_flags;
    o_step = r.dcsr_step;
    o_flushi_ena = r.flushi_ena;
    o_flushi_addr = r.flushi_addr;
}

void CsrRegs::registers() {
    if (async_reset_ && i_nrst.read() == 0) {
        R_RESET(r);
    } else {
        r = v;
    }
}

}  // namespace debugger

