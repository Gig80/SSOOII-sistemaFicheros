#include "ficheros_basico.h"

// Declaraciones de funciones
int printSB();

// Declaraciones de variables globales
struct superbloque testSB;

int main(int argc, char **argsv){
    if(argc == 2){
        if (bmount(argsv[1]) == -1)
        { // File paths are relative????
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
    return 0;
}

int printSB()
{
    if(bread(posSB, &testSB) == -1){
        perror("Error al leer superbloque.");
        return -1;
    }

    printf("DATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB: %i\n", testSB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB: %i\n", testSB.posUltimoBloqueMB);
    printf("posPrimerBloqueAI: %i\n", testSB.posPrimerBloqueAI);
    printf("posUltimoBloqueAI: %i\n", testSB.posUltimoBloqueAI);
    printf("posPrimerBloqueDatos: %i\n", testSB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos: %i\n", testSB.posUltimoBloqueDatos);
    printf("posInodoRaíz: %i\n", testSB.posInodoRaiz);
    printf("posPrimerInodoLibre: %i\n", testSB.posPrimerInodoLibre);
    printf("cantBloquesLibres: %i\n", testSB.cantBloquesLibres);
    printf("cantInodosLibres: %i\n", testSB.cantInodosLibres);
    printf("totBloques: %i\n", testSB.totBloques);
    printf("totInodos: %i\n\n", testSB.totInodos);
    printf("sizeof struct superbloque: %li\n", sizeof(struct superbloque));
    printf("sizeof struct inodo: %li\n\n", sizeof(struct inodo));

    return 0;
}