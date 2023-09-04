#include "key_manager.h"

int generate_and_save_keys(const char *public_key_filename, const char *private_key_filename, int bits) {
    // Gerar chave RSA
    BIGNUM *bn = BN_new();
    if (BN_set_word(bn, RSA_F4) != 1) {
        BN_free(bn);
        return -1;
    }

    RSA *rsa = RSA_new();
    if (RSA_generate_key_ex(rsa, bits, bn, NULL) != 1) {
        RSA_free(rsa);
        BN_free(bn);
        return -1;
    }

    // Salvar chave privada
    FILE *private_key_file = fopen(private_key_filename, "wb");
    if (!private_key_file) {
        RSA_free(rsa);
        BN_free(bn);
        return -1;
    }
    PEM_write_RSAPrivateKey(private_key_file, rsa, NULL, NULL, 0, NULL, NULL);
    fclose(private_key_file);

    // Salvar chave pública
    FILE *public_key_file = fopen(public_key_filename, "wb");
    if (!public_key_file) {
        RSA_free(rsa);
        BN_free(bn);
        return -1;
    }
    PEM_write_RSA_PUBKEY(public_key_file, rsa);
    fclose(public_key_file);

    // Liberação da memória
    RSA_free(rsa);
    BN_free(bn);

    return 0;
}


RSA *load_public_key(const char *file_name) {
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL) {
        return NULL;
    }
    RSA *rsa = PEM_read_RSA_PUBKEY(fp, NULL, NULL, NULL);
    fclose(fp);
    return rsa;
}

// Função para carregar a chave privada
RSA *load_private_key(const char *file_name) {
    FILE *fp = fopen(file_name, "rb");
    if (fp == NULL) {
        return NULL;
    }
    RSA *rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    return rsa;
}