/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AUTORES: Santiago Garea Cidre (s.garea@udc.es)
         Daniel Queijo Seoane (daniel.queijo.seoane@udc.es)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "list.h"

bool isEmpty(tList L) {
    return L == NULL;
}

void createList(tList *L) {
    *L = NULL;
}

tPos first(tList L) {
    return L;
} // Devolvemos la primera posición

tPos next(tPos p) {
    return p->next;
} // Devolvemos la posición siguiente

tPos last(tList L) {
    tPos p;
    for (p = L; next(p) != NULL; p = next(p));
    return p;
} // Buscamos la última posición y la devolvemos



bool createNode(tPos *p) {
    *p = malloc(sizeof(struct tNode));
    return *p != NULL;
} // Función auxiliar en la que intentamos reservar memoria para una posición y devolvemos si lo hemos logrado o no.

bool insertElement(tItem item, tList *L, void (*allocateItem)(tItem*, tItem)) {
    tPos p, q;
    tItem qElement;
    
    if (!createNode(&q)) {
        free(q);
        printf("Node creation failed\n");
        return false;
    }
    
    allocateItem(&qElement, item);
    // LLama a la función que pasamos por parámetro
    
    q->data = qElement;
    q->next = NULL;

    if (isEmpty(*L)) {
        *L = q;
    } else {
        p = last(*L);
        p->next = q;
    }
    
    return true;
}

void removeElement(tPos p, tPos *L, void (*freeElement)(void *)) {
    tPos q;

    if (p == NULL) {
        printf("Not valid position\n");
        return;
    }

    if (p == *L) {
        *L = p->next;
    } else {
        for (q = *L; q->next != p; q = q->next);
        q->next = p->next;
    }

    if (freeElement != NULL) {
        freeElement(p->data);  // Llamada a la función para liberar la memoria del elemento
    }

    free(p);
}

void freeList(tList *L, void (*freeElement)(void *)) {
    while(!isEmpty(*L))
        removeElement(*L, L, freeElement);
}



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~FILE_LIST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void allocateItemF(tItem *qElement, tItem item) {
    tItemF *newItemF;

    if ((newItemF = (tItemF*)malloc(sizeof(tItemF))) == NULL) {
        perror("Error allocating memory for newItemF");
        exit(EXIT_FAILURE);
    }
    

    // Asigna memoria para fileName
    if ((newItemF->fileName = (char*)malloc(strlen(((tItemF*)item)->fileName) + 1)) == NULL) {
        perror("Memory allocation for fileName failed\n");
        free(newItemF->fileName);
        free(newItemF);
        exit(EXIT_FAILURE);
    }

    strcpy(newItemF->fileName, ((tItemF*)item)->fileName);
    newItemF->descriptor = ((tItemF*)item)->descriptor;
    newItemF->mode = ((tItemF*)item)->mode;

    *qElement = (tItem)newItemF;
}

void freeItemF(void* element) {
    tItemF *item = (tItemF *)element;

    if (item != NULL) {
        free(item->fileName);
        item->fileName = NULL;
        free(item);
    }
}

tPos findElementF(int fileDescriptor, tListF L) {
    tPos p;

    for (p = L; (p != NULL) && (((tItemF *)(p->data))->descriptor != fileDescriptor); p = p->next);

    return p;
}

int getFileDescriptor(tPos p) {
    return ((tItemF *)(p->data))->descriptor;
}

int getFileMode(tPos p) {
    return ((tItemF *)(p->data))->mode;
}

char *getFileName(tPos p) {
    return ((tItemF *)(p->data))->fileName;
}

char *getFileModeString (int mode) {
    char *resultString = (char *) malloc(58);
    if (resultString == NULL) {
        printf("Memory allocation for modeString failed\n");
        return "Error allocating memory for modeString\n";
    } else {
        strcpy(resultString, "");
    }

    if (mode & O_CREAT) 
        strcat(resultString, "O_CREAT ");
    if (mode & O_EXCL) 
        strcat(resultString, "O_EXCL ");
    if (mode & O_RDONLY) 
        strcat(resultString, "O_RDONLY ");
    if (mode & O_WRONLY) 
        strcat(resultString, "O_WRONLY ");
    if (mode & O_RDWR)
        strcat(resultString, "O_RDWR ");
    if (mode & O_APPEND) 
        strcat(resultString, "O_APPEND ");
    if (mode & O_TRUNC) 
        strcat(resultString, "O_TRUNC ");
    strcat(resultString, "\0");
    return resultString;
}

void displayListF(tListF L) {
    tPos p;
    tItemF *pElement;
    char *modeString;

    for (p = L; p != NULL; p = next(p)) {
        if (p->data != NULL) {
            pElement = ((tItemF *)(p->data));
            modeString = getFileModeString(pElement->mode);
            printf("descriptor: \033[33m%d\033[0m -> \033[34m%s\033[0m %s\n", pElement->descriptor, pElement->fileName, modeString);
            free(modeString);
        }
    }
}

bool insertStdFiles (tListF *L) {
    if (!insertElement(&(tItemF) {"stdin", 0, O_RDWR}, L, allocateItemF)) {
        perror("Error al insertar stdin");
        return false;
    }
    if (!insertElement(&(tItemF) {"stdout", 1, O_RDWR}, L, allocateItemF)) {
        perror("Error al insertar stdout");
        return false;
    }
    if (!insertElement(&(tItemF) {"stderr", 2, O_RDWR}, L, allocateItemF)) {
        perror("Error al insertar stderr");
        return false;
    }

    return true;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~COMMAND_LIST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void allocateItemC(tItem *qElement, tItem item) {
    tItemC newItemC;

    if ((newItemC = (tItemC)malloc(strlen((tItemC)item) + 1)) == NULL) {
        perror("Memory allocation failed\n");
        free(newItemC);
        exit(EXIT_FAILURE);
    }

    strcpy(newItemC, (tItemC)item);

    *qElement = (tItem)newItemC;
}

void freeItemC(void* element) {
    tItemC *item = (tItemC *)element;

    if (item != NULL) {
        free(item);
        item = NULL;
    }
}


bool getNthElement(int n, tListC L, tItemC *item){
    tPos p = NULL;
    int cnt=0;
    for (p = L; (p!= NULL) && (cnt != n) ; p = next(p))
        cnt++;
    if (p!=NULL){
        *item = p->data;
        return true;
    }
    else
        return false;
}

void displayNFirstElements(int n, tListC L) {
    tPos p;
    tItemC pElement;
    int cnt = 0;

    for (p = L; p != NULL && cnt != n; p = next(p)) {
        if (p->data != NULL) {
            pElement = *((tItemC *)(p->data));
            printf("\033[33m%d\033[0m -> \033[34m%s\033[0m \n", cnt, pElement);
            cnt = cnt + 1;
        }
    } // Recorremos la lista y mostramos cada elemento
}

void displayListC(tListC L) {
    tPos p;
    tItemC pElement;
    int cnt = 0;

    for (p = L; p != NULL; p = next(p)) {
        if (p->data != NULL) {
            pElement = ((tItemC)(p->data));
            printf("\033[33m%d\033[0m -> \033[34m%s\033[0m \n", cnt, pElement);
            cnt = cnt + 1;
        }
    } // Recorremos la lista y mostramos cada elemento
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MEMORY_LIST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void allocateItemM(tItem *qElement, tItem item) {
    tItemM *newItemM;

    if ((newItemM = (tItemM*)malloc(sizeof(tItemM))) == NULL) {
        perror("Memory allocation failed\n");
        free(newItemM);
        exit(EXIT_FAILURE);
    }
    // Asigna memoria para el struct tItemM

    newItemM->address = ((tItemM*)item)->address;
    newItemM->size = ((tItemM*)item)->size;
    newItemM->allocationTime = ((tItemM*)item)->allocationTime;
    newItemM->allocationType = ((tItemM*)item)->allocationType;
    // Copia los datos del item que le pasamos por parámetro
    
    if (newItemM->allocationType == SHARED_MEMORY) {
        newItemM->sharedInfo.key = ((tItemM*)item)->sharedInfo.key;
    } else if (newItemM->allocationType == MAPPED_FILE) {
        newItemM->mappedFileInfo.fileName = (char*)malloc(strlen(((tItemM*)item)->mappedFileInfo.fileName) + 1);
        newItemM->mappedFileInfo.fileName = strcpy(newItemM->mappedFileInfo.fileName, ((tItemM*)item)->mappedFileInfo.fileName);
        newItemM->mappedFileInfo.fileDescriptor = ((tItemM*)item)->mappedFileInfo.fileDescriptor;
    }
    // Copia los datos específicos de cada tipo de memoria

    *qElement = (tItem)newItemM;
    // Asigna el nuevo elemento a la posición que le pasamos por parámetro
}

void freeItemM(void* element) {
    tItemM *item = (tItemM *)element;

    if (item != NULL) {
        if (item->allocationType == SHARED_MEMORY) {
            if((shmget(item->sharedInfo.key, 0, 0)) != -1){
                if(shmdt(item->address) == -1){
                    perror("shmdt");
                    return;
                } 
            }
            // Desmapea la memoria compartida
        } else if (item->allocationType == MAPPED_FILE) {
            if (item->mappedFileInfo.fileDescriptor != -1) {
                if (munmap(item->address, item->size) == -1) {
                    perror("Error al liberar memoria mapeada");
                }
                if (close(item->mappedFileInfo.fileDescriptor) == -1) {
                    perror("Error closing file");
                }
                free(item->mappedFileInfo.fileName);
            }
            // Libera la memoria mapeada, cierra el archivo y libera la memoria del nombre del archivo
        } else if (item->allocationType == MALLOC_MEMORY) {
            if (item->address != NULL) {
                free(item->address);
            }
            // Libera la memoria del bloque
        }

        free(item);     // Libera la memoria del elemento
        item = NULL;
    }
}

void displayListM(tListM L, allocationType type) {
    tPos p;
    tItemM* pElement;
    char typeString[20];
    char timeString[20];

    if (type == SHARED_MEMORY) strcpy(typeString, "shared");
    else if (type == MAPPED_FILE) strcpy(typeString, "mmap");
    else if (type == MALLOC_MEMORY) strcpy(typeString, "malloc");
    else if (type == ALL_TYPES) strcpy(typeString, "all");
    // Consigue una cadena con el tipo de memoria que le pasamos por parámetro
    printf("******Lista de bloques asignados %s para el proceso %d\n", typeString, getpid());

    for (p = L; p != NULL; p = next(p)) {
        if (p->data == NULL) exit(EXIT_FAILURE);
        pElement = (tItemM*)(p->data);

        // Verifica si se debe mostrar el bloque según el tipo
        if (type != ALL_TYPES && pElement->allocationType != type) continue;

        strftime(timeString, sizeof(timeString), "%e %b %y %H:%M", localtime(&pElement->allocationTime));
        printf("%22p%30s %s\n", pElement->address, timeString,  typeString);

        switch (pElement->allocationType) {
            case SHARED_MEMORY:
                printf(" (key %d)", pElement->sharedInfo.key);
                break;
            case MAPPED_FILE:
                printf(" %s  (descriptor %d)", pElement->mappedFileInfo.fileName, pElement->mappedFileInfo.fileDescriptor);
                break;
            default:
                break;
        }

        puts("");
    }


    puts("");
    }// Recorremos la lista y mostramos cada elemento

tPos findElementM(void* index, tListF L, allocationType type) {
    tPos p = NULL;
    tItemM* pElement = NULL;

    if (isEmpty(L) || index == NULL) {
        return NULL;
    }

    switch (type) {
        case SHARED_MEMORY:
            for (p = L; p != NULL; p = p->next) {       // Busca según la key
                pElement = (tItemM*)(p->data);
                if (pElement->allocationType == type && pElement->sharedInfo.key == *(int*)index) 
                    break;
            }
            break;
        case MAPPED_FILE:
            for (p = L; p != NULL; p = p->next) {       // Busca según el nombre del fichero
                pElement = (tItemM*)(p->data);
                if (pElement->allocationType == type && strcmp(pElement->mappedFileInfo.fileName, (char*)index) == 0) {
                    break;
                }
            }
            break;
        case MALLOC_MEMORY:
            for (p = L; p != NULL; p = p->next) {       // Busca según el tamaño
                pElement = (tItemM*)(p->data);
                if (pElement->allocationType == type && pElement->size == *(int*)index) {
                    break;
                }
            }
            break;
        default:
            break;
    }
    return p;
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PROCCESS_LIST~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void allocateItemP(tItem *qElement, tItem item) {
    tItemP *newItemP;

    if ((newItemP = (tItemP*)malloc(sizeof(tItemP))) == NULL) {
        perror("Error allocating memory for newItemP");
        exit(EXIT_FAILURE); 
    }
    

    // Asigna memoria para fileName
    if ((newItemP->command = (char*)malloc(strlen(((tItemP*)item)->command) + 1)) == NULL) {
        perror("Memory allocation for fileName failed\n");
        free(newItemP);
        exit(EXIT_FAILURE);
    }

    newItemP->pid = ((tItemP*)item)->pid;
    newItemP->time = ((tItemP*)item)->time;
    newItemP->status = ((tItemP*)item)->status;
    strcpy(newItemP->command, ((tItemP*)item)->command);
    newItemP->priority = ((tItemP*)item)->priority;


    *qElement = (tItem)newItemP;
}

void freeItemF(void* element) {
    tItemP *item = (tItemP *)element;

    if (item != NULL) {
        free(item->command);
        item->command = NULL;
        free(item);
    }
}

tPos findElementP(int pid, tListP L) {
    tPos p;

    for (p = L; (p != NULL) && (((tItemP *)(p->data))->pid != pid); p = p->next);

    return p;
}


void displayListP(tListP L) {
    tPos p;
    tItemP *pElement;

    for (p = L; p != NULL; p = next(p)) {
        if (p->data != NULL) {
            pElement = ((tItemP *)(p->data));
            printf("descriptor: \033[33m%d\033[0m -> \033[34m%s\033[0m\n", pElement->pid, pElement->command);
        }
    }
}