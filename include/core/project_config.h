// include/core/project_config.h
#ifndef PROJECT_CONFIG_H
#define PROJECT_CONFIG_H

#include "stm32f4xx_hal.h"

// SPI Pin Definitions for STM32F429I-DISC1
#define GYRO_SPI                  SPI5
#define GYRO_SPI_CLK_ENABLE()    __HAL_RCC_SPI5_CLK_ENABLE()

// SPI GPIO Configuration    
// PC1 ------> SPI5_MEMS_CS    
// PF7 ------> SPI5_SCK     
// PF8 ------> SPI5_MISO    
// PF9 ------> SPI5_MOSI  
#define GYRO_CS_PIN              GPIO_PIN_1
#define GYRO_CS_GPIO_PORT        GPIOC
#define GYRO_CS_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()

#define GYRO_SCK_PIN            GPIO_PIN_7
#define GYRO_SCK_GPIO_PORT      GPIOF
#define GYRO_SCK_CLK_ENABLE()   __HAL_RCC_GPIOF_CLK_ENABLE()

#define GYRO_MISO_PIN           GPIO_PIN_8
#define GYRO_MISO_GPIO_PORT     GPIOF
#define GYRO_MISO_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

#define GYRO_MOSI_PIN           GPIO_PIN_9
#define GYRO_MOSI_GPIO_PORT     GPIOF
#define GYRO_MOSI_CLK_ENABLE()  __HAL_RCC_GPIOF_CLK_ENABLE()

// SPI Communication Parameters
#define GYRO_SPI_TIMEOUT        1000  // Timeout in milliseconds
#define GYRO_SPI_BAUDRATE       SPI_BAUDRATEPRESCALER_8  // SPI Clock = APB2/8


// 新增LED和按钮引脚定义
#define LED_GREEN_PIN           GPIO_PIN_13
#define LED_RED_PIN            GPIO_PIN_14
#define LED_GPIO_PORT          GPIOG
#define LED_CLK_ENABLE()       __HAL_RCC_GPIOG_CLK_ENABLE()

#define BUTTON_PIN             GPIO_PIN_0
#define BUTTON_GPIO_PORT       GPIOA
#define BUTTON_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()

// 按钮相关配置
#define DEBOUNCE_DELAY         20    // ms
#define LONG_PRESS_TIME        3000  // ms

// 系统时钟配置函数声明
void SystemClock_Config(void);


#endif /* PROJECT_CONFIG_H */