# HammingDistanceBtwnX_inX_out
This is a Code Repository with the end goal of finding out the delay in the system due to optics, using the Hamming Distance between the input signal and output.

What the Code is meant to do:
Create subsets of 25 units of the binary input sent through hardware cables in OUT_2 and traverse it along the signal received in OUT_1 to find the position of minimum hamming distance.
This position is then kept as the starting pointer in case of time delay due to optics.

I’ve created several iterations of the code, in order to learn why the previous iteration wasn't working.
V3(With simulated RedPitaya):
In this code, everything was working fine but the minimum Hamming Distance was always 0
V4(With simulated RedPitaya):
I changed the code to use a random bit generator instead of a sine wave as the input. Now the code was giving me a non-0 output but changed in every instance.
V5(With simulated RedPitaya):
To combat this I modified the code to only give an output when the hamming distance is 0, and made it find the first instance of this. But this was giving me “no valid answers”. I also created a function that records the start time just before acquiring data from the ADC and the end time immediately after acquiring the data.
The difference between these timestamps represents the time taken for the signal to travel from the DAC to the ADC. This approach assumes that the Red Pitaya device has a common time reference for both the DAC and ADC, and it uses the clock_gettime function to capture the timestamps. I wanted to see what would happen if I started the data acquisition as soon as the program ran.
V6:
In V6 I only used the time function to see what was happening in this. 
V7(With hardware RedPitaya):
Here I realized that the code was directly transferring data internally instead of using the hardware, so then I created a function to receive the data from the external cable. But this didn’t work either as I was getting a “Segmentation Error” even after multiple tries of debugging using gbp and trying to allocate memory properly.

This is the Compilation of the working code:

Pure C code for Finding min position:
This Code takes a binary array x_in and an array x_out that is the same as x_in after K bits, takes the first 25 bits of x_in (sampled at 16 samples per bit), compares the Hamming distance of it with each 25 consecutive binary valued subset of x_out, and then returns the position of the minimum Hamming distance, as well as the Hamming Distance of that position.

Integrated With RedPitaya Hardware:


