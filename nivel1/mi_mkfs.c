#include "bloques.c"

int main(int argc, char **argv){
        bmount(argv[1]);
        unsigned char arr[BLOCKSIZE];
        memset(arr,0,BLOCKSIZE);
        int size = atoi(argv[2]);
        for(int i = 0; i < size; i++){
            bwrite(i,arr);
        }
        bumount();
}