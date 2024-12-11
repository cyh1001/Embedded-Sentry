/* Define to prevent recursive inclusion */
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_gpio.h"
#include "core/project_config.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
void Error_Handler(void);
void SystemClock_Config(void);

/* Exported variables */
extern SPI_HandleTypeDef hspi5;

#endif /* __MAIN_H */
