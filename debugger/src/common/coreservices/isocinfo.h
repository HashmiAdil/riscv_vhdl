/**
 * @file
 * @copyright  Copyright 2016 GNSS Sensor Ltd. All right reserved.
 * @author     Sergey Khabarov - sergeykhbr@gmail.com
 * @brief      SOC Information interface.
 */

#ifndef __DEBUGGER_ISOCINFO_H__
#define __DEBUGGER_ISOCINFO_H__

#include "iface.h"
#include "attribute.h"

namespace debugger {

static const char *IFACE_SOC_INFO = "ISocInfo";

static const uint16_t VENDOR_GNSSSENSOR        = 0x00F1;

static const uint16_t GNSSSENSOR_DUMMY         = 0x5577;
static const uint16_t GNSSSENSOR_BOOTROM       = 0x0071;
static const uint16_t GNSSSENSOR_FWIMAGE       = 0x0072;
static const uint16_t GNSSSENSOR_SRAM          = 0x0073;
static const uint16_t GNSSSENSOR_PNP           = 0x0074;
static const uint16_t GNSSSENSOR_SPI_FLASH     = 0x0075;
static const uint16_t GNSSSENSOR_GPIO          = 0x0076;
static const uint16_t GNSSSENSOR_RF_CONTROL    = 0x0077;
static const uint16_t GNSSSENSOR_ENGINE        = 0x0078;
static const uint16_t GNSSSENSOR_ENGINE_STUB   = 0x0068;
static const uint16_t GNSSSENSOR_FSE_V2        = 0x0079;
static const uint16_t GNSSSENSOR_UART          = 0x007a;
static const uint16_t GNSSSENSOR_ACCELEROMETER = 0x007b;
static const uint16_t GNSSSENSOR_GYROSCOPE     = 0x007c;
static const uint16_t GNSSSENSOR_IRQCTRL       = 0x007d;
static const uint16_t GNSSSENSOR_ETHMAC        = 0x007f;
static const uint16_t GNSSSENSOR_GPTIMERS      = 0x0081;

static const uint8_t PNP_CONFIG_DEFAULT_BYTES  = 16;

static const uint32_t TECH_INFERRED            = 0;
static const uint32_t TECH_VIRTEX6             = 36;
static const uint32_t TECH_KINTEX7             = 49;


typedef struct PnpConfigType {
    uint32_t xmask;
    uint32_t xaddr;
    uint16_t did;
    uint16_t vid;
    uint8_t size;
    uint8_t rsrv[3];
} PnpConfigType;

typedef struct PnpMapType {
    uint32_t hwid;         /// RO: HW ID
    uint32_t fwid;         /// RW: FW ID
    union TechType {
        struct bits_type {
            uint8_t tech;
            uint8_t slv_total;
            uint8_t mst_total;
            uint8_t adc_detect;
        } bits;
        uint32_t val;
    } tech;         /// RO: technology index
    uint32_t rsrv1;        /// 
    uint64_t idt;          /// 
    uint64_t malloc_addr;  /// RW: debuggind memalloc pointer 0x18
    uint64_t malloc_size;  /// RW: debugging memalloc size 0x20
    uint64_t fwdbg1;       /// RW: FW debug register
    uint64_t rsrv[2];
    PnpConfigType slaves[256];  // RO: slaves config
} PnpMapType;

struct GpioType {
    union {
        struct MapType {
            uint32_t led;
            uint32_t dip;
        } map;
        uint64_t val[1];
        uint8_t buf[8];
    } u;
};

union DsuRunControlRegType {
    struct bits_type {
        uint64_t halt     : 1;
        uint64_t stepping : 1;
        uint64_t rsv1     : 2;
        uint64_t core_id  : 16;
        uint64_t rsv2     : 44;
    } bits;
    uint64_t val;
    uint8_t  buf[8];
};

const uint64_t REG_ADDR_ERROR = 0xFFFFFFFFFFFFFFFFull;

class ISocInfo : public IFace {
public:
    ISocInfo() : IFace(IFACE_SOC_INFO) {}

    virtual unsigned getRegsTotal() =0;
    virtual void getRegsList(AttributeType *lst) =0;
    virtual unsigned getCsrTotal() =0;
    virtual void getCsrList(AttributeType *lst) =0;
    virtual uint64_t csr2addr(const char *name) =0;
    virtual uint64_t reg2addr(const char *name) =0;

    virtual uint64_t addressPlugAndPlay() =0;
    virtual uint64_t addressGpio() =0;
    virtual uint64_t addressBreakCreate() =0;
    virtual uint64_t addressBreakRemove() =0;
    virtual uint64_t addressRunControl() =0;
    virtual uint64_t addressStepCounter() =0;
    virtual uint64_t valueHalt() =0;
    virtual uint64_t valueRun() =0;
    virtual uint64_t valueRunStepping() =0;
};

}  // namespace debugger

#endif  // __DEBUGGER_ISOCINFO_H__
