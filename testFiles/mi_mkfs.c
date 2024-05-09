#include "ficheros_basico.h"
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){

        bmount(argv[1]);
        unsigned char arr[BLOCKSIZE];
        memset(arr,0,BLOCKSIZE);
        int size = atoi(argv[2]);

        for(int i = 0; i < size; i++){
            bwrite(i,arr);
        }
        
        initSB(atoi(argv[2]), atoi(argv[2])/4);
        initMB();
        initAI();

        int inodoReservado = reservar_inodo('d',7);
        printf("Inodo reservado: %i\n", inodoReservado);

        reservar_bloque();

        bumount();
}