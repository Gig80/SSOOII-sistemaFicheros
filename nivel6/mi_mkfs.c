#include "bloques.c"
#include <string.h>
#include <stdlib.h>

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