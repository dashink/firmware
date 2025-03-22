#include "rhi.h"
// #include "rhi_arduino_gfx.h"
// #include "rhi_gxepd2.h"
#include "rhi_epdiy.h"

RHI *RHI::create(RHIBackend backend)
{
    switch (backend) {
        // case RHIBackend::Arduino_GFX:
        //     return new RHI_Arduino_GFX();
        // case RHIBackend::GxEPD2:
        //     return new RHI_GxEPD2();
        case RHIBackend::EPDIY:
            return new RHI_EPDIY();
        default:
            return nullptr;
    }
}
