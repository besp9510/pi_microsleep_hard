// Hard Microsleep Library via System Timer for the Raspberry Pi
//
// Copyright (c) 2021 Benjamin Spencer
// ============================================================================
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// ============================================================================

// Include C standard libraries:
#include <stdlib.h> // C Standard library
#include <stdio.h>  // C Standard I/O libary
#include <time.h>   // C Standard date and time manipulation

// Include pi_gpio.c:
#include "pi_microsleep_hard.h"
#include "pi_lw_gpio.h"

#define MAX_TOGGLE_TIME_USEC 1e6

// Simple GPIO test script
int main(void) {
    int ret;

    // Keep track of long we have been toggling:
    int elapsed_time_usec = 0;

    // Choose which GPIO pin to toggle on & off:
    int gpio_pin = 4;

    // Choose different clock periods to toggle the GPIO pin:
    int slow_usec = 4;
    int fast_usec = slow_usec / 2;

    printf("Setting up pi_microsleep_hard\n");

    // Setup microsleep so we can use it:
    if ((ret = setup_microsleep_hard()) < 0) {
        printf("setup_microsleep_hard() has returned %d\n", ret);
        printf("Exiting test_pi_microsleep_hard\n");

        return -1;
    }

    printf("arm_timer_microsleep() has returned %d\n", ret);

    // Get GPIO into known state:
    gpio_set_mode(GPIO_OUTPUT, gpio_pin);
    gpio_clear(gpio_pin);

    printf("Set GPIO pin %d to output mode and cleared\n", gpio_pin);
    printf("GPIO pin %d now reads %d\n", gpio_pin, gpio_read_level(gpio_pin));
    printf("Toggling GPIO pin %d using clock period %d us\n", gpio_pin,
           slow_usec);

    // Toggle the pin:
    while (elapsed_time_usec <= MAX_TOGGLE_TIME_USEC) {
        // Toggle on & off like so:
        gpio_set(gpio_pin);
        microsleep_hard(slow_usec/2);
        gpio_clear(gpio_pin);
        microsleep_hard(slow_usec/2);

        // Add time here:
        elapsed_time_usec += slow_usec;
    }

    // Reset so we can do another round:
    elapsed_time_usec = 0;

    printf("Now reached maximum toggle time; stopping\n");
    printf("Holding for 1 second\n");

    sleep(1);

    printf("Toggling GPIO pin %d using clock period %d us\n", gpio_pin,
           fast_usec);

    // Toggle the pin:
    while (elapsed_time_usec <= MAX_TOGGLE_TIME_USEC) {
        // Toggle on & off like so:
        gpio_set(gpio_pin);
        microsleep_hard(fast_usec/2);
        gpio_clear(gpio_pin);
        microsleep_hard(fast_usec/2);

        // Add time here:
        elapsed_time_usec += fast_usec;
    }

    printf("Now reached maximum toggle time; stopping\n");
    printf("Completed test_pi_microsleep_hard\n");
}
