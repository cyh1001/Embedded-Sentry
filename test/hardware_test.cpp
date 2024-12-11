// test/hardware_test.cpp
#include "main.h"
#include "core/project_config.h"
#include "drivers/i3g4250d.h"
#include "drivers/button_led.h"
#include <math.h>

// 定义测试阶段
typedef enum {
    TEST_LED = 0,      // LED测试阶段
    TEST_BUTTON,       // 按钮测试阶段
    TEST_GYRO,        // 陀螺仪测试阶段
} TestPhase_t;

// 全局变量
static TestPhase_t currentPhase = TEST_LED;
static uint32_t phaseStartTime = 0;
static const uint32_t LED_TEST_DURATION = 5000;  // LED测试持续5秒
static const uint32_t BUTTON_TEST_DURATION = 10000;  // 按钮测试持续10秒

// LED测试阶段，展示所有LED控制功能
void TestLED(void)
{
    uint32_t currentTime = HAL_GetTick();
    uint32_t elapsedTime = currentTime - phaseStartTime;
    
    if (elapsedTime < 1000) {  // 第1秒：绿灯亮
        LED_Green_On();
        LED_Red_Off();
    }
    else if (elapsedTime < 2000) {  // 第2秒：红灯亮
        LED_Green_Off();
        LED_Red_On();
    }
    else if (elapsedTime < 3000) {  // 第3秒：两灯都亮
        LED_Green_On();
        LED_Red_On();
    }
    else if (elapsedTime < 4000) {  // 第4秒：交替闪烁
        if ((elapsedTime % 200) < 100) {
            LED_Green_On();
            LED_Red_Off();
        } else {
            LED_Green_Off();
            LED_Red_On();
        }
    }
    else if (elapsedTime < 5000) {  // 第5秒：同时闪烁
        if ((elapsedTime % 200) < 100) {
            LED_Green_On();
            LED_Red_On();
        } else {
            LED_Green_Off();
            LED_Red_Off();
        }
    }
    else {  // 测试结束，进入下一阶段
        LED_Green_Off();
        LED_Red_Off();
        currentPhase = TEST_BUTTON;
        phaseStartTime = currentTime;
        
        // 提示进入按钮测试阶段：绿灯快闪3次
        for(int i = 0; i < 3; i++) {
            LED_Green_On();
            HAL_Delay(100);
            LED_Green_Off();
            HAL_Delay(100);
        }
    }
}

// 按钮测试阶段，测试短按和长按功能
void TestButton(void)
{
    uint32_t currentTime = HAL_GetTick();
    
    if (currentTime - phaseStartTime >= BUTTON_TEST_DURATION) {
        // 按钮测试结束，进入陀螺仪测试
        currentPhase = TEST_GYRO;
        phaseStartTime = currentTime;
        
        // 提示进入陀螺仪测试阶段：红灯快闪3次
        for(int i = 0; i < 3; i++) {
            LED_Red_On();
            HAL_Delay(100);
            LED_Red_Off();
            HAL_Delay(100);
        }
        return;
    }
    
    // 短按测试：按下点亮绿灯
    if (Button_IsPressed()) {
        LED_Green_On();
    } else {
        LED_Green_Off();
    }
    
    // 长按测试：长按时红灯闪烁
    static uint32_t lastToggleTime = 0;
    if (Button_IsLongPressed()) {
        if (currentTime - lastToggleTime >= 200) {  // 200ms闪烁频率
            LED_Red_Toggle();
            lastToggleTime = currentTime;
        }
    } else {
        LED_Red_Off();
    }
}

// 陀螺仪测试阶段，用LED指示运动
void TestGyroscope(void)
{
    static const float THRESHOLD_LOW = 30.0f;   // 较小动作阈值
    static const float THRESHOLD_HIGH = 100.0f; // 较大动作阈值
    I3G4250D_Axes_t axes;
    
    if (I3G4250D_ReadAxes(&axes) == HAL_OK) {
        float maxValue = fmax(fabs(axes.x), fmax(fabs(axes.y), fabs(axes.z)));
        
        if (maxValue > THRESHOLD_HIGH) {
            // 大幅度运动：两个LED都亮
            LED_Green_On();
            LED_Red_On();
        }
        else if (maxValue > THRESHOLD_LOW) {
            // 小幅度运动：只亮绿灯
            LED_Green_On();
            LED_Red_Off();
        }
        else {
            // 静止状态：都不亮
            LED_Green_Off();
            LED_Red_Off();
        }
    }
}

int main(void)
{
    // 初始化
    HAL_Init();
    SystemClock_Config();
    LED_Init();
    Button_Init();
    
    if (I3G4250D_Init() != HAL_OK) {
        // 初始化失败：红灯快速闪烁
        while (1) {
            LED_Red_Toggle();
            HAL_Delay(100);
        }
    }
    
    // 记录起始时间
    phaseStartTime = HAL_GetTick();
    
    while (1) {
        // 根据当前阶段执行相应测试
        switch (currentPhase) {
            case TEST_LED:
                TestLED();
                break;
                
            case TEST_BUTTON:
                TestButton();
                break;
                
            case TEST_GYRO:
                TestGyroscope();
                break;
        }
        
        HAL_Delay(10);  // 10ms延时，避免过于频繁的循环
    }
}