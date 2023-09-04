#ifndef STRING_CHUNK_UTILS_H
#define STRING_CHUNK_UTILS_H

#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MESSAGE_CHUNK_SIZE 3
#define END_CHUNK_FLAG "##END"


/**
 * Divide uma string em "chunks" (partes) de um tamanho máximo especificado.
 *
 * @param input A string de entrada para ser dividida.
 * @param output Um ponteiro para um array de strings que armazenará os "chunks".
 * @param max_chunk_size O tamanho máximo de cada "chunk".
 * @param num_chunks Um ponteiro para um inteiro que armazenará o número total de "chunks".
 */
void splitStringIntoChunks(char *input, char **output, int max_chunk_size, int *num_chunks);

#endif // STRING_CHUNK_UTILS_H
