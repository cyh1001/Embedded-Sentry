// /include/drivers/i3g4250d.h
#ifndef I3G4250D_H
#define I3G4250D_H

#include "stm32f4xx_hal.h"

// 设备地址和标识符
#define I3G4250D_WHO_AM_I_VALUE    0xD3

// 寄存器地址定义
#define I3G4250D_WHO_AM_I          0x0F
#define I3G4250D_CTRL_REG1         0x20
#define I3G4250D_CTRL_REG2         0x21
#define I3G4250D_CTRL_REG3         0x22
#define I3G4250D_CTRL_REG4         0x23
#define I3G4250D_CTRL_REG5         0x24
#define I3G4250D_OUT_X_L           0x28
#define I3G4250D_OUT_X_H           0x29
#define I3G4250D_OUT_Y_L           0x2A
#define I3G4250D_OUT_Y_H           0x2B
#define I3G4250D_OUT_Z_L           0x2C
#define I3G4250D_OUT_Z_H           0x2D

// 数据结构
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} I3G4250D_AxesRaw_t;

typedef struct {
    float x;
    float y;
    float z;
} I3G4250D_Axes_t;

// 函数声明
HAL_StatusTypeDef I3G4250D_Init(void);
HAL_StatusTypeDef I3G4250D_ReadID(uint8_t *id);
HAL_StatusTypeDef I3G4250D_ReadAxes(I3G4250D_Axes_t *axes);
HAL_StatusTypeDef I3G4250D_ReadAxesRaw(I3G4250D_AxesRaw_t *axes);

// 配置函数
HAL_StatusTypeDef I3G4250D_SetODR(uint8_t odr);
HAL_StatusTypeDef I3G4250D_SetFullScale(uint8_t fs);

#endif /* I3G4250D_H */