#pragma once
#include "rhi.h"
#include <Arduino.h>
#include "epd_driver.h"
#include "utilities.h"

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

void convert1BitTo4Bit(
    const unsigned char *src,
    unsigned char *dst,
    UWORD width,
    UWORD height
)
{
    UWORD x, y;
    // 1bit 图像每行占的字节数
    UWORD srcLineBytes = (width + 7) / 8;
    // 4bit 图像每行占的字节数
    UWORD dstLineBytes = (width + 1) / 2;

    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            // 1) 读出 1bit 像素 (bit: 0 or 1)
            UBYTE bit = (
                src[(x >> 3) + y * srcLineBytes]
                >> (7 - (x & 7))
            ) & 0x01;

            // 2) 将 1bit 映射到 4bit 的值(示例：0->0x0, 1->0xF)
            UBYTE gray = bit ? 0x0F : 0x00;

            // 3) 写入 4bit 目标缓冲
            //    每两个像素(2 x)共用 1 个字节: 高 4bit + 低 4bit
            UWORD addr = (x >> 1) + y * dstLineBytes;
            if ((x & 1) == 0) {
                dst[addr] = (dst[addr] & 0xF0) | (gray & 0x0F);
            } else {
                dst[addr] = (dst[addr] & 0x0F) | (gray << 4);
            }
        }
    }
}

/**
 * @brief  将 1bpp 黑白位图数据转换为 4bpp（16级灰度）数据
 * @param  bwBitmap    [in]  Waveshare 黑白 1bpp 数据指针 (width*height/8 bytes)
 * @param  out4bpp     [out] 转换后的 4bpp 缓存(大小至少 width*height/2)
 * @param  width
 * @param  height
 * @param  invert      若 1bpp 数据中 "1=白,0=黑", 而你需要 "0=黑,1=白", 设置为 true 以翻转
 */
void convert1bppTo4bpp(
    const uint8_t* bwBitmap,
    uint8_t* out4bpp,
    int width,
    int height,
    bool invert
) {
    // out4bpp 的字节数大约 = (width * height) / 2
    // 如果总像素是奇数，还需按实际做边界处理。这里先假设 width*height 是偶数，保证两两配对。

    int outIndex = 0; // 写 out4bpp 的字节索引
    memset(out4bpp, 0, (width * height) / 2); // 可选：先清0

    for (int y = 0; y < height; y++) {
        UWORD flippedY = height - 1 - y;  
        for (int x = 0; x < width; x += 2) {
            // —— 第一个像素 (x) ——
            int pixelIndex0 = flippedY * width + x;
            int byteIndex0  = pixelIndex0 / 8;
            int bitIndex0   = 7 - (pixelIndex0 % 8); 
            // 若你的 1bpp 数据是从 bit0 -> 左侧像素，则改成 (pixelIndex0 % 8)

            uint8_t bit0 = (bwBitmap[byteIndex0] >> bitIndex0) & 1;
            // 4 bit 灰度，黑=0x0, 白=0xF（或者反过来）
            uint8_t gray0 = (bit0 ^ invert) ? 0x0 : 0xF;

            // —— 第二个像素 (x+1) ——
            // 若 width 是奇数，最后一列可能没有 (x+1)，你要额外处理
            uint8_t gray1 = 0xF; // 先默认白，若有 x+1 再覆盖
            if (x + 1 < width) {
                int pixelIndex1 = flippedY * width + (x + 1);
                int byteIndex1  = pixelIndex1 / 8;
                int bitIndex1   = 7 - (pixelIndex1 % 8);

                uint8_t bit1 = (bwBitmap[byteIndex1] >> bitIndex1) & 1;
                gray1 = (bit1 ^ invert) ? 0x0 : 0xF;
            }

            // 高 4 位放第一个像素，低 4 位放第二个像素
            uint8_t combined = (gray1 << 4) | (gray0 & 0x0F);
            out4bpp[outIndex++] = combined;
        }
    }
}


class RHI_EPDIY : public RHI
{
private:
    uint8_t *framebuffer = NULL;

public:
    void init() override
    {
        // Serial.begin(115200);
        epd_init();
        framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
        if (!framebuffer)
        {
            Serial.println("alloc memory failed !!!");
            while (1)
                ;
        }
        memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    }

    void reset() override
    {
    }

    void drawImage(const unsigned char *image_buffer) override
    {
        // Rect_t area = {
        //     .x = 0,
        //     .y = 0,
        //     .width = demo_width,
        //     .height = demo_height
        // };

        Rect_t area = {
            .x = 0,
            .y = 0,
            .width = 800,
            .height = 480
        };
        epd_poweron();
        epd_clear();

        // convert1BitTo4Bit(wifi_connect_qr, framebuffer, 800, 480);
        convert1bppTo4bpp(image_buffer, framebuffer, 800, 480, true);

        epd_draw_grayscale_image(area, (uint8_t *)framebuffer);
        epd_poweroff();
    }

    void refresh() override
    {
    }
};