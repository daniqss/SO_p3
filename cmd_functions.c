/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AUTORES: Santiago Garea Cidre (s.garea@udc.es)
         Daniel Queijo Seoane (daniel.queijo.seoane@udc.es)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "cmd_functions.h"

#define MAX_PATH 4096

#define INCLUDE_HIDDEN 0b1     // 1
#define RECURSIVE_BEFORE 0b10       // 2
#define RECURSIVE_AFTER 0b100        // 4
#define INCLUDE_LONG 0b1000       // 8
#define INCLUDE_LINK 0b10000      
#define INCLUDE_ACC 0b100000        // 32
#define INCLUDE_FILE 0b1000000       // 64


bool esEnteroPositivo(const char *cadena, int *numero);

int directoryElements (const char *directory);
void delete (const char *file);
void deltree (const char *notEmptyDirectory);

void reca(const char *directoryName, int include);
void recb(const char *directoryName, int include);
void print_Stat(const char *file, const struct stat *fileStat, int include);


void sharedDelkey (key_t key);
void *sharedCreate (key_t key, size_t size);
void mapFile (char * file, int protection, tItemM *newItem);
ssize_t readFile (char *fileName, void *p, size_t cont);
ssize_t writeFile (char *fileName, void *p, size_t cont, bool overwrite);
void fillMemory(void *p, size_t cont, unsigned char byte);

void fillMemory (void *p, size_t cont, unsigned char byte);
void recursive (int n);

bool isEnvVar(char *argument);
char* getCommandPath(const char *command);
void executeInForeground(char* commandPath, char **arguments);
void executeInBackground(char* commandPath, char **arguments, tListP *processList);

// Variables globales
int variableGlobal1 = 10;
int variableGlobal2 = 20;
int variableGlobal3 = 30;

// Variables globales no inicializadas (N.I.)
int variableGlobal1Ni;
int variableGlobal2Ni;
int variableGlobal3Ni;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~COMANDOS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void cmd_authors(char *arguments[MAX_ARGUMENTS], int nArguments) {
    char authorsNames[] = "Santiago Daniel";
    char authorsLogins[] = "s.garea@udc.es daniel.queijo.seoane@udc.es";

    switch (nArguments) {
        case 1:
            printf("%s\n%s\n", authorsNames, authorsLogins);
            break;
        case 2:
            if (strcmp(arguments[1], "-l") == 0)
                printf("%s\n", authorsLogins);
            else if (strcmp(arguments[1], "-n") == 0)
                printf("%s\n", authorsNames);
            else
                printf("\033[31mError:\033[0m Unexpected argument \033[33m'%s'\033[0m found\n", arguments[1]);
            break;
        default:
            printf("\033[31mError:\033[0m Multiple arguments\n");
            break;
    }
}

void cmd_pid(char *arguments[MAX_ARGUMENTS], int nArguments) {

    switch (nArguments) {
        case 1:
            printf("Current process identifier: %d\n", getpid());
            break;
        case 2:
            if (strcmp(arguments[1], "-l") == 0)
                printf("Current process parent identifier: %d\n", getppid());
            else
                printf("\033[31mError:\033[0m Unexpected argument '%s' found\n", arguments[1]);
            break;
        default:
            printf("\033[31mError:\033[0m Multiple arguments\n");
            break;
    }
}

void cmd_chdir(char *arguments[MAX_ARGUMENTS], int nArguments) {
    char *cwd = (char *) malloc(MAX_BUFFER);
    if (cwd == NULL) {
        perror("\033[31mError:\033[0m Memory allocation error");
        free(cwd);
        exit(EXIT_FAILURE);
    }

    switch (nArguments) {
        case 1:
            if (getcwd(cwd, MAX_BUFFER) == NULL) {
                perror("\033[31mError:\033[0m getcwd() error");
                free(cwd);
                exit(EXIT_FAILURE);
            } else {
                printf("Current working directory:\033[34m %s\n\033[0m", cwd);
                free(cwd);
            }
            break;

        case 2:
            chdir(arguments[1]);
            free(cwd);
            break;

        default:
            printf("\033[31mError:\033[0m Multiple arguments\n");
            free(cwd);
            break;
    }
}

struct tm getDateTime(){
    time_t actual;
    struct tm *organized;
    time(&actual); //Conseguimos el tiempo actual
    organized = localtime(&actual); //Descomponemos el tiempo en valores (horas, minutos, segundos, años, dia,...)
    return *organized;
} //Función auxiliar que devuelve un struct con la hora y fechas actuales

void cmd_date(){
    struct tm organizedData;
    organizedData = getDateTime();
    printf("%02d/%02d/%04d\n", organizedData.tm_mday,organizedData.tm_mon, organizedData.tm_year);
}

void cmd_time(){
    struct tm organizedTime;
    organizedTime = getDateTime();
    printf("%02d:%02d:%02d\n", organizedTime.tm_hour,organizedTime.tm_min, organizedTime.tm_sec);
}

bool esEnteroPositivo(const char *cadena, int *numero){
    if (*cadena == '\0')
        return false;
    char *temp;
    *numero = (int)strtol(cadena,  &temp, 10); //Guarda los números y en temp el resto de caracteres

    if (*temp == '\0' && *numero >= 0) //Si no hay otros caracteres o el número es mayor igual que cero devolvemos true.
        return true;

    else 
        return false;
}//Función auxiliar que comprueba si una cadena está compuesta solo de dígitos (por lo tanto un número positivo).

void cmd_hist(char *arguments[MAX_ARGUMENTS], int nArguments, tListC *commandList){
    char *primerChar;
    int numero;
    char *restoDeLaCadena;
    int length;

    switch (nArguments) {
        case 1: // Mostrar historial
            displayListC(*commandList);
            break;
        case 2:
            primerChar = &arguments[1][0];
            if (*primerChar == '-') {
                restoDeLaCadena = arguments[1] + 1;
                length = strlen(restoDeLaCadena) - 1;
                if (restoDeLaCadena[length] == '\n') {
                    restoDeLaCadena[length] = '\0'; // Reemplaza el '\n' con '\0'
                }
                if (strcmp(restoDeLaCadena, "c") == 0) {
                    freeList(commandList, freeItemC); // Borrar historial de comandos
                } else if (esEnteroPositivo(restoDeLaCadena, &numero)) {
                    displayNFirstElements(numero, *commandList); // Mostrar los N primeros elementos si se introduce un número natural positivo
                } else {
                    printf("\033[31mError:\033[0m Unexpected argument \033[33m'%s'\033[0m found\n", arguments[1]);
                }
            } else {
                printf("\033[31mError:\033[0m Unexpected argument \033[33m'%s'\033[0m found\n", arguments[1]);
            }
            break;
        default:
            printf("\033[31mError:\033[0m Multiple arguments\n");
            break;
    }
}

void cmd_infosys(char *arguments[MAX_ARGUMENTS], int nArguments) {
    struct utsname machineInfo;

    if (uname(&machineInfo) == -1) {
        perror("\033[31mError:\033[0m ename error");
        exit(EXIT_FAILURE);
    }
    printf("%s (%s), OS: %s-%s-%s\n",machineInfo.nodename,machineInfo.machine,machineInfo.sysname,machineInfo.release, machineInfo.version);
}

void cmd_listopen(tListF *fileList){
    // Listar archivos abiertos
    if (isEmpty(*fileList))
        printf("No hay archivos abiertos");
    else 
        displayListF(*fileList);
}

void cmd_open(char *arguments[MAX_ARGUMENTS], int nArguments, tListF *fileList) {
    int i, fileDescriptor, mode = 0;

    if (nArguments == 1)
        // Listar archivos abiertos
        cmd_listopen(fileList);
    else {
        
        for (i = 2; arguments[i] != NULL; i++) {
            if (!strcmp(arguments[i], "cr"))
                mode |= O_CREAT;
            else if (!strcmp(arguments[i], "ex"))
                mode |= O_EXCL;
            else if (!strcmp(arguments[i], "ro"))
                mode |= O_RDONLY;
            else if (!strcmp(arguments[i], "wo"))
                mode |= O_WRONLY;
            else if (!strcmp(arguments[i], "rw"))
                mode |= O_RDWR;
            else if (!strcmp(arguments[i], "ap"))
                mode |= O_APPEND;
            else if (!strcmp(arguments[i], "tr"))
                mode |= O_TRUNC;
            else
                break;
        }

        if ((fileDescriptor = open(arguments[1], mode, DEFAULT_PERMISSIONS)) == -1)
            perror("\033[31mImposible abrir fichero:\033[0m No such file or directory");
        else {
            insertElement(&(tItemF) {arguments[1], fileDescriptor, mode}, fileList, allocateItemF);

            printf("Añadida entrada \033[33m%d\033[0m a la tabla de ficheros abiertos\n", fileDescriptor);
        }
    }
}

void cmd_close (char *arguments[MAX_ARGUMENTS], tListF *fileList) {
    int fileDescriptor;

    if (arguments[1]==NULL || (fileDescriptor=atoi(arguments[1]))<0) {
        cmd_listopen(fileList);
        return;
    }
    if (close(fileDescriptor)==-1)
        perror("\033[31mError:\033[0m Imposible cerrar descriptor");
    else {
        removeElement(findElementF(fileDescriptor, *fileList), fileList, freeItemF);
    }
}

void cmd_dup (char *arguments[MAX_ARGUMENTS], tListF *fileList) {
    int fileDescriptor, newFileDescriptor;
    tPos file;
    char aux[MAX_ARGUMENTS];

    if (arguments[1]==NULL || (fileDescriptor=atoi(arguments[1]))<0) {
        cmd_listopen(fileList);
        return;
    }
    if ((file = findElementF(fileDescriptor, *fileList)) == NULL) {
        perror("\033[31mImposible duplicar descriptor: \033[0m Bad file descriptor\n");
        return;
    }
    fileDescriptor = getFileDescriptor(file);
    newFileDescriptor = dup(fileDescriptor);

    sprintf (aux,"dup \033[33m%d\033[0m (\033[34m%s\033[0m)",fileDescriptor, getFileName(file));
    insertElement(&(tItemF) {aux, newFileDescriptor, getFileMode(file)}, fileList, allocateItemF);
}

void cmd_create (char *arguments[MAX_ARGUMENTS], int nArguments) {
    if (nArguments < 2) {
        cmd_chdir(arguments, nArguments);
        return;
    }
        
    if (strcmp(arguments[1], "-f") == 0) {
            if (nArguments < 3) {
                cmd_chdir(arguments, nArguments);
                return;
            }
            if (creat(arguments[2], DEFAULT_PERMISSIONS) == -1)
                perror("creat");
        }
    else {
            if (mkdir(arguments[1], DEFAULT_PERMISSIONS) != 0)
                perror("mkdir");
    }
}

void cmd_delete (char *arguments[MAX_ARGUMENTS], int nArguments) {
    int i;

    if (nArguments < 2) {
        cmd_chdir(arguments, nArguments);
        return;
    }

    for (i = 1; i < nArguments; i++) {
        delete(arguments[i]);
    }

}

void delete (const char *file) {
    int nElements = directoryElements(file);

    if (nElements <= -2) {
        perror("opendir");
        return;
    }
    else if (nElements == -1) {
        if (unlink(file) == -1) {
            if (errno == EISDIR) {
                if (rmdir(file) == -1)
                    perror("rmdir");
            else
                perror("unlink");
            }
        }
    }
    else if (nElements == 0) {
        if (rmdir(file) == -1) {
            perror("rmdir");
            return;
        }
    }
    else {
        fprintf(stderr, "El directorio no está vacío\n");
        return;
    }


}

void cmd_deltree (char *arguments[MAX_ARGUMENTS], int nArguments) {
    int i;

    if (nArguments < 2) {
        cmd_chdir(arguments, nArguments);
        return;
    }

    for (i = 1; i < nArguments; i++) 
            deltree(arguments[i]);
}

void deltree(const char *notEmptyDirectory) {
    struct dirent *entry;
    DIR *dirp;
    char *completePath;
    struct stat fileStat;

    if ((dirp = opendir(notEmptyDirectory)) == NULL) {
        perror("opendir");
        return;
    }
    // Abrimos el directorio y comprobamos que no haya habido ningún error

    // Mientras haya entradas en el directorio que no sean"." o".." las borramos
    while ((entry = readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {

            // Conseguimos el path completo del directorio o fichero
            int pathLength = strlen(notEmptyDirectory) + strlen(entry->d_name) + 2;
            if ((completePath = (char *)malloc(pathLength)) == NULL) {
                perror("malloc");
                closedir(dirp);
                return;
            }

            snprintf(completePath, pathLength, "%s/%s", notEmptyDirectory, entry->d_name);

            if (stat(completePath, &fileStat) < 0) {
                perror("stat");
                free(completePath);
                closedir(dirp);
                return;
            }

            // Si es un fichero lo borramos, si es un directorio no vacío llamamos a la función recursivamente
            if (S_ISREG(fileStat.st_mode)) {
                delete(completePath);
            } else if (S_ISDIR(fileStat.st_mode)) {
                deltree(completePath);
            }
            free(completePath);
        }
    }

    closedir(dirp);
    // Borramos el directorio actual (ya vacío)
    delete(notEmptyDirectory);
}

int directoryElements (const char *directory) {
    struct dirent *entry;
    DIR *dirp;
    int aux;

    if ((dirp = opendir(directory)) == NULL) {
        if (errno == ENOTDIR)
            return -1;
        // Si no es un directorio devolvemos -1
        else
            return -2;
        // Si hay algún otro error devolvemos -2
        closedir(dirp);
        return -2;
    }


    aux = 0;

    // Mientras haya entradas en el directorio que no sean "." o ".." las contamos
    while((entry = readdir(dirp)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
            aux++;
        }
    }
    closedir(dirp);
    return aux;
}

char LetraTF (mode_t m) {

    switch (m & S_IFMT) { //and bit a bit con los bits de formato,0170000
        case S_IFSOCK:
            return 's'; //socket
        case S_IFLNK:
            return 'l'; //symbolic link
        case S_IFREG:
            return '-'; //fichero normal
        case S_IFBLK:
            return 'b'; //block device
        case S_IFDIR:
            return 'd'; //directorio
        case S_IFCHR:
            return 'c'; //char device
        case S_IFIFO:
            return 'p'; //pipe
        default:
            return '?'; //desconocido, no deberia aparecer
    }
}

char *get_permissions(mode_t mode) {
    char *permissions;

    if ((permissions = (char *)malloc(12)) == NULL)
        return NULL;
    strcpy(permissions, "---------- ");

    permissions[0] = LetraTF(mode);

    if (mode & S_IRUSR) permissions[1] = 'r';    //propietario
    if (mode & S_IWUSR) permissions[2] = 'w';
    if (mode & S_IXUSR) permissions[3] = 'x';
    if (mode & S_IRGRP) permissions[4] = 'r';    //grupo
    if (mode & S_IWGRP) permissions[5] = 'w';
    if (mode & S_IXGRP) permissions[6] = 'x';
    if (mode & S_IROTH) permissions[7] = 'r';    //resto
    if (mode & S_IWOTH) permissions[8] = 'w';
    if (mode & S_IXOTH) permissions[9] = 'x';
    if (mode & S_ISUID) permissions[3] = 's';    //setuid, setgid y stickybit
    if (mode & S_ISGID) permissions[6] = 's';
    if (mode & S_ISVTX) permissions[9] = 't';

    return permissions;
}

void print_Stat(const char *file, const struct stat *fileStat, int include) {
    if (include & INCLUDE_LONG) {
        char time[20];
        struct passwd *pw = getpwuid(fileStat->st_uid);
        struct group *gr = getgrgid(fileStat->st_gid);
        char *permissions = get_permissions(fileStat->st_mode);
        // conseguimos usuario grupo y permisos

        strftime(time, sizeof(time), "%Y/%m/%d-%H:%M", localtime(&fileStat->st_mtime));
        // Conseguimos la fecha de modificación del archivo

        printf("%s   %lld (%ld)", time, (long long)fileStat->st_nlink, (long)fileStat->st_ino);
        printf("%s %s %s", pw->pw_name, gr->gr_name, permissions); //Cadena de caracteres con los permisos del archivo
    }
    else if (include & INCLUDE_ACC) {
        // Si pasamos el argumento -acc mostramos la fecha de acceso

        char time[20];
        strftime(time, sizeof(time), "%Y/%m/%d-%H:%M", localtime(&fileStat->st_atime));
        printf("%s \n",time);
    }
    else if ((include & INCLUDE_LINK) && S_ISLNK(fileStat->st_mode)) {
        // Si pasamos el argumento -link y es un enlace mostramos el enlace simbólico

        char link[MAX_BUFFER];
        ssize_t len = readlink(file, link, sizeof(link) - 1);

        if (len != -1) {
            link[len] = '\0';
            printf(" %s -> %s \n",file, link);
        }
    }
    printf(" %lld %s \n", (long long)fileStat->st_size, file);
}


void cmd_stat (char *arguments[MAX_ARGUMENTS], int nArguments){
    int include = 0;

    struct stat fileStat;
    if (nArguments < 2) {
        cmd_chdir(arguments, nArguments);
        return;
    }
    for (int i = 1; i < nArguments; i++) {
        if (strcmp(arguments[i], "-long") == 0) {
            include |= INCLUDE_LONG;
        } else if (strcmp(arguments[i], "-acc") == 0) {
            include |= INCLUDE_ACC;
        } else if (strcmp(arguments[i], "-link") == 0) {
            include |= INCLUDE_LINK;
        } else {
            include |= INCLUDE_FILE;

            if (lstat(arguments[i], &fileStat)==-1){ //Obtenemos información del archivo/directorio, si devuelve un -1 ha habido un error
                //Usamos lstat para que si es un enlace simbólico nos dé información de él y no de a lo que apunta
                perror("stat");
                continue; //Aunque haya un error queremos trabajar con el resto si existen
            }
            else {
                print_Stat(arguments[i], &fileStat, include);
            }
        }
    }
    if (!(include & INCLUDE_FILE)) {
        cmd_chdir(arguments, 1);
        return;
    }
}

void cmd_help(char *arguments[MAX_ARGUMENTS], int nArguments) {
    char *comando = arguments[1];
    switch (nArguments) {
        case 1:
            printf("'help [cmd]' ayuda sobre comandos\n"
                   "\t\tComandos disponibles: \n");
            printf("exit bye fin quit help infosys listopen dup close open command hist time date chdir pid authors create delete deltree list stat\n");
            break;
        case 2:
            if (strcmp(comando, "time") == 0) {
                printf("time \t Muestra la hora actual\n");
            } else if (strcmp(comando, "date") == 0) {
                printf("date\tMuestra la fecha actual\n");
            } else if (strcmp(comando, "hist") == 0) {
                printf("hist [-c|-N]\tMuestra (o borra) el historial de comandos\n");
                printf(" \t -N: muestra los N primeros\n");
                printf(" \t -c: borra el historial\n");
            } else if (strcmp(comando, "command") == 0) {
                printf("command [-N] \t Repite el comando N (del historial)\n");
            } else if (strcmp(comando, "open") == 0) {
                printf("open fich m1 m2... \t Abre el fichero fich. y lo añade a la lista de ficheros abiertos del shell\n");
                printf("\tm1, m2... es el modo de apertura (or bit a bit de los siguientes).\n");
                printf("\tcr: O_CREAT\tap: O_APPEND\n");
                printf("\tex: O_EXCL \tro: O_RDONLY\n");
                printf("\trw: O_RDWR \two: O_WRONLY\n");
                printf("\ttr: O_TRUNC\n");
            } else if (strcmp(comando, "close") == 0) {
                    printf("close df \t Cierra el descriptor df y elimina el correspondiente fichero de la lista de ficheros abiertos\n");
            } else if (strcmp(comando, "dup") == 0) {
                printf("dup df \t Duplica el descriptor de fichero df y añade una nueva entrada a la lista de ficheros abiertos\n");
            } else if (strcmp(comando, "listopen") == 0) {
                printf("listopen [n] \t Lista los ficheros abiertos (al menos n) del shell\n");
            } else if (strcmp(comando, "infosys") == 0) {
                printf("infosys\tMuestra información de la máquina donde corre el shell\n");
            } else if (strcmp(comando, "help") == 0) {
                printf("help [cmd|-lt|-T] \t Muestra ayuda sobre los comandos\n");
                printf("\tcmd: info sobre el comando cmd\n");
            } else if (strcmp(comando, "quit") == 0 || strcmp(comando, "exit") == 0 || strcmp(comando, "bye") == 0) {
                printf("%s\tTermina la ejecución del shell\n", comando);
            } else if (strcmp(comando, "chdir") == 0) {
                printf("chdir [dir] \t Cambia (o muestra) el directorio actual del shell\n");
            } else if (strcmp(comando, "pid") == 0) {
                printf("pid [-p] \t Muestra el PID del shell o de su proceso padre\n");
                printf("\t-p: muestra el PID del proceso padre\n");
            } else if (strcmp(comando, "authors") == 0) {
                printf("authors [-n|-l] \t Muestra los nombres y/o logins de los autores\n");
                printf("\t-n: muestra solo los nombres de los autores\n");
                printf("\t-l: muestra solo los logins de los autores\n");
            } else if (strcmp(comando, "create") == 0)
                printf("create [-f] [name]\tCrea un directorio o un fichero (-f)\n");
            else if (strcmp(comando, "delete") == 0)
                printf("delete [name1 name2 ..]\tBorra ficheros o directorios vacios\n");
            else if (strcmp(comando, "deltree") == 0)
                printf("deltree [name1 name2 ..]\tBorra ficheros o directorios no vacios recursivamente\n");
            else if (strcmp(comando, "stat") == 0) {
                printf("stat [-long][-link][-acc] name1 name2...\tlista ficheros;\n");
                printf("\t-long: listado largo\n");
                printf("\t-acc: acesstime\n");
                printf("\t-link: si es enlace simbolico, el path contenido\n");
            } else if (strcmp(comando, "list") == 0) {
                printf("list [-reca] [-recb] [-hid][-long][-link][-acc] n1 n2... \tlista contenidos de directorios\n");
                printf("\t-hid: incluye los ficheros ocultos\n");
                printf("\t-recb: recursivo (antes)\n");
                printf("\t-reca: recursivo (despues)\n");
                printf("\tresto parametros como stat\n");
            } else if (strcmp(comando, "malloc") == 0) {
                printf("malloc [-free] [tam]	asigna un bloque memoria de tamano tam con malloc\n");
                printf("\t-free: desasigna un bloque de memoria de tamano tam asignado con malloc\n");
            } else if (strcmp(comando, "shared") == 0) {
                printf("shared [-free|-create|-delkey] cl [tam]	asigna memoria compartida con clave cl en el programa\n");
                printf("\t-create cl tam: asigna (creando) el bloque de memoria compartida de clave cl y tamano tam\n");
                printf("\t-free cl: desmapea el bloque de memoria compartida de clave cl\n");
                printf("\t-delkey elimina del sistema (sin desmapear) la clave de memoria cl\n");
            } else if (strcmp(comando, "mmap") == 0) {
                printf("mmap [-free] fich prm	mapea el fichero fich con permisos prm\n");
                printf("\t-free fich: desmapea el ficherofich\n");
            } else if (strcmp(comando, "read") == 0) {
                printf("read fiche addr cont\tLee cont bytes desde fich a la direccion addr\n");
            } else if (strcmp(comando, "write") == 0) {
                printf("write [-o] fiche addr cont\tEscribe cont bytes desde la direccion addr a fich (-o sobreescribe)\n");
            } else if (strcmp(comando,"memdump") == 0){
                printf("memdump addr cont \tVuelca en pantallas los contenidos (cont bytes) de la posicion de memoria addr\n");
            }else if (strcmp(comando,"memfill") == 0){
                printf("memfill addr cont byte \tLlena la memoria a partir de addr con byte\n");
            } else if (strcmp(comando,"mem") == 0){
                printf("mem [-blocks|-funcs|-vars|-all|-pmap] ..\tMuestra muestra detalles de la memoria del proceso\n"
                       "\t\t-blocks: los bloques de memoria asignados\n"
                       "\t\t-funcs: las direcciones de las funciones\n"
                       "\t\t-vars: las direcciones de las variables\n"
                       "\t\t-all: todo\n"
                       "\t\t-pmap: muestra la salida del comando pmap(o similar)\n");
            } else if(strcmp(comando,"recurse")==0){
                printf("recurse [n]\tInvoca a la funcion recursiva n veces\n");
            }
            break;
        default:
            printf("Comando desconocido: %s\n", comando);
            break;
    }

}

void list_directory(const char *directoryName, int include) {
    DIR *dir = opendir(directoryName); // Abre el directorio

    if (dir == NULL) { //Si no lo damos abierto da error
        perror("opendir");
        return;
    }

    struct dirent *dirEntry;
    struct stat fileStat;

    printf("****%s\n", directoryName); // Imprime la dirección antes de los archivos

    while ((dirEntry = readdir(dir)) != NULL) {
        if (!(include & INCLUDE_HIDDEN) && dirEntry->d_name[0] == '.')  //Si no incluimos él -hid no mostramos los ocultos
            continue; //Pasamos a la siguiente iteración del bucle

        char path[MAX_PATH];
        snprintf(path, MAX_PATH, "%s/%s", directoryName, dirEntry->d_name);

        if (lstat(path, &fileStat) == 0) {
            print_Stat(dirEntry->d_name, &fileStat, include);
        }
    }

    closedir(dir);
} //Función que muestra la el contenido de un directorio


void reca(const char *directoryName, int include) {
    list_directory(directoryName, include);

    DIR *dir = opendir(directoryName);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *dirEntry;
    struct stat fileStat;

    while ((dirEntry = readdir(dir)) != NULL) {
        // Omitir "." y ".." en cualquier caso
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) {
            continue;
        }

        if (!(include & INCLUDE_HIDDEN) && dirEntry->d_name[0] == '.') {
            // Omitir archivos ocultos si includeHidden es falso
            continue;
        }

        char path[MAX_PATH];
        snprintf(path, MAX_PATH, "%s/%s", directoryName, dirEntry->d_name);

        if (lstat(path, &fileStat) == 0 && S_ISDIR(fileStat.st_mode)) {
            // Si es un directorio, realiza una llamada recursiva
            reca(path, include);
        }
    }

    closedir(dir);
}

void recb(const char *directoryName, int include) {
    DIR *dir = opendir(directoryName);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *dirEntry;
    struct stat fileStat;

    while ((dirEntry = readdir(dir)) != NULL) {
        // Omitir "." y ".." en cualquier caso
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) {
            continue;
        }

        if (!(include & INCLUDE_HIDDEN) && dirEntry->d_name[0] == '.') {
            // Omitir archivos ocultos si includeHidden es falso
            continue;
        }

        char path[MAX_PATH];
        snprintf(path, MAX_PATH, "%s/%s", directoryName, dirEntry->d_name);

        if (lstat(path, &fileStat) == 0 && S_ISDIR(fileStat.st_mode)) {
            // Si es un directorio, realiza una llamada recursiva antes de listar su contenido
            recb(path, include);
        }
    }

    list_directory(directoryName, include); // Utiliza list_directory para mostrar el contenido del directorio después de listar los subdirectorios

    closedir(dir);
}

void cmd_list(char *arguments[MAX_ARGUMENTS], int nArguments) {
    int include = 0; 

    int firstFile = 1;
    if (nArguments < 2) {
        cmd_chdir(arguments, nArguments);
        return;
    }

    for (int i = 1; i < nArguments; i++) {
        if (strcmp(arguments[i], "-hid") == 0) {
            include |= INCLUDE_HIDDEN;
            firstFile++;
        } else if (strcmp(arguments[i], "-recb") == 0) {
            include |= RECURSIVE_BEFORE;
            firstFile++;
        } else if (strcmp(arguments[i], "-reca") == 0) {
            include |= RECURSIVE_AFTER;
            firstFile++;
        } else if (strcmp(arguments[i], "-long") == 0) {
            include |= INCLUDE_LONG;
            firstFile++;
        } else if (strcmp(arguments[i], "-link") == 0) {
            include |= INCLUDE_LINK;
            firstFile++;
        } else if (strcmp(arguments[i], "-acc") == 0) {
            include |= INCLUDE_ACC;
            firstFile++;
        } else {
            include |= INCLUDE_FILE;
        }
    }

    const char *directoryName = arguments[firstFile]; //Nos saltamos los argumentos que no sea un directorio.
    if (include & INCLUDE_FILE) {
        if (include & RECURSIVE_AFTER) { //Con reca
            reca(directoryName, include);
        } else if (include & RECURSIVE_BEFORE) { //Con recb
            recb(directoryName, include);
        } else {
            list_directory(directoryName, include);
        }
    }
}


/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MEMORIA~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



void cmd_malloc(char *arguments[MAX_ARGUMENTS], int nArguments, tListM* memoryList) {
    tItemM newItem;
    int size = 0;
    
    if (nArguments < 2 || nArguments > 3) {
        displayListM(*memoryList, MALLOC_MEMORY);
        return;
    }

    if (strcmp(arguments[1], "-free") == 0) {
        if (nArguments < 3) {
            displayListM(*memoryList, MALLOC_MEMORY);
            return;
        }

        if (esEnteroPositivo(arguments[2], &size) ) {
            removeElement(findElementM(&size, *memoryList, MALLOC_MEMORY), memoryList, freeItemM);
        }
        else printf("No hay bloque de ese tamano asignado con malloc\n");
    }
    else {
        if (esEnteroPositivo(arguments[1], &size)) {
            newItem.address = malloc(size);
            fillMemory(newItem.address, size, 0);
            // Asignamos el bloque de memoria con malloc y lo rellenamos con 0s

            newItem.size = (size_t)size;
            newItem.allocationTime = time(NULL);
            newItem.allocationType = MALLOC_MEMORY;
            printf("Asignados %ld bytes en %p\n", newItem.size, newItem.address);
            insertElement(&newItem, memoryList, allocateItemM);
        }
        else printf("\033[31mError:\033[0m No se asignan bloques de 0 bytes\n");
    }
}

void cmd_shared(char *arguments[MAX_ARGUMENTS], int nArguments, tListM* memoryList) {
    tItemM newItem;
    int size = 0;
    key_t key;
    tPos foundElement;

    if (nArguments < 3 || nArguments > 4) {
        displayListM(*memoryList, SHARED_MEMORY);
        return;
    }

    key = (key_t)  strtoul(arguments[2], NULL, 10);
    // Conseguimos la clave del segmento de memoria compartida 

    if (strcmp(arguments[1], "-free") == 0) {
        if (arguments[2] != NULL && esEnteroPositivo(arguments[2], &key) ) {
            foundElement = findElementM(&key, *memoryList, SHARED_MEMORY);
            if (foundElement == NULL) {
                printf("No hay bloque con esa clave asignado con shared\n");
                return;
            }

            removeElement(foundElement, memoryList, freeItemM);
        }
        else printf("Invalid key\n");
    }
    else if (strcmp(arguments[1], "-create") == 0) {
            if (nArguments < 4) {
                displayListM(*memoryList, SHARED_MEMORY);
                return;
            }
            size = (int)strtoul(arguments[3], NULL, 10);
            // Conseguimos el tamaño del bloque de memoria compartida

            if (size <= 0) {
                printf("\033[31mError:\033[0m No se asignan bloques de %d bytes\n", size);
                return;
            }

            if ((newItem.address = sharedCreate(key, size)) == NULL) {
                perror("sharedCreate");
                return;
            }
            newItem.size = (size_t)size;
            newItem.allocationTime = time(NULL);
            newItem.allocationType = SHARED_MEMORY;
            newItem.sharedInfo.key = key;
            printf("Asignados %ld bytes en %p\n", newItem.size, newItem.address);
            insertElement(&newItem, memoryList, allocateItemM);
            // Insertamos el elemento en la lista de memoria
    }
    else if (strcmp(arguments[1], "-delkey") == 0) {
        sharedDelkey(key);
    }
    else {
        key = (key_t)  strtoul(arguments[1],NULL,10);
        printf ("Imposible asignar memoria compartida clave %lu:%s\n",(unsigned long) key, strerror(errno));
    }
}

void *sharedCreate(key_t key, size_t size) {
    void *p;
    int aux, id, flags = DEFAULT_PERMISSIONS;
    struct shmid_ds sharedMemSeg;

    if (size)     /*tam distito de 0 indica crear */
        flags = flags | IPC_CREAT | IPC_EXCL;

    if (key == IPC_PRIVATE) {
        errno = EINVAL;
        return NULL;
    }

    if ((id = shmget(key, size, flags)) == -1) {
        perror("Error en shmget");
        return NULL;
    }
    // Conseguimos el id del segmento de memoria compartida

    if ((p = shmat(id, NULL, 0)) == (void *)-1) {
        aux=errno;
        if (size)
             shmctl(id,IPC_RMID,NULL);
        errno=aux;
        return (NULL);
    }
    // Conseguimos la dirección del segmento de memoria compartida

    shmctl(id, IPC_STAT, &sharedMemSeg);

    return p;
}

void sharedDelkey (key_t key) {
    int id;

    if (key == IPC_PRIVATE) {
        printf ("-delkey necesita clave valida\n");
        return;
    }
    if ((id = shmget(key, 0, 0666)) == -1){
        perror ("shmget: imposible obtener memoria compartida");
        return;
    }
    if (shmctl(id, IPC_RMID, NULL) == -1)
        perror ("shmctl: imposible eliminar id de memoria compartida\n");
        // Elimina el segmento de memoria compartida asociado al identificador id conseguido con shmget y la key
}

void cmd_mmap(char *arguments[MAX_ARGUMENTS], int nArguments, tListM* memoryList) {
    tItemM newItem;
    char* permissions;
    int protection = 0;

    if (nArguments < 2) {
        displayListM(*memoryList, MAPPED_FILE);
        return;
    }

    if (strcmp(arguments[1], "-free") == 0) {
        if (nArguments < 3) {
            displayListM(*memoryList, MAPPED_FILE);
            return;
        }
        removeElement(findElementM(arguments[2], *memoryList, MAPPED_FILE), memoryList, freeItemM);
        // Elimina el elemento de la lista de memoria
    }
    else {
        if ((permissions = arguments[2]) != NULL && strlen(permissions) < 4) {
            if (strchr(permissions, 'r') != NULL) protection |= PROT_READ;
            if (strchr(permissions, 'w') != NULL) protection |= PROT_WRITE;
            if (strchr(permissions, 'x') != NULL) protection |= PROT_EXEC;
        }
        // Conseguimos los permisos de escritura, lectura y ejecución

        mapFile(arguments[1], protection, &newItem);
        if (newItem.address == NULL) {
            return;
        }
        newItem.allocationTime = time(NULL);
        newItem.allocationType = MAPPED_FILE;
        newItem.mappedFileInfo.fileName = arguments[1];
        insertElement(&newItem, memoryList, allocateItemM);
        // Añadimos el elemento a la lista de memoria
        printf("fichero %s mapeado en %p\n", newItem.mappedFileInfo.fileName, newItem.address);
    }
}

void mapFile (char * file, int protection, tItemM *newItem) {
    int fileDescriptor;
    int map = MAP_PRIVATE, mode = O_RDONLY;
    struct stat fileStat;

    if (protection & PROT_WRITE)
        mode = O_RDWR;
    // Si tenemos permisos de escritura abrimos el fichero en modo lectura/escritura

    if (stat(file, &fileStat) == -1 || (fileDescriptor = open(file, mode)) == -1) {
        newItem->address = NULL;
        perror ("Imposible mapear fichero");
        return;
    }
    // Conseguimos información del fichero y lo abrimos

    if ((newItem->address = mmap (NULL, fileStat.st_size, protection, map, fileDescriptor, 0)) == MAP_FAILED) {
        perror("Imposible mapear fichero");
        return;
    }
    // Mapeamos el fichero

    newItem->size = fileStat.st_size;
    newItem->mappedFileInfo.fileDescriptor = fileDescriptor;
}

void cmd_read(char *arguments[MAX_ARGUMENTS], int nArguments) {
    void *p;
    size_t cont = -1;  // -1 indica leer el fichero entero
    ssize_t n;

    if (arguments[0] == NULL || arguments[1] == NULL || arguments[2] == NULL) { 
        printf ("faltan parametros\n");
        return;
    }

    p = (void*)strtoull(arguments[2], NULL, 16);
    // Pasamos el argumento a hexadecimal

    if (arguments[3] != NULL)
        cont = (size_t) atoll(arguments[3]);
        // Conseguimos el desplazamiento

    if ((n = readFile(arguments[1], p, cont)) == -1)
        perror ("Imposible leer fichero");
        // Leemos el fichero

    else
        printf ("leidos %lld bytes de %s en %p\n",(long long) n, arguments[1], p);
}

ssize_t readFile (char *fileName, void *p, size_t cont) {
    struct stat fileStat;
    ssize_t n;  
    int fileDescriptor, aux;

    if (stat(fileName, &fileStat) == -1 || (fileDescriptor = open(fileName, O_RDONLY)) == -1)
        return -1;    
    // Conseguimos información del fichero y lo abrimos

    if (cont == -1)   /* si pasamos -1 como bytes a leer lo leemos entero*/
        cont=fileStat.st_size;

    if ((n = read(fileDescriptor, p, cont)) == -1) {
        aux = errno;
        close(fileDescriptor);
        errno = aux;
        return -1;
    }
    // Leemos el fichero

    close (fileDescriptor);
    return n;
}

void cmd_write(char *arguments[MAX_ARGUMENTS], int nArguments) {
    size_t cont = -1;  // -1 indica leer el fichero entero
    int overwrite = 0;
    void *p;
    ssize_t n;

    if (arguments[1] == NULL || arguments[2] == NULL || arguments[3] == NULL){
        printf ("faltan parametros\n");
        return;
    }

    if (strcmp(arguments[1], "-o") == 0)
        overwrite = 1;
        // Si pasamos -o sobreescribimos el fichero

    if (arguments[3 + overwrite] == NULL){
        printf ("faltan parametros\n");
        return;
    }

    p = (void*)strtoull(arguments[2 + overwrite], NULL, 16);
    // Pasamos el argumento a hexadecimal

    cont = (size_t) atoll(arguments[3 + overwrite]);
    // Conseguimos el desplazamiento

    if ((n = writeFile(arguments[1 + overwrite], p, cont, overwrite)) == -1)
        perror ("Imposible leer fichero");
        // Escribimos en el fichero
    else
        printf ("Escritos  %lld bytes de %p en %s\n",(long long) n, p, arguments[1 + overwrite]);
}

ssize_t writeFile (char *fileName, void *p, size_t cont, bool overwrite) {
    int flags = O_CREAT | O_EXCL | O_WRONLY;
    ssize_t n;
    int fileDescriptor, aux; 

    if (overwrite)
        flags = O_CREAT | O_WRONLY | O_TRUNC;
        // Si pasamos -o añadimos los flags para sobreescribir el fichero
        
    if ((fileDescriptor = open(fileName, flags, DEFAULT_PERMISSIONS)) == -1)
        return -1;
        // Abrimos el fichero

    if ((n = write(fileDescriptor, p, cont)) == -1){
        printf("Error write\n");
        aux = errno;
        close(fileDescriptor);
        errno = aux;
        return -1;
    }
    // Escribimos en el fichero

    close (fileDescriptor);
    return n;
}

void cmd_memdump(char *arguments[MAX_ARGUMENTS], int nArguments, tListM *memoryList) {
    if (nArguments < 2 || nArguments > 3) {
        printf("Usage: memdump <addr> [<cont>]\n");
        return;
    }

    void *address = (void *) strtoul(arguments[1], NULL, 16);
    // Pasamos el argumento a hexadecimal

    ssize_t count = -1;
    if (nArguments == 3) {
        count = (ssize_t) strtoul(arguments[2], NULL, 10);
    }

    printf("Volcando %zd bytes desde la direccion %p\n", count, address);

    for (ssize_t i = 0; count < 0 || i < count; i++) {
        char byte;

        memcpy(&byte, address + i, sizeof(char));
        // Copiamos el byte de memoria

        printf("  %02x->%2c ", (unsigned char) byte, (byte >= 32 && byte <= 126) ? (unsigned char) byte : ' ');

        if ((i + 1) % 8 == 0) {
            printf("\n");
        } else {
            printf(" ");
        }
        // Imprimimos el byte
    }

    printf("\n");
}

void cmd_memfill(char *arguments[MAX_ARGUMENTS], int nArguments, tListM *memoryList) {
    if (nArguments < 2 || nArguments > 4) {
        printf("Usage: memfill <addr> [<size>] [<byte>]\n");
        return;
    }

    void *addr = (void *)strtoul(arguments[1], NULL, 16);
    // Pasamos el argumento a hexadecimal

    size_t size = (nArguments >= 3) ? (size_t)strtoul(arguments[2], NULL, 10) : 128;
    // Conseguimos el tamaño a rellenar

    unsigned char byte = (nArguments == 4) ? arguments[3][0] : 0;
    // Conseguimos el byte con el que se debe rellenar

    fillMemory(addr, size, byte);

    printf("Llenando %zu bytes de memoria con el byte (%02X) a partir de la direccion %p\n", size, byte, addr);
}

void fillMemory(void *p, size_t cont, unsigned char byte){
    unsigned char *arr = (unsigned char *)p;
    for (size_t i = 0; i < cont; i++) {
        arr[i] = byte;
    }
    // Bucle q rellena la memoria
}

void showVars() {
    // Variables locales
    int variableLocal1 = 40;
    int variableLocal2 = 50;
    int variableLocal3 = 60;
    // Variables estáticas
    static int variableEstatica1;
    static int variableEstatica2;
    static int variableEstatica3;
    // Variables estáticas no inicializadas (N.I.)
    static int variableEstaticaNi1;
    static int variableEstaticaNi2;
    static int variableEstaticaNi3;

    printf("Variables locales       %p,    %p,    %p\n", (void*)&variableLocal1, (void*)&variableLocal2, (void*)&variableLocal3);
    printf("Variables globales      %p,    %p,    %p\n", (void*)&variableGlobal1, (void*)&variableGlobal2, (void*)&variableGlobal3);
    printf("Var (N.I.)globales      %p,    %p,    %p\n", (void*)&variableGlobal1Ni, (void*)&variableGlobal2Ni, (void*)&variableGlobal3Ni);
    printf("Variables estáticas     %p,    %p,    %p\n", (void*)&variableEstatica1, (void*)&variableEstatica2, (void*)&variableEstatica3);
    printf("Var (N.I.)estáticas     %p,    %p,    %p\n", (void*)&variableEstaticaNi1, (void*)&variableEstaticaNi2, (void*)&variableEstaticaNi3);
    // Mostramos las direcciones de las variables, dependiendo del tipo de variable están en distintas zonas de memoria
}

void showFuncs() {
    printf("Funciones programa      %p,    %p,    %p\n", (void*)&cmd_mem, (void*)&showVars, (void*)&showFuncs);
    printf("Funciones libreria      %p,    %p,    %p\n", (void*)&isEmpty, (void*)&createList, (void*)&next);
    // Mostramos las direcciones de las funciones, dependiendo del tipo de función están en distintas zonas de memoria
}

void cmd_mem(char *arguments[MAX_ARGUMENTS], int nArguments, tListM *memoryList) {
    if (nArguments != 2) {
        printf("Error\n");
        return;
    }
    if (strcmp(arguments[1], "-blocks") == 0) {
        displayListM(*memoryList, ALL_TYPES);
    } else if (strcmp(arguments[1], "-funcs") == 0) {
        showFuncs();
    } else if (strcmp(arguments[1], "-vars") == 0) {
        showVars();
    } else if (strcmp(arguments[1], "-all") == 0) {
        showVars();
        showFuncs();
        displayListM(*memoryList, ALL_TYPES);
    } else if (strcmp(arguments[1], "-pmap") == 0) {
        pid_t pid = getpid();
        char command[50];
        snprintf(command, sizeof(command), "pmap %d", pid);
        system(command);
    } else {
        printf("Error, argumento inválido.\n");
    }
}


void cmd_recurse(char *arguments[MAX_ARGUMENTS], int nArguments) {
    if (nArguments < 2) {
        printf("Error: Se requiere un argumento para la cantidad de recursiones.\n");
        return;
    }

    int n = atoi(arguments[1]);

    if (n < 0) {
        printf("Error: El número de recursiones debe ser no negativo.\n");
        return;
    }

    recursive(n);
    // LLamamos a la función recursiva
}

void recursive(int n) {
    char automatico[TAMANO];
    static char estatico[TAMANO];

    printf("parametro:%3d(%p) array %p, arr estatico %p\n", n, &n, automatico, estatico);
    // Mostramos la dirección de la variable y de los arrays

    if (n > 0)
        recursive(n - 1);
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~process_COMMANDS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


void cmd_fork (tList *processList) {
	pid_t pid;
    tItem p;
	
	if ((pid = fork()) == 0){
        for(p = first(*processList); p != NULL; p = next(p)) {
            removeElement(p, processList, freeItemP);
        }
		printf ("Ejecutando proceso %d\n", getpid());
	}
	else if (pid!=-1)
		waitpid (pid,NULL,0);
}


void cmd_exec (char *arguments[MAX_ARGUMENTS], int nArguments) {
    char *argv[MAX_ARGUMENTS] = {NULL};      // Comando y argumentos a ejecutar
    char *envp[MAX_ARGUMENTS] = {NULL};      // Variables de entorno
    int argc = 0;     // Número de argumentos del comando a ejecutar
    int envc = 0;     // Número de variables de entorno
    char *commandPath; // Path del comando a ejecutar

    if (nArguments < 2) {
        perror("Imposible ejecutar\n");
        return;
    }


    while (isEnvVar(arguments[envc + 1])) {
        envp[envc] = arguments[envc + 1];
        envc++;
    }
    envc++;
    if (envc == 1) *envp = NULL;
    else envp[envc] = NULL;
    // Consigue las variables de entorno

    for (argc = 0; argc < nArguments - envc; argc++) {
        if (strcmp(arguments[argc + envc], "&") != 0)
            argv[argc] = arguments[argc + envc];
    }
    argc++;
    argv[argc] = NULL;
    // Copiamos los argumentos
     
    commandPath = getCommandPath(arguments[1]);
    if (execve(commandPath, argv, envp) == -1)
        perror("execve");
    free(commandPath);
    // Ejecutamos el comando con los argumentos pasados
}

bool isEnvVar(char *argument) {
    return strchr(argument, '=') != NULL;
}

char* getCommandPath(const char *command) {
    char *path = getenv("PATH");
    char *pathCopy = strdup(path);
    // Copiamos el path para no modificar el original
    char *pathToken = strtok(pathCopy, ":");
    char *commandPath = malloc(strlen(path) + strlen(command) + 2);
    // Reservamos memoria para el path del comando (path + '/' + command + '\0')

    while (pathToken != NULL) {
        snprintf(commandPath, MAX_PATH, "%s/%s", pathToken, command);
        if (access(commandPath, X_OK) == 0) {
            free(pathCopy);
            return commandPath;
            // Si el comando es ejecutable devolvemos el path
        }
        pathToken = strtok(NULL, ":");
    }

    free(pathCopy);
    free(commandPath);
    return NULL;
    // Si no encontramos el comando devolvemos NULL
}

void cmd_jobs(tListP *processList) {
    printf("PID\tTIME\tSTATUS\tCOMMAND\n");
    displayListP(*processList);
}



void externalProgram(char **arguments, int nArguments, tList *processList) {
    char *commandPath;
    char *argv[MAX_ARGUMENTS] = {NULL};
    int argc;

    commandPath = getCommandPath(arguments[0]);
    if (commandPath == NULL) {
        printf("\033[31mError:\033[0m Command '%s' not found\n", arguments[0]);
        return;
    }

    for (argc = 0; argc < nArguments; argc++) {
        if (strcmp(arguments[argc], "&") != 0)
            argv[argc] = arguments[argc];
    }
    argc++;
    argv[argc] = NULL;

    if (arguments[nArguments - 1][0] != '&')
        executeInForeground(commandPath, argv);
    else 
        executeInBackground(commandPath, argv, processList);

}

void executeInForeground(char* commandPath, char **argv) {
    pid_t pid;

    if ((pid = fork()) == 0) {
        // Proceso hijo
        if (execv(commandPath, argv) == -1) {
            perror("execv");
            free(commandPath);
            exit(EXIT_FAILURE);
        }
        free(commandPath);
	}
	else if (pid == -1) {
        // Error al crear el proceso hijo
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else {
        // Proceso padre
		waitpid (pid, NULL, 0);
    }
}

void executeInBackground(char* commandPath, char **argv, tListP *processList)  {
    pid_t pid;
    tItemP newItem;

    if ((pid = fork()) == -1) {
        // Error al crear el proceso hijo
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Proceso hijo
        if (execv(commandPath, argv) == -1) {
            perror("execv");
            free(commandPath);
            exit(EXIT_FAILURE);
        }
    } else {
        // Proceso padre
        newItem.pid = pid;
        newItem.time = time(NULL);
        newItem.status = ACTIVE;
        newItem.command = argv[0];

        insertElement(&newItem, processList, allocateItemP);
        printf("Ejecutando proceso %d en segundo plano\n", pid);
        displayListP(*processList);
    }
}