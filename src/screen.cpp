#include "screen.h"

LCD_DISCO_F429ZI lcd;
TS_DISCO_F429ZI ts;

//
// Display
//

void init_screen()
{
    lcd.Clear(LCD_COLOR_BLACK);
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.SetFont(&Font16);

    ts.Init(lcd.GetXSize(), lcd.GetYSize());
}

void draw_ui()
{
    lcd.Clear(LCD_COLOR_BLACK);

    // Data display area
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.DrawRect(5, 5, 230, 130);

    // Control buttons
    lcd.SetTextColor(LCD_COLOR_GREEN);
    lcd.FillRect(RECORD_BUTTON_X_MIN, BUTTON_Y_MIN, BUTTON_WIDTH, BUTTON_HEIGHT);
    lcd.SetTextColor(LCD_COLOR_BLUE);
    lcd.FillRect(VERIFY_BUTTON_X_MIN, BUTTON_Y_MIN, BUTTON_WIDTH, BUTTON_HEIGHT);

    lcd.SetBackColor(LCD_COLOR_GREEN);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.DisplayStringAt(RECORD_BUTTON_X_MIN + TEXT_X_OFFSET, BUTTON_Y_MIN + TEXT_Y_OFFSET, (uint8_t *)"RECORD", LEFT_MODE);

    lcd.SetBackColor(LCD_COLOR_BLUE);
    lcd.DisplayStringAt(VERIFY_BUTTON_X_MIN + TEXT_X_OFFSET, BUTTON_Y_MIN + TEXT_Y_OFFSET, (uint8_t *)"VERIFY", LEFT_MODE);

    lcd.SetBackColor(LCD_COLOR_BLACK);
}

void update_status(const char *status)
{
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.DisplayStringAt(0, BUTTON_Y_MIN + BUTTON_HEIGHT + TEXT_Y_OFFSET * 3, (uint8_t *)status, CENTER_MODE);
}

void show_data(const GYRO_DISCO_SPI::GyroData &motion)
{
    char buf[6][32];

    // Convert values to strings with fixed precision
    for (int i = 0; i < 3; i++)
    {
        int whole = (int)motion.data[i];
        int frac = (int)abs((motion.data[i] - whole) * 100);
        snprintf(buf[i], sizeof(buf[i]), "%d.%02d", whole, frac);
        snprintf(buf[i + 3], sizeof(buf[i + 3]), "%d", motion.raw[i]);
    }

    // Clear previous values
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_BLACK);
    lcd.FillRect(40, 10, 190, 100);

    lcd.SetTextColor(LCD_COLOR_WHITE);

    // Display readings
    const char *labels[] = {"X:", "Y:", "Z:"};
    for (int i = 0; i < 3; i++)
    {
        lcd.DisplayStringAt(10, 20 + i * 30, (uint8_t *)labels[i], LEFT_MODE);
        lcd.DisplayStringAt(40, 20 + i * 30, (uint8_t *)buf[i], LEFT_MODE);
        lcd.DisplayStringAt(130, 20 + i * 30, (uint8_t *)buf[i + 3], LEFT_MODE);
    }
}

void show_count(uint8_t count)
{
    lcd.SetBackColor(LCD_COLOR_BLACK);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    char countStr[10];
    sprintf(countStr, "%d", count);
    lcd.SetFont(&Font20);
    lcd.DisplayStringAt(0, BUTTON_Y_MIN + BUTTON_HEIGHT + TEXT_Y_OFFSET, (uint8_t *)countStr, CENTER_MODE);
    lcd.SetFont(&Font16);
}

void show_result(bool success)
{
    if (success)
    {
        lcd.Clear(LCD_COLOR_GREEN); // Green background for success
        lcd.SetBackColor(LCD_COLOR_WHITE);
        lcd.SetTextColor(LCD_COLOR_GREEN);
        lcd.DisplayStringAt(0, 120, (uint8_t *)"SUCCESS!", CENTER_MODE);
    }
    else
    {
        lcd.Clear(LCD_COLOR_RED); // Red background for failure
        lcd.SetBackColor(LCD_COLOR_WHITE);
        lcd.SetTextColor(LCD_COLOR_RED);
        lcd.DisplayStringAt(0, 120, (uint8_t *)"FAILED!", CENTER_MODE);
    }

    // Wait for 2 seconds to allow the user to see the result
    thread_sleep_for(2000);

    // After the message is shown, reset the display and draw the main UI again
    init_screen();
    draw_ui();
}

//
// Touchscreen
//

uint16_t map_touch_x(uint16_t x)
{
    return x;
}

uint16_t map_touch_y(uint16_t y)
{
    return lcd.GetYSize() - y;
}

void process_touch(bool pattern_stored, bool &is_capturing, bool &is_matching, uint8_t &sample_count)
{
    TS_StateTypeDef state;
    ts.GetState(&state);

    if (!state.TouchDetected)
        return;

    uint16_t x = map_touch_x(state.X);
    uint16_t y = map_touch_y(state.Y);

    printf("Touch detected at X: %d, Y: %d (mapped)\n", x, y);

    if (x >= RECORD_BUTTON_X_MIN && x <= RECORD_BUTTON_X_MIN + BUTTON_WIDTH && y >= BUTTON_Y_MIN && y <= BUTTON_Y_MIN + BUTTON_HEIGHT)
    {
        is_capturing = true;
        sample_count = 0;
        printf("Record button pressed\n");
        update_status("Recording...");
    }

    else if (x >= VERIFY_BUTTON_X_MIN && x <= VERIFY_BUTTON_X_MIN + BUTTON_WIDTH && y >= BUTTON_Y_MIN && y <= BUTTON_Y_MIN + BUTTON_HEIGHT)
    {
        if (!pattern_stored)
        {
            update_status("Record pattern first!");
            thread_sleep_for(1500);
            draw_ui();
            return;
        }
        is_matching = true;
        sample_count = 0;
        printf("Verify button pressed\n");
        update_status("Verifying...");
    }
}