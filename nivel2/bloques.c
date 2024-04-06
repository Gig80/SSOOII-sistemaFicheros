#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define ERROR -1
#define BLOCKSIZE 1024



int descriptor;

int bmount(const char* camino){
     umask(000);
     descriptor = open(camino, O_RDWR | O_CREAT, 0666);
     if(descriptor == -1){
          perror("Error al abrir el archivo.");
          return ERROR;
     }
     return descriptor;
}

int bumount(){
     close(descriptor);
}

int bwrite(unsigned int nbloque, const void *buf){
     lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
     int result = write(descriptor, buf, BLOCKSIZE);
     if(result == -1){
          perror("Error al escribir.");
          return ERROR;
     }
     return result;
}

int bread(unsigned int nbloque, void *buf){
     lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
     int result = read(descriptor, buf, BLOCKSIZE);
     if(result == -1){
          perror("Error el leer.");
          return ERROR;
     }
     return result;
}
