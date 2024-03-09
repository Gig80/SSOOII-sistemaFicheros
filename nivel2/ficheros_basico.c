#include "ficheros_basico.h"

struct superbloque SB;

// tamMB() devuelve el tamaño del mapa de bits.
int tamMB(unsigned int nbloques){
    if((nbloques/8)%BLOCKSIZE != 0){
        return ceil((nbloques/8)/BLOCKSIZE);
    }else{
        return (nbloques/8)/BLOCKSIZE;
    }
}


// tamAI() devuelve el tamaño del array de inodos.
int tamAI(unsigned int ninodos){
    if((ninodos*INODOSIZE)%BLOCKSIZE != 0){
        return ceil((ninodos*INODOSIZE)/BLOCKSIZE);
    }else{
        return (ninodos*INODOSIZE)/BLOCKSIZE;
    }
}

// initSB() inicializa el superbloque.
int initSB(unsigned int nbloques, unsigned int ninodos){
    SB.posPrimerBloqueMB = posSB + tamSB;
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
    SB.posUltimoBloqueDatos = nbloques - 1;
    SB.posInodoRaiz = 0;
    SB.posPrimerInodoLibre = 0;
    SB.cantBloquesLibres = nbloques;
    SB.cantInodosLibres = ninodos;
    SB.totBloques = nbloques;
    SB.totInodos = ninodos;
    bwrite(posSB, &SB);
}

// initMB() inicializa el mapa de bits.
int initMB(){
    int tamDatos = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    char bufferMB[BLOCKSIZE];

    for(int i = 0; i < tamDatos/8; i++){
        bufferMB[i] = 255;
    }

    //bufferMB[tamDatos/8] = ;

    for(int i = tamDatos/8 + 1; i < BLOCKSIZE; i++){
        bufferMB[i] = 0;
    }

    bwrite(0,bufferMB);
}


// initAI() el bloque de inodos.
int initAI(){
    struct inodo inodos [BLOCKSIZE/INODOSIZE];
    void *buf [BLOCKSIZE];

    int contInodos = SB.posPrimerInodoLibre + 1;
    for(int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
        bread(i, inodos);
        for(int j = 0; j < BLOCKSIZE/INODOSIZE; j++){
            inodos[j].tipo = 'l';
            if(contInodos < SB.totInodos){
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }else{
                inodos[j].punterosDirectos[0] = UINT_MAX;
                break;
            }
        }
        bwrite(i, inodos);
    }
}   