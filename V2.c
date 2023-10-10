#include <stdio.h>
#include <stdint.h>
#include "rp.h"

#define BUFFER_SIZE (16 * 1024) // Adjust buffer size as needed

float *x_in; // ADC waveform buffer
float *x_out; // DAC waveform buffer
uint32_t buff_size = BUFFER_SIZE;
float hamming_distance = 0.0;

float calculate_hamming_distance(float *x1, float *x2, uint32_t size) {
    if (x1 == NULL || x2 == NULL) {
        return -1.0; // Error
    }

    float distance = 0.0;

    for (uint32_t i = 0; i < size; i++) {
        distance += fabs(x1[i] - x2[i]);
    }

    return distance;
}


int main(int argc, char **argv) {
    // Initialize Red Pitaya resources
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya initialization failed!\n");
        return 1;
    }

    // Configure ADC
    rp_AcqReset();
    rp_AcqSetDecimation(1);

    // Configure DAC
    rp_GenReset();
    rp_GenWaveform(RP_CH_2, RP_WAVEFORM_SINE);
    rp_GenFreq(RP_CH_2, 1000.0); // Set DAC frequency to 1000 Hz
    rp_GenAmp(RP_CH_2, 1.0);     // Set DAC amplitude to 1.0 Vpp

    // Allocate memory for ADC and DAC buffers
    x_in = (float *)malloc(buff_size * sizeof(float));
    x_out = (float *)malloc(buff_size * sizeof(float));

    if (x_in == NULL || x_out == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        return 1;
    }
    
    // Perform data acquisition
    rp_AcqStart();
    rp_GenOutEnable(RP_CH_2);
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW);
    rp_AcqGetOldestDataV(RP_CH_1, &buff_size, x_in);
    rp_AcqStop();
    rp_GenOutDisable(RP_CH_2);
    
    // Calculate Hamming distance
    hamming_distance = calculate_hamming_distance(x_in, x_out, buff_size);
    
    // Print the Hamming distance
    printf("Hamming distance: %f\n", hamming_distance);
    
    // Release Red Pitaya resources and free memory
    rp_Release();
    free(x_in);
    free(x_out);

    return 0;
}
