#ifndef MY_STRING_UTILS_H
#define MY_STRING_UTILS_H

#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define END_CHUNK_FLAG "##END"
#define MESSAGE_CHUNK_SIZE 3


/**
 * Divide uma string em "chunks" (partes) de um tamanho máximo especificado.
 *
 * @param input A string de entrada para ser dividida.
 * @param output Um ponteiro para um array de strings que armazenará os "chunks".
 * @param max_chunk_size O tamanho máximo de cada "chunk".
 * @param num_chunks Um ponteiro para um inteiro que armazenará o número total de "chunks".
 */
void splitStringIntoChunks(char *input, char **output, int max_chunk_size, int *num_chunks);

/**
 * Verifica se uma string termina com um "end tag" específico.
 *
 * @param str A string a ser verificada.
 * @return 1 se a string termina com o "end tag", 0 caso contrário.
 */
int endsWithEndTag(const char *str);

/**
 * Remove o "end tag" do final de uma string, se estiver presente.
 *
 * @param str A string da qual o "end tag" será removido.
 */
void removeEndTag(char *str);

#endif // MY_STRING_UTILS_H