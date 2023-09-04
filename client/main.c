#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "stream_manipulation.h"
#include "key_manager.h"

int main() {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[1024] = {0};
    char buffer[1024] = {0};
    FILE *file = fopen("public_key.pem", "r"); // Tenta abrir o arquivo apenas para leitura.

    if (file) {
      fclose(file); // Fecha o arquivo se foi aberto com sucesso.
      printf("O arquivo public_key.pem existe.\n");
    } else {
      generate_and_save_keys("public_key.pem", "private_key.pem", 2048);
    }


    // Criar soquete do cliente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Falha na criação do soquete");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Converter endereço IP do servidor
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Falha na conversão do endereço IP");
        exit(EXIT_FAILURE);
    }

    // Conectar ao servidor
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Falha na conexão");
        exit(EXIT_FAILURE);
    }

   // Carregar as chaves
    RSA *public_key = load_public_key("public_key.pem");
    RSA *private_key = load_private_key("private_key.pem");

    if (public_key == NULL || private_key == NULL) {
      fprintf(stderr, "Falha ao carregar chaves.\n");
      return 1;
    }

    unsigned char encrypted[4096];
    unsigned char decrypted[4096];

    // Criptografar a mensagem
    int encrypted_len;
    encrypted_len = RSA_private_encrypt(strlen(SECURITY_WORD) + 1, (unsigned char*)SECURITY_WORD, encrypted, private_key, RSA_PKCS1_PADDING);

    if (encrypted_len == -1) {
        fprintf(stderr, "Falha na criptografia: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return 1;
    }
    printf("Cripftografada: %s\n", encrypted);

    // Descriptografar a mensagem
    int decrypted_len;
    decrypted_len = RSA_public_decrypt(encrypted_len, encrypted, decrypted, public_key, RSA_PKCS1_PADDING);

    if (decrypted_len == -1) {
        fprintf(stderr, "Falha na descriptografia: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return 1;
    }

    // Imprimir a mensagem descriptografada
    printf("Mensagem descriptografada: %s\n", decrypted);

    // Liberação da memória
    RSA_free(public_key);
    RSA_free(private_key);
    printf("Tamanho enviado: %d\n", encrypted_len);

    send(sock, &encrypted_len, sizeof(encrypted_len), 0);
    // printf("SIGLEN: %d\n", encrypted_message);
    // unsigned int to_send = htonl(sig_len);
    send(sock, encrypted, sizeof(encrypted), 0);





    // char *chunked_security_word;
    // int chunked_security_word_size;
    // splitStringIntoChunks(encrypted, &chunked_security_word, MESSAGE_CHUNK_SIZE, chunked_security_word_size);

    // for (int i = 0; i < chunked_security_word_size; i++) {
    //   send(sock, chunked_security_word + i * (MESSAGE_CHUNK_SIZE + 1), strlen(chunked_security_word + i * (MESSAGE_CHUNK_SIZE + 1)), 0);
    // }

    // while(1) { // Loop infinito
    //   char *chunks;
    //   int chunks_size;
    //   printf("Digite a mensagem para o servidor: ");
    //   fgets(message, sizeof(message), stdin); // Ler uma linha do stdin (teclado)
    //   #ifdef DEBUG
    //     printf("Input lido: %s\n", message);
    //   #endif

    //   // Remover o caractere de nova linha do final, se presente
    //   if(message[strlen(message) - 1] == '\n') {
    //       message[strlen(message) - 1] = '\0';
    //   }

    //   splitStringIntoChunks(message, &chunks, MESSAGE_CHUNK_SIZE, &chunks_size);
    //   for (int i = 0; i < chunks_size; i++) {
    //     printf("Chunk %d: %s de tamanho %d\n", i + 1, chunks + i * (MESSAGE_CHUNK_SIZE + 1),  (int)strlen(chunks + i * (MESSAGE_CHUNK_SIZE + 1)));
    //     send(sock, chunks + i * (MESSAGE_CHUNK_SIZE + 1), strlen(chunks + i * (MESSAGE_CHUNK_SIZE + 1)), 0);
    //     read(sock, buffer, 1024);
    //   }

    //   send(sock, END_CHUNK_FLAG, strlen(END_CHUNK_FLAG), 0);



    //   read(sock, buffer, 1024);
    //   if(strncmp(message, "exit", 4) == 0) {
    //       break; // Sair do loop se a entrada for "exit"
    //   }
    //   printf("Mensagem recebida: %s\n", buffer);
    //   // printf("Você digitou: %s\n", message); // Imprimir a entrada
    // }

    printf("Finalizando cliente\n");

    close(sock);

    // Enviar dados para o servidor

    // Ler dados do servidor

    return 0;
}
