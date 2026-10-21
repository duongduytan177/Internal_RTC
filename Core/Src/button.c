#include "button.h"

/* Trang thai Encoder */
static GPIO_PinState lastEncoderA;
static uint32_t lastEncoderTime = 0;

/* Trang thai Nut bam */
static GPIO_PinState lastButtonState = GPIO_PIN_SET;
static uint32_t buttonPressTime = 0;
static uint8_t  longPressDone = 0;

/*ENCODER*/
static int8_t readEncoder(void)
{
    GPIO_PinState currentA = HAL_GPIO_ReadPin(PORT_A, PIN_A);
    int8_t change = 0;

    if (currentA != lastEncoderA)
    {
        if (currentA == GPIO_PIN_SET)
        {
            if ((HAL_GetTick() - lastEncoderTime) > 3)
            {
                lastEncoderTime = HAL_GetTick();

                if (HAL_GPIO_ReadPin(PORT_B, PIN_B) == GPIO_PIN_RESET)
                    change = 1;
                else
                    change = -1;
            }
        }
        lastEncoderA = currentA;
    }
    return change;
}

void Encoder_Handle(void)
{
    int8_t change = readEncoder();

    if (change == 0)
        return;

    if (screen == SCREEN_MENU)
    {
        menuItem += change;
        if (menuItem > 2) menuItem = 0;
        if (menuItem < 0) menuItem = 2;
        updateDisplay();
        return;
    }

    if (screen == SCREEN_SET_TIME)
    {
        if (editTimeField == 0)
        {
            editHour += change;
            if (editHour > 23) editHour = 0;
            if (editHour < 0)  editHour = 23;
        }
        else if (editTimeField == 1)
        {
            editMinute += change;
            if (editMinute > 59) editMinute = 0;
            if (editMinute < 0)  editMinute = 59;
        }
        else
        {
            editSecond += change;
            if (editSecond > 59) editSecond = 0;
            if (editSecond < 0)  editSecond = 59;
        }
        updateDisplay();
        return;
    }

    if (screen == SCREEN_SET_ALARM)
    {
        if (editAlarmField == 0)
        {
            editAlarmHour += change;
            if (editAlarmHour > 23) editAlarmHour = 0;
            if (editAlarmHour < 0)  editAlarmHour = 23;
        }
        else if (editAlarmField == 1)
        {
            editAlarmMinute += change;
            if (editAlarmMinute > 59) editAlarmMinute = 0;
            if (editAlarmMinute < 0)  editAlarmMinute = 59;
        }
        else
        {
            editAlarmEnable = !editAlarmEnable;
        }
        updateDisplay();
    }
}

/*NUT BAM*/
static void shortPress(void)
{
    uint8_t h, m, s;

    if (alarmRinging)
    {
        alarmRinging = 0;
        HAL_GPIO_WritePin(PORT_COI, PIN_COI, GPIO_PIN_RESET);
        screen = SCREEN_CLOCK;
        updateDisplay();
        return;
    }

    if (screen == SCREEN_CLOCK)
    {
        menuItem = 0;
        screen = SCREEN_MENU;
        updateDisplay();
        return;
    }

    if (screen == SCREEN_MENU)
    {
        if (menuItem == 0)
        {
            Clock_GetTime(&h, &m, &s);
            editHour = h;
            editMinute = m;
            editSecond = s;
            editTimeField = 0;
            screen = SCREEN_SET_TIME;
        }
        else if (menuItem == 1)
        {
            editAlarmHour = alarmHour;
            editAlarmMinute = alarmMinute;
            editAlarmEnable = alarmEnable;
            editAlarmField = 0;
            screen = SCREEN_SET_ALARM;
        }
        else
        {
            screen = SCREEN_CLOCK;
        }
        updateDisplay();
        return;
    }

    if (screen == SCREEN_SET_TIME)
    {
        editTimeField++;
        if (editTimeField > 2) editTimeField = 0;
        updateDisplay();
        return;
    }

    if (screen == SCREEN_SET_ALARM)
    {
        editAlarmField++;
        if (editAlarmField > 2) editAlarmField = 0;
        updateDisplay();
        return;
    }
}

static void longPress(void)
{
    if (screen == SCREEN_SET_TIME)
    {
        Clock_SetTime(editHour, editMinute, editSecond);
        Screen_ShowSaved();
        screen = SCREEN_MENU;
        menuItem = 0;
        updateDisplay();
        return;
    }

    if (screen == SCREEN_SET_ALARM)
    {
        alarmHour = editAlarmHour;
        alarmMinute = editAlarmMinute;
        alarmEnable = editAlarmEnable;
        alarmTriggered = 0;
        Screen_ShowSaved();
        screen = SCREEN_MENU;
        menuItem = 1;
        updateDisplay();
        return;
    }

    if (screen == SCREEN_MENU)
    {
        screen = SCREEN_CLOCK;
        updateDisplay();
    }
}

void Button_Handle(void)
{
    GPIO_PinState buttonState = HAL_GPIO_ReadPin(PORT_SW, PIN_SW);

    if ((buttonState == GPIO_PIN_RESET) && (lastButtonState == GPIO_PIN_SET))
    {
        buttonPressTime = HAL_GetTick();
        longPressDone = 0;
    }

    if ((buttonState == GPIO_PIN_RESET) && (!longPressDone))
    {
        if ((HAL_GetTick() - buttonPressTime) >= LONG_PRESS_TIME)
        {
            longPressDone = 1;
            longPress();
        }
    }

    if ((buttonState == GPIO_PIN_SET) && (lastButtonState == GPIO_PIN_RESET))
    {
        uint32_t pressDuration = HAL_GetTick() - buttonPressTime;

        if ((pressDuration > 30) && (pressDuration < LONG_PRESS_TIME) && (!longPressDone))
        {
            shortPress();
        }
    }

    lastButtonState = buttonState;
}

void Button_Init(void)
{
    lastEncoderA    = HAL_GPIO_ReadPin(PORT_A, PIN_A);
    lastButtonState = HAL_GPIO_ReadPin(PORT_SW, PIN_SW);
}
