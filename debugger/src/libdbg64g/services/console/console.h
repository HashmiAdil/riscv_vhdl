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

#ifndef __DEBUGGER_CONSOLE_H__
#define __DEBUGGER_CONSOLE_H__

#include "iclass.h"
#include "iservice.h"
#include "ihap.h"
#include "coreservices/ithread.h"
#include "coreservices/iserial.h"
#include "coreservices/iclock.h"
#include "coreservices/iautocomplete.h"
#include "coreservices/icmdexec.h"
#include "coreservices/isrccode.h"
#include "coreservices/irawlistener.h"
#include <string>
//#define DBG_ZEPHYR

namespace debugger {

class ConsoleService : public IService,
                       public IThread,
                       public IHap,
                       public IRawListener,
                       public IClockListener {
public:
    explicit ConsoleService(const char *name);
    virtual ~ConsoleService();

    /** IService interface */
    virtual void postinitService();
    virtual void predeleteService();

    /** IHap */
    virtual void hapTriggered(IFace *isrc, EHapType type, const char *descr);

    /** IRawListener (default stream) */
    virtual void updateData(const char *buf, int buflen);

    /** IClockListener */
    virtual void stepCallback(uint64_t t);

protected:
    /** IThread interface */
    virtual void busyLoop();

private:
    friend class RawPortType;
    void writeBuffer(const char *buf);
    bool isData();
    uint32_t getData();
    void clearLine(int num);
    void processCommandLine();

    class RawPortType : public IRawListener {
    public:
        RawPortType(ConsoleService *parent, const char *name, bool wait_line = false) {
            parent_ = parent;
            waitLine_ = wait_line;
            if (name[0]) {
                name_ = "<" + std::string(name) + "> ";
            } else {
                name_ = "";
            }
            outdata_ = "";
        }

        // Fake IService method:
        virtual IFace *getInterface(const char *name) {
            if (strcmp(name, IFACE_RAW_LISTENER) == 0) {
                return static_cast<IRawListener *>(this);
            } else {
                return parent_->getInterface(name);
            }
        }
        // IRawListener
        virtual void updateData(const char *buf, int buflen) {
            if (!waitLine_) {
                outdata_ = name_ + std::string(buf);
                parent_->writeBuffer(outdata_.c_str());
                return;
            }
            for (int i = 0; i < buflen; i++) {
                if (buf[i] == '\r' || buf[i] == '\n') {
                    if (outdata_.size()) {
                            outdata_ = name_ + outdata_ + "\n";
                        parent_->writeBuffer(outdata_.c_str());
                    }
                    outdata_.clear();
                } else {
                    outdata_ += buf[i];
                }
            }
        }

    private:
        ConsoleService *parent_;
        std::string name_;
        std::string outdata_;
        bool waitLine_;
    };

private:
    AttributeType isEnable_;
    AttributeType stepQueue_;
    AttributeType autoComplete_;
    AttributeType cmdexec_;
    AttributeType defaultLogFile_;
    AttributeType signals_;
    AttributeType inPort_;

    event_def config_done_;
    mutex_def mutexConsoleOutput_;
    IClock *iclk_;
    IAutoComplete *iautocmd_;
    ICmdExecutor *iexec_;
    ISourceCode *isrc_;

    RawPortType portSerial_;

    AttributeType cursor_;
    std::string cmdLine_;
    unsigned cmdSizePrev_;  // used to clear symbols if string shorter 
                            // than previous

#ifdef DBG_ZEPHYR
    int tst_cnt_;
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
#else
    struct termios original_settings_;
    int term_fd_;
#endif
};

DECLARE_CLASS(ConsoleService)

}  // namespace debugger

#endif  // __DEBUGGER_CONSOLE_H__
