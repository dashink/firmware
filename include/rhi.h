#pragma once

enum class RHIBackend {
    Arduino_GFX,
    GxEPD2,
    EPDIY,
};

class RHI {
public:
    virtual ~RHI() = default;
    virtual void init() = 0;
    virtual void reset() = 0;
    virtual void drawImage(const unsigned char *image_buffer) = 0;
    virtual void refresh() = 0;

    static RHI* create(RHIBackend backend);
};