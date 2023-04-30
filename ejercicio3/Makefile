# Compilador del make
CC=gcc
# Opciones de compilación usadas
CFLAGS=-g -Wall -I/usr/include/tirpc
# Lista de ejecutables
BIN_FILES=cliente servidor
# Ninguna opción del preprocesador implementada
LDLIBS+=-lnsl -lpthread -ldl -ltirpc

all: $(BIN_FILES)

cliente: libclaves.so
	$(CC) $(CFLAGS)  $@.c -o $@ -L. libclaves.so

libclaves.so: claves.c servicios_clnt.o servicios_xdr.o
	$(CC) $(CFLAGS)  -o $@ -shared $< -fPIC servicios_clnt.o servicios_xdr.o

servidor: servicios_server.o impl.o servicios_svc.o servicios_xdr.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	@echo "Compiling... $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Tarea de limpieza del proyecto
.PHONY: clean
clean:
	rm -f $(BIN_FILES) *.o *.so
	rm arrays.txt
