#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include <stdio.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/sha.h>


#define SECURITY_WORD "O RATO ROEU A ROUPA DO REI DE ROMA"

int generate_and_save_keys(const char *public_key_filename, const char *private_key_filename, int bits);
RSA *load_public_key(const char *file_name);
RSA *load_private_key(const char *file_name);

#endif // KEY_MANAGER_H