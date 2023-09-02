#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char message[1024] = {0};
    char buffer[1024] = {0};

    // Criar soquete do cliente
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Falha na criação do soquete");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

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

    while(1) { // Loop infinito
      printf("Digite a mensagem para o servidor: ");
      fgets(message, sizeof(message), stdin); // Ler uma linha do stdin (teclado)

      // Remover o caractere de nova linha do final, se presente
      if(message[strlen(message) - 1] == '\n') {
          message[strlen(message) - 1] = '\0';
      }

      // Comparar a entrada com "exit"

      send(sock, message, strlen(message), 0);
      read(sock, buffer, 1024);
      if(strncmp(message, "exit", 4) == 0) {
          break; // Sair do loop se a entrada for "exit"
      }
      printf("Mensagem recebida: %s\n", buffer);
      // printf("Você digitou: %s\n", message); // Imprimir a entrada
    }

    printf("Finalizando cliente\n");


    // Enviar dados para o servidor

    // Ler dados do servidor

    return 0;
}
