#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "rp.h"

#define SAMPLES_PER_BIT 16
#define BITS_TO_COMPARE 25

#define X_IN_LENGTH 1600
#define X_OUT_LENGTH 2720

bool x_in[X_IN_LENGTH];
bool x_out[X_OUT_LENGTH];


// Function to calculate Hamming distance between two arrays
int calculate_hamming_distance(const bool *x1, const bool *x2, int length) {
    int distance = 0;

    for (int i = 0; i < length; i++) {
        if (x1[i] != x2[i]) {
            distance++;
        }
    }

    return distance;
}

int find_min_hamming_position(const bool *x_in, const bool *x_out, int in_length, int out_length) {
    int min_distance = X_OUT_LENGTH + 1;
    int min_position = -1;

    for(int i = 0; i < X_OUT_LENGTH - BITS_TO_COMPARE; i++)
    {
        int distance = calculate_hamming_distance(x_in, &x_out[i], BITS_TO_COMPARE);
        if (distance < min_distance) {
            min_distance = distance;
            min_position = i;
        }
        if(min_distance == 0) break;
    }
    return min_position;
}

/* Red Pitaya C API example of Synced Generation and acquisition
on a specific channel */


int main(int argc, char **argv){

    // Seed the random number generator
    srand(time(NULL));

    // Generate random binary data for x_out
    for (int i = 0; i < X_OUT_LENGTH; i+= SAMPLES_PER_BIT) {
        x_out[i] = rand() & 1;
        for(int j = 1; j < SAMPLES_PER_BIT; j++){
            x_out[i+j] = x_out[i];
        }
    }

    // Generate x_in by taking 25 random digits from x_out
    for (int i = 0; i < X_IN_LENGTH; i++) {
        x_in[i] = x_out[i + K];
    }
  
//C API for acquisition of the signal, and storing the buffer to find the hamming distance between x[k+1] and x[k]

    /* Print error, if rp_Init() function failed */
    if(rp_Init() != RP_OK){
        fprintf(stderr, "Rp api init failed!\n");
    }

    /* acquire the signal*/

    rp_GenOutEnable(RP_CH_1);

    /* Acquisition */
    uint32_t buff_size = 16384;
    float *buff = (float *)malloc(buff_size * sizeof(float));

    rp_AcqReset();
    rp_AcqSetDecimation(RP_DEC_1);
    rp_AcqSetTriggerLevel(RP_CH_1, 0.5);    // Trig level is set in Volts while in SCPI
    rp_AcqSetTriggerDelay(0);

    // There is an option to select coupling when using SIGNALlab 250-12
    // rp_AcqSetAC_DC(RP_CH_1, RP_AC);      // enables AC coupling on Channel 1

    // By default LV level gain is selected
    rp_AcqSetGain(RP_CH_1, RP_LOW);         // user can switch gain using this command

    rp_AcqStart();

    /* After the acquisition is started some time delay is needed to acquire fresh samples into buffer
    Here we have used a time delay of one second but you can calculate the exact value taking into account buffer
    length and sampling rate*/

    sleep(1);
    rp_AcqSetTriggerSrc(RP_TRIG_SRC_CHA_PE);
    rp_acq_trig_state_t state = RP_TRIG_STATE_TRIGGERED;

    sleep(0.5);
    rp_GenTrigger(RP_CH_1);         // Trigger generator

    while(1){
        rp_AcqGetTriggerState(&state);
        if(state == RP_TRIG_STATE_TRIGGERED){
            break;
        }
    }

    bool fillState = false;
    while(!fillState){
        rp_AcqGetBufferFillState(&fillState);
    }

    rp_AcqGetOldestDataV(RP_CH_1, &buff_size, buff);
    int i;
    for(i = 0; i < buff_size; i++){
        printf("%f\n", buff[i]);
    }

    /* Releasing resources */
    free(buff);
    rp_Release();
    return 0;
}
