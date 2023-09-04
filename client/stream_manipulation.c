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
