/**
 ******************************************************************************
 * @file           : screen.c
 * @brief          : Trien khai cac man hinh hien thi UI tren OLED SH1106
 * @author         : Do an He thong Nhung STM32
 ******************************************************************************
 */

#include "screen.h"
#include <stdio.h>

static char textBuffer[32];

/* =========================================================================
 * MAN HINH KHOI DONG (WELCOME)
 * ========================================================================= */
void Screen_ShowWelcome(void)
{
    OLED_Clear();

    OLED_SetCursor(20, 16);
    OLED_WriteString("ALARM CLOCK", 1);

    OLED_SetCursor(20, 36);
    OLED_WriteString("RTC STM32F103", 1);

    OLED_UpdateScreen();
    HAL_Delay(1000);
}

/* =========================================================================
 * MAN HINH DONG HO CHINH
 * ========================================================================= */
void Screen_DrawClock(uint8_t h, uint8_t m, uint8_t s, int alarmHour, int alarmMinute, uint8_t alarmEnable)
{
    OLED_Clear();

    /* HEADER */
    OLED_SetCursor(28, 1);
    OLED_WriteString("ALARM CLOCK", 1);

    OLED_SetCursor(0, 11);
    OLED_WriteString("---------------------", 1);

    /* GIO PHUT GIAY CHU TO (SCALE = 2) */
    sprintf(textBuffer, "%02u:%02u:%02u", h, m, s);
    OLED_SetCursor(16, 22);
    OLED_WriteString(textBuffer, 2);

    /* TRANG THAI BAO THUC */
    sprintf(textBuffer, "AL: %02d:%02d %s", alarmHour, alarmMinute, alarmEnable ? "ON" : "OFF");
    OLED_SetCursor(19, 51);
    OLED_WriteString(textBuffer, 1);

    OLED_UpdateScreen();
}

/* =========================================================================
 * MAN HINH MENU CAI DAT
 * ========================================================================= */
void Screen_DrawMenu(int menuItem)
{
    OLED_Clear();

    OLED_SetCursor(40, 1);
    OLED_WriteString("CAI DAT", 1);

    OLED_SetCursor(0, 11);
    OLED_WriteString("---------------------", 1);

    /* MUC 1: GIO THUC */
    OLED_SetCursor(10, 20);
    if (menuItem == 0)
        OLED_WriteString("> GIO THUC", 1);
    else
        OLED_WriteString("  GIO THUC", 1);

    /* MUC 2: BAO THUC */
    OLED_SetCursor(10, 35);
    if (menuItem == 1)
        OLED_WriteString("> BAO THUC", 1);
    else
        OLED_WriteString("  BAO THUC", 1);

    /* MUC 3: THOAT */
    OLED_SetCursor(10, 50);
    if (menuItem == 2)
        OLED_WriteString("> THOAT", 1);
    else
        OLED_WriteString("  THOAT", 1);

    OLED_UpdateScreen();
}

/* =========================================================================
 * MAN HINH CAI DAT GIO THUC
 * ========================================================================= */
void Screen_DrawSetTime(int editHour, int editMinute, int editSecond, int editTimeField)
{
    OLED_Clear();

    OLED_SetCursor(39, 1);
    OLED_WriteString("GIO THUC", 1);

    OLED_SetCursor(0, 11);
    OLED_WriteString("---------------------", 1);

    sprintf(textBuffer, "%02d:%02d:%02d", editHour, editMinute, editSecond);
    OLED_SetCursor(16, 22);
    OLED_WriteString(textBuffer, 2);

    /* CON TRO GACH CHAN CHON TRUONG CAN CHINH */
    if (editTimeField == 0)
    {
        OLED_SetCursor(16, 42);
        OLED_WriteString("--", 1);
    }
    else if (editTimeField == 1)
    {
        OLED_SetCursor(52, 42);
        OLED_WriteString("--", 1);
    }
    else
    {
        OLED_SetCursor(88, 42);
        OLED_WriteString("--", 1);
    }

    OLED_SetCursor(7, 54);
    OLED_WriteString("NHAN:TIEP GIU:LUU", 1);

    OLED_UpdateScreen();
}

/* =========================================================================
 * MAN HINH CAI DAT BAO THUC
 * ========================================================================= */
void Screen_DrawSetAlarm(int editAlarmHour, int editAlarmMinute, uint8_t editAlarmEnable, int editAlarmField)
{
    OLED_Clear();

    OLED_SetCursor(39, 1);
    OLED_WriteString("BAO THUC", 1);

    OLED_SetCursor(0, 11);
    OLED_WriteString("---------------------", 1);

    sprintf(textBuffer, "%02d:%02d %s", editAlarmHour, editAlarmMinute, editAlarmEnable ? "ON" : "OFF");
    OLED_SetCursor(16, 22);
    OLED_WriteString(textBuffer, 2);

    /* CON TRO GACH CHAN CHON TRUONG CAN CHINH */
    if (editAlarmField == 0)
    {
        OLED_SetCursor(16, 42);
        OLED_WriteString("--", 1);
    }
    else if (editAlarmField == 1)
    {
        OLED_SetCursor(52, 42);
        OLED_WriteString("--", 1);
    }
    else
    {
        OLED_SetCursor(88, 42);
        OLED_WriteString("----", 1);
    }

    OLED_SetCursor(7, 54);
    OLED_WriteString("NHAN:TIEP GIU:LUU", 1);

    OLED_UpdateScreen();
}

/* =========================================================================
 * MAN HINH KHI CHUONG BAO THUC DANG REO
 * ========================================================================= */
void Screen_DrawAlarmRinging(int alarmHour, int alarmMinute)
{
    OLED_Clear();

    OLED_SetCursor(25, 5);
    OLED_WriteString("BAO THUC!", 1);

    sprintf(textBuffer, "%02d:%02d", alarmHour, alarmMinute);
    OLED_SetCursor(34, 23);
    OLED_WriteString(textBuffer, 2);

    OLED_SetCursor(18, 52);
    OLED_WriteString("NHAN DE TAT COI", 1);

    OLED_UpdateScreen();
}

/* =========================================================================
 * THONG BAO DA LUU
 * ========================================================================= */
void Screen_ShowSaved(void)
{
    OLED_Clear();

    OLED_SetCursor(42, 25);
    OLED_WriteString("DA LUU", 1);

    OLED_UpdateScreen();
    HAL_Delay(500);
}
