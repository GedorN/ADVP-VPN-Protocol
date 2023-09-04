# Variáveis de compilação
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Nome do programa de saída
OUTPUT = server

# Fontes e objetos
SOURCES = main.c stream_manipulation.c
OBJECTS = $(SOURCES:.c=.o)

# Regras padrão
all: $(OUTPUT)

# Regra para o programa de saída
$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJECTS)

# Regras de compilação
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regras adicionais
clean:
	rm -f $(OBJECTS) $(OUTPUT)

run: $(OUTPUT)
	./$(OUTPUT)

.PHONY: all clean run
