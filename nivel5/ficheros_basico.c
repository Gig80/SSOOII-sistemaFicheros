#include "ficheros_basico.h"

struct superbloque SB;

// tamMB() devuelve el tamaño del mapa de bits.
int tamMB(unsigned int nbloques)
{
    if ((nbloques / 8) % BLOCKSIZE != 0)
    {
        return ceil((nbloques / 8) / BLOCKSIZE);
    }
    else
    {
        return (nbloques / 8) / BLOCKSIZE;
    }
}

// tamAI() devuelve el tamaño del array de inodos.
int tamAI(unsigned int ninodos)
{
    if ((ninodos * INODOSIZE) % BLOCKSIZE != 0)
    {
        return ceil((ninodos * INODOSIZE) / BLOCKSIZE);
    }
    else
    {
        return (ninodos * INODOSIZE) / BLOCKSIZE;
    }
}

// initSB() inicializa el superbloque.
int initSB(unsigned int nbloques, unsigned int ninodos)
{
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
int initMB()
{
    int tamDatos = tamSB + tamMB(SB.totBloques) + tamAI(SB.totInodos);
    char bufferMB[BLOCKSIZE];

    for (int i = 0; i < tamDatos / 8; i++)
    {
        bufferMB[i] = 255;
    }

    // bufferMB[tamDatos/8] = ;

    for (int i = tamDatos / 8 + 1; i < BLOCKSIZE; i++)
    {
        bufferMB[i] = 0;
    }

    return bwrite(0, bufferMB);
}

// initAI() el bloque de inodos.
int initAI()
{
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    void *buf[BLOCKSIZE];

    int contInodos = SB.posPrimerInodoLibre + 1;
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    {
        bread(i, inodos);
        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            inodos[j].tipo = 'l';
            if (contInodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = contInodos;
                contInodos++;
            }
            else
            {
                inodos[j].punterosDirectos[0] = UINT_MAX;
                break;
            }
        }
        bwrite(i, inodos);
    }
}

// Función que escribe el valor indicado por el parámetro bit: 0 (libre),
// 1 (ocupado) en un determinado bit del MB que representa nbloque

int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    int posbyte = (nbloque / 8) % BLOCKSIZE;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Ahora debemos de cargar el contenido en un buffer

    unsigned char bufferMB[BLOCKSIZE];
    unsigned char mascara = 128;
    mascara >>= posbit; // Desplazamiento de bits

    if (bread(nbloqueabs, bufferMB) == -1)
    { // Leemos bloque
        fprintf(stderr, "Error while reading\n");
        return -1;
    }

    if (bit == 1)
    {
        bufferMB[posbyte] = mascara;
    }
    else
    {
        bufferMB[posbyte] &= ~mascara;
    }

    // Escribimos el buffer en el dispositivo virtual

    if (bwrite(nbloqueabs, bufferMB) == -1)
    {
        fprintf(stderr, "Error while writing\n");
        return -1;
    }
    return 0;
}

char leer_bit(unsigned int nbloque)
{
    int posbyte = (nbloque / 8) % BLOCKSIZE;
    int posbit = nbloque % 8;
    int nbloqueMB = posbyte / BLOCKSIZE;
    int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Ahora debemos de cargar el contenido en un buffer

    unsigned char bufferMB[BLOCKSIZE];
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

// Función que lee un determinado inodo del array de inodos para volcarlo en
// una variable de tipo struct inodo pasada por referencia
int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    // Leemos el superbloque:
    if (bread(posSB, &SB) == -1)
    {
        fprintf(stderr, "Error while reading SB\n");
        return -1;
    }

    int posbloque = ninodo / (BLOCKSIZE / INODOSIZE); // Numero de veces a moverse
    int posinodo = ninodo % (BLOCKSIZE / INODOSIZE);  // Posición del inodo dentro del bloque
    // Empleamos un array de inodos:
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // Obtenemos el inodo solicitado:
    *inodo = inodos[posinodo];

    return 0;
}
