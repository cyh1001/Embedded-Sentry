// src/drivers/button_led.cpp
#include "drivers/button_led.h"
#include "core/project_config.h"

// Constants for button debounce and long press
#define DEBOUNCE_DELAY  50    // ms
#define LONG_PRESS_TIME 3000  // ms (3 seconds)

static uint32_t lastPressTime = 0;
static uint8_t lastButtonState = 0;
static uint8_t buttonPressed = 0;

void LED_Init(void)
{
    // LED已经在MX_GPIO_Init中初始化了，这里可以为空
}

void LED_Green_On(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
}

void LED_Green_Off(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
}

void LED_Red_On(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
}

void LED_Red_Off(void)
{
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}

void LED_Green_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);
}

void LED_Red_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
}

void Button_Init(void)
{
    // 按钮已经在MX_GPIO_Init中初始化了，这里可以为空
}

uint8_t Button_Read(void)
{
    return (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET);
}

uint8_t Button_IsPressed(void)
{
    uint32_t currentTime = HAL_GetTick();
    uint8_t currentState = Button_Read();
    
    // 如果按钮状态改变且经过去抖时间
    if (currentState != lastButtonState && (currentTime - lastPressTime) > DEBOUNCE_DELAY)
    {
        lastPressTime = currentTime;
        lastButtonState = currentState;
        if (currentState == 1)  // 按下时
        {
            buttonPressed = 1;
            return 1;
        }
    }
    
    // 如果按钮已释放，重置按下标志
    if (currentState == 0)
    {
        buttonPressed = 0;
    }
    
    return 0;
}

uint8_t Button_IsLongPressed(void)
{
    uint32_t currentTime = HAL_GetTick();
    uint8_t currentState = Button_Read();
    
    // 只有当按钮持续按下时才检查长按
    if (currentState && buttonPressed)
    {
        if ((currentTime - lastPressTime) >= LONG_PRESS_TIME)
        {
            buttonPressed = 0;  // 防止重复触发
            return 1;
        }
    }
    
    return 0;
}