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

        int bloqueReservado = reservar_bloque();
        printf("Bloque reservado: %i\n", bloqueReservado);

        int bloqueLiberado = liberar_bloque(bloqueReservado);
        printf("Bloque liberado: %i\n", bloqueLiberado);

        struct inodo inodoLeido;

        leer_inodo(inodoReservado, &inodoLeido);

        printf("tipo: %c\n", inodoLeido.tipo);
        printf("permisos: %i\n", inodoLeido.permisos);
        printf("nLinks: %i\n", inodoLeido.nlinks);
        printf("tamEnBytesLog: %i\n", inodoLeido.tamEnBytesLog);
        printf("numBloquesOcupados: %i\n", inodoLeido.numBloquesOcupados);

        bumount();
}