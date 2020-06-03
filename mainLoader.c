#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// Para usar strings

// SOIL é a biblioteca para leitura das imagens
#ifndef __APPLE__
#include "SOIL.h"
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#include "SOIL/SOIL.h"
#endif

// Uma imagem em tons de cinza
typedef struct {
    int width, height;   // Largura e alstura da imagem
    unsigned char* img;  // Este campo contém os bytes da imagem
} Img;

// Protótipos
void load(char* name, Img* pic);

// Carrega uma imagem para a struct Img
void load(char* name, Img* pic)
{
    int chan;
    printf("Filename: %s\n", name);
    pic->img = (unsigned char*) SOIL_load_image(name, &pic->width, &pic->height, &chan, SOIL_LOAD_AUTO);
    if(!pic->img)
    {
        printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
        exit(1);
    }
    printf("Load: %d x %d x %d\n", pic->width, pic->height, chan);
}

int main(int argc, char** argv)
{
    Img pic;
    if(argc < 1) {
        printf("loader [img]\n");
        exit(1);
    }
    load(argv[1], &pic);

    printf("Primeiros 256 bytes da imagem:\n");
    for(int i=0; i<256; i++) {
        if(i%16==0) printf("\n");
        printf("%02X ", pic.img[i]);
    }
    printf("\n");

    free(pic.img);
}
