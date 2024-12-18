#include "GYRO_DISCO_SPI.h"

GYRO_DISCO_SPI::GYRO_DISCO_SPI() : spi(PF_9, PF_8, PF_7, PC_1, use_gpio_ssel),
                                   window_index(0)
{
}

GYRO_DISCO_SPI::~GYRO_DISCO_SPI()
{
    delete &spi;
}

void GYRO_DISCO_SPI::init(AverageType average_type, int window_size)
{
    this->average_type = average_type;
    this->window_size = window_size;

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
}

void GYRO_DISCO_SPI::read_original(float &gx, float &gy, float &gz)
{
    // Wait for data ready flag (timeout 255 ms --> 0xFF)
    flags.wait_all(DATA_READY_FLAG, 0xFF, true);

    // Read GYRO Data using SPI transfer --> 6 bytes!
    write_buf[0] = OUT_X_L | 0x80 | 0x40;
    spi.transfer(write_buf, 7, read_buf, 7, callback(this, &GYRO_DISCO_SPI::spi_callback));
    flags.wait_all(SPI_FLAG);

    // Extract raw 16-bit gyroscope data for X, Y, Z
    raw_gx = (read_buf[2] << 8) | read_buf[1];
    raw_gy = (read_buf[4] << 8) | read_buf[3];
    raw_gz = (read_buf[6] << 8) | read_buf[5];

    // Convert raw data to radians per second!
    gx = raw_gx * SCALING_FACTOR;
    gy = raw_gy * SCALING_FACTOR;
    gz = raw_gz * SCALING_FACTOR;
}

void GYRO_DISCO_SPI::read(float &gx, float &gy, float &gz, bool calibrate)
{
    read_original(gx, gy, gz);

    if (calibrate)
    {
        gx -= gx_offset;
        gy -= gy_offset;
        gz -= gz_offset;
    }

    // Moving Average FIR
    if (average_type == MOVING_AVERAGE)
    {
        window_gx[window_index] = gx;
        window_gy[window_index] = gy;
        window_gz[window_index] = gz;
        float sum_gx = 0.0f, sum_gy = 0.0f, sum_gz = 0.0f;
        for (int i = 0; i < window_size; i++)
        {
            sum_gx += window_gx[i];
            sum_gy += window_gy[i];
            sum_gz += window_gz[i];
        }
        gx = sum_gx / window_size;
        gy = sum_gy / window_size;
        gz = sum_gz / window_size;
        window_index = (window_index + 1) % window_size;
    }
}

void GYRO_DISCO_SPI::calibrate(int samples)
{
    float gx, gy, gz;
    float sum_gx = 0.0f, sum_gy = 0.0f, sum_gz = 0.0f;
    for (int i = 0; i < samples; i++)
    {
        read_original(gx, gy, gz);
        sum_gx += gx;
        sum_gy += gy;
        sum_gz += gz;
    }
    gx_offset = sum_gx / samples;
    gy_offset = sum_gy / samples;
    gz_offset = sum_gz / samples;

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