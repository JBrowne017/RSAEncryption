# RSA Encryption
This program is an implementation of the RSA encryption algorithm. In order to do the
computation required for this algorithm we’ll be using the GMP library. We will be creating,
using various theoretical functions, to compute a private and public key in order to encrypt
and decrypt files.\
    This program will be split into three separate: encrypt, decrypt, keygen. The encryptor will
take in a file to encrypt, use the public-key, and output the encrypted file. The decryptor will
intake a file to decrypt, use the private-key, and output the decrypted file. The keygen in ques-
tion will take a specified amount of bits, a file for the private key, a file for the public key, and a
random seed for the state. The keygen will then run our theoretic functions in order to generate
our primes, coprimes, inverses, and totients to generate our public and private keys.

## Building
`make`          Equivelent to `make all`.\
`make all`      Makes keygen, encrypt, and decrypt.\
`make keygen`   Makes keygen program.\
`make encrypt`  Makes encrypt program.\
`make decrypt`  Makes decrypt program.\
`make clean`    Cleans all .o files and programs.\
`make format`   Clang formats all .[ch] files.\
`make debug`    Makes all programs with debug flags.

## Running
`./encrypt -[vh] -[i infile] -[o outfile] -[n pbfile]`\
`./decrypt -[vh] -[i infile] -[o outfile] -[d pvfile]`\
`./keygen -[vh] -[b bits] -[s seed] -[c confidence] -[n pbfile] -[d pvfile]`

## Arguments List
```
-h  Displays help message for respective program.
-v  Displays statistics and verbose messages for the respective program.
-i  Infile to decrypt / encrypt.
-o  Outfile to decrypt / encrypt.
-n  File to read / write the public key.
-d  File to read / write the private key.
-s  Seed for random seed generation.
-c  Confidence level for the Miller-Rabin primality test.
```
