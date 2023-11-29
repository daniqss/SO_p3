/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AUTORES: Santiago Garea Cidre (s.garea@udc.es)
         Daniel Queijo Seoane (daniel.queijo.seoane@udc.es)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <stdint.h>
#include <openssl/sha.h>
#include <sys/wait.h>


#include "list.h"


#define MAX_ARGUMENTS 1024
#define MAX_BUFFER 1024
#define DEFAULT_PERMISSIONS 0777
#define TAMANO 2048
#define MAX_SHARED_MEM_SIZE 1024 * 1024

void cmd_authors(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Muestra el nombre y los logins los autores del programa
 * Adicionalmente, si agregamos a la entrada un:
 *  -l Muestra solo los logins
 *  -n Muestra solo los nombres
 */
void cmd_pid(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Muestra el pid del proceso que se está ejecutando en el shell:
 * Adicionalmente, si agregamos a la entrada un:
 *  -p Muestra el pid del proceso padre del proceso
 */
void cmd_chdir(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Cambia la dirección de trabajo del shell por la que se le indique.
 * Adicionalmente, si no se le indica ningunha dirección muestra la dirección actual.
 */
void cmd_date();
/*
 * Muestra la fecha actual.
 */
void cmd_time();
/*
 * Muestra la hora actual.
 */
void cmd_hist(char *arguments[MAX_ARGUMENTS], int nArguments, tListC *commandList);
/*
 * Muestra todos los comandos ejecutados en el programa por orden (historial de comando)
 * Adicionalmente, si agregamos a la entrada un:
 *  -c Borra el historial en vez de mostrarlo
 *  -N (siendo N un número natural) muestra los N primeros comandos.
 */
void cmd_command(char *arguments[MAX_ARGUMENTS], int nArguments, int *recursiveCount, tListC *commandList, tListF *fileList, tListM *memoryList);
/*
 * Ejecuta el comando número N (siendo N un número natural)
 *
 */
bool esEnteroPositivo(const char *cadena,int *numero);
/*
 * Comprueba si una cadena es un número entero positivo
 */
void cmd_open(char *arguments[MAX_ARGUMENTS], int nArguments, tListF *fileList);
/*
 * Permite abrir un archivo y añadirlo a la lista de archivos abiertos
 * Le debemos pasar el nombre del fichero y el/los modos en los que queremos abrirlo
 * Adicionalmente, si no le pasamos ningún parámetro muestra la lista de archivos abiertos
 */
void cmd_close (char *arguments[MAX_ARGUMENTS], tListF *fileList);
/*
 * Permite cerrar un fichero abierto y eliminarlo de la lista de archivos abiertos
 * Debemos pasarle como parámetro el descriptor del fichero que queremos cerrar
 * Adicionalmente, si no le pasamos ningún parámetro muestra la lista de archivos abiertos
 */
void cmd_dup (char *arguments[MAX_ARGUMENTS], tListF *fileList);
/*
 * Permite duplicar el descriptor de fichero abierto y añadirlo a la lista de archivos abiertos
 * Le debemos pasar el descriptor del fichero que queremos duplicar
 * Adicionalmente, si no le pasamos ningún parámetro muestra la lista de archivos abiertos
 */
void cmd_listopen( tListF *fileList);
/*
 * Muestra la lista de archivos abiertos
 */
void cmd_infosys(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Muestra la información de la máquina que ejecuta el shell
 */

void cmd_create (char *arguments[MAX_ARGUMENTS], int nArguments);
/*
* Crea un fichero con el nombre que le pasemos como parámetro
*/

void cmd_delete (char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Borra un fichero o directorio con el nombre que le pasemos como parámetro
 */

void cmd_deltree(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Borra recursivamente un directorio con el nombre que le pasemos como parámetro
 */

void cmd_list (char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Muestra los ficheros del directorio indicado
 */

void cmd_clear ();
/*
 * Limpia la pantalla
 */

void cmd_stat (char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Muestra la información del fichero o directorio que le pasemos como parámetro
 */

void cmd_help(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Muestra la lista de comandos disponibles del shell
 * Adicionalmente, si agregamos a la entrada un comando muestra una descripción sobre lo que hace ese comando
 */
void cmd_malloc(char *arguments[MAX_ARGUMENTS], int nArguments, tListM* memoryList);
/*
 * Reserva un bloque de memoria del tamaño que le pasemos como parámetro
 * Con -free liberamos un bloque de memoria con el tamaño que le pasemos como parámetro
 */
void cmd_shared(char *arguments[MAX_ARGUMENTS], int nArguments, tListM* memoryList);
/*
 * Reserva un bloque de memoria compartida (-create) del tamaño que le pasemos como parámetro
 * Con -free liberamos un bloque de memoria compartida con el tamaño que le pasemos como parámetro
 * Con -delkey eliminamos del sistema la clave de memoria compartida que le pasemos como parámetro
 */

void cmd_mmap(char *arguments[MAX_ARGUMENTS], int nArguments, tListM* memoryList);
/*
 * Reserva un bloque de memoria mapeada a fichero con los permisos que le pasemos como parámetro
 * Con -free liberamos un bloque de memoria mapeada a fichero del nombre que le pasemos como parámetro
 */
void cmd_read(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Carga el contenido de un fichero en memoria reservada previamente
 */

void cmd_write(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * Escribe el contenido de un bloque de memoria reservada previamente en un fichero
 */

void cmd_memdump(char *arguments[MAX_ARGUMENTS], int nArguments, tListM *memoryList);
/*
 * Muestra el contenido de la memoria a partir de la dirección que le pasemos como parámetro
 */

void cmd_memfill(char *arguments[MAX_ARGUMENTS], int nArguments, tListM *memoryList);
/*
 * Rellena el contenido de la memoria a partir de la dirección que le pasemos como parámetro
 */

void cmd_mem(char *arguments[MAX_ARGUMENTS], int nArguments, tListM *memoryList);
/*
 * Muestra información de la memoria del proceso
 */

void cmd_recurse(char *arguments[MAX_ARGUMENTS], int nArguments);
/*
 * LLama a una funcion recursiva las veces que le pasemos como parámetro
 */
