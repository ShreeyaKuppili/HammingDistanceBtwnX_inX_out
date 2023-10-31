#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rp.h"

#define BUFFER_SIZE (10000)
#define SAMPLES_PER_BIT 16
#define BITS_TO_COMPARE 25

// Define the lengths for x_in and x_out
#define X_LENGTH 4000

float x_in[X_LENGTH];
float x_out[X_LENGTH];
uint32_t buff_size = BUFFER_SIZE;

// Function to calculate Hamming distance between two arrays
int calculate_hamming_distance(const float *x1, const float *x2, int length) {
    int distance = 0;

    for (int i = 0; i < length; i++) {
        if (x1[i] != x2[i]) {
            distance++;
        }
    }

    return distance;
}

int main(int argc, char **argv) {
    // Seed the random number generator
    srand(time(NULL));

    // Initialize Red Pitaya resources
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya initialization failed!\n");
        return 1;
    }

    // Configure ADC
    rp_AcqReset();
    rp_AcqSetDecimation(1);

    // Generate random data for DAC (x_out)
    for (int i = 0; i < X_LENGTH; i++) {
        x_out[i] = (float)rand() / RAND_MAX; // Generates random floating-point values between 0 and 1
    }

    // Perform data acquisition from Channel 1 (RP_CH_1) and output it to Channel 2 (RP_CH_2)
    rp_GenReset();
    rp_GenAmp(RP_CH_1, 1.0); // Set amplitude for channel 1
    rp_GenWaveform(RP_CH_1, RP_WAVEFORM_ARBITRARY); // Set waveform to arbitrary
    rp_GenArbWaveform(RP_CH_1, x_out, X_LENGTH); // Load x_out data to channel 1
    rp_GenFreq(RP_CH_1, 1.0); // Set frequency for channel 1

    rp_GenAmp(RP_CH_2, 1.0); // Set amplitude for channel 2
    rp_GenWaveform(RP_CH_2, RP_WAVEFORM_ARBITRARY); // Set waveform to arbitrary
    rp_GenArbWaveform(RP_CH_2, x_out, X_LENGTH); // Load x_out data to channel 2
    rp_GenFreq(RP_CH_2, 1.0); // Set frequency for channel 2

    rp_GenOutEnable(RP_CH_1); // Enable output on channel 1
    rp_GenOutEnable(RP_CH_2); // Enable output on channel 2

    rp_AcqStart();
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW);

    if (rp_AcqGetOldestDataV(RP_CH_1, &buff_size, x_in) != RP_OK) {
        fprintf(stderr, "Data acquisition from channel 1 failed!\n");
        return 1;
    }

    rp_GenOutDisable(RP_CH_1); // Disable output on channel 1

    if (rp_AcqGetOldestDataV(RP_CH_2, &buff_size, x_in) != RP_OK) {
        fprintf(stderr, "Data acquisition from channel 2 failed!\n");
        return 1;
    }

    rp_AcqStop();

    int min_distance = calculate_hamming_distance(x_in, x_out, X_LENGTH);

    printf("Minimum Hamming Distance: %d\n", min_distance);

    // Release Red Pitaya resources (no need to free x_in as it's not dynamically allocated)
    rp_Release();

    return 0;
}
