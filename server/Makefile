# Variáveis de compilação
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I/usr/local/include
LDFLAGS = -lssl -lcrypto

# Nome do programa de saída
OUTPUT = server

# Fontes e objetos
SOURCES = main.c stream_manipulation.c key_manager.c
OBJECTS = $(SOURCES:.c=.o)

# Regras padrão
all: $(OUTPUT)

debug: CFLAGS += -DDEBUG -g
debug: all run

# Regra para o programa de saída
$(OUTPUT): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(OUTPUT) $(OBJECTS) $(LDFLAGS)

# Regras de compilação
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regras adicionais
clean:
	rm -f $(OBJECTS) $(OUTPUT)

run: $(OUTPUT)
	./$(OUTPUT)

.PHONY: all clean run
