#include "GYRO_DISCO_SPI.h"

GYRO_DISCO_SPI::GYRO_DISCO_SPI() : spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel)
{
}

GYRO_DISCO_SPI::~GYRO_DISCO_SPI()
{
    delete &spi;

    delete[] window_gx;
    delete[] window_gy;
    delete[] window_gz;
}

void GYRO_DISCO_SPI::init(AverageType average_type, int window_size, int timeout, bool debug)
{
    this->debug = debug;

    this->average_type = average_type;
    this->window_size = window_size;
    this->timeout = timeout;

    if (average_type == MOVING_AVERAGE)
    {
        window_gx = new float[window_size];
        window_gy = new float[window_size];
        window_gz = new float[window_size];

        for (int i = 0; i < window_size; ++i)
        {
            window_gx[i] = 0.0f;
            window_gy[i] = 0.0f;
            window_gz[i] = 0.0f;
        }
    }

    spi.format(8, 0);
    spi.frequency(1000000);

    // GYRO Configuration!
    // 1. Control Register 1
    write_buf[0] = CTRL_REG1;
    write_buf[1] = CTRL_REG1_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, callback(this, &GYRO_DISCO_SPI::spi_callback));
    flags.wait_all(SPI_FLAG);

    // 2. Control Register 4
    write_buf[0] = CTRL_REG4;
    write_buf[1] = CTRL_REG4_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, callback(this, &GYRO_DISCO_SPI::spi_callback));
    flags.wait_all(SPI_FLAG);

    // 3. Control Register 3
    write_buf[0] = CTRL_REG3;
    write_buf[1] = CTRL_REG3_CONFIG;
    spi.transfer(write_buf, 2, read_buf, 2, callback(this, &GYRO_DISCO_SPI::spi_callback));
    flags.wait_all(SPI_FLAG);

    // dUMMY BYTE for write_buf[1] --> Placeholder Value!
    // We have to send an address and a value for write operation!
    // We have the address but have to send a placeholder value as well!
    write_buf[1] = 0xFF;

    timer.start();
}

void GYRO_DISCO_SPI::read_raw()
{
    // Wait for data ready flag (timeout 500 ms)
    flags.wait_all(DATA_READY_FLAG, 500, true);

    // Read GYRO Data using SPI transfer --> 6 bytes!
    write_buf[0] = OUT_X_L | 0x80 | 0x40;
    spi.transfer(write_buf, 7, read_buf, 7, callback(this, &GYRO_DISCO_SPI::spi_callback));
    flags.wait_all(SPI_FLAG);

    // Extract raw 16-bit gyroscope data for X, Y, Z
    raw_gx = (((uint16_t)read_buf[2]) << 8) | ((uint16_t)read_buf[1]);
    raw_gy = (((uint16_t)read_buf[4]) << 8) | ((uint16_t)read_buf[3]);
    raw_gz = (((uint16_t)read_buf[6]) << 8) | ((uint16_t)read_buf[5]);
}

void GYRO_DISCO_SPI::read(float &gx, float &gy, float &gz, bool calibrate)
{
    while (std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count() < timeout)
    {
        // wait for data ready flag
    }
    flags.set(DATA_READY_FLAG);

    read_raw();
    timer.reset();

    // Convert raw data to radians per second!
    gx = ((float)raw_gx) * SCALING_FACTOR;
    gy = ((float)raw_gy) * SCALING_FACTOR;
    gz = ((float)raw_gz) * SCALING_FACTOR;

    if (calibrate)
    {
        gx -= gx_offset;
        gy -= gy_offset;
        gz -= gz_offset;
    }

    if (debug)
        printf("gx = %8.5f, gy = %8.5f, gz = %8.5f\n", gx, gy, gz);

    // Moving Average FIR
    if (average_type == MOVING_AVERAGE)
    {
        if (debug)
        {
            printf("gx = %8.5f, window_gx[%zu] = %8.5f\n", gx, window_index, window_gx[window_index]);
            printf("gx_delta = %8.5f, gy_delta = %8.5f, gz_delta = %8.5f\n", gx - window_gx[window_index], gy - window_gy[window_index], gz - window_gz[window_index]);
        }
        cache_sum_gx += (gx - window_gx[window_index]);
        cache_sum_gy += (gy - window_gy[window_index]);
        cache_sum_gz += (gz - window_gz[window_index]);
        window_gx[window_index] = gx;
        window_gy[window_index] = gy;
        window_gz[window_index] = gz;

        gx = cache_sum_gx / window_size;
        gy = cache_sum_gy / window_size;
        gz = cache_sum_gz / window_size;
        window_index = (window_index + 1) % window_size;
    }
}

void GYRO_DISCO_SPI::calibrate(int samples)
{
    printf("========== Calibrating ==========\n");
    printf("Please keep the sensor still for %d samples...\n", samples);

    float gx, gy, gz;
    float sum_gx = 0, sum_gy = 0, sum_gz = 0;

    for (int i = 0; i < samples; i++)
    {
        read(gx, gy, gz, false);
        if (debug)
            printf("Sample %d: gx: %f, gy: %f, gz: %f\n", i, gx, gy, gz);
        sum_gx += gx;
        sum_gy += gy;
        sum_gz += gz;
    }
    gx_offset = sum_gx / samples;
    gy_offset = sum_gy / samples;
    gz_offset = sum_gz / samples;

    flags.set(DATA_READY_FLAG);

    printf("========== Calibration complete! ==========\n");
    printf("X Offset: %f, Y Offset: %f, Z Offset: %f\n", gx_offset, gy_offset, gz_offset);
}

// Callback function for SPI Transfer Completion
void GYRO_DISCO_SPI::spi_callback(int event)
{
    flags.set(SPI_FLAG);
}

// Callback function for Data Ready Interrupt
void GYRO_DISCO_SPI::data_callback()
{
    flags.set(DATA_READY_FLAG);
}