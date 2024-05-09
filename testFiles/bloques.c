#include "bloques.h"

int descriptor;

// bmount() crea la maquina virtual
int bmount(const char* camino){
     umask(000);
     descriptor = open(camino, O_RDWR | O_CREAT, 0666);
     if(descriptor == -1){
          perror("Error al abrir el archivo.");
          return FALLO;
     }
     return descriptor;
}

// bumount() cierra la maquina virtual
int bumount(){
     descriptor = close(descriptor);
     return descriptor;
}

// bwrite() escribe *buf en nbloque cantidad de bloques.
int bwrite(unsigned int nbloque, const void *buf){
     lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
     int result = write(descriptor, buf, BLOCKSIZE);
     if(result == -1){
          perror("Error al escribir.");
          return FALLO;
     }
     return result;
}

// bread() lee el bloque especificado con nbloque y lo escribe en *buf.
int bread(unsigned int nbloque, void *buf){
     lseek(descriptor, nbloque*BLOCKSIZE, SEEK_SET);
     int result = read(descriptor, buf, BLOCKSIZE);
     if(result == -1){
          perror("Error el leer.");
          return FALLO;
     }
     return result;
}