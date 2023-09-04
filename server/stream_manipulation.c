#include "stream_manipulation.h"

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
