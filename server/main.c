#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
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
    // char command[50];
    // sprintf(command, "lsof -ti:8080 | xargs kill -9 &", PORT);
    // printf("Olha o comando ai: %s\n", command);
    // system(command);
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char *message = NULL;
    size_t  message_len = 0;
    char *hello = "OK";
   FILE *file = fopen("public_key.pem", "r"); // Tenta abrir o arquivo apenas para leitura.

    if (file) {
      fclose(file); // Fecha o arquivo se foi aberto com sucesso.
      printf("O arquivo public_key.pem existe.\n");
    } else {
      generate_and_save_keys("public_key.pem", "private_key.pem", 2048);
    }

    // Criar soquete do servidor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha na criação do soquete");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular soquete à porta 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Falha no bind");
        exit(EXIT_FAILURE);
    }

    // Escutar
    if (listen(server_fd, 3) < 0) {
        perror("Falha no listen");
        exit(EXIT_FAILURE);
    }

    // Aceitar
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Falha no accept");
        exit(EXIT_FAILURE);
    }

    RSA *private_key = load_private_key("private_key.pem");

    if (private_key == NULL) {
      fprintf(stderr, "Falha ao carregar chaves.\n");
      return 1;
    }

    RSA *public_key = load_public_key("../client/public_key.pem");
    RSA *my_public_key = load_public_key("public_key.pem");

    int encrypted_len;
    int decrypted_len;
    unsigned char encrypted[4096];
    unsigned char decrypted[4096];
    unsigned char aux_decrypted[4096];
    int security_word_granted = 0;
    HandshakePackage package;
    ssize_t bytes_read = read(new_socket, &package, sizeof(package));  // Deixando espaço para o terminador nulo
    // bytes_read = read(new_socket, &encrypted, sizeof(encrypted));  // Deixando espaço para o terminador nulo
  

    printf("E aqui? %s\n", package.message);
    decrypted_len = RSA_public_decrypt(package.message_lenght, package.message, decrypted, public_key, RSA_PKCS1_PADDING);

    if (decrypted_len == -1) {
        fprintf(stderr, "Falha na descriptografia: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return 1;
    }
    printf("tamanho recebido: %d\n", package.message_lenght);
    printf("Mensagem descriptografada: %s\n", decrypted);
    time_t current_timestamp = time(NULL);
    HandshakePackage response;
    if (strcmp(decrypted, SECURITY_WORD) == 0 && (current_timestamp - package.timestamp) <= SECURITY_WORD_TOLERANCE_DELAY) {
        security_word_granted = 1;
        response.message_lenght = RSA_private_encrypt(strlen(SERVER_SECURITY_WORD) + 1, (unsigned char*)SERVER_SECURITY_WORD, response.message, private_key, RSA_PKCS1_PADDING);
        printf("A mensagem antes de ser enviada: %s\n", response.message);
        printf("Tamanho resposta: %d\n", response.message_lenght);
        RSA_public_decrypt(response.message_lenght, response.message, aux_decrypted, my_public_key, RSA_PKCS1_PADDING);
        printf("Resposta descriptografada: %s\n", aux_decrypted);
        // send(new_socket, &response, sizeof(response), 0);
    } else {
        response.message_lenght = RSA_private_encrypt(strlen("-") + 1, (unsigned char*)"-", response.message, private_key, RSA_PKCS1_PADDING);
    }
    send(new_socket, &response, sizeof(response), 0);
    HandshakePackage key_exchange;

    bytes_read = read(new_socket, &key_exchange, sizeof(key_exchange));
    print_data("\n Chave aleatória:", key_exchange.key, sizeof(key_exchange.key));
    RSA_free(public_key);
 
    RSA_free(private_key);
    // close(new_socket);

    // while (1) {
    //   char buffer[MESSAGE_CHUNK_SIZE] = {0};  // Buffer temporário para receber a string
    //   read(new_socket, buffer, MESSAGE_CHUNK_SIZE);
    //   #ifdef DEBUG
    //     printf("Chunk recebido: %s.\n", buffer);
    //   #endif
        
    //   // Enviar dados para o cliente
    //   send(new_socket, hello, strlen(hello), 0);
     

    //   size_t newLen = message_len + strlen(buffer);  // Novo comprimento necessário para a string concatenada

    //   // Realoca a memória para acomodar a nova string
    //   char *newResult = realloc(message, newLen + 1);  // +1 para o '\0'
      
    //   if (newResult == NULL) {
    //       #ifdef DEBUG
    //         printf("Erro ao alocar memória para o newResult\n");
    //       #endif
    //       fprintf(stderr, "Falha ao alocar memória\n");
    //       free(message);  // Libera qualquer memória já alocada
    //       return 1;
    //   }

    //   message = newResult;
      
    //   strcpy(message + message_len, buffer);  // Copia a nova string para o final da string concatenada
    //   message_len = newLen;  // Atualiza o comprimento da string concatenad

    //    if (strncmp(buffer, END_CHUNK_FLAG, strlen(END_CHUNK_FLAG)) == 0 || endsWithEndTag(message)) {
    //     removeEndTag(message);
    //     printf("Mensagem recebida: %s\n", message);
    //     if(strncmp(message, "exit", 4) == 0) {
    //         printf("Recebi o exit\n");
    //         break;
    //     }
    //     free(message);
    //     message = NULL;
    //     message_len = 0;
    //   }


    // }

    printf("Finalizando servidor\n");

    // Ler dados do cliente

    return 0;
}
