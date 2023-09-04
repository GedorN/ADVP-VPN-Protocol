#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "stream_manipulation.h"
#include "key_manager.h"


int main() {
    // char command[50];
    // sprintf(command, "lsof -ti:8080 | xargs kill -9 &", PORT);
    // printf("Olha o comando ai: %s\n", command);
    // system(command);
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
    RSA *public_key = load_public_key("public_key_client.pem");
    int encrypted_len;
    int decrypted_len;
    unsigned char encrypted[4096];
    unsigned char decrypted[4096];
    ssize_t bytes_read = read(new_socket, &encrypted_len, sizeof(encrypted_len));  // Deixando espaço para o terminador nulo
    bytes_read = read(new_socket, &encrypted, sizeof(encrypted));  // Deixando espaço para o terminador nulo
  

    if (decrypted_len == -1) {
        fprintf(stderr, "Falha na descriptografia: %s\n", ERR_error_string(ERR_get_error(), NULL));
        return 1;
    }

    printf("E aqui? %s\n", encrypted);
    decrypted_len = RSA_public_decrypt(encrypted_len, encrypted, decrypted, public_key, RSA_PKCS1_PADDING);
    printf("tamanho recebido: %d\n", encrypted_len);
    printf("Mensagem descriptografada: %s\n", decrypted);
    RSA_free(public_key);

    // Inicializa o buffer com zeros para garantir que os dados recebidos formem uma string válida
    // memset(signed_message_buffer, 0, sizeof(signed_message_buffer));

    
    // if (bytes_read < 0) {
    //     perror("Erro na leitura do socket");
    //     close(new_socket);
    //     return;
    // }
    // // Imprime a mensagem recebida
    // for (unsigned int i = 0; i < 256; ++i) {
    //     printf("%02x", signed_message_buffer[i]);
    // }
    // bytes_read = read(new_socket, &sig_len, sizeof(sig_len));
    //  if (bytes_read < 0) {
    //     perror("Erro na leitura do socket");
    //     close(new_socket);
    //     return;
    // }

    // if (bytes_read < sizeof(sig_len)) {
    //     fprintf(stderr, "Dados insuficientes lidos do socket.\n");
    //     close(new_socket);
    //     return;
    // }
    // sig_len = ntohl(sig_len);
    // printf("\nTam: %d\n", sig_len);
    // const char* msg = "Hello, world!";
    // if (verify_signature(msg, signed_message_buffer, sig_len, public_key)) {
    //     printf("A assinatura é válida.\n");
    // } else {
    //     printf("A assinatura é inválida.\n");
    // }
    close(new_socket);

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
