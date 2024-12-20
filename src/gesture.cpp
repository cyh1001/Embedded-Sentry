#include "gesture.h"
#include "GYRO_DISCO_SPI.h"

#include "main.h"

bool match_gesture(double key_pattern[MATCH_SEQ_LENGTH][3], double trial_pattern[MATCH_SEQ_LENGTH][3])
{
    double mse = 0.0;

    // Calculate MSE
    for (int i = 0; i < MATCH_SEQ_LENGTH; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            // MSE
            double diff = key_pattern[i][j] - trial_pattern[i][j];
            mse += diff * diff;
        }
    }

    mse /= (MATCH_SEQ_LENGTH * 3);
    printf("MSE: %.6f\n", mse);

    double similarity = 1.0 / (1.0 + mse);
    printf("Similarity: %.6f\n", similarity);

    return similarity > MATCH_THRESHOLD;
}