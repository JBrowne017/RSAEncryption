#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hvi:o:n:"

void help(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Decrypts a file from input to output.\n\n"
        "USAGE\n"
        "   %s [-hv] [-n privkey] [-i input file] [-o output file]\n"
        "OPTIONS"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -i infile       Specifies the input file to decrypt ( default: stdin).\n"
        "   -o outfile      Specifies the output file to decrypt ( default: stdout).\n"
        "   -n privfile     Private key file (default: rsa.priv).\n",
        exec);
}

int main(int argc, char **argv) {
    FILE *pvfile = fopen("rsa.priv", "r");
    FILE *infile = stdin;
    FILE *outfile = stdout;
    int opt = 0;
    bool verbose = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'n': pvfile = fopen(optarg, "r"); break;
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w"); break;
        case 'v': verbose = true; break;
        case 'h': {
            help(argv[0]);
            return EXIT_FAILURE;
        }
        }
    }
    // Read Private Key
    mpz_t n, e;
    mpz_inits(n, e, NULL);
    rsa_read_priv(n, e, pvfile);

    // If verbose
    if (verbose) {
        // mod n
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        // exp e
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    // Encrypt using rsa_encrypt_file()
    rsa_decrypt_file(infile, outfile, n, e);

    // Close public key file and clear any mpz_t vairables used
    mpz_clears(n, e, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pvfile);
}
