#include "bloques.h"

int bmount(const char* camino);
int bumount();
int bwrite(unsigned int nbloque, const void *buf);
int bread(unsigned int nbloque, void *buf);

int descriptor;

int bmount(const char* camino){
     umask(000);
     descriptor = open(camino, O_RDWR | O_CREAT, 0666);
     if(descriptor == -1){
          perror("Error al abrir el archivo.");
          return FALLO;
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
          return FALLO;
     }
     return result;
}

int bread(unsigned int nbloque, void *buf){
     lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
     int result = read(descriptor, buf, BLOCKSIZE);
     if(result == -1){
          perror("Error el leer.");
          return FALLO;
     }
     return result;
}