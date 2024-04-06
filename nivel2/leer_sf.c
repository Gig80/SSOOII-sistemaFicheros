 #include "ficheros_basico.h"

// Declaraciones de funciones
int printSB();

// Declaraciones de variables globales
struct inodo inodos[BLOCKSIZE / INODOSIZE];
struct superbloque SB;


int main(int argc, char **argsv)
{
    if (argc == 2)
    {
        if (bmount(argsv[1]) == -1)
        {
            fprintf(stderr, "Error while mounting\n");
            return -1;
        }

        printSB();

        if (bumount() == -1)
        {
            fprintf(stderr, "Error while unmounting\n");
            return -1;
        }
    }
    else
    {
        fprintf(stderr, "Not enough arguments\n");
    }
    return 0;
}

int printSB()
{
    /* if(bread(posSB, &SB) == -1){
         fprintf(stderr, "Error while reading\n");
         return -1;
     }
     */

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
    printf("sizeof struct superbloque: %li\n", sizeof(SB));
    printf("sizeof struct inodo: %li\n\n", sizeof(inodos));

    return 0;
}
