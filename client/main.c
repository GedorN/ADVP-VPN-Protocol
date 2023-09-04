#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "stream_manipulation.h"

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

    while(1) { // Loop infinito
      char *chunks;
      int chunks_size;
      printf("Digite a mensagem para o servidor: ");
      fgets(message, sizeof(message), stdin); // Ler uma linha do stdin (teclado)

      // Remover o caractere de nova linha do final, se presente
      if(message[strlen(message) - 1] == '\n') {
          message[strlen(message) - 1] = '\0';
      }

      splitStringIntoChunks(message, &chunks, MESSAGE_CHUNK_SIZE, &chunks_size);
      for (int i = 0; i < chunks_size; i++) {
        printf("Chunk %d: %s de tamanho %d\n", i + 1, chunks + i * (MESSAGE_CHUNK_SIZE + 1),  (int)strlen(chunks + i * (MESSAGE_CHUNK_SIZE + 1)));
        send(sock, chunks + i * (MESSAGE_CHUNK_SIZE + 1), strlen(chunks + i * (MESSAGE_CHUNK_SIZE + 1)), 0);
        read(sock, buffer, 1024);
      }

      send(sock, END_CHUNK_FLAG, strlen(END_CHUNK_FLAG), 0);



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
