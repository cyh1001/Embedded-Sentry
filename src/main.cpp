#include <mbed.h>

#include "LCD_DISCO_F429ZI.h"
#include "GYRO_DISCO_SPI.h"
#include "BUTTON.h"

LCD_DISCO_F429ZI lcd;
GYRO_DISCO_SPI gyro;
ButtonPressDetector button;

DigitalOut led(LED1);

void led_blink(int times = 2)
{
    for (int i = 0; i < times; i++)
    {
        led = !led;
        thread_sleep_for(50);
    }
}

void init_lcd()
{
    lcd.Clear(LCD_COLOR_BLUE);
    lcd.SetBackColor(LCD_COLOR_BLUE);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"Program Started!", CENTER_MODE);

    thread_sleep_for(2000);

    lcd.Clear(LCD_COLOR_WHITE);
}

void test_gyro(int max_iterations)
{
    float gx, gy, gz;
    int i = 0;
    while (++i <= max_iterations)
    {
        if (button.released())
        {
            if (button.isShortPress())
            {
                printf("Button short pressed!\n");
                led_blink(2);
            }
            else
            {
                printf("Button long pressed!\n");
                led_blink(6);
                break;
            }
            button.reset();
        }

        gyro.read(gx, gy, gz, true);
        printf("%03d: gx: %8.5f, gy: %8.5f, gz: %8.5f\n", i, gx, gy, gz);
    }

    lcd.Clear(LCD_COLOR_BLUE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"Program Ended!", CENTER_MODE);

    thread_sleep_for(2000);

    lcd.Clear(LCD_COLOR_WHITE);
}

int main()
{

    init_lcd();

    gyro.init(MOVING_AVERAGE, 20, false);

    // Calibrate the gyroscope
    lcd.Clear(LCD_COLOR_BLUE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"Calibrating...", CENTER_MODE);
    gyro.calibrate(100);
    lcd.Clear(LCD_COLOR_WHITE);

    // Test the gyroscope
    test_gyro(400);

    return 0;
}