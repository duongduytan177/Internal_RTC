#ifndef BUTTON_H
#define BUTTON_H

#include "main.h"
#include "screen.h"
#include <stdint.h>

/* Chan phan cung */
#define PORT_A       GPIOB
#define PIN_A        GPIO_PIN_3
#define PORT_B       GPIOB
#define PIN_B        GPIO_PIN_4
#define PORT_SW      GPIOB
#define PIN_SW       GPIO_PIN_5
#define PORT_COI     GPIOA
#define PIN_COI      GPIO_PIN_12

#define LONG_PRESS_TIME  1500

/*Bien dung chung, dinh nghia trong main.c*/
extern Screen_t screen;

extern int     editHour, editMinute, editSecond, editTimeField;
extern int     alarmHour, alarmMinute;
extern uint8_t alarmEnable;
extern int     editAlarmHour, editAlarmMinute, editAlarmField;
extern uint8_t editAlarmEnable;
extern uint8_t alarmRinging, alarmTriggered;
extern int     menuItem;

/*Ham dung chung, dinh nghia trong main.c*/
void updateDisplay(void);
void Clock_GetTime(uint8_t *h, uint8_t *m, uint8_t *s);
void Clock_SetTime(uint8_t h, uint8_t m, uint8_t s);

/*API cua thu vien*/
void Button_Init(void);
void Button_Handle(void);    /* quet nut bam (goi trong while) */
void Encoder_Handle(void);   /* quet encoder (goi trong while) */

#endif
