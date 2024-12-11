// src/core/stm32f4xx_it.c
#include "main.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"

// 声明外部变量
extern SPI_HandleTypeDef hspi5;

/* SPI5 MSP 初始化函数 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if(hspi->Instance == SPI5)
    {
        /* 使能时钟 */
        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_SPI5_CLK_ENABLE();

        /* SPI5 GPIO 配置 
         * PF7  ------> SPI5_SCK
         * PF8  ------> SPI5_MISO
         * PF9  ------> SPI5_MOSI
         * PC1  ------> SPI5_NSS (手动控制)
         */
        GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* NSS引脚配置为输出 */
        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        
        /* 初始状态设置NSS为高电平 */
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);
    }
}

// 中断服务函数
void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}

// 中断回调函数
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_2)
    {
        // 在这里添加你的中断处理代码
    }
}