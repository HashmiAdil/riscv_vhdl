/**
 * @file
 * @copyright  Copyright 2016 GNSS Sensor Ltd. All right reserved.
 * @author     Sergey Khabarov - sergeykhbr@gmail.com
 * @brief      Plug'n'Play device functional model.
 */

#ifndef __DEBUGGER_SOCSIM_PLUGIN_PNP_H__
#define __DEBUGGER_SOCSIM_PLUGIN_PNP_H__

#include "iclass.h"
#include "iservice.h"
#include "coreservices/imemop.h"
#include "coreservices/iirq.h"
#include "debug/dsumap.h"
#include "periphmap.h"

namespace debugger {

class PNP : public IService, 
            public IMemoryOperation {
public:
    PNP(const char *name);
    ~PNP();

    /** IService interface */
    virtual void postinitService();

    /** IMemoryOperation */
    virtual ETransStatus b_transport(Axi4TransactionType *trans);

private:
    void addMaster(unsigned idx, unsigned vid, unsigned did);
    void addSlave(uint64_t addr, uint64_t size, unsigned irq,
                  unsigned vid, unsigned did);

    AttributeType tech_;
    AttributeType irqController_;
    AttributeType irqId_;
    AttributeType adc_detector_;

    IIrqController *iirq_;

    PnpMapType regs_;
    union DescriptorTableType {
        union DescriptorItemType {
            MasterConfigType mst;
            SlaveConfigType slv;
        } *item;
        uint8_t *buf;
    } iter_;
};

DECLARE_CLASS(PNP)

}  // namespace debugger

#endif  // __DEBUGGER_SOCSIM_PLUGIN_PNP_H__
