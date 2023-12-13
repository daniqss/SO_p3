/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AUTORES: Santiago Garea Cidre (s.garea@udc.es)
         Daniel Queijo Seoane (daniel.queijo.seoane@udc.es)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/resource.h>

#define MAX_ELEMENTS 4096

/////////////////FILE LIST///////
typedef void *tItem;

typedef struct {
    char *fileName; // Puntero al nombre del archivo
    int descriptor; // Descriptor de archivo (file descriptor)
    int mode;       // Modo de apertura del archivo
} tItemF;

typedef char *tItemC;

typedef enum {
    MALLOC_MEMORY, SHARED_MEMORY, MAPPED_FILE, ALL_TYPES
} allocationType;

// Struct to represent a memory block
typedef struct {
    void* address;
    size_t size;
    time_t allocationTime;
    allocationType allocationType;
    union {
        struct {
            key_t key;
        } sharedInfo;
        struct {
            char *fileName;
            int fileDescriptor;
        } mappedFileInfo;
    };
} tItemM;

typedef enum {
    FINISHED, STOPPED, SIGNALED, ACTIVE
} statusType;

typedef struct {
    pid_t pid;
    time_t startTime;
    time_t endTime;
    statusType status;
    char *command;
} tItemP;

typedef struct tNode *tPos;

struct tNode {
    void *data;
    tPos next;
};

typedef tPos tList;
typedef tList tListF;
typedef tList tListC;
typedef tList tListM;
typedef tList tListP;

bool isEmpty(tList L); // Función que devuelve si una lista está vacía o no.

tPos first(tList L); // Función que accede a la primera posición de una lista.

void createList(tList *L); // Función que crea una lista vacía.

tPos next(tPos pos); // Función que accede a la siguiente posición a una dada en una lista.

tPos last(tList L); // Función que accede a la última posición de una lista.

bool insertElement(tItem item, tList *L, void (*allocateItem)(tItem*, tItem));

void removeElement(tPos p, tList *L, void (*freeItem)(void*)); // Función que remueve un elemento en la posición dada de una lista.

void freeList(tList *L, void (*freeElement)(void *)); // Función que libera la memoria de una lista.



void allocateItemF(tItem *p, tItem item); // Función que reserva memoria para un item de tipo tItemF
void freeItemF (void *p); // Función que libera la memoria de un item de tipo tItemF
void displayListF(tListF L);
int getFileDescriptor(tPos p);
int getFileMode(tPos p);
char *getFileName(tPos p);
char *getFileModeString(int mode);
tPos findElementF(int fileDescriptor, tListF L);
bool insertStdFiles (tListF *L);        // Inserta los ficheros estándar


void allocateItemC(tItem *p, tItem item);   // Función que reserva memoria para un item de tipo tItemC
void freeItemC (void *p);  // Función que libera la memoria de un item de tipo tItemC
void displayNFirstElements(int n, tListC L);
void displayListC(tListC L); 
bool getNthElement(int n, tListC L, tItemC *item);

void allocateItemM(tItem *qElement, tItem item);
void freeItemM(void *p);
void displayListM(tListM L, allocationType type);
tPos findElementM(void* index, tListF L, allocationType type);

void allocateItemP(tItem *qElement, tItem item);
void freeItemP(void* p);
tItemP *findElementP(int pid, tListP L);
void displayListP(tListP L);
void displayItemP(tItemP *p);
void updateItemP (tItemP *item, int options);
void removeTermSig(tListP *processList, statusType status);

#endif