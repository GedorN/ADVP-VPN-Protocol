#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "stream_manipulation.h"


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

    while (1) {
      char buffer[MESSAGE_CHUNK_SIZE] = {0};  // Buffer temporário para receber a string
      read(new_socket, buffer, MESSAGE_CHUNK_SIZE);
        
      // Enviar dados para o cliente
      send(new_socket, hello, strlen(hello), 0);
     

      size_t newLen = message_len + strlen(buffer);  // Novo comprimento necessário para a string concatenada

      // Realoca a memória para acomodar a nova string
      char *newResult = realloc(message, newLen + 1);  // +1 para o '\0'
      
      if (newResult == NULL) {
          fprintf(stderr, "Falha ao alocar memória\n");
          free(message);  // Libera qualquer memória já alocada
          return 1;
      }

      message = newResult;
      
      strcpy(message + message_len, buffer);  // Copia a nova string para o final da string concatenada
      message_len = newLen;  // Atualiza o comprimento da string concatenad

       if (strncmp(buffer, END_CHUNK_FLAG, strlen(END_CHUNK_FLAG)) == 0 || endsWithEndTag(message)) {
        removeEndTag(message);
        printf("Mensagem recebida: %s\n", message);
        if(strncmp(message, "exit", 4) == 0) {
            printf("Recebi o exit\n");
            break;
        }
        free(message);
        message_len = 0;
      }


    }

    printf("Finalizando servidor\n");

    // Ler dados do cliente

    return 0;
}
