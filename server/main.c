#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define END_CHUNK_FLAG "##END"
#define MESSAGE_CHUNK_SIZE 3


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


int endsWithEndTag(const char *str) {
  if (str == NULL) {
    return 0;
  }
  size_t len = strlen(str);
  if (len < strlen(END_CHUNK_FLAG)) {
      return 0;  // A string é muito curta para terminar com "##END"
  }

  // Compara os últimos 5 caracteres com "##END"
  return strcmp(str + len - strlen(END_CHUNK_FLAG), END_CHUNK_FLAG) == 0;
}

void removeEndTag(char *str) {
    size_t len = strlen(str);
    if (len < strlen(END_CHUNK_FLAG)) {
        return;  // A string é muito curta para terminar com "##END"
    }

    // Verifica se os últimos 5 caracteres são "##END"
    if (strcmp(str + len - strlen(END_CHUNK_FLAG), END_CHUNK_FLAG) == 0) {
        str[len - strlen(END_CHUNK_FLAG)] = '\0';  // Remove "##END" substituindo o primeiro '#' por '\0'
    }
}

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

       if(strncmp(buffer, "exit", 4) == 0) {
        break; // Sair do loop se a entrada for "exit"
      } else if (strncmp(buffer, END_CHUNK_FLAG, strlen(END_CHUNK_FLAG)) == 0 || endsWithEndTag(message)) {
        removeEndTag(message);
        printf("Mensagem recebida: %s\n", message);
        free(message);
        message_len = 0;
      }


    }

    printf("Finalizando servidor\n");

    // Ler dados do cliente

    return 0;
}
