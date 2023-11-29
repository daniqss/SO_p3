# /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
# AUTORES: Santiago Garea Cidre (s.garea@udc.es)
#          Daniel Queijo Seoane (daniel.queijo.seoane@udc.es)
# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


# Compilador
CC = gcc

# Nombre del ejecutable
TARGET = p3

# Fuentes y objetos
SRCS = p3.c cmd_functions.c list.c
OBJS = $(SRCS:.c=.o)

# Opciones del compilador
CFLAGS = -Wall

# Regla predeterminada
all: $(TARGET)

# Compilación
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<


# Limpieza de archivos generados
clean:
	rm -rf $(OBJS) $(TARGET)

# Ejecutar el programa
run: $(TARGET)
	./$(TARGET)


# Ejecutar Valgrind
valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes -s ./$(TARGET)

.PHONY: all clean valgrind run
