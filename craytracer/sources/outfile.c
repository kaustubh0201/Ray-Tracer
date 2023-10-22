#include "outfile.h"

#include <stdio.h>
#include <stdlib.h>

void writeToPPM(const char * filename, int width, int height, 
                const RGBColorU8* arr){

    FILE *fptr = fopen(filename, "w");
    
    if(fptr == NULL){
        printf("ERROR: File not found.\n");
        exit(1);
    }

    fprintf(fptr, "P3\n");
    fprintf(fptr,"%d %d\n", width, height);
    fprintf(fptr,"255\n");

    for(int i = 0; i < width*height; i++){
        fprintf(fptr, "%hu %hu %hu\n", arr[i].r, arr[i].g, arr[i].b);
    }

    fclose(fptr);
}
