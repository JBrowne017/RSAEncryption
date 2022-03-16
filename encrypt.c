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
        "   Excryptes a file from input to output.\n\n"
        "USAGE\n"
        "   %s [-hv] [-n pbfile] [-i input file] [-o output file]\n"
        "OPTIONS"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -i infile       Specifies the input file to encrypt ( default: stdin).\n"
        "   -o outfile      Specifies the output file to decrypt ( default: stdout).\n"
        "   -n pbfile       Public key file (default: rsa.pub).\n",
        exec);
}

int main(int argc, char **argv) {
    FILE *pbfile = fopen("rsa.pub", "r");
    FILE *infile = stdin;
    FILE *outfile = stdout;
    int opt = 0;
    bool verbose = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'n': pbfile = fopen(optarg, "r"); break;
        case 'i': infile = fopen(optarg, "r"); break;
        case 'o': outfile = fopen(optarg, "w+"); break;
        case 'v': verbose = true; break;
        case 'h': {
            help(argv[0]);
            return EXIT_FAILURE;
        }
        }
    }
    // Read Public Key
    char *username = NULL;
    username = malloc(sizeof(char) * 100);
    mpz_t n, e, s, mpz_username;
    mpz_inits(n, e, s, mpz_username, NULL);
    rsa_read_pub(n, e, s, username, pbfile);

    // Convert username to mpz_t, verify using rsa_verify()
    mpz_set_str(mpz_username, username, 62);
    rsa_verify(mpz_username, s, e, n);

    // Encrypt using rsa_encrypt_file()
    rsa_encrypt_file(infile, outfile, n, e);

    // If verbose
    if (verbose) {
        // username
        gmp_printf("user = %Zd\n", mpz_username);
        // signature s
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        // mod n
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        // exp e
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    // Close public key file and clear any mpz_t vairables used
    mpz_clears(n, e, s, mpz_username, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pbfile);
    free(username);
}
