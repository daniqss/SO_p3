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
void moveToForeground(tItemP *p);
void executeInForeground(char* commandPath, char **arguments);
void executeInBackground(char* commandPath, char **arguments, tListP *processList);
int signalValue(char* sen);
char *signalName(int sen);

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
            printf("exit bye fin quit help infosys listopen dup close open command hist time date chdir pid authors create delete deltree list stat ");
            printf("malloc shared mmap read write memdump memfill mem recurse ");
            printf("uid showvar changevar subsvar showenv fork exec jobs deljobs job");
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
            else if (strcmp(comando, "uid") == 0) {
                printf("uid [-get|-set] [-l] [id]\tAccede a las credenciales del proceso que ejecuta el shell\n");
                printf("\t-get: muestra las credenciales\n");
                printf("\t-set id: establece la credencial al valor numerico id\n");
                printf("\t-set -l id: establece la credencial a login id\n");
            } else if (strcmp(comando, "showvar") == 0) {
                printf("showvar var\tMuestra el valor y las direcciones de la variable de entorno var\n");
            } else if (strcmp(comando, "changevar") == 0) {
                printf("changevar [-a|-e|-p] var valor\tCambia el valor de una variable de entorno\n");
                printf("\t-a: accede por el tercer arg de main\n");
                printf("\t-e: accede mediante environ\n");
                printf("\t-p: accede mediante putenv\n");
            } else if (strcmp(comando, "subsvar") == 0) {
                printf("subsvar [-a|-e] var1 var2 valor\tSustituye la variable de entorno var1 con var2=valor\n");
                printf("\t-a: accede por el tercer arg de main\n");
                printf("\t-e: accede mediante environ\n");
            } else if (strcmp(comando, "showenv") == 0) {
                printf("showenv [-environ|-addr]\tMuestra el entorno del proceso\n");
                printf("\t-environ: accede usando environ (en lugar del tercer arg de main)\n");
                printf("\t-addr: muestra el valor y donde se almacenan environ y el 3er arg main\n");
            } else if (strcmp(comando, "fork") == 0) {
                printf("fork\tEl shell hace fork y queda en espera a que su hijo termine\n");
            } else if (strcmp(comando, "exec") == 0) {
                printf("exec VAR1 VAR2 ..prog args....[@pri]\tEjecuta, sin crear proceso, prog con argumentos\n");
                printf("\ten un entorno que contiene solo las variables VAR1, VAR2...\n");
            } else if (strcmp(comando, "jobs") == 0) {
                printf("jobs\tLista los procesos en segundo plano\n");
            } else if (strcmp(comando, "deljobs") == 0) {
                printf("deljobs [-term][-sig]\tElimina los procesos de la lista procesos en sp\n");
                printf("\t-term: los terminados\n");
                printf("\t-sig: los terminados por senal\n");
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
void printUidInfo(uid_t uid, const char *label) {
    struct passwd *pwd = getpwuid(uid);
    if (pwd == NULL) {
        perror("getpwuid");
        return;
    }

    printf("%s: %d, (%s)\n", label, uid, pwd->pw_name);
}

bool setUid(uid_t uid) {
    if (setuid(uid) == 0) {
        return true;
    } else {
        perror("setuid");
        return false;
    }
}

bool setUidByName(const char *login) {
    struct passwd *pwd = getpwnam(login);
    if (pwd == NULL) {
        perror("getpwnam");
        return false;
    }

    return setUid(pwd->pw_uid);
}

void cmd_uid(char *arguments[MAX_ARGUMENTS], int nArguments) {
    uid_t currentUid = getuid();
    uid_t effectiveUid = geteuid();

    if (nArguments == 1 || (nArguments > 1 && strcmp(arguments[1], "-get") == 0)) {
        printUidInfo(currentUid, "Credencial real");
        printUidInfo(effectiveUid, "Credencial efectiva");
    }
    else if (nArguments > 1 && strcmp(arguments[1], "-set") == 0) {
        if (nArguments > 2) {
            if (strcmp(arguments[2], "-l") == 0 && nArguments > 3) {
                if (setUidByName(arguments[3]))
                    printf("Credencial establecida por login: %s\n", arguments[3]);
                else
                    printf("Imposible cambiar credencial: %s\n", strerror(errno));
            }
            else {
                int newUid;
                if (esEnteroPositivo(arguments[2], &newUid)) {
                    if (setUid(newUid))
                        printUidInfo(newUid, "Credencial establecida");
                    else
                        printf("Imposible cambiar credencial: %s\n", strerror(errno));
                }
                else
                    printf("El ID es no válido\n");
            }
        }
        else
            printf("Falta el ID para establecer la credencial\n");
    }
    else
        printf("Comando no reconocido uid [-get|-set] [-l] [id]\n");
}

void showVar(const char *var) {
    extern char **environ;

    uintptr_t base_address = (uintptr_t)__builtin_frame_address(0);  // Obtener la dirección base

    // Obtener la dirección en arg3
    char *arg3_value = getenv(var);
    if (arg3_value != NULL)
        printf("Con arg3 main %s=%s(%p) @0x%lx\n", var, arg3_value, (void *)arg3_value, base_address);
    else {
        printf("Variable %s not found\n", var);
        return;
    }

    // Obtener la dirección en environ
    char *environ_value = NULL;
    for (int i = 0; environ[i] != NULL; i++) {
        if (strncmp(environ[i], var, strlen(var)) == 0 && environ[i][strlen(var)] == '=') {
            environ_value = environ[i] + strlen(var) + 1;
            break;
        }
    }

    if (environ_value != NULL)
        printf(" Con environ %s=%s(%p) @0x%lx\n", var, environ_value, (void *)environ_value, base_address);
    else {
        printf("Variable %s not found\n", var);
        return;
    }

    // Imprimir la información de getenv
    char *getenv_value = getenv(var);
    if (getenv_value != NULL)
        printf("   Con getenv %s(%p)\n", getenv_value, (void *)getenv_value);
    else
        printf("Variable %s not found\n", var);
}

void showAllVars() {
    extern char **environ;
    for (int i = 0; environ[i] != NULL; i++) {
        // Copiar la variable de entorno antes de modificarla
        char *var = strdup(environ[i]);
        char *value = strchr(var, '=');

        if (value != NULL) {
            *value = '\0';
            value++;

            printf("%p->main arg3[%d]=%s(%p) %s=%s\n", (void *)&environ[i], i, value, (void *)value, var, value);

        }
        // Liberar la memoria
        free(var);
    }
}

void cmd_showvar(char *arguments[MAX_ARGUMENTS], int nArguments) {
    // Verificar el número de argumentos
    if (nArguments > 2) {
        fprintf(stderr, "Error: showvar [variable]\n");
        return;
    }

    // Si no se proporciona ninguna variable de entorno específica
    if (arguments[1] == NULL) {
        showAllVars();
    } else {
        showVar(arguments[1]);
    }
}

int BuscarVariable (char * var, char *e[]){
    int pos=0;
    char aux[MAX_BUFFER];

    strcpy (aux,var);
    strcat (aux,"=");

    while (e[pos]!=NULL)
        if (!strncmp(e[pos],aux,strlen(aux)))
            return (pos);
        else
            pos++;
    errno=ENOENT;   /*no hay tal variable*/
    return(-1);
}

int CambiarVariable(char * var, char * valor, char *e[]){
    int pos;
    char *aux;

    if ((pos=BuscarVariable(var,e))==-1)
        return(-1);

    if ((aux=(char *)malloc(strlen(var)+strlen(valor)+2))==NULL)
        return -1;
    strcpy(aux,var);
    strcat(aux,"=");
    strcat(aux,valor);
    strcpy(e[pos],aux);
    free(aux);
    return (pos);

}

void changeByArg3(const char *var, const char *value) {
    setenv(var, value, 1); //Establecemos el nuevo valor de la variable
    showVar(var); //Mostramos el cambio
}

void changeByEnviron(const char *var, const char *value) {
    extern char **environ;

    int result = CambiarVariable((char *)var, (char *)value, environ); //Establecemos el nuevo valor de la variable

    if (result == -1) {
        fprintf(stderr, "Error: No se pudo cambiar la variable %s en el entorno\n", var);
        return;
    }

    showVar(var); //Mostramos el cambio
}

void changeByGetenv(const char *var, const char *value) {

    char *newVar = malloc(strlen(var) + 1 + strlen(value) + 1);// Crear una cadena en el formato "VAR=VAL"
    snprintf(newVar, strlen(var) + 1 + strlen(value) + 1, "%s=%s", var, value);

    // Modificar el valor mediante putenv
    putenv(newVar);

    // Mostrar información utilizando la función existente showVar
    showVar(var);

    // Liberar la memoria después de su uso
    free(newVar);
}

void cmd_changevar(char *arguments[MAX_ARGUMENTS], int nArguments) {
    if (nArguments < 4 || nArguments > 5) {
        fprintf(stderr, "Error: changevar [-a|-e|-p] var valor\n");
        return;
    }

    const char *choice = arguments[1];
    const char *var = arguments[2];
    const char *value = arguments[3];

    if (strcmp(choice, "-a") == 0) {
        changeByArg3(var, value);
    } else if (strcmp(choice, "-e") == 0) {
        changeByEnviron(var, value);
    } else if (strcmp(choice, "-p") == 0) {
        changeByGetenv(var, value);
    } else {
        fprintf(stderr, "Error: changevar [-a|-e|-p] var valor\n");
    }
}

void subsvarByArg3(const char *var1, const char *var2, const char *value) {
    // Obtener el valor actual de var1 en arg3
    char *currentValue = getenv(var1);

    if (currentValue == NULL) {
        fprintf(stderr, "Variable %s not found\n", var1);
        return;
    }

    // Eliminar var1 del entorno
    unsetenv(var1);

    // Establecer el nuevo valor de var2 en arg3
    setenv(var2, value, 1);

    // Mostrar el resultado
    showVar(var2);
}

void subsvarByEnviron(const char *var1, const char *var2, const char *value) {

    int pos = BuscarVariable((char *)var1, __environ);   //Buscamos la variable

    if (pos == -1) { //Si no existe mandamos error
        fprintf(stderr, "Imposible sustituir variable %s por %s: No such file or directory\n", var1, var2);
        return;
    }

    int len = strlen(var2) + 1 + strlen(value) + 1;
    char *aux = malloc(len);
    if (aux == NULL) {
        perror("Error en malloc");
        exit(EXIT_FAILURE);
    }

    snprintf(aux, len, "%s=%s", var2, value);
    __environ[pos] = aux;

    showVar(var2);
}

void cmd_subsvar(char *arguments[MAX_ARGUMENTS], int nArguments) {
    if (nArguments != 5) {
        fprintf(stderr, "Error: Uso incorrecto: subsvar [-a|-e] var1 var2 valor\n");
        return;
    }

    const char *choice = arguments[1];
    const char *var1 = arguments[2];
    const char *var2 = arguments[3];
    const char *valor = arguments[4];

    if (strcmp(choice, "-a") == 0) {
        // Acceder por el tercer argumento de main
        subsvarByArg3(var1, var2, valor);
    } else if (strcmp(choice, "-e") == 0) {
        // Acceder mediante environ
        subsvarByEnviron(var1, var2, valor);
    } else {
        fprintf(stderr, "Error: Opción no reconocida: subsvar [-a|-e] var1 var2 valor\n");
    }
}

void cmd_showenv(char *arguments[MAX_ARGUMENTS], int nArguments) {
    if (nArguments < 1 || nArguments > 2) {
        fprintf(stderr, "Error: Uso incorrecto: showenv [-environ|-addr]\n");
        return;
    }

    if (nArguments == 1){
        showAllVars();
    } else{
        const char *choice = arguments[1];

        if (strcmp(choice, "-environ") == 0) {
            extern char **environ;
            for (int i = 0; environ[i] != NULL; i++) {
                printf("%p->environ[%d]=(%p) %s\n", (void *)&environ[i], i, (void *)environ[i], environ[i]);
            }
        } else if (strcmp(choice, "-addr") == 0) {
            uintptr_t base_address = (uintptr_t)__builtin_frame_address(0);

            // Mostrar la dirección de environ
            printf("environ:   %p (almacenado en %p)\n", (void *)__environ, (void *)&__environ);

            // Mostrar la dirección del tercer argumento de main (arg3)
            printf("main arg3: %p (almacenado en %p)\n", (void *)__environ, (void *)base_address);
        } else {
            fprintf(stderr, "Error: Opción no reconocida: showenv [-environ|-addr]\n");
        }
    }
}

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

    /* No implementada totalmente la parte opcional, solo recoje los argumentos */
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

void cmd_job(char **arguments, int nArguments, tListP *processList) {
    tPos p;
    tItemP *item;
    int wstatus;

    if (nArguments < 2) {
        displayListP(*processList);
        return;
    }
    

    if (strcmp(arguments[1], "-fg") == 0) {
        p = findElementP(atoi(arguments[2]), *processList);
        item = (tItemP*)(p->data);

        if (arguments[2] == NULL) {
            printf("Error, no se ha especificado el proceso\n");   
            return;
        }

        moveToForeground(item);
        removeElement(p, processList, freeItemP);
    }
    else {
        p = findElementP(atoi(arguments[1]), *processList);
        item = (tItemP*)(p->data);

        wstatus = updateItemP(item, WNOHANG | WUNTRACED);
        displayItemP(item, wstatus);
    }
}

void moveToForeground(tItemP *p) {
    int wstatus;
    pid_t pid;

    if (p->status == STOPPED) {
        // Reanuda el proceso
        if (kill(p->pid, SIGCONT) == -1) {
            perror("Error al reanudar el proceso");
            return;
        }
    }
    // Verifica si el proceso está detenido

    if (tcsetpgrp(STDIN_FILENO, getpgid(p->pid)) == -1) {
        perror("Error al establecer el grupo de procesos");
        return;
    }
    // Establece el grupo de procesos para la terminal actual

    pid = waitpid(p->pid, &wstatus, WUNTRACED);

    if (pid == p->pid) {
        // El proceso hijo ha terminado
        if (WIFEXITED(wstatus)) {
            printf("Proceso hijo (%d) terminado con código de salida %d\n", p->pid, WEXITSTATUS(wstatus));
        } else if (WIFSIGNALED(wstatus)) {
            printf("Proceso hijo (%d) terminado por señal %d\n", p->pid, WTERMSIG(wstatus));
        } else if (WIFSTOPPED(wstatus)) {
            printf("Proceso hijo (%d) detenido por señal %d\n", p->pid, WSTOPSIG(wstatus));
        }

        // Restaura el grupo de procesos original de la terminal
        if (tcsetpgrp(STDIN_FILENO, getpgid(getpid())) == -1) {
            perror("Error al restablecer el grupo de procesos");
            return;
        }
        updateItemP(p, WUNTRACED);
    } else if (pid == -1) {
        // Error al esperar al proceso hijo
        perror("Error al esperar al proceso hijo");
        return;
    }
}


void cmd_jobs(tListP *processList) {
    displayListP(*processList);
}

void cmd_deljobs(char **arguments, int nArguments, tListP *processList) {

    if (nArguments != 2) {
        displayListP(*processList);
        return;
    }
    
    if (strcmp(arguments[1], "-term") == 0) {
        removeTermSig(processList, FINISHED);
    }
    else if (strcmp(arguments[1], "-sig") == 0) {
        removeTermSig(processList, SIGNALED);   
    }
    else {
        displayListP(*processList);
    }
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
    argv[argc - 1] = NULL;

    if (arguments[nArguments - 1][0] != '&')
        executeInForeground(commandPath, argv);
    else 
        executeInBackground(commandPath, argv, processList);

    free(commandPath);
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
        free(commandPath);
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
        newItem.startTime = time(NULL);
        newItem.status = ACTIVE;
        newItem.command = argv[0];

        insertElement(&newItem, processList, allocateItemP);
        printf("Ejecutando proceso %d en segundo plano\n", pid);
    }
}
