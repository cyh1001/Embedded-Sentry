// #include "main.h"
// #include "drivers/i3g4250d.h"
// #include "core/project_config.h"
// #include "stm32f4xx_hal_spi.h"
// #include "drivers/button_led.h"

// /* Private variables ---------------------------------------------------------*/
// SPI_HandleTypeDef hspi5;

// /* Private function prototypes -----------------------------------------------*/
// void SystemClock_Config(void);
// static void MX_GPIO_Init(void);
// static void MX_SPI5_Init(void);

// /**
//   * @brief  The application entry point.
//   * @retval int
//   */
// int main(void)
// {
//     /* MCU Configuration--------------------------------------------------------*/
//     HAL_Init();
//     SystemClock_Config();
//     MX_GPIO_Init();
//     MX_SPI5_Init();

//     // 初始化陀螺仪
//     if (I3G4250D_Init() != HAL_OK)
//     {
//         Error_Handler();
//     }

//     I3G4250D_Axes_t axes;


//     /* Infinite loop */
//     while (1)
//     {
//         // 读取陀螺仪数据
//         if (I3G4250D_ReadAxes(&axes) == HAL_OK)
//         {
//             // 暂时不处理陀螺仪数据
//         }

//         // 检查按钮状态
//         if (Button_IsPressed())
//         {
//             LED_Green_Toggle();
//             HAL_Delay(200);  // 延时防止多次触发
//         }
        
//         if (Button_IsLongPressed())
//         {
//             LED_Red_Toggle();
//             HAL_Delay(200);  // 延时防止多次触发
//         }

//         HAL_Delay(10);  // 100Hz 采样率
//     }
// }

// /**
//   * @brief System Clock Configuration
//   * @retval None
//   */
// void SystemClock_Config(void)
// {
//     // 这里需要添加你的时钟配置代码
//     // 如果你有 STM32CubeMX 生成的代码，可以直接使用
// }

// /**
//   * @brief SPI5 Initialization Function
//   * @retval None
//   */
// static void MX_SPI5_Init(void)
// {
//     /* SPI5 配置 */
//     hspi5.Instance = SPI5;

//     /* SPI5 参数配置 */
//     hspi5.Init.Mode = SPI_MODE_MASTER;           // 主机模式
//     hspi5.Init.Direction = SPI_DIRECTION_2LINES; // 全双工
//     hspi5.Init.DataSize = SPI_DATASIZE_8BIT;     // 8位数据帧
//     hspi5.Init.CLKPolarity = SPI_POLARITY_HIGH;  // 时钟极性CPOL=1
//     hspi5.Init.CLKPhase = SPI_PHASE_2EDGE;       // 时钟相位CPHA=1
//     hspi5.Init.NSS = SPI_NSS_SOFT;               // 软件片选控制
//     hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; // 波特率分频
//     hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;      // MSB在前
//     hspi5.Init.TIMode = SPI_TIMODE_DISABLE;      // 禁用TI模式
//     hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // 禁用CRC
//     hspi5.Init.CRCPolynomial = 10;              // CRC多项式(虽然未使用)

//     /* SPI5 初始化 */
//     if (HAL_SPI_Init(&hspi5) != HAL_OK)
//     {
//         Error_Handler();
//     }
// }

// /**
//   * @brief GPIO Initialization Function
//   * @retval None
//   */
// static void MX_GPIO_Init(void)
// {
//     GPIO_InitTypeDef GPIO_InitStruct = {0};

//     /* 使能所有要用到的GPIO端口时钟 */
//     __HAL_RCC_GPIOA_CLK_ENABLE();
//     __HAL_RCC_GPIOC_CLK_ENABLE();
//     __HAL_RCC_GPIOF_CLK_ENABLE();
//     __HAL_RCC_GPIOG_CLK_ENABLE();

//     /* 配置LED引脚 */
//     HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);

//     GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
//     HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

//     /* 配置用户按钮 */
//     GPIO_InitStruct.Pin = GPIO_PIN_0;
//     GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     /* 配置陀螺仪中断引脚 */
//     GPIO_InitStruct.Pin = GPIO_PIN_2;
//     GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

//     /* 配置陀螺仪片选引脚 */
//     HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET);

//     GPIO_InitStruct.Pin = GPIO_PIN_1;
//     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
//     HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

//     /* Configure SPI5 Pins */
//     GPIO_InitStruct.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//     GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
//     HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

//     /* 使能中断 */
//     HAL_NVIC_SetPriority(EXTI2_IRQn, 0x0F, 0x00);
//     HAL_NVIC_EnableIRQ(EXTI2_IRQn);
// }

// /**
//   * @brief  This function is executed in case of error occurrence.
//   * @retval None
//   */
// void Error_Handler(void)
// {
//     /* User can add his own implementation to report the HAL error return state */
//     __disable_irq();
//     while (1)
//     {
//         // 可以添加错误指示，比如闪烁LED
//         HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14); // 闪烁红色LED
//         HAL_Delay(200);
//     }
// }
// src/main.cpp
#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    // 启用GPIOA和GPIOG的时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    // 配置LED引脚 (PG13和PG14)为输出
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    // 配置按钮引脚 (PA0)为输入
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 确保LED初始状态是关闭的
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);

    while (1)
    {
        // 读取按钮状态
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET)
        {
            // 按钮被按下时，两个LED都亮
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_SET);
        }
        else
        {
            // 按钮松开时，两个LED都灭
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);
        }

        HAL_Delay(10);  // 小延时防止CPU占用过高
    }
}

// 时钟配置函数
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 360;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

void Error_Handler(void)
{
    while(1)
    {
    }
}