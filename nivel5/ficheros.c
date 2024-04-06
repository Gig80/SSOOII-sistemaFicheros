#include "ficheros.h"

// Escribe el contenido procedente de un buffer de memoria en un fichero/directorio
// Devuelve la cantidad de bytes escritos realmente
int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes)
{
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    // Comprobamos que haya permiso de escritura sobre el inodo
    if ((inodo.permisos & 2) != 2)
    {
        fprintf(stderr, RED "No hay permisos de escritura\n" RESET);
        return FALLO;
    }

    int bytesescritos = 0;
    int primerBL = offset / BLOCKSIZE;                // primer bloque lógico
    int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE; // último bloque lógico
    int desp1 = offset % BLOCKSIZE;
    int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
    unsigned char buf_bloque[BLOCKSIZE];

    if (primerBL == ultimoBL)
    { // El buffer cabe en un solo bloque
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (bread(nbfisico, buf_bloque) == -1)
        {
            fprintf(stderr, "Error while reading\n");
            return -1;
        }
        // Escribimos los nbytes
        memcpy(buf_bloque + desp1, buf_original, nbytes);

        // Escribimos buf_bloque modificado en el bloque físico correspondiente
        if (bwrite(nbfisico, buf_bloque) == -1)
        {
            fprintf(stderr, "Error while writing\n");
            return -1;
        }
        bytesescritos = nbytes;
    }
    else
    { // Afecta a más de un bloque
        // Primer bloque lógico:
        // Preservamos el contenido original del dispositivo virtual:
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (bread(nbfisico, buf_bloque) == -1)
        {
            fprintf(stderr, "Error while reading\n");
            return -1;
        }
        int bytesrestantes = BLOCKSIZE - desp1;
        // Copiamos los bytes restantes del buf_original al buf_bloque
        memcpy(buf_bloque + desp1, buf_original, bytesrestantes);
        if (bwrite(nbfisico, buf_bloque) == -1)
        {
            fprintf(stderr, "Error while writing\n");
            return -1;
        }
        bytesescritos += BLOCKSIZE - desp1;

        // Bloques lógicos intermedios
        for (int bl = primerBL + 1; bl < ultimoBL; bl++)
        {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 1);
            if (bwrite(nbfisico, buf_original + (bytesrestantes) + (bl - primerBL - 1) * BLOCKSIZE) == -1)
            {
                fprintf(stderr, "Error while writing\n");
                return -1;
            }
            bytesescritos += BLOCKSIZE;
        }

        // Último bloque lógico
        int nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 1);
        if (bread(nbfisico, buf_bloque) == -1)
        {
            fprintf(stderr, "Error while reading\n");
            return -1;
        }
        // Copiamos los bytes al último bloque:
        memcpy(buf_bloque, buf_original + (nbytes - (desp2 + 1)), desp2 + 1);

        if (bwrite(nbfisico, buf_bloque) == -1)
        {
            fprintf(stderr, "Error while writing\n");
            return -1;
        }
        bytesescritos += desp2 + 1;
    }
    // Actualizamos la metainformación del inodo:
    leer_inodo(ninodo, &inodo);
    if (inodo.tamEnBytesLog < (bytesescritos + offset))
    {
        inodo.tamEnBytesLog = bytesescritos + offset;
        inodo.ctime = time(NULL);
    }
    inodo.mtime = time(NULL);
    escribir_inodo(ninodo, inodo);

    return bytesescritos;
}

// Lee información de un fichero/directorio (correspondiente al nº de inodo,
// ninodo, pasado como argumento) y la almacena en un buffer de memoriam buf_original
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    if ((inodo.permisos & 4) != 4)
    {
        fprintf(stderr, RED "No hay permisos de lectura\n" RESET);
        return FALLO;
    }

    int bytesleidos = 0;
    int primerBL = offset / BLOCKSIZE;                // primer bloque lógico
    int ultimoBL = (offset + nbytes - 1) / BLOCKSIZE; // último bloque lógico
    int desp1 = offset % BLOCKSIZE;
    int desp2 = (offset + nbytes - 1) % BLOCKSIZE;
    unsigned char buf_bloque[BLOCKSIZE];

    if (offset >= inodo.tamEnBytesLog)
    {
        bytesleidos = 0;
        return bytesleidos;
    }
    if ((offset + nbytes) >= inodo.tamEnBytesLog)
    {
        nbytes = inodo.tamEnBytesLog - offset;
    }

    if (primerBL == ultimoBL)
    { // El buffer cabe en un solo bloque
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 0);
        if (nbfisico != -1)
        {
            if (bread(nbfisico, buf_bloque) == -1)
            {
                fprintf(stderr, "Error while reading\n");
                return -1;
            }
            // Escribimos los nbytes
            memcpy(buf_original, buf_bloque + desp1, nbytes);
        }

        bytesleidos = nbytes;
    }
    else
    { // Afecta a más de un bloque
        // Primer bloque lógico:
        // Preservamos el contenido original del dispositivo virtual:
        int nbfisico = traducir_bloque_inodo(ninodo, primerBL, 1);
        if (nbfisico != -1)
        {
            if (bread(nbfisico, buf_bloque) == -1)
            {
                fprintf(stderr, "Error while reading\n");
                return -1;
            }
            int bytesrestantes = BLOCKSIZE - desp1;
            // Copiamos los bytes restantes del buf_original al buf_bloque
            memcpy(buf_original, buf_bloque + desp1, bytesrestantes);
        }

        bytesleidos += BLOCKSIZE - desp1;

        // Bloques lógicos intermedios
        for (int bl = primerBL + 1; bl < ultimoBL; bl++)
        {
            nbfisico = traducir_bloque_inodo(ninodo, bl, 0);
            if (nbfisico != -1)
            {
                if (bread(nbfisico, buf_bloque) == -1)
                {
                    {
                        fprintf(stderr, "Error while writing\n");
                        return -1;
                    }
                    memcpy(buf_original + (BLOCKSIZE - desp1) + (bl - primerBL - 1) * BLOCKSIZE, buf_bloque, BLOCKSIZE); // leemos todo el bloque
                }
                bytesleidos += BLOCKSIZE;
            }
        }

        // Último bloque lógico
        nbfisico = traducir_bloque_inodo(&inodo, ultimoBL, 0);
        if (nbfisico != -1)
        {
            if (bread(nbfisico, buf_bloque) == -1)
            {
                fprintf(stderr, "Error while reading\n");
                return -1;
            }
            // Copiamos los bytes al último bloque:
            memcpy(buf_original + (nbytes - (desp2 + 1)), buf_bloque, desp2 + 1);
        }

        bytesleidos += desp2 + 1;
    }
    return bytesleidos;
}

// Función que devuelve la metainformación de un fichero/directorio
// correspondiente al número de inodo pasado por argumento
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    struct inodo inodo;
    leer_inodo(ninodo,&inodo);
    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;

    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return 0;
}

// Cambia los permisos de un fichero/directorio (correspondiente 
// al nº de inodo pasado como argumento, ninodo) con el valor que 
// indique el argumento permisos.

int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{

}
