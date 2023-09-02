#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    // char command[50];
    // sprintf(command, "lsof -ti:%d | xargs kill -9 &", PORT);
    // printf("Olha o comando ai: %s\n", command);
    // system(command);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
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
      read(new_socket, buffer, 1024);
      printf("Mensagem recebida: %s\n", buffer);

      // Enviar dados para o cliente
      send(new_socket, hello, strlen(hello), 0);
       if(strncmp(buffer, "exit", 4) == 0) {
          break; // Sair do loop se a entrada for "exit"
      }

    }

    printf("Finalizando servidor\n");

    // Ler dados do cliente

    return 0;
}
