#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MESSAGE_CHUNK_SIZE 3
#define END_CHUNK_FLAG "##END"


void splitStringIntoChunks(char *input, char **output, int max_chunk_size, int *num_chunks) {
    int input_len = strlen(input);
    *num_chunks = (input_len + max_chunk_size - 1) / max_chunk_size;  // Calcula o número de "chunks" necessários

    // Alocar memória para os "chunks"
    *output = (char *)malloc(*num_chunks * (max_chunk_size + 1) * sizeof(char));

    for (int i = 0, j = 0; i < input_len; i += max_chunk_size, j++) {
        // Copia no máximo "max_chunk_size" caracteres para cada "chunk"
        strncpy(*output + j * (max_chunk_size + 1), input + i, max_chunk_size);

        // Garante que cada "chunk" é uma string válida em C (terminada em '\0')
        (*output)[j * (max_chunk_size + 1) + max_chunk_size] = '\0';
    }
}


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
        char *chunk_start = chunks + i * (MESSAGE_CHUNK_SIZE + 1);
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
