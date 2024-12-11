/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#ifdef __cplusplus
 extern "C" {
#endif

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void EXTI2_IRQHandler(void);

/* 新增的函数声明 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */