/**
 ******************************************************************************
 * @file           : oled.c
 * @brief          : Driver dieu khien truc tiep man hinh OLED SH1106 I2C
 * @author         : Do an He thong Nhung STM32
 ******************************************************************************
 */

#include "oled.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

/* Bo dem khung hinh 1024 bytes (128 cot x 64 dong / 8 bit) */
static uint8_t buffer[1024];
static uint8_t cursorX = 0;
static uint8_t cursorY = 0;

/* Bang ma font chu 5x7 chuan ASCII 32 -> 90 */
static const uint8_t font5x7[][5] =
{
    {0x00,0x00,0x00,0x00,0x00}, /* Khoang trang */
    {0x00,0x00,0x5F,0x00,0x00}, /* ! */
    {0x00,0x07,0x00,0x07,0x00}, /* " */
    {0x14,0x7F,0x14,0x7F,0x14}, /* # */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* $ */
    {0x23,0x13,0x08,0x64,0x62}, /* % */
    {0x36,0x49,0x55,0x22,0x50}, /* & */
    {0x00,0x05,0x03,0x00,0x00}, /* ' */
    {0x00,0x1C,0x22,0x41,0x00}, /* ( */
    {0x00,0x41,0x22,0x1C,0x00}, /* ) */
    {0x14,0x08,0x3E,0x08,0x14}, /* * */
    {0x08,0x08,0x3E,0x08,0x08}, /* + */
    {0x00,0x50,0x30,0x00,0x00}, /* , */
    {0x08,0x08,0x08,0x08,0x08}, /* - */
    {0x00,0x60,0x60,0x00,0x00}, /* . */
    {0x20,0x10,0x08,0x04,0x02}, /* / */

    /* 0 - 9 */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0 */
    {0x00,0x42,0x7F,0x40,0x00}, /* 1 */
    {0x42,0x61,0x51,0x49,0x46}, /* 2 */
    {0x21,0x41,0x45,0x4B,0x31}, /* 3 */
    {0x18,0x14,0x12,0x7F,0x10}, /* 4 */
    {0x27,0x45,0x45,0x45,0x39}, /* 5 */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 6 */
    {0x01,0x71,0x09,0x05,0x03}, /* 7 */
    {0x36,0x49,0x49,0x49,0x36}, /* 8 */
    {0x06,0x49,0x49,0x29,0x1E}, /* 9 */

    {0x00,0x36,0x36,0x00,0x00}, /* : */
    {0x00,0x56,0x36,0x00,0x00}, /* ; */
    {0x08,0x14,0x22,0x41,0x00}, /* < */
    {0x14,0x14,0x14,0x14,0x14}, /* = */
    {0x00,0x41,0x22,0x14,0x08}, /* > */
    {0x02,0x01,0x51,0x09,0x06}, /* ? */
    {0x32,0x49,0x79,0x41,0x3E}, /* @ */

    /* A - Z */
    {0x7E,0x11,0x11,0x11,0x7E}, /* A */
    {0x7F,0x49,0x49,0x49,0x36}, /* B */
    {0x3E,0x41,0x41,0x41,0x22}, /* C */
    {0x7F,0x41,0x41,0x22,0x1C}, /* D */
    {0x7F,0x49,0x49,0x49,0x41}, /* E */
    {0x7F,0x09,0x09,0x09,0x01}, /* F */
    {0x3E,0x41,0x49,0x49,0x7A}, /* G */
    {0x7F,0x08,0x08,0x08,0x7F}, /* H */
    {0x00,0x41,0x7F,0x41,0x00}, /* I */
    {0x20,0x40,0x41,0x3F,0x01}, /* J */
    {0x7F,0x08,0x14,0x22,0x41}, /* K */
    {0x7F,0x40,0x40,0x40,0x40}, /* L */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* M */
    {0x7F,0x04,0x08,0x10,0x7F}, /* N */
    {0x3E,0x41,0x41,0x41,0x3E}, /* O */
    {0x7F,0x09,0x09,0x09,0x06}, /* P */
    {0x3E,0x41,0x51,0x21,0x5E}, /* Q */
    {0x7F,0x09,0x19,0x29,0x46}, /* R */
    {0x46,0x49,0x49,0x49,0x31}, /* S */
    {0x01,0x01,0x7F,0x01,0x01}, /* T */
    {0x3F,0x40,0x40,0x40,0x3F}, /* U */
    {0x1F,0x20,0x40,0x20,0x1F}, /* V */
    {0x3F,0x40,0x38,0x40,0x3F}, /* W */
    {0x63,0x14,0x08,0x14,0x63}, /* X */
    {0x07,0x08,0x70,0x08,0x07}, /* Y */
    {0x61,0x51,0x49,0x45,0x43}  /* Z */
};

static void OLED_Command(uint8_t cmd)
{
    uint8_t d[2] = {0x00, cmd};
    HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, d, 2, 100);
}

void OLED_Init(void)
{
    HAL_Delay(100);

    /* Cac lenh cau hinh khoi tao chip SH1106 */
    OLED_Command(0xAE); /* Tat hien thi (Display OFF) */
    OLED_Command(0xD5); /* Set Display Clock Divide Ratio */
    OLED_Command(0x80);
    OLED_Command(0xA8); /* Set Multiplex Ratio */
    OLED_Command(0x3F); /* 64 dong (1/64 duty) */
    OLED_Command(0xD3); /* Set Display Offset */
    OLED_Command(0x00); /* 0 offset */
    OLED_Command(0x40); /* Set Display Start Line (dong 0) */
    OLED_Command(0xAD); /* Set DC-DC Control */
    OLED_Command(0x8B); /* Bat mach tang ap noi cho OLED */
    OLED_Command(0xA1); /* Segment Re-map: 0xA1 (dao chieu ngang) */
    OLED_Command(0xC8); /* COM Scan Direction: 0xC8 (dao chieu doc) */
    OLED_Command(0xDA); /* Set COM Pins Hardware Configuration */
    OLED_Command(0x12);
    OLED_Command(0x81); /* Set Contrast Control (do tuong phan) */
    OLED_Command(0x80); /* Muc do sang trung binh */
    OLED_Command(0xD9); /* Set Pre-charge Period */
    OLED_Command(0x22);
    OLED_Command(0xDB); /* Set VCOMH Deselect Level */
    OLED_Command(0x35);
    OLED_Command(0xA4); /* Toan bo diem anh hien thi theo RAM */
    OLED_Command(0xA6); /* Set Normal Display (khong dao mau) */
    OLED_Command(0xAF); /* Bat hien thi (Display ON) */

    OLED_Clear();
    OLED_UpdateScreen();
}

void OLED_Clear(void)
{
    memset(buffer, 0, sizeof(buffer));
}

void OLED_UpdateScreen(void)
{
    uint8_t data[129];
    data[0] = 0x40; /* Control byte: Data stream */

    for (uint8_t page = 0; page < 8; page++)
    {
        OLED_Command(0xB0 + page); /* Chon Page 0 -> 7 */

        /* SH1106 co RAM 132x64 nhung man hinh chi 128x64 nen phai bu lech 2 cot */
        OLED_Command(0x02); /* Column Low Address = 2 */
        OLED_Command(0x10); /* Column High Address = 0 */

        memcpy(&data[1], &buffer[page * 128], 128);

        HAL_I2C_Master_Transmit(&hi2c1, OLED_ADDR, data, 129, 100);
    }
}

void OLED_SetCursor(uint8_t x, uint8_t y)
{
    cursorX = x;
    cursorY = y;
}

void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT)
        return;

    if (color)
        buffer[x + (y / 8) * 128] |= (1 << (y % 8));
    else
        buffer[x + (y / 8) * 128] &= ~(1 << (y % 8));
}

void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        OLED_DrawPixel(x + i, y, 1);
    }
}

void OLED_WriteChar(char c, uint8_t scale)
{
    if (c < 32 || c > 90)
        c = ' ';

    const uint8_t *f = font5x7[c - 32];

    for (uint8_t col = 0; col < 5; col++)
    {
        for (uint8_t row = 0; row < 7; row++)
        {
            if (f[col] & (1 << row))
            {
                for (uint8_t sx = 0; sx < scale; sx++)
                {
                    for (uint8_t sy = 0; sy < scale; sy++)
                    {
                        uint8_t x = cursorX + col * scale + sx;
                        uint8_t y = cursorY + row * scale + sy;

                        if (x < OLED_WIDTH && y < OLED_HEIGHT)
                        {
                            buffer[x + (y / 8) * 128] |= (1 << (y % 8));
                        }
                    }
                }
            }
        }
    }

    cursorX += 6 * scale;
}

void OLED_WriteString(const char *str, uint8_t scale)
{
    while (*str)
    {
        OLED_WriteChar(*str++, scale);
    }
}
