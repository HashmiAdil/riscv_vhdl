/*
 *  Copyright 2021 Sergey Khabarov, sergeykhbr@gmail.com
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

#pragma once

#include <inttypes.h>
#include <iface.h>

namespace debugger {

static const char *const IFACE_IIRQ_CONTROLLER = "IIrqController";

class IIrqController : public IFace {
 public:
    IIrqController() : IFace(IFACE_IIRQ_CONTROLLER) {}

    virtual int requestInterrupt(IFace *isrc, int idx) = 0;
};

}  // namespace debugger
