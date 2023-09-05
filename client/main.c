#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <openssl/aes.h>
#include <openssl/rand.h>  // Para RAND_bytes
#include "stream_manipulation.h"
#include "key_manager.h"

typedef struct {
  int message_lenght;
  unsigned char message[4096];
  time_t timestamp;
  unsigned char key[16];
} HandshakePackage;


void print_data(const char *title, const void *data, int len) {
    printf("%s", title);
    const unsigned char *p = (const unsigned char *)data;
    int i = 0;

    for (; i < len; ++i)
        printf("%02X ", *p++);

    printf("\n");
}
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
    RSA *public_key = load_public_key("../server/public_key.pem");
    RSA *private_key = load_private_key("private_key.pem");

    if (public_key == NULL || private_key == NULL) {
      fprintf(stderr, "Falha ao carregar chaves.\n");
      return 1;
    }

    unsigned char encrypted[4096];
    unsigned char decrypted[4096];
    HandshakePackage package;

    // Criptografar a mensagem
    int encrypted_len;
    package.message_lenght = RSA_private_encrypt(strlen(SECURITY_WORD) + 1, (unsigned char*)SECURITY_WORD, package.message, private_key, RSA_PKCS1_PADDING);

    if (package.message_lenght == -1) {
        fprintf(stderr, "Falha na criptografia: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return 1;
    }
    printf("Cripftografada: %s\n", package.message);

    // Descriptografar a mensagem
    // int decrypted_len;
    // decrypted_len = RSA_public_decrypt(package.message_lenght, package.message, decrypted, public_key, RSA_PKCS1_PADDING);

    // if (decrypted_len == -1) {
    //     fprintf(stderr, "Falha na descriptografia: %s\n", ERR_error_string(ERR_get_error(), NULL));
    //     return 1;
    // }

    // Imprimir a mensagem descriptografada
    // printf("Mensagem descriptografada: %s\n", decrypted);

    // Liberação da memória
 
    package.timestamp = time(NULL);
    send(sock, &package, sizeof(package), 0);
    HandshakePackage response;
    ssize_t bytes_read = read(sock, &response, sizeof(response));
    printf("Olha os bytes lidos: %d\n", bytes_read);
    printf("Mensagem lida do response %s\n", response.message);
    printf("Tamanho resposta: %d\n", response.message_lenght);
    int decrypted_len = RSA_public_decrypt(response.message_lenght, response.message, decrypted, public_key, RSA_PKCS1_PADDING);

    if (decrypted_len == -1) {
        fprintf(stderr, "Falha na descriptografia: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return 1;
    }
    printf("tamanho recebido: %d\n", package.message_lenght);
    printf("Resposta descriptografada: %s\n", decrypted);

    HandshakePackage key_exchange;

    /* Gerar chave aleatória */
    if (!RAND_bytes(key_exchange.key, sizeof(key_exchange.key))) {
        fprintf(stderr, "Erro ao gerar chave aleatória\n");
        return 1;
    }
    send(sock, &key_exchange, sizeof(key_exchange), 0);
    // AES_KEY enc_key, dec_key;
    //  unsigned char aux_encrypted[128];
    // unsigned char aux_decrypted[128];

    // /* Inicialização da chave de criptografia e descriptografia */
    // AES_set_encrypt_key(key, 128, &enc_key);
    // AES_set_decrypt_key(key, 128, &dec_key);
    // unsigned char input[] = "oi amiguinho";
    print_data("\n Chave aleatória:", key_exchange.key, sizeof(key_exchange.key));

    // /* Criptografar e imprimir o resultado */
    // AES_encrypt(input, aux_encrypted, &enc_key);
    // print_data("\n Dados criptografados:", aux_encrypted, sizeof(aux_encrypted));
    // printf("\n Dados criptografados: %s\n", aux_encrypted);

    // /* Descriptografar e imprimir o resultado */
    // AES_decrypt(aux_encrypted, aux_decrypted, &dec_key);
    // print_data("\n Dados descriptografados:", aux_decrypted, sizeof(aux_decrypted));
    // printf("\n Dados descriptografados: %s\n", aux_decrypted);

    // printf("SIGLEN: %d\n", encrypted_message);
    // unsigned int to_send = htonl(sig_len);
    // send(sock, encrypted, sizeof(encrypted), 0);

    RSA_free(public_key);
    RSA_free(private_key);




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
