#include "ficheros_basico.h"
#include <string.h>
#include <stdlib.h>

/*
int main(int argc, char **argv){
        bmount(argv[1]);
        unsigned char arr[1024];
        memset(arr,0,1024);
        int size = atoi(argv[2]);
        for(int i = 0; i < size; i++){
            bwrite(i,arr);
        }
        bumount();
}
*/
extern struct superbloque SB;

int main(int argc, char **argsv)
{
    if (argc == 3)
    { // Not enough arguments
        if (bmount(argsv[1]) == -1)
        { // File paths are relative????
            fprintf(stderr, "Error while mounting\n");
            return -1;
        }
        char buffer[BLOCKSIZE];
        memset(buffer, 0, sizeof(buffer));

        for (int i = 0; i < atoi(argsv[2]); i++)
        {
            if (bwrite(i, buffer) == -1)
            {
                fprintf(stderr, "Error while writting\n");
                return -1;
            }
            memset(buffer, 0, sizeof(buffer));
        }

        initSB(atoi(argsv[2]), atoi(argsv[2]) / 4);
        initMB();
        initAI();

        printf("DATOS DEL SUPERBLOQUE\n");
        printf("posPrimerBloqueMB: %i\n", SB.posPrimerBloqueMB);
        printf("posUltimoBloqueMB: %i\n", SB.posUltimoBloqueMB);
        printf("posPrimerBloqueAI: %i\n", SB.posPrimerBloqueAI);
        printf("posUltimoBloqueAI: %i\n", SB.posUltimoBloqueAI);
        printf("posPrimerBloqueDatos: %i\n", SB.posPrimerBloqueDatos);
        printf("posUltimoBloqueDatos: %i\n", SB.posUltimoBloqueDatos);
        printf("posInodoRaíz: %i\n", SB.posInodoRaiz);
        printf("posPrimerInodoLibre: %i\n", SB.posPrimerInodoLibre);
        printf("cantBloquesLibres: %i\n", SB.cantBloquesLibres);
        printf("cantInodosLibres: %i\n", SB.cantInodosLibres);
        printf("totBloques: %i\n", SB.totBloques);
        printf("totInodos: %i\n\n", SB.totInodos);
        printf("sizeof struct superbloque: %li\n", sizeof(struct superbloque));
        printf("sizeof struct inodo: %li\n\n", sizeof(struct inodo));


        if (bumount() == -1)
        {
            fprintf(stderr, "Error while unmounting\n");
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "Not enough arguments\n");
        return -1;
    }
}