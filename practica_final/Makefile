# Compilador del make
CC=gcc
# Opciones de compilación usadas
CFLAGS=-g -Wall -pthread
# Lista de ejecutables
BIN_FILES=server
# Ninguna opción del preprocesador implementada

all: $(BIN_FILES)

server: server.o impl.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	@echo "Compiling... $<" 
	$(CC) $(CFLAGS) -c $< -o $@

# Tarea de limpieza del proyecto
.PHONY: clean 
clean:
	rm -f $(BIN_FILES) *.o *.so
