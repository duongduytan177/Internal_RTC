/**
 ******************************************************************************
 * @file           : screen.h
 * @brief          : Header file quan ly giao dien nguoi dung (UI) tren OLED
 * @author         : Do an He thong Nhung STM32
 ******************************************************************************
 */

#ifndef __SCREEN_H
#define __SCREEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "oled.h"

/*Cac trang giao dien (Finite State Machine)*/
typedef enum
{
    SCREEN_CLOCK = 0,
    SCREEN_MENU,
    SCREEN_SET_TIME,
    SCREEN_SET_ALARM
} Screen_t;

/*API cua thu vien*/
void Screen_ShowWelcome(void);
void Screen_DrawClock(uint8_t h, uint8_t m, uint8_t s, int alarmHour, int alarmMinute, uint8_t alarmEnable);
void Screen_DrawMenu(int menuItem);
void Screen_DrawSetTime(int editHour, int editMinute, int editSecond, int editTimeField);
void Screen_DrawSetAlarm(int editAlarmHour, int editAlarmMinute, uint8_t editAlarmEnable, int editAlarmField);
void Screen_DrawAlarmRinging(int alarmHour, int alarmMinute);
void Screen_ShowSaved(void);

#ifdef __cplusplus
}
#endif

#endif /* __SCREEN_H */
