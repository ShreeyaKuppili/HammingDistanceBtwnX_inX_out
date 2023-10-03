#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <pthread.h>

#include "rp.h"

// Constants
#define M_PI 3.14159265358979323846
#define BUFFER_SIZE (16 * 1024)
#define SYNC_BUFFER_SIZE 100

// Global variables
float *x_in;
float *x_out;
int buff_size = BUFFER_SIZE;
int trig_delay;
float ascale = 1.005;
int p_step = 1000;
int t1 = 7830;  //16384+3900;
int t2 = 8150;  //3900+16384+1200;
int breps = 1;  // 3
int bcounts = 1;

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
    int min_distance = 4;  // Initialize with the maximum possible Hamming distance
    int min_position = -1; // Initialize with an invalid position

    // Ensure x_out has at least 4 characters
    if (strlen(x_out) < 4) {
        printf("x_out should have at least 4 characters.\n");
        return -1;
    }

    int length = 4; // Length of the substrings to compare
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

static void acquire_data() {
    // Initialize and allocate memory for x_in and x_out
    x_out = (float *)calloc(buff_size, sizeof(float));
    x_in = (float *)calloc(buff_size, sizeof(float));

    // Initialization of Red Pitaya API
    if (rp_Init() != RP_OK) {
        fprintf(stderr, "Red Pitaya API init failed!\n");
        return;
    } else {
        printf("Initialization of Red Pitaya API done!\n");
    }

    // Configure Red Pitaya settings (channel, voltage range, etc.)
    // Make sure to configure your Red Pitaya setup here

    // Configure ADC
    rp_AcqReset();
    rp_AcqSetDecimation(1);

    // Configure DAC

    // Set arbitrary waveform
    rp_GenWaveform(RP_CH_2, RP_WAVEFORM_ARBITRARY);

    // Enable burst mode
    rp_GenMode(RP_CH_2, RP_GEN_MODE_BURST);

    // Set burst count and burst repetitions
    rp_GenBurstCount(RP_CH_2, bcounts);
    rp_GenBurstRepetitions(RP_CH_2, breps);

    // Set Generation Frequency
    rp_GenFreq(RP_CH_2, freq);

    // Perform data acquisition

    // Fill DAC buffer
    rp_GenArbWaveform(RP_CH_2, x_out, buff_size);

    // Reset Acquisition to Defaults
    rp_AcqReset();

    // Configure acquisition
    rp_AcqSetTriggerDelay(trig_delay);

    // Start Acquisition
    rp_AcqStart();

    // Start DAC Operation
    rp_GenOutEnable(RP_CH_2);

    // Trigger immediately
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_NOW);

    rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;

    // Wait for buffer to get full after Trigger.
    do {
        rp_AcqGetTriggerState(&state);
    } while (state == RP_TRIG_STATE_TRIGGERED);

    // Get data from buffer to code
    rp_AcqGetOldestDataV(RP_CH_1, &buff_size, x_in);

    // Stop acquisition
    rp_AcqStop();

    // Disable output
    rp_GenOutDisable(RP_CH_2);

    // Set new trigger delay
    trig_delay = min_position;

    // Release Red Pitaya resources
    rp_Release();

    // Process acquired data if needed
    int position = find_min_hamming_position(x_in, x_out);

    if (position != -1) {
        printf("Position of the substring with the least Hamming distance: %d\n", position);
    } else {
        printf("No valid position found.\n");
    }

    // Free allocated memory
    free(x_in);
    free(x_out);
}

int main() {
    // Call the data acquisition function
    acquire_data();

    return 0;
}
