// include/drivers/button_led.h
#ifndef BUTTON_LED_H
#define BUTTON_LED_H

#include "stm32f4xx_hal.h"

// LED control
void LED_Init(void);
void LED_Green_On(void);
void LED_Green_Off(void);
void LED_Red_On(void);
void LED_Red_Off(void);
void LED_Green_Toggle(void);
void LED_Red_Toggle(void);

// Button control
void Button_Init(void);
uint8_t Button_Read(void);
uint8_t Button_ReadWithDebounce(void);
uint8_t Button_IsPressed(void);
uint8_t Button_IsLongPressed(void);

#endif /* BUTTON_LED_H */