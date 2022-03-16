# RSA Encryption
The following program contains three parts: keygen, encrypt, decrypt. These three together are an implementation of the RSA Encryption algorithm in C.


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
`./keygen -[vh] -[b bits] -[s seed] -[c confidence] -[n pbfile] -[d pvfile]

## Arguments List
```
-h  Displays help message for respective program.\
-v  Displays statistics and verbose messages for the respective program.\
-i  Infile to decrypt / encrypt.\
-o  Outfile to decrypt / encrypt.\
-n  File to read / write the public key.\
-d  File to read / write the private key.\
-s  Seed for random seed generation.\
-c  Confidence level for the Miller-Rabin primality test.
```
