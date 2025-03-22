#pragma once
#include "rhi.h"
#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_4C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <memory>
#include <string>

template<typename T>
std::string type_name();

#define REGISTER_TYPE_NAME(TYPE, NAME) \
    template<> std::string type_name<TYPE>() { return NAME; }

REGISTER_TYPE_NAME(GxEPD2_290_GDEY029T94, "GxEPD2_290_GDEY029T94")
REGISTER_TYPE_NAME(GxEPD2_213_Z98c, "GxEPD2_213_Z98c")
REGISTER_TYPE_NAME(GxEPD2_426_GDEQ0426T82, "GxEPD2_426_GDEQ0426T82")

#define DEFINE_DISPLAY_CASE(TYPE) \
    if (backend_type == type_name<TYPE>()) { \
        display = { \
            new GxEPD2_BW<TYPE, TYPE::HEIGHT>(TYPE(/*CS=*/ 6, /*DC=*/ 5, /*RST=*/ 10, /*BUSY=*/ 4)), \
            [](void* ptr) { delete reinterpret_cast<GxEPD2_BW<TYPE, TYPE::HEIGHT>*>(ptr); } \
        }; \
    }

#define HANDLE_DISPLAY(action) \
    if (!display) return; \
    if (backend_type == "GxEPD2_213_Z98c") { \
        auto* disp = reinterpret_cast<GxEPD2_BW<GxEPD2_213_Z98c, GxEPD2_213_Z98c::HEIGHT>*>(display.get()); \
        action; \
    } else if (backend_type == "GxEPD2_290_GDEY029T94") { \
        auto* disp = reinterpret_cast<GxEPD2_BW<GxEPD2_290_GDEY029T94, GxEPD2_290_GDEY029T94::HEIGHT>*>(display.get()); \
        action; \
    } else if (backend_type == "GxEPD2_426_GDEQ0426T82") { \
        auto* disp = reinterpret_cast<GxEPD2_BW<GxEPD2_426_GDEQ0426T82, GxEPD2_426_GDEQ0426T82::HEIGHT>*>(display.get()); \
        action; \
    }

class RHI_GxEPD2 : public RHI {

private:
std::string backend_type;
std::unique_ptr<void, std::function<void(void*)>> display;

public:
    void init() override {
        backend_type = "GxEPD2_426_GDEQ0426T82";

        DEFINE_DISPLAY_CASE(GxEPD2_213_Z98c)
        else DEFINE_DISPLAY_CASE(GxEPD2_290_GDEY029T94)
        else DEFINE_DISPLAY_CASE(GxEPD2_426_GDEQ0426T82)
        
        SPI.begin(7, -1, 8, 6);
        HANDLE_DISPLAY(
            disp->init(115200, true, 2, false);

            disp->setRotation(1);
            disp->setFont(&FreeMonoBold9pt7b);
            disp->setTextColor(GxEPD_BLACK);
        )
    }

    void reset() override {
    }

    void drawImage() override {
        const char HelloWorld[] = "Hello World!";
        int16_t tbx, tby; uint16_t tbw, tbh;
        HANDLE_DISPLAY(
            disp->getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
            // center the bounding box by transposition of the origin:
            uint16_t x = ((disp->width() - tbw) / 2) - tbx;
            uint16_t y = ((disp->height() - tbh) / 2) - tby;
            disp->setFullWindow();
            disp->firstPage();
            do
            {
                disp->fillScreen(GxEPD_WHITE);
                disp->setCursor(x, y);
                disp->print(HelloWorld);
            }
            while (disp->nextPage());
        )
    }

    void refresh() override {
        
    }
};