#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>

// LOG FUNCTION FOR MPZ
// @param n : mpz_t to calculate the log base 2 of
uint64_t log_2(mpz_t n) {
    uint64_t log = 0;
    mpz_t n_log;
    // FOR LOOP TO CALCULATE LOG_2 OF N
    for (mpz_init_set(n_log, n); mpz_cmp_ui(n_log, 0) == 1; mpz_tdiv_q_2exp(n_log, n_log, 1)) {
        log += 1;
    }
    mpz_clear(n_log);
    return log;
}

// GENERATE PUBLIC RSA KEY
// @param p : initialized mpz_t variable for prime number p
// @param q : initialized mpz_t variable for prime number q
// @param n : initialized mpz_t variable for mod n
// @param e : initialized mpt_t variable for public exponent
// @param nbits : minimum number of bits for public key
// @param iters : number of iterations for Miller-Rabin
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
    // SET BITS FOR P AND Q
    uint64_t pbits = 0, qbits = 0;
    uint64_t upper = (3 * nbits) / 4; // UPPER BOUND OF RANDOM GENERATION
    uint64_t lower = nbits / 4; // LOWER BOUND OF RANDOM GENERATION
    // WHILE log_2(n) < nbits, MAKE P,Q,N
    do {
        // GENERATE RANDOM BITS WITHIN GIVEN BOUNDS
        pbits = (random() % (upper - lower + 1)) + lower;
        qbits = nbits - pbits; // Q IS REMAINING BITS GIVEN P
        make_prime(p, pbits, iters); // GENERATE P
        make_prime(q, qbits, iters); // GENERATE Q
        mpz_mul(n, p, q); // N = PQ
    } while (log_2(n) < nbits);

    // COMPUTE VARPHI
    mpz_t varphi;
    mpz_init(varphi);
    mpz_t p_1, q_1;
    mpz_inits(p_1, q_1, NULL);
    mpz_sub_ui(p_1, p, 1); // P-1
    mpz_sub_ui(q_1, q, 1); // Q-1
    mpz_mul(varphi, p_1, q_1); // TOTIENT = (P-1)(Q-1)

    // FIND E
    mpz_t d;
    mpz_init(d);
    // IF GCD == 1 THEN WE HAVE OUR PUBLIC EXPONENT E
    for (mpz_urandomb(e, state, nbits); mpz_cmp_ui(d, 1) != 0; mpz_urandomb(e, state, nbits)) {
        gcd(d, e, varphi);
    }
    mpz_clears(d, p_1, q_1, varphi, NULL); // CLEAR USED VARIABLES
    return;
}

// WRITE PUBLIC KEY TO PBFILE
// @param n : mod n
// @param e : public exponent e
// @param s : signature s
// @param username : username of the user
// @param pbfile : Outfile to write public key to
// Outputs formatted as hexstring with trailing newlines
void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    gmp_fprintf(pbfile, "%s\n", username);
    return;
}

// READ PUBLIC KEY FROM PBFILE
// @param n : mod n
// @param e : public exponent e
// @param s : signature s
// @param username : username of the user
// @param pbfile : Infile to read public key from
// Reads format as hexstring with trailing newlines
void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    gmp_fscanf(pbfile, "%s\n", username);
    return;
}

// Creates a new RSA private key d given primes p and q and exp e
// To compute d simply computer the inverse of e mod varphi
// CREATE PRIVATE RSA KEY
// @param d : Initialized mpz_t variable for private key d
// @param e : Public exponent e
// @param p : Prime number p
// @param q : Prime number q
// Computes private key d given public key information p,q,e
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
    mpz_t varphi, p_1, q_1;
    mpz_init(varphi);
    mpz_inits(p_1, q_1, NULL);
    mpz_sub_ui(p_1, p, 1);
    mpz_sub_ui(q_1, q, 1);
    mpz_mul(varphi, p_1, q_1);

    // PRIVATE KEY IS INVERSE MODULO OF E AND TOTIENT
    mod_inverse(d, e, varphi);
    mpz_clears(varphi, p_1, q_1, NULL);
    return;
}

// WRITES PRIVATE KEY TO PARAMETERIZED FILE
// @param n : Mod n
// @param d : Private key d
// @param pvfile : Output file to write the private key to
// Format of output is hexstring on trailing newlines
void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
    return;
}

// READ PRIVATE KEY FROM PARAMETERIZED FILE
// @param n : Mod n
// @param d : Private key d
// @param pvfile : Input file to read private key from
void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
    return;
}

// RSA ENCRYPT MESSAGE
// @param c : Initialized variable for ciphertext
// @param m : Message m
// @param e : Public exponenet
// @param n : Mod n
// E(M) = M*exp(e) (mod n) = c
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    pow_mod(c, m, e, n);
    return;
}

// RSA ENCRYPT FILE
// @param infile : Input file to read data from
// @param outfile : Output file to write ciphertexts to
// @param n : Mod n
// @param e : Public exponent e
// Reads all bytes from a parameterized infile and runs rsa_encrypt for
// each block of data k. Then writes all blocks to parameterized outfile.
// Written outputs are formatted as hexstrings on trailing newlines.
void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    uint64_t k = 0; // BLOCK SIZE
    uint64_t j = 0; // BYTES READ
    mpz_t nlog, m, c;
    mpz_inits(m, c, NULL);
    // BLOCK = k = floor((log2(n)-1)/8)
    // While nlog != 0, bitshift right to divide by 2
    for (mpz_init_set(nlog, n); mpz_cmp_ui(nlog, 0) == 1; mpz_tdiv_q_2exp(nlog, nlog, 1)) {
        k += 1;
    }
    k -= 1;
    k /= 8;
    uint8_t *buffer = (uint8_t *) calloc(k, sizeof(uint8_t));
    // READ k-1 BYTES UNTIL EOF
    // USE fread() instead of fgetc
    while ((j += fread(buffer + 1, sizeof(uint8_t), k - 1, infile)) > 0) {
        // increment j by bytes_read
        buffer[0] = 0xFF;
        // mpz_import to convert read bytes to mpz_t m
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, buffer);
        // call rsa_encrypt
        rsa_encrypt(c, m, e, n);
        // output to outfile as hexstring w/ newline
        gmp_fprintf(outfile, "%Zx\n", c);
        j = 0;
    }
    mpz_clears(nlog, m, c, NULL);
    free(buffer);
    return;
}

// RSA DECRYPT
// @param m : Stores decrypted message
// @param c : Ciphertext to decrypt
// @param d : Private key
// @param n : Mod n
// Decryptes a parameterized ciphertext
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {
    pow_mod(m, c, d, n);
    return;
}

// RSA DECRYPT FILE
// @param infile : Input file to read data from
// @param outfile : Output file to write decrypted messages to
// @param n : Mod n
// @param e : Private key d
// Reads all ciphertexts in parameterized infile until end of file.
// For each ciphertext, decrypt it using rsa_decrypt.
// Write the decrypted message to the parameterized outfile.
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    uint64_t k = 0; // BLOCK SIZE
    uint64_t j = 0; // BYTES READ
    mpz_t nlog, m, c;
    mpz_inits(m, c, NULL);
    // While nlog != 0, bitshift right to divide by 2
    for (mpz_init_set(nlog, n); mpz_cmp_ui(nlog, 0) == 1; mpz_tdiv_q_2exp(nlog, nlog, 1)) {
        k += 1;
    }
    k -= 1;
    k /= 8;
    uint8_t *buffer = (uint8_t *) calloc(k, sizeof(uint8_t));
    while (gmp_fscanf(infile, "%Zx\n", c) != EOF) {
        rsa_decrypt(m, c, d, n);
        mpz_export(buffer, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(buffer + 1, sizeof(uint8_t), j - 1, outfile);
        j = 0;
    }
    free(buffer);
    mpz_clears(nlog, m, c, NULL);
    return;
}

// RSA SIGN
// @param s : Initialized variable to store sign
// @param m : Message to sign
// @param d : Private key
// @param n : Mod n
// Signs the message by computing the modulo exponent of the message given
// the public and private keys.
void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {
    pow_mod(s, m, d, n); // SIGN IS POW_MOD OF MSG, PRIV_KEY, N
    return;
}

// VERIFY RSA SIGN
// @param m : Message m
// @param s : Sign s
// @param e : Public exponent
// @param n : Mod n
// Verifies the sign of a message. If its inverse and the message are equal.
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n); // VERIFYING IS THE INVERSE OF SIGNING
    if (mpz_cmp(t, m) == 0) { // IF T AND M ARE EQUAL, VERIFIED
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false;
}
