#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdlib.h>
#include "unistd.h"
#include "time.h"
#include "sys/stat.h"

#define OPTIONS "b:i:n:d:s:c:vh"

void help(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Generates an RSA public/private pair.\n\n"
        "USAGE\n"
        "   %s [-hv] [-s seed] [-c confidence] [-b bits] [-n pbfile] [-d pvfile]\n"
        "OPTIONS\n"
        "   -h              Display program help and usage.\n"
        "   -v              Display verbose program output.\n"
        "   -b bits         Minimum bits needed for public key n (default: 256).\n"
        "   -c confidence   Miller-Rabin iterations for testing primes (default: 50).\n"
        "   -n pbfile       Public key file (default: rsa.pub).\n"
        "   -d pvfile       Private key file (default: rsa.priv).\n"
        "   -s seed         Random seed for testing (default: time(NULL)).\n",
        exec);
}

int main(int argc, char **argv) {
    FILE *pbfile = fopen("rsa.pub", "w+");
    FILE *pvfile = fopen("rsa.priv", "w+");
    int opt = 0;
    uint64_t nbits = 256;
    uint64_t iters = 50;
    uint64_t seed = time(NULL); // time(NULL) Default Seed
    bool verbose = false;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': nbits = atoi(optarg); break;
        case 'c': iters = atoi(optarg); break;
        case 'n': pbfile = fopen(optarg, "w+"); break;
        case 'd': pvfile = fopen(optarg, "w+"); break;
        case 's': seed = atoi(optarg); break;
        case 'v': verbose = true; break;
        case 'h': {
            help(argv[0]);
            return EXIT_FAILURE;
        }
        }
    }

    // fchmod() and filno() to set pvfile permissions to 0600
    // indicating read and write permissions for the user
    // and no permissions for anyone else
    int filenum = fileno(pvfile);
    fchmod(filenum, 0600);

    // initialize the random state using randstate_init() using the seed
    randstate_init(seed);

    // make the public and private keys
    mpz_t p, q, n, e, d, mpz_username, s;
    mpz_inits(p, q, n, e, d, mpz_username, s, NULL);
    rsa_make_pub(p, q, n, e, nbits, iters);
    rsa_make_priv(d, e, p, q);

    // get the current users name as a string using getenv()
    char *username = getenv("USER");

    // convert the username into mpz_t using mpz_set_str(),
    // specifying the base as 62
    // use rsa_sign() to compute the signature of the username
    mpz_set_str(mpz_username, username, 62);
    rsa_sign(s, mpz_username, d, n);

    // write the keys to their respective files
    rsa_write_pub(n, e, s, username, pbfile);
    rsa_write_priv(n, d, pvfile);

    // if verbose printing is enabled print on trailing newlines
    // printed with information about the number of bits that consitute them
    // along with their respective values in decimal
    if (verbose) {
        //  username
        gmp_printf("user = %Zd\n", mpz_username);
        //  signature s
        gmp_printf("s (%d bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        //  first large prime p
        gmp_printf("p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        //  second large prime q
        gmp_printf("q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        //  public mod n
        gmp_printf("n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        //  public mod e
        gmp_printf("e (%d bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        //  private key d
        gmp_printf("d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // close files and use randstate_clear() and clear any used mpz_t
    fclose(pbfile);
    fclose(pvfile);
    randstate_clear();
    mpz_clears(p, q, n, e, d, mpz_username, s, NULL);
}
