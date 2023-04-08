# Compilador del make
CC=gcc
# Opciones de compilación usadas
CFLAGS=-g -Wall
# Lista de ejecutables
BIN_FILES=cliente servidor
# Ninguna opción del preprocesador implementada
#export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. 

all: $(BIN_FILES)

cliente: libclaves.so 
	$(CC) $(CFLAGS)  $@.c -o $@ -L. libclaves.so

libclaves.so: claves.c
	$(CC) $(CFLAGS) -o $@ -shared $< -fPIC

servidor: servidor.o impl.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	@echo "Compiling... $<" 
	$(CC) $(CFLAGS) -c $< -o $@

# Tarea de limpieza del proyecto
.PHONY: clean 
clean:
	rm -f $(BIN_FILES) *.o
