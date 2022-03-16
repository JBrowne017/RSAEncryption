#include "randstate.h"
#include <stdint.h>
#include <gmp.h>

gmp_randstate_t state;

void randstate_init(uint64_t seed) {
    // Initializes the global random state named state with a Mersenne Twister
    // algo using seed as the random seed. This entails a call to
    // gmp_randinit_mt and a callto gmp_randseed_ui
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
    return;
}

void randstate_clear(void) {
    gmp_randclear(state);
}
