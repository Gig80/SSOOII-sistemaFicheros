#include "ficheros_basico.h"

struct superbloque SB;

int tamMB(unsigned int nbloques){
    if((nbloques/8)%BLOCKSIZE != 0){
        return ceil((nbloques/8)/BLOCKSIZE);
    }else{
        return (nbloques/8)/BLOCKSIZE;
    }
}

int tamAI(unsigned int ninodos){
    if((ninodos*INODOSIZE)%BLOCKSIZE != 0){
        return ceil((ninodos*INODOSIZE)/BLOCKSIZE);
    }else{
        return (ninodos*INODOSIZE)/BLOCKSIZE;
    }
}

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

int initMB(){
    bread(0,&SB);
    int tamDatos = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    char bufferMB[BLOCKSIZE];

    for(int i = 0; i < tamDatos/8; i++){
        bufferMB[i] = 255;
    }

    bufferMB[tamDatos/8] = 224;

    for(int i = tamDatos/8 + 1; i < BLOCKSIZE; i++){
        bufferMB[i] = 0;
    }

    bwrite(0,bufferMB);
}

int initAI(){
    
}