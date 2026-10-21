/**
 ******************************************************************************
 * @file           : oled.h
 * @brief          : Header file thu vien dieu khien OLED SH1106 I2C 128x64
 * @author         : Do an He thong Nhung STM32
 ******************************************************************************
 * @attention
 * - Module su dung giao tiep I2C1 (PB6 - SCL, PB7 - SDA) tren STM32F103C8T6.
 * - Chip dieu khien: SH1106 (do lech 2 cot so voi SSD1306).
 ******************************************************************************
 */

#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define OLED_ADDR       (0x3C << 1)

/* Kich thuoc man hinh */
#define OLED_WIDTH      128
#define OLED_HEIGHT     64

void OLED_Init(void);
void OLED_Clear(void);
void OLED_UpdateScreen(void);
void OLED_SetCursor(uint8_t x, uint8_t y);
void OLED_WriteChar(char c, uint8_t scale);
void OLED_WriteString(const char *str, uint8_t scale);
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void OLED_DrawHLine(uint8_t x, uint8_t y, uint8_t len);

#define SH1106_Init         OLED_Init
#define SH1106_Clear        OLED_Clear
#define SH1106_UpdateScreen OLED_UpdateScreen
#define SH1106_SetCursor    OLED_SetCursor
#define SH1106_WriteChar    OLED_WriteChar
#define SH1106_WriteString  OLED_WriteString

#ifdef __cplusplus
}
#endif

#endif /* __OLED_H */
