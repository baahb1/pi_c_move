#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_DIGITS 500000
#define ARRAY_SIZE ((10 * NUM_DIGITS) / 3)

int main() {
    // Algorithm from "A Spigot Algorithm for the Digits of π,"
    //  Rabinowitz, S. and Wagon, S., https://doi.org/10.1080/00029890.1995.11990560

    // 1. Initialize: Let A = (2, 2, 2, 2, . . ., 2) be an array of length floor(lOn/3).
    printf("Preparing to calculate %d digits of pi\n", NUM_DIGITS);

    uint64_t *a = (uint64_t *)malloc(sizeof(uint64_t) * ARRAY_SIZE);
    uint8_t *predigit_buffer = (uint8_t *)malloc(sizeof(uint8_t) * NUM_DIGITS);
    uint8_t *predigit_buffer_ptr = NULL;
    char *pi = (char *)malloc(sizeof(char) * (NUM_DIGITS + 2));

    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        a[i] = 2;
    }

    for (size_t i = 0; i < NUM_DIGITS + 2; ++i) {
        pi[i] = '\0';
    }

    // 2. Repeat n times:
    for (size_t digit_num = 0; digit_num < NUM_DIGITS; ++digit_num) {
        // Multiply by 10: Multiply each entry of A by 10.
        for (size_t i = 0; i < ARRAY_SIZE; ++i) {
            a[i] = a[i] * 10;
        }

        // Put A into regular form: Starting from the right, reduce the ith element of A
        //  (corresponding to c-entry (i- 1)/(2i- 1)) modulo 2i- 1, to get a quotient q and
        //  a remainder r. Leave r in place and carry q(i - 1) one place left. The last
        //  integer carried (from the position where i - 1 = 2) may be as large as 19.
        for (size_t i = ARRAY_SIZE - 1; i > 0; --i) {
            // Calculate the quotient and remainder
            uint64_t quotient = a[i] / (2 * i + 1);
            uint64_t remainder = a[i] % (2 * i + 1);

            // keep the remainder and carry the quotient
            a[i] = remainder;
            a[i - 1] += quotient * i;
        }

        // Get the next predigit: Reduce the leftmost entry of A (which is at most
        //  109[= 9 * 10 + 19]) modulo 10. The quotient q is the new predigit of
        //  π, the remainder staying in place.
        uint64_t predigit = a[0] / 10;
        uint64_t remainder = a[0] % 10;

        a[0] = remainder;

        // Adjust the predigits: If q is neither 9 nor 10 release the held predigits as
        //  true digits of π and hold q. If q is 9, add q to the queue of held predigits.
        //  If q is 10 then:
        //  - set the current predigit to 0 and hold it
        //  - increase all other held predigits by 1 (9 becomes 0);
        //  - release as true digits vf v all but the current held predigit

        // If the predigit is nine, just add it to the buffer
        if (predigit == 9) {
            if (predigit_buffer_ptr != NULL) {
                ++predigit_buffer_ptr;
            }

            else {
                predigit_buffer_ptr = predigit_buffer;
            }

            *predigit_buffer_ptr = predigit;
        }

        // If the predigit is not nine, the existing predigits will be released and
        //  the new predigit will be added
        else {
            // Only process previous predigits if there are predigits to process
            if (predigit_buffer_ptr != NULL) {
                // If the predigit is 10, set the predigit to zero and increment all
                //  of the held predigits
                if (predigit == 10) {
                    predigit = 0;

                    for (uint8_t *ptr = predigit_buffer; ptr <= predigit_buffer; ++ptr) {
                        ++*ptr;
                        *ptr %= 10;
                    }
                }

                // Write all of the buffered predigits to the pi string
                // If this is the first digit written, also write a decimal point
                for (uint8_t *ptr = predigit_buffer; ptr <= predigit_buffer; ++ptr) {
                    if (strlen(pi) == 0) {
                        pi[0] = *ptr + '0';
                        pi[1] = '.';
                    }

                    else {
                        pi[strlen(pi)] = *ptr + '0';
                    }
                }

                // Print out the number of digits computed so far
                size_t num_digits = strlen(pi) - 1;
                double percent_complete = (100.0 * num_digits) / NUM_DIGITS;
                printf("\033[1F\033[2K");
                printf("%lu digits computed (%f%%)\n", num_digits, percent_complete);
            }

            // Clear the buffer and save the current predigit
            predigit_buffer_ptr = predigit_buffer;
            *predigit_buffer_ptr = predigit;
        }
    }

    printf("%s\n", pi);

    return 0;
}