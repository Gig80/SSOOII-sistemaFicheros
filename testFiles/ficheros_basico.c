#include "ficheros_basico.h"

struct superbloque SB;

// tamMB() devuelve el tamaño del mapa de bits.
int tamMB(unsigned int nbloques){
    if((nbloques/8)%BLOCKSIZE != 0){
        return (nbloques/8)/BLOCKSIZE + 1;
    }else{
        return (nbloques/8)/BLOCKSIZE;
    }
}


// tamAI() devuelve el tamaño del array de inodos.
int tamAI(unsigned int ninodos){
    if((ninodos*INODOSIZE)%BLOCKSIZE != 0){
        return (ninodos*INODOSIZE)/BLOCKSIZE + 1;
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
    return bwrite(posSB, &SB);
}

// initMB() inicializa el mapa de bits.
int initMB(){

    bread(posSB, &SB); //Leer bloque de metadatos en el buffer struct superbloque

    int tamDatos = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    int primerBloqueLibre = SB.posPrimerBloqueMB;
    int nBloques = tamDatos/8/BLOCKSIZE;
    char bufferMB[BLOCKSIZE];

    if(tamDatos/8/BLOCKSIZE >= 1){
        memset(bufferMB, 255, sizeof(bufferMB));
        for(int i = 0; i < nBloques; i++){
            if(bwrite(primerBloqueLibre++, bufferMB) == FALLO){
                perror("Error escribiendo bloques MB.");
                return FALLO;
            }
            tamDatos =- 8*BLOCKSIZE;
        }
    }

    memset(bufferMB, 0, sizeof(bufferMB));

    for(int i = 0; i <= (tamDatos/8 - 1); i++){
        bufferMB[i] = 255;
    }

    if(tamDatos%8 != 0){
        bufferMB[tamDatos/8] = 255;
        bufferMB[tamDatos/8] = bufferMB[tamDatos/8] << (7-tamDatos%8);
    }

    if(bwrite(primerBloqueLibre, bufferMB) == FALLO){
        perror("Error escribiendo bloques MB.");
        return FALLO;
    }

    SB.cantBloquesLibres = SB.cantBloquesLibres - (tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos));
    bwrite(posSB, &SB);
    return EXITO;
}


// initAI() el bloque de inodos.
int initAI(){
    bread(posSB, &SB);
    struct inodo inodos [BLOCKSIZE/INODOSIZE];

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
        if(bwrite(i, inodos) == FALLO){
            perror(RED "Error escribiendo inodo.");
            return FALLO;
        }
    }

    return EXITO;
}

// Función que escribe el valor indicado por el parámetro bit: 0 (libre),
// 1 (ocupado) en un determinado bit del MB que representa nbloque

int escribir_bit(unsigned int nbloque, unsigned int bit){
    int posbyte = (nbloque / 8) % BLOCKSIZE;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Ahora debemos de cargar el contenido en un buffer

    unsigned char bufferMB[BLOCKSIZE];
    unsigned char mascara = 128;
    mascara >>= posbit; // Desplazamiento de bits

    if (bread(nbloqueabs, bufferMB) == -1){ // Leemos bloque
        fprintf(stderr, "Error while reading\n");
        return -1;
    }

    if (bit == 1){
        bufferMB[posbyte] |= mascara;
    }
    else{
        bufferMB[posbyte] &= ~mascara;
    }

    // Escribimos el buffer en el dispositivo virtual

    if (bwrite(nbloqueabs, bufferMB) == -1){
        fprintf(stderr, "Error while writing\n");
        return FALLO;
    }
    return EXITO;
}

char leer_bit(unsigned int nbloque){

    int posbyte = (nbloque / 8) % BLOCKSIZE;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Ahora debemos de cargar el contenido en un buffer

    unsigned char bufferMB[BLOCKSIZE];
    memset(bufferMB, 0, sizeof(bufferMB));
    unsigned char mascara = 128;
    mascara >>= posbit;
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit); // Desplazamiento bits a la derecha

    if (mascara == 0)
    {
        fprintf(stderr, "leer_bit(%d) = 0\n", nbloque / BLOCKSIZE);
        return 0;
    }
    else
    {
        fprintf(stderr, "leer_bit(%d) = 1\n", nbloque / BLOCKSIZE);
        return 1;
    }

    // Escribimos el buffer en el dispositivo virtual

    if (bwrite(nbloqueabs, bufferMB) == -1)
    {
        fprintf(stderr, "Error while writing\n");
        return -1;
    }
    return 0;
}

int reservar_bloque(){
    unsigned char bufferMB[BLOCKSIZE];
    unsigned char bufferAux[BLOCKSIZE];

    memset(bufferAux, 255, BLOCKSIZE);

    bread(posSB, &SB);
    
    int nbloque = 0;

    if(SB.cantBloquesLibres != 0){
        int nBloqueMB;
        for(nBloqueMB = 0; nBloqueMB <= SB.posUltimoBloqueMB; nBloqueMB++){
            bread(SB.posPrimerBloqueMB + nBloqueMB, bufferMB);
            if(memcmp(bufferMB, bufferAux, BLOCKSIZE) != 0){
                break;
            }
        }
        int posbyte = 0;
        for(int i = 0; i < BLOCKSIZE; i++){
            if(bufferMB[i] != 255){
                posbyte = i;
                break;
            }
        }
        unsigned char mascara = 128;
        int posbit = 0;
        while(bufferMB[posbyte] & mascara){
            bufferMB[posbyte] <<= 1;
            posbit++;
        }
        nbloque = (nBloqueMB*BLOCKSIZE+posbyte)*8+posbit;
        escribir_bit(nbloque, 1);
        SB.cantBloquesLibres--;
        bwrite(posSB, &SB);
        memset(bufferAux, 0, BLOCKSIZE);
        bwrite(nbloque, bufferAux);
        return nbloque;
    }
    perror("No quedan bloques libres.");
    return FALLO;
}

int liberar_bloque(unsigned int nbloque){
    bread(posSB, &SB);
    escribir_bit(nbloque, 0);
    SB.cantBloquesLibres++;
    bwrite(posSB, &SB);
    return nbloque;
}

int escribir_inodo(unsigned int ninodo, struct inodo *inodo){
    bread(0, &SB);
    struct inodo inodos[BLOCKSIZE/INODOSIZE];
    int nBloqueAI = (ninodo*INODOSIZE)/BLOCKSIZE;
    int nBloqueAbs = nBloqueAI + SB.posPrimerBloqueAI;
    bread(nBloqueAbs, inodos);
    int posinodo = ninodo % (BLOCKSIZE/INODOSIZE);
    inodos[posinodo] = *inodo;
    return bwrite(nBloqueAbs, inodos);
    //Control de errores
}

// Función que lee un determinado inodo del array de inodos para volcarlo en
// una variable de tipo struct inodo pasada por referencia
int leer_inodo(unsigned int ninodo, struct inodo *inodo){
    // Leemos el superbloque:
    if (bread(posSB, &SB) == -1){
        perror("Error al leer SB.");
        return FALLO;
    }

    int posbloque = ninodo / (BLOCKSIZE / INODOSIZE); // Numero de veces a moverse
    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);  // Posición del inodo dentro del bloque
    // Empleamos un array de inodos:
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    if(bread(SB.posPrimerBloqueAI + posbloque, inodos) == -1){
        perror("Error al leer inodo.");
        return FALLO;
    }
    // Obtenemos el inodo solicitado:
    *inodo = inodos[posinodo];

    return EXITO;
}


int reservar_inodo(unsigned char tipo, unsigned char permisos){
    bread(posSB, &SB);
    if(SB.cantInodosLibres != 0){
        int posInodoReservado = SB.posPrimerInodoLibre;
        SB.posPrimerInodoLibre++;
        struct inodo inodoRes;
        
        inodoRes.tipo = tipo;
        inodoRes.permisos = permisos;
        inodoRes.nlinks = 1;
        inodoRes.tamEnBytesLog = 0;
        inodoRes.atime = time(NULL);
        inodoRes.ctime = time(NULL);
        inodoRes.mtime = time(NULL);
        inodoRes.numBloquesOcupados = 0;
        memset(inodoRes.punterosDirectos, 0, sizeof(int) * 12);
        memset(inodoRes.punterosIndirectos, 0, sizeof(int) * 3);
        escribir_inodo(posInodoReservado, &inodoRes);

        SB.cantInodosLibres--;;
        bwrite(posSB, &SB);
        return posInodoReservado;
    }
    return FALLO;
}
/*
int obtener_nRangoBL(struct inodo *inodo, unsigned int nblogico, unsigned int *ptr){
    if(nblogico < DIRECTOS){
        *ptr = inodo->punterosDirectos[nblogico];
        return 0;
    }else if(nblogico < INDIRECTOS0){
        *ptr = inodo->punterosDirectos[0];
        return 1;
    }else if(nblogico < INDIRECTOS1){
        *ptr = inodo->punterosDirectos[1];
        return 2;
    }else if(nblogico < INDIRECTOS2){
        *ptr = inodo->punterosDirectos[2];
        return 3;
    }else{
        *ptr = 0;
        perror("Bloque lógico fuera de rango.");
        return FALLO;
    }
}

int obtener_indice(unsigned int nblogico, int nivel_punteros){
    if(nblogico < DIRECTOS){
        return nblogico;
    }else if(nblogico < INDIRECTOS0){
        return nblogico - DIRECTOS;
    }else if(nblogico < INDIRECTOS1){
        if(nivel_punteros == 2){
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        }else if(nivel_punteros == 1){
            return (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }else if(nblogico < INDIRECTOS2){
        if(nivel_punteros == 3){
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }else if(nivel_punteros == 2){
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }else if(nivel_punteros == 1){
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;
        }
    }
    return FALLO;
}

int traducir_bloque_inodo(struct inodo *inodo, unsigned int nblogico, unsigned char reservar){
    unsigned int ptr, ptr_ant;
    int nRangoBL, nivel_punteros, indice;
    unsigned int buffer[NPUNTEROS];

    ptr = 0, ptr_ant = 0;
    nRangoBL = obtener_nRangoBL(inodo, nblogico, &ptr);
    nivel_punteros = nRangoBL;
    while(nivel_punteros > 0){
        if(ptr == 0){
            if(reservar == 0){
                return FALLO;
            }else{
                ptr = reservar_bloque();
                inodo->numBloquesOcupados++;
                inodo->ctime = time(NULL);
                if(nivel_punteros == nRangoBL){
                    inodo->punterosDirectos[nRangoBL-1] = ptr;
                }else{
                    buffer[indice] = ptr;
                    bwrite(ptr_ant, buffer);
                }
                memset(buffer, 0, BLOCKSIZE);
            }
        }else{
            bread(ptr, buffer);
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }

    if(ptr == 0){
        if(reservar == 0){
            return FALLO;
        }else{
            ptr = reservar_bloque();
            inodo->numBloquesOcupados++;
            inodo->ctime = time(NULL);
            if(nRangoBL == 0){
                inodo->punterosDirectos[nblogico] = ptr;
            }else{
                buffer[indice] = ptr;
                bwrite(ptr_ant, buffer);
            }
        }
    }
    return ptr;
}

int liberar_inodo(unsigned int ninodo){
    struct inodo *inodo;
    leer_inodo(ninodo, inodo);
    inodo->numBloquesOcupados -= bloliberar_bloques_inodo(0, inodo);
    //inodo libre
    inodo->tamEnBytesLog = 0;
    bread(0, &SB);

}

int librear_bloques_inodo(unsigned int primerBL, struct inodo *inodo){
    unsigned int nivel_punteros = 3, nBL = primerBL, ultimoBL, ptr = 0;
    int nRangoBL = 0, liberados = 0, eof = 0, bloqueLiberado;

    if(inodo->tamEnBytesLog == 0){
        return 0;
    }
    if(inodo->tamEnBytesLog%BLOCKSIZE == 0){
        ultimoBL = inodo->tamEnBytesLog/BLOCKSIZE - 1;
    }else{
        ultimoBL = inodo->tamEnBytesLog/BLOCKSIZE;
    }

    liberados = liberar_bloques_recursivo(&nBL, nRangoBL, ptr, nivel_punteros, inodo, &eof, ultimoBL, &bloqueLiberado);
}

int liberar_bloques_recursivo(unsigned int *nBL, int nRangoBL, unsigned int ptr, int nivel_punteros,
struct inodo *inodo, int *eof, int ultimoBL, int *bloqueLiberado){
    int liberados = 0, indice, aux;
    unsigned int bloquePunteros[NPUNTEROS], bloquePunteros_Aux[NPUNTEROS], bufferCeros[NPUNTEROS];
    memset(bloquePunteros, 0, BLOCKSIZE);
    memset(bufferCeros, 0, BLOCKSIZE);
    if(ptr == 0){
        nRangoBL = obtener_nRangoBL(inodo, *nBL, &ptr);
        nivel_punteros = nRangoBL;
    }
    if(nRangoBL == 0){
        while(!*eof&&*nBL < DIRECTOS){
            if(inodo->punterosDirectos[*nBL] != 0){
                liberar_bloque(inodo->punterosDirectos[*nBL]);
                liberados++;
                inodo->punterosDirectos[*nBL] = 0;
            }
            *nBL = *nBL + 1;
            if(*nBL > ultimoBL){
                *eof = 1;
            }
        }
    }else if(ptr == 0){
        //añadir esta parte
        //saltar los bloques que no es necesrio explorar al valer 0 un puntero de inodo
        //sesegún valor nivel_punteros
    }else if(nivel_punteros > 0){
        indice = obtener_indice(*nBL, nivel_punteros);
        aux = (indice > 0) ? 1 : 0;
        bread(ptr, bloquePunteros);
        memcpy(bloquePunteros_Aux, bloquePunteros, BLOCKSIZE);
        for(int i  = 0; i < NPUNTEROS; i++){
            if(bloquePunteros[indice] != 0){
                if(nivel_punteros == 1){
                    liberar_bloque(bloquePunteros[indice]);
                    liberados++;
                    *bloqueLiberado = *nBL;
                    *nBL = *nBL + 1;
                    bloquePunteros[indice] = 0;
                }else{
                    liberados += liberar_bloques_recursivo(nBL, nRangoBL, bloquePunteros[indice],
                    nivel_punteros - 1, inodo, eof, ultimoBL, bloqueLiberado);
                    if(aux != 0){
                        aux = 0;
                    }else{
                        bloquePunteros[indice] = 0;
                    }
                }
            }else{
                //Añadir lo siguiente
                //IMPRESCINDIBLE: saltar los bloques que no es necesario explorar
                //al eliminar un bloque de punteros, según valor nivel_punteros
            }
            if(*nBL > ultimoBL){
                *eof = 1;
            }
        }
        if(memcmp(bloquePunteros, bloquePunteros_Aux, BLOCKSIZE) != 0){
            //Si el bloque es distinto al original
            if(memcmp(bloquePunteros, bufferCeros, BLOCKSIZE) != 0){
                //Si el bloque es distinto a ceros
                bwrite(ptr, bloquePunteros);
            }else{
                liberar_bloque(ptr);
                if(nivel_punteros == nRangoBL && nivel_punteros > 0){
                    inodo->punterosIndirectos[nivel_punteros-1] = 0;
                }
                liberados++;
            }
        }
    }
    //Llamada recursiva si no ha acabado
    if(!*eof && nivel_punteros == nRangoBL){
        liberados += liberar_bloques_recursivo(nBL, nRangoBL, 0, 0, inodo, eof, ultimoBL, bloqueLiberado);
    }
    return liberados;
    
}*/