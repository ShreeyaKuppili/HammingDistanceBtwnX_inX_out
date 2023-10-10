#include <stdio.h>
#include <stdint.h>
#include "rp.h"

#define BUFFER_SIZE (16 * 1024) // Adjust buffer size as needed

float *x_in; // ADC waveform buffer
float *x_out; // DAC waveform buffer
uint32_t buff_size = BUFFER_SIZE;
float hamming_distance = 0.0;

// Function to calculate Hamming distance between two strings of equal length
int hamming_distance(const char *s1, const char *s2, int length) {
    int distance = 0;
    for (int i = 0; i < length; i++) {
        if (s1[i] != s2[i]) {
            distance++;
        }
    }
    return distance;
}

int find_min_hamming_position(const char *x_in, const char *x_out) {
    int min_distance = 25;  // Initialize with the maximum possible Hamming distance
    int min_position = -1; // Initialize with an invalid position

    // Ensure x_out has at least 25 characters
    if (strlen(x_out) < 25) {
        printf("x_out should have at least 25 characters.\n");
        return -1;
    }

    int length = 25; // Length of the substrings to compare
    int num_subsets = strlen(x_out) - length + 1; // Number of possible subsets

    for (int i = 0; i < num_subsets; i++) {
        const char *substring = x_out + i; // Pointer to the current substring of x_out
        int distance = hamming_distance(x_in, substring, length);

        // Check if the current Hamming distance is less than the minimum found so far
        if (distance < min_distance) {
            min_distance = distance;
            min_position = i;
        }
    }

    return min_position;
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
    
    int position = find_min_hamming_position(x_in, x_out);

    if (position != -1) {
        printf("Position of the substring with the least Hamming distance: %d\n", position);
    } else {
        printf("No valid position found.\n");
    }
    
    // Release Red Pitaya resources and free memory
    rp_Release();
    free(x_in);
    free(x_out);

    return 0;
}
