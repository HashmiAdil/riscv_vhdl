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

#ifndef __DEBUGGER_CPU_RISCV_FUNCTIONAL_H__
#define __DEBUGGER_CPU_RISCV_FUNCTIONAL_H__

#include <riscv-isa.h>
#include "instructions.h"
#include "generic/cpu_generic.h"
#include "generic/cmd_br_generic.h"
#include "cmds/cmd_br_riscv.h"
#include "coreservices/icpuriscv.h"

namespace debugger {

class CpuRiver_Functional : public CpuGeneric,
                            public ICpuRiscV {
 public:
    explicit CpuRiver_Functional(const char *name);
    virtual ~CpuRiver_Functional();

    /** IService interface */
    virtual void postinitService();
    virtual void predeleteService();

    /** IResetListener interface */
    virtual void reset(IFace *isource);

    /** ICpuGeneric interface */
    virtual void raiseSignal(int idx);
    virtual void lowerSignal(int idx);

    /** ICpuFunctional interface */
    virtual void raiseSoftwareIrq() {}
    virtual void setReg(int idx, uint64_t val) override {
        if (idx) {
            CpuGeneric::setReg(idx, val);
        }
    }
    virtual uint64_t getIrqAddress(int idx) { return readCSR(CSR_mtvec); }
    virtual void exceptionLoadInstruction(Axi4TransactionType *tr);
    virtual void exceptionLoadData(Axi4TransactionType *tr);
    virtual void exceptionStoreData(Axi4TransactionType *tr);

    /** IDPort interface */
    virtual uint64_t readCSR(uint32_t regno) override;
    virtual void writeCSR(uint32_t regno, uint64_t val) override;

    /** ICpuRiscV interface */
    virtual void mmuAddrReserve(uint64_t addr) override {
        mmuReservatedAddr_ = addr;
        mmuReservedAddrWatchdog_ = 64;
    }
    virtual bool mmuAddrRelease(uint64_t addr) override {
        bool success = 0;
        if (mmuReservedAddrWatchdog_ && mmuReservatedAddr_ == addr) {
            success = true;
            mmuReservedAddrWatchdog_ = 0;
        }
        return success;
    }

 protected:
    /** CpuGeneric common methods */
    virtual EEndianessType endianess() { return LittleEndian; }
    virtual GenericInstruction *decodeInstruction(Reg64Type *cache);
    virtual void generateIllegalOpcode();
    virtual void handleTrap();
    /** Tack Registers changes during execution */
    virtual void trackContextStart();
    /** // Stop tracking and write trace file */
    virtual void traceOutput() override;

    void addIsaUserRV64I();
    void addIsaPrivilegedRV64I();
    void addIsaExtensionA();
    void addIsaExtensionC();
    void addIsaExtensionD();
    void addIsaExtensionF();
    void addIsaExtensionM();
    unsigned addSupportedInstruction(RiscvInstruction *instr);
    uint32_t hash32(uint32_t val) { return (val >> 2) & 0x1f; }
    /** Compressed instruction */
    uint32_t hash16(uint16_t val) {
        uint32_t t1 = val & 0x3;
        return 0x20 | ((val >> 13) << 2) | t1;
    }

 private:
    AttributeType vendorid_;
    AttributeType implementationid_;
    AttributeType hartid_;
    AttributeType listExtISA_;

    static const int INSTR_HASH_TABLE_SIZE = 1 << 6;
    AttributeType listInstr_[INSTR_HASH_TABLE_SIZE];

    GenericReg64Bank portCSR_;      // Not mapped since moved to DMI, just a storage

    CmdBrRiscv *pcmd_br_;
    ICommand *pcmd_cpu_;

    uint64_t mmuReservatedAddr_;
    int mmuReservedAddrWatchdog_;   // not exceed 64 instructions between LR/SC
};

DECLARE_CLASS(CpuRiver_Functional)

}  // namespace debugger

#endif  // __DEBUGGER_CPU_RISCV_FUNCTIONAL_H__
