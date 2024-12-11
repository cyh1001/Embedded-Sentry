// /src/drivers/i3g4250d.cpp

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_spi.h"
#include "drivers/i3g4250d.h"
#include "core/project_config.h"

// 外部变量声明
extern SPI_HandleTypeDef hspi5;
#define GYRO_SPI hspi5

// CS引脚控制宏定义
#define GYRO_CS_LOW()       HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)
#define GYRO_CS_HIGH()      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET)

// 私有函数声明
static HAL_StatusTypeDef I3G4250D_ReadReg(uint8_t reg, uint8_t *data);
static HAL_StatusTypeDef I3G4250D_WriteReg(uint8_t reg, uint8_t data);

// 灵敏度系数 (dps/digit)
static float sensitivity = 0.00875f;  // 默认 ±245 dps

HAL_StatusTypeDef I3G4250D_SetODR(uint8_t odr)
{
    uint8_t ctrl1;
    
    // 读取当前CTRL_REG1值
    if(I3G4250D_ReadReg(I3G4250D_CTRL_REG1, &ctrl1) != HAL_OK)
        return HAL_ERROR;
    
    // 清除ODR位，设置新的ODR
    ctrl1 &= ~(0xF0);  // 清除ODR位
    ctrl1 |= (odr << 4);
    
    return I3G4250D_WriteReg(I3G4250D_CTRL_REG1, ctrl1);
}

HAL_StatusTypeDef I3G4250D_SetFullScale(uint8_t fs)
{
    uint8_t ctrl4;
    
    // 读取当前CTRL_REG4值
    if(I3G4250D_ReadReg(I3G4250D_CTRL_REG4, &ctrl4) != HAL_OK)
        return HAL_ERROR;
    
    // 更新灵敏度值
    switch(fs) {
        case 0: // 245 dps
            sensitivity = 0.00875f;
            break;
        case 1: // 500 dps
            sensitivity = 0.0175f;
            break;
        case 2: // 2000 dps
            sensitivity = 0.07f;
            break;
        default:
            return HAL_ERROR;
    }
    
    // 清除FS位，设置新的FS
    ctrl4 &= ~(0x30);  // 清除FS位
    ctrl4 |= (fs << 4);
    
    return I3G4250D_WriteReg(I3G4250D_CTRL_REG4, ctrl4);
}

HAL_StatusTypeDef I3G4250D_Init(void)
{
    uint8_t ctrl1 = 0x0F;    // 使能所有轴，正常模式
    uint8_t ctrl4 = 0x80;    // Block Data Update, ±245 dps
    
    // 初始化 CS 引脚
    GYRO_CS_HIGH();
    
    // 验证设备 ID
    uint8_t id;
    if(I3G4250D_ReadID(&id) != HAL_OK)
        return HAL_ERROR;
        
    if(id != I3G4250D_WHO_AM_I_VALUE)
        return HAL_ERROR;
    
    // 配置陀螺仪
    if(I3G4250D_WriteReg(I3G4250D_CTRL_REG1, ctrl1) != HAL_OK)
        return HAL_ERROR;
        
    if(I3G4250D_WriteReg(I3G4250D_CTRL_REG4, ctrl4) != HAL_OK)
        return HAL_ERROR;
    
    return HAL_OK;
}

HAL_StatusTypeDef I3G4250D_ReadID(uint8_t *id)
{
    return I3G4250D_ReadReg(I3G4250D_WHO_AM_I, id);
}

HAL_StatusTypeDef I3G4250D_ReadAxesRaw(I3G4250D_AxesRaw_t *axes)
{
    uint8_t buffer[6];
    uint8_t reg = I3G4250D_OUT_X_L | 0x80 | 0x40;  // 多字节读取
    
    GYRO_CS_LOW();
    
    if(HAL_SPI_Transmit(&GYRO_SPI, &reg, 1, 1000) != HAL_OK) {
        GYRO_CS_HIGH();
        return HAL_ERROR;
    }
    
    if(HAL_SPI_Receive(&GYRO_SPI, buffer, 6, 1000) != HAL_OK) {
        GYRO_CS_HIGH();
        return HAL_ERROR;
    }
    
    GYRO_CS_HIGH();
    
    axes->x = (int16_t)(buffer[1] << 8 | buffer[0]);
    axes->y = (int16_t)(buffer[3] << 8 | buffer[2]);
    axes->z = (int16_t)(buffer[5] << 8 | buffer[4]);
    
    return HAL_OK;
}

HAL_StatusTypeDef I3G4250D_ReadAxes(I3G4250D_Axes_t *axes)
{
    I3G4250D_AxesRaw_t raw_axes;
    
    if(I3G4250D_ReadAxesRaw(&raw_axes) != HAL_OK)
        return HAL_ERROR;
    
    axes->x = (float)raw_axes.x * sensitivity;
    axes->y = (float)raw_axes.y * sensitivity;
    axes->z = (float)raw_axes.z * sensitivity;
    
    return HAL_OK;
}

static HAL_StatusTypeDef I3G4250D_ReadReg(uint8_t reg, uint8_t *data)
{
    reg |= 0x80;  // 读操作需要设置最高位
    
    GYRO_CS_LOW();
    
    if(HAL_SPI_Transmit(&GYRO_SPI, &reg, 1, 1000) != HAL_OK) {
        GYRO_CS_HIGH();
        return HAL_ERROR;
    }
    
    if(HAL_SPI_Receive(&GYRO_SPI, data, 1, 1000) != HAL_OK) {
        GYRO_CS_HIGH();
        return HAL_ERROR;
    }
    
    GYRO_CS_HIGH();
    
    return HAL_OK;
}

static HAL_StatusTypeDef I3G4250D_WriteReg(uint8_t reg, uint8_t data)
{
    uint8_t buffer[2];
    buffer[0] = reg & 0x7F;  // 写操作需要清除最高位
    buffer[1] = data;
    
    GYRO_CS_LOW();
    
    if(HAL_SPI_Transmit(&GYRO_SPI, buffer, 2, 1000) != HAL_OK) {
        GYRO_CS_HIGH();
        return HAL_ERROR;
    }
    
    GYRO_CS_HIGH();
    
    return HAL_OK;
}