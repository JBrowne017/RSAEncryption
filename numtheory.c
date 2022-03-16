#include "numtheory.h"
#include "randstate.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>

// GREATEST COMMON DIVISOR
// @param g : Output to store GCD value in
// @param a,b : Numbers to calculate the greatest common divisor of
// Calculues the greatest common divisor of two parameterized numbers.
void gcd(mpz_t g, mpz_t a, mpz_t b) {
    mpz_t tmp_a, tmp_b, t;
    mpz_inits(t, tmp_a, tmp_b, NULL);
    mpz_set(tmp_b, b);
    mpz_set(tmp_a, a);
    while (mpz_cmp_ui(tmp_b, 0) != 0) {
        mpz_set(t, tmp_b);
        mpz_mod(tmp_b, tmp_a, tmp_b);
        mpz_set(tmp_a, t);
    }
    mpz_set(g, tmp_a);
    mpz_clears(t, tmp_a, tmp_b, NULL);
}

// MODULAR INVERSE
// @param o : Initialized variable to store output into
// @param a : Base a
// @param n : Modulo n
// Calculates the modular inverse o of a mod n.
void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
    // DECLARE & INITIALIZE VARIABLES
    mpz_t q, t, tP, r, rP, tmp;
    mpz_inits(q, t, tP, r, rP, tmp, NULL);
    mpz_set(r, n); // r = n
    mpz_set(rP, a); // r' = a
    mpz_set_ui(t, 0); // t = 0
    mpz_set_ui(tP, 1); // t' = 1
    mpz_set_ui(q, 0); // q = 0
    mpz_set_ui(tmp, 0); // tmp = 0
    while (mpz_cmp_ui(rP, 0) != 0) { // if r' != 0
        mpz_fdiv_q(q, r, rP); // q = floor(r/r')

        mpz_set(tmp, r); // tmp = r
        mpz_set(r, rP); // r = r'
        mpz_mul(rP, q, rP); // r' = q*r'
        mpz_sub(rP, tmp, rP); // r' = tmp - r'

        mpz_set(tmp, t); // tmp = t
        mpz_set(t, tP); // t = t'
        mpz_mul(tP, q, tP); // t' = q*t'
        mpz_sub(tP, tmp, tP); // t' = tmp - t'
    }
    if (mpz_cmp_ui(r, 1) == 1) { // if r > 1
        mpz_set_ui(o, 0); // return no inverse
        mpz_clears(q, t, tP, r, rP, tmp);
        return;
    }
    if (mpz_cmp_ui(t, 0) == -1) { // if t < 0
        mpz_add(t, t, n); // t += n
        mpz_set(o, t); // return t
        mpz_clears(q, t, tP, r, rP, tmp);
        return;
    }
    mpz_set(o, t); // return t
    mpz_clears(q, t, tP, r, rP, tmp);
    return;
}

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    mpz_t tmp, v, p, exp;
    mpz_init_set_ui(v, 1); // v = 1
    mpz_init_set(p, a); // p = a
    mpz_init_set(exp, d);
    mpz_init(tmp);
    while (mpz_cmp_ui(exp, 0) == 1) { // d > 0
        if (mpz_odd_p(exp)) { // d is odd
            mpz_mul(tmp, v, p); // v *= p
            mpz_mod(v, tmp, n); // v %= n
        }
        mpz_mul(tmp, p, p); // p *= p
        mpz_mod(p, tmp, n); // p %= n

        mpz_tdiv_q_2exp(exp, exp, 1); // d /= 2
    }
    mpz_set(o, v); // return v
    mpz_clears(tmp, exp, v, p, NULL);
    return;
}

// MILLER-RABIN PRIMALITY TEST
// @param n : Number n to calculate the primality of
// @param iters : Number of iterations to run the test for
// Calculate whether or not n is prime using iters number of iterations
bool is_prime(mpz_t n, uint64_t iters) {

    // IF IS EVEN AND GREATER THAN TWO IT IS NEVER PRIME
    if ((mpz_cmp_ui(n, 2) < 0) || (mpz_even_p(n) != 0 && mpz_cmp_ui(n, 2) != 0)) {
        return false;
    }

    // TWO AND THREE ARE PRIME
    if (mpz_cmp_ui(n, 4) < 0) {
        return true;
    }

    // DECLARE AND INITIALIZE VARIABLES
    mpz_t n_1, r, j, y, a, two, s_1, n_4;
    mpz_inits(n_1, r, j, s_1, y, a, two, n_4, NULL);
    mpz_sub_ui(n_1, n, 1); // n_1 = n-1
    mpz_sub_ui(n_4, n, 4); // n_4 = n-4
    uint64_t s = 0;

    // Write n-1 = 2exp(s)*r
    while (mpz_even_p(r)) { // While r is an even integer
        mpz_tdiv_q_2exp(r, n_1, s); // r = (n-1)/2^s
        mpz_fdiv_q_ui(r, r, 2); // r /= 2
        s += 1; // s++
    }

    mpz_init_set_ui(s_1, s - 1);
    for (uint64_t i = 0; i < iters; i += 1) {
        // choose random a from 2 to n-2
        mpz_urandomm(a, state, n_4); // random a from 2 to n-2
        mpz_add_ui(a, a, 2);
        pow_mod(y, a, r, n);
        if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_1) != 0) {
            mpz_set_ui(j, 1); // j = 1
            mpz_set_ui(two, 2);
            while (mpz_cmp(j, s_1) != 1 && mpz_cmp(y, n_1) != 0) {
                pow_mod(y, y, two, n); // causing failure, after multiple passes, leads to 1
                if (mpz_cmp_ui(y, 1) == 0) { // if y == 1
                    mpz_clears(n_1, r, j, y, a, two, s_1, n_4, NULL);
                    return false; // not prime // 17 is failing here
                }
                mpz_add_ui(j, j, 1); // j += 1
            }
            if (mpz_cmp(y, n_1) != 0) { // if y != n-1
                mpz_clears(n_1, r, j, y, a, two, s_1, n_4, NULL);
                return false; // not prime
            }
        }
    }
    mpz_clears(n_1, r, j, y, a, two, s_1, n_4, NULL);
    return true; // prime
}

// MAKE RANDOM PRIME NUMBER
// @param p : Variable to store prime number in
// @param bits : Minimum number of bits for prime number p
// @param iters : Iterations to run Miller-Rabin primality test for
// Generates a new prime number stores in p
// Generated prime should be a least bits number of bits long
// The primality of the number should be tested with is_prime using iters
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    for (mpz_urandomb(p, state, bits); is_prime(p, iters) == false; mpz_urandomb(p, state, bits))
        ;
    return;
}
