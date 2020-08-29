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

#include <api_core.h>
#include "dsu_regs.h"
#include "dsu.h"

namespace debugger {

DsuRegisters::DsuRegisters(IService *parent) :
    csr_region_(parent, "csr_region", 0, 0x00000, 4096),
    reg_region_(parent, "reg_region", 1, 0x08000, 4096),
    dbg_region_(parent, "dbg_region", 2, 0x10000, 4096),
    dmcontrol_(parent, "dmcontrol", 0x18000 + 0x10*8),
    dmstatus_(parent, "dmstatus", 0x18000 + 0x11*8),
    haltsum0_(parent, "haltsum0", 0x18000 + 0x40*8),
    bus_util_(parent, "bus_util", 0x18040, 2*64) {
}

uint64_t DsuRegisters::DMCONTROL_TYPE::aboutToWrite(uint64_t new_val) {
    DSU *p = static_cast<DSU *>(parent_);
    ValueType tnew;
    ValueType tprv;
    tprv.val = value_.val;
    tnew.val = new_val;
    uint64_t hartid = (tnew.bits.hartselhi << 10) | tnew.bits.hartsello;

    if (tnew.bits.ndmreset != tprv.bits.ndmreset) {
        p->softReset(tnew.bits.ndmreset ? true: false);
    }
    if (tnew.bits.haltreq) {
        p->haltCpu(static_cast<uint32_t>(hartid));
    } else if (tnew.bits.resumereq) {
        p->resumeCpu(static_cast<uint32_t>(hartid));
    }
    return new_val;
}

uint64_t DsuRegisters::DMCONTROL_TYPE::aboutToRead(uint64_t cur_val) {
    DSU *p = static_cast<DSU *>(parent_);
    ValueType t;
    t.val = cur_val;
    t.bits.hartsello = p->getCpuContext();
    t.bits.dmactive = 1;
    return t.val;
}

uint64_t DsuRegisters::DMSTATUS_TYPE::aboutToRead(uint64_t cur_val) {
    DSU *p = static_cast<DSU *>(parent_);
    ValueType t;
    bool halted = p->isCpuHalted(p->getCpuContext());
    t.val = 0;
    t.bits.allhalted = halted;
    t.bits.anyhalted = halted;
    t.bits.allrunning = !halted;
    t.bits.anyrunning = !halted;
    t.bits.authenticated = 1;
    t.bits.version = 2;
    return t.val;
}

uint64_t DsuRegisters::HALTSUM_TYPE::aboutToRead(uint64_t cur_val) {
    DSU *p = static_cast<DSU *>(parent_);
    uint64_t ret = 0;
    for (unsigned i = 0; i < p->getCpuTotal(); i++) {
        if (p->isCpuHalted(i)) {
            ret |= 1ull << i;
        }
    }
    return ret;
}

ETransStatus
DsuRegisters::DSU_REGION_BANK64::b_transport(Axi4TransactionType *trans) {
    RISCV_error("b_transport() to debug port NOT SUPPORTED", NULL);
    trans->response = MemResp_Error;
    return TRANS_ERROR;
}

ETransStatus
DsuRegisters::DSU_REGION_BANK64::nb_transport(Axi4TransactionType *trans,
                                              IAxi4NbResponse *cb) {
    uint64_t off64 = (trans->addr - getBaseAddress());
    if (!icpu_) {
        trans->response = MemResp_Error;
        cb->nb_response(trans);
        return TRANS_ERROR;
    }

    nb_trans_.p_axi_trans = trans;
    nb_trans_.iaxi_cb = cb;

    nb_trans_.dbg_trans.write = 0;
    nb_trans_.dbg_trans.bytes = trans->xsize;
    if (trans->action == MemAction_Write) {
        nb_trans_.dbg_trans.write = 1;
        nb_trans_.dbg_trans.wdata = trans->wpayload.b64[0];
    }

    ETransStatus ret = TRANS_OK;
    nb_trans_.dbg_trans.addr = static_cast<uint16_t>(off64);
    nb_trans_.dbg_trans.region = region_id_;
    icpu_->nb_transport_debug_port(&nb_trans_.dbg_trans, this);
    return ret;
}

void DsuRegisters::DSU_REGION_BANK64::nb_response_debug_port(
                                        DebugPortTransactionType *trans) {
    nb_trans_.p_axi_trans->response = MemResp_Valid;
    nb_trans_.p_axi_trans->rpayload.b64[0] = trans->rdata;
    nb_trans_.iaxi_cb->nb_response(nb_trans_.p_axi_trans);
}


}  // namespace debugger

