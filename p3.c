/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AUTORES: Santiago Garea Cidre (s.garea@udc.es)
         Daniel Queijo Seoane (daniel.queijo.seoane@udc.es)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "list.h"

#include "cmd_functions.h"

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DECLARACIÓN~~~DE~~~FUNCIONES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

bool readInputs(char *cmd, char *arguments[], int *nArguments, tListC *commandList);
// Lee los inputs del usuarios

int chopCmd(char cmd[MAX_BUFFER], char *tokens[]);
// Separa los argumentos del comando

void printPrompt();
// Imprime Prompt (Salida por defecto)

bool processCommand(char **arguments, int nArguments, int * recursiveCount, tListF *fileList, tListC *comandList, tListM *memoryList, tListP *processList);
// Identifica a que comando se refiere la entrada y lo llama.

void freeMemory(char *cmd, char **arguments);
// Libera la memoria de los argumentos y el comando



/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PROGRAMA~~~~PRINCIPAL~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



int main(int argc, char *argv[], char *envp[]) {
    char cmd[MAX_BUFFER];
    char *arguments[MAX_ARGUMENTS];    // PASAR TODO A ESTATICO
    int nArguments;
    int recursiveCount = 0;
    bool quit;
    tListF fileList;
    tListC commandList;
    tListM memoryList;
    tListP processList;

    createList(&commandList);
    createList(&fileList);
    createList(&memoryList);
    createList(&processList);

    if (!insertStdFiles(&fileList)) 
        exit(EXIT_FAILURE);

    do {
        printPrompt();
        quit = readInputs(cmd, arguments, &nArguments, &commandList);
        if (quit)
            quit = processCommand(arguments, nArguments, &recursiveCount , &fileList, &commandList, &memoryList, &processList);
        // freeMemory(cmd, arguments);
    } while (quit);

    freeList(&commandList, freeItemC);
    freeList(&fileList, freeItemF);
    freeList(&memoryList, freeItemM);
    freeList(&processList, freeItemP);
    return EXIT_SUCCESS;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DEFINICIÓN~~~DE~~~FUNCIONES~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void printPrompt() {
    char *userName = getenv("USER"); //Conseguimos el usuario del dispositvo
    char hostName[250];
    if (userName == NULL || gethostname(hostName, sizeof(hostName)) != 0)
        printf("\033[1;31m[?]>> \033[0m"); //Si no obtenemos el usuario y nombre del dispositivo mostramos un interrogante por defecto
    else {
        printf("\033[1;32m");
        printf("[%s@%s]>> \033[0m", userName, hostName); // Formato : [usuario@nombreDelDispositivo]>>
    }
}


bool readInputs(char *cmd, char *arguments[], int *nArguments, tListC *commandList) {
    size_t stringSize = 0;
    bool aux = true;
    int actualCharacter;
    char *inputCopy;


    while (aux) {
        actualCharacter = getchar();
        if (actualCharacter == '\n' || actualCharacter == EOF) {
            cmd[stringSize] = '\0';
            aux = false;
        } else {
            cmd[stringSize] = (char)actualCharacter;
            stringSize++;

            if (stringSize == MAX_BUFFER - 1) {
                printf("\n\033[31mError:\033[0m buffer size exceeded, extra characters will be ignore\n");
                printPrompt();
                while ((actualCharacter = getchar()) != '\n' && actualCharacter != EOF)
                aux = false; // Terminar la lectura
            }
        }
    }

    inputCopy = (char *) malloc(strlen(cmd) + 1);
    strcpy(inputCopy, cmd);

    if (!insertElement(inputCopy, commandList, allocateItemC)) {
        perror("\033[31mError:\033[0m read failed");
        free(inputCopy);
        exit(EXIT_FAILURE);
    }
    free(inputCopy);

    *nArguments = chopCmd(cmd, arguments);
    return true;
}

int chopCmd(char cmd[MAX_BUFFER], char *tokens[]) {
    int i = 1;
    if ((tokens[0] = strtok(cmd, " \n\t")) == NULL)
        return 0;
    while ((tokens[i] = strtok(NULL, " \n\t")) != NULL)
        i++;
    return i;
}

bool processCommand(char **arguments, int nArguments, int *recursiveCount, tListF* fileList, tListC* commandList, tListM* memoryList, tListP* processList) {
    if(*recursiveCount > 10){
        printf("Demasiada recursión en hist \n");
        (*recursiveCount) = 0;
        return true;
    }
    else if(arguments == NULL) return false;
    else if(strcmp(arguments[0], "authors") == 0)
        cmd_authors(arguments, nArguments);
    else if (strcmp(arguments[0], "pid") == 0)
        cmd_pid(arguments, nArguments);
    else if (strcmp(arguments[0], "chdir") == 0)
        cmd_chdir(arguments, nArguments);
    else if (strcmp(arguments[0], "infosys") == 0)
        cmd_infosys(arguments, nArguments);
    else if (strcmp(arguments[0], "date") == 0)
        cmd_date();
    else if (strcmp(arguments[0],"time")==0)
        cmd_time();
    else if (strcmp(arguments[0],"listopen")==0)
        cmd_listopen(fileList);
    else if (strcmp(arguments[0],"open")==0)
        cmd_open(arguments, nArguments, fileList);
    else if (strcmp(arguments[0],"close")==0)
        cmd_close(arguments, fileList);
    else if (strcmp(arguments[0],"dup")==0)
        cmd_dup(arguments, fileList);
    else if (strcmp(arguments[0],"hist")==0)
        cmd_hist(arguments, nArguments, commandList);
    else if (strcmp(arguments[0],"command")==0)
        cmd_command(arguments, nArguments, recursiveCount ,commandList, fileList, memoryList, processList);
    else if (strcmp(arguments[0],"create")==0)
        cmd_create(arguments, nArguments);
    else if (strcmp(arguments[0],"delete")==0)
        cmd_delete(arguments, nArguments);
    else if (strcmp(arguments[0],"deltree")==0)
        cmd_deltree(arguments, nArguments);
    else if (strcmp(arguments[0],"stat")==0)
        cmd_stat(arguments, nArguments);
    else if (strcmp(arguments[0],"list")==0)
        cmd_list(arguments, nArguments);
    else if (strcmp(arguments[0],"help")==0)
        cmd_help(arguments,nArguments);
    else if (strcmp(arguments[0],"malloc")==0)
        cmd_malloc(arguments,nArguments, memoryList);
    else if (strcmp(arguments[0],"shared") == 0)
        cmd_shared(arguments,nArguments, memoryList);
    else if (strcmp(arguments[0],"mmap") == 0)
        cmd_mmap(arguments,nArguments, memoryList);
    else if (strcmp(arguments[0],"read") == 0)
        cmd_read(arguments,nArguments);
    else if (strcmp(arguments[0],"write") == 0)
        cmd_write(arguments,nArguments);
    else if (strcmp(arguments[0],"memfill") == 0)
        cmd_memfill(arguments, nArguments, memoryList);
    else if (strcmp(arguments[0],"memdump") == 0)
        cmd_memdump(arguments, nArguments, memoryList);
    else if (strcmp(arguments[0],"mem") == 0)
        cmd_mem(arguments,nArguments, memoryList);
    else if (strcmp(arguments[0],"recurse") == 0)
        cmd_recurse(arguments, nArguments);
    else if (strcmp(arguments[0],"fork") == 0)
        cmd_fork(processList);
    else if (strcmp(arguments[0],"exec") == 0)
        cmd_exec(arguments, nArguments);
    else if (strcmp(arguments[0],"job") == 0)
        cmd_job(arguments, nArguments, processList);
    else if (strcmp(arguments[0], "jobs")==0)
        cmd_jobs(processList);
    else if (strcmp(arguments[0], "deljobs")==0)
        cmd_deljobs(arguments, nArguments, processList);

    else if ((strcmp(arguments[0], "quit") == 0) || (strcmp(arguments[0], "bye") == 0) || (strcmp(arguments[0], "exit") == 0))
        return false;
    else
        externalProgram(arguments, nArguments, processList);
    return true;
}

void freeMemory(char *cmd, char **arguments) {

    if (arguments != NULL) {
        //freeMyStrTok(arguments);
        arguments = NULL;
    }

    if (cmd != NULL) {
        free(cmd);
        cmd = NULL;
    }
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~COMANDOS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void cmd_command(char *arguments[MAX_ARGUMENTS], int nArguments, int *recursiveCount, tListC *commandList, tListF *fileList, tListM *memoryList, tListP *processList) {
    int numero, nArgumentsHist;
    tItemC command = NULL;
    char *argumentsHist[MAX_ARGUMENTS];
    
    switch (nArguments) {
        case 1:
            printf("Necesitas especificar el número del comando que deseas repetir \n");
            displayListC(*commandList);
            break;
        case 2:
            if(esEnteroPositivo(arguments[1],&numero)){
                if(!getNthElement(numero,*commandList,&command)) //Comprueba si existe el Nth elemento en la lista
                    printf("No hay elemento %d en el historico \n",numero);
                else{
                    command = strdup(command); //Duplicamos la lista, para evitar los problemas de memoria
                    printf("Ejecutando hist (%d): %s \n",numero,command);
                    nArgumentsHist = chopCmd(command, argumentsHist); //Conseguimos el número de argumentos
                    (*recursiveCount)++; //Aumentamos el contador de recursividad
                    processCommand(argumentsHist, nArgumentsHist, recursiveCount, fileList, commandList, memoryList, processList);
                }
                free(command);
            }
            else
                printf("\033[31mError:\033[0m Unexpected argument '%s' found\n", arguments[1]);
            break;
        default:
            printf("\033[31mError:\033[0m Multiple arguments\n");
            break;
    }
}