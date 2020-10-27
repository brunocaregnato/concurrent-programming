#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#pragma pack(1)

/* Cabecalho da imagem bmp */
typedef struct cabecalho {
	unsigned short tipo;
	unsigned int tamanho_arquivo;
	unsigned short reservado1;
	unsigned short reservado2;
	unsigned int offset;
	unsigned int tamanho_image_header;
	int largura;
	int altura;
	unsigned short planos;
	unsigned short bits_por_pixel;
	unsigned int compressao;
	unsigned int tamanho_imagem;
	int largura_resolucao;
	int altura_resolucao;
	unsigned int numero_cores;
	unsigned int cores_importantes;
} CABECALHO;

/* Pixeis da imagem bmp 
 * utiliza char sem sinal para ir de 0 a 256 bytes
 */
typedef struct pixel {
   unsigned char blue; //
   unsigned char green;
   unsigned char red;
} RGB;

RGB *allocate_image(FILE *file, CABECALHO *header) {
   RGB *image = (RGB*) malloc(sizeof(RGB));

   image = (RGB*) malloc(sizeof(RGB) * (header->altura * header->largura));

   int offset = 0, ali = (header->largura * 3) % 4;
   char aux;

   if (ali != 0)
      ali = 4 - ali;

   for (int i = 0; i < header->altura; i++) {
      fread(image + offset, sizeof(RGB), header->largura, file);
      offset += header->largura;

      if (ali) fread(&aux, sizeof(unsigned char), ali, file);
   }

    return image;
}

RGB *copy_image(RGB *image, CABECALHO *header) {
   RGB *imageAux = (RGB*) malloc(sizeof(RGB) * (header->largura * header->altura));
   memcpy(imageAux, image, sizeof(RGB) * (header->largura * header->altura));
   return imageAux;
}


void create_image(FILE *file, RGB *image, CABECALHO header) {    
   fwrite(&header, sizeof(CABECALHO), 1, file);

   int offset = 0, ali = (header.largura * 3) % 4;
   char aux;

   if (ali != 0)
      ali = 4 - ali;

   for (int i = 0; i < header.altura; i++) {
      fwrite(image + offset, sizeof(RGB), header.largura, file);
      offset += header.largura;
       
      if (ali) fwrite(&aux, sizeof(unsigned char), ali, file);
   }
}

RGB *setOffset(RGB *image, CABECALHO header, int row, int col) {
   if (row < 0 || row > header.altura || col < 0 || col > header.largura)
      return NULL;

   return image + ((row * header.largura) + col);
}

void quick_sort(unsigned char *array, int first, int last) {
   int i, j, pivot, temp;

   if(first < last) {
      pivot = first;
      i = first;
      j = last;

      while(i<j) {
         while(array[i] <= array[pivot] && i < last) i++;
         while(array[j] > array[pivot]) j--;
         if(i < j) {
            temp = array[i];
            array[i] = array[j];
            array[j] = temp;
         }
      }

      temp = array[pivot];
      array[pivot] = array[j];
      array[j] = temp;
      quick_sort(array, first, j-1);
      quick_sort(array, j+1, last);
   }
}

int main(int argc, char **argv) {
   FILE *fileIn, *fileOut;
   CABECALHO headerIn, headerOut;
   RGB *imageIn, *imageOut;
    
   if (argc != 4) {
       printf("%s <imagem_de_entrada> <imagem_de_saida> <tamanho_mascara> <numero_threads>\n", argv[0]);
       exit(0);
   }

   if ((fileIn = fopen(argv[1], "rb")) == NULL) {
      printf("Nao foi possivel abrir o arquivo de entrada!\n");
      exit(0);
   }

   if ((fileOut = fopen(argv[2], "wb")) == NULL) {
      printf("Nao foi possivel abrir o arquivo de saida!\n");
      exit(0);
   }

   fread(&headerIn, sizeof(CABECALHO), 1, fileIn);

   imageIn = allocate_image(fileIn, &headerIn);

   headerOut = headerIn;
   imageOut = copy_image(imageIn, &headerIn);

   int maskSize = atoi(argv[3]);
   int size = maskSize * maskSize;
   unsigned char color_blue[size], color_green[size], color_red[size];

   for (int row = 0;row < headerIn.altura; row++) {
      for (int col = 0;col < headerIn.largura; col++) {
         if (row < (maskSize/2) || row >= headerIn.altura - (maskSize/2)
            || col < (maskSize/2) || col >= headerIn.largura - (maskSize/2)) { 
               RGB *pixel = setOffset(imageOut, headerOut, row, col);
               pixel->red = 0;
               pixel->green = 0;
               pixel->blue = 0;
               continue;
         }

         int point = 0;
         for (int rowAux =- (maskSize/2);rowAux <= (maskSize/2); rowAux++) {
            for (int colAux =- (maskSize/2);colAux <= (maskSize/2); colAux++) {
               RGB *pixel = setOffset(imageIn, headerIn, row + rowAux, col + colAux);
               color_blue[point] = pixel->blue;
               color_green[point] = pixel->green;
               color_red[point] = pixel->red;
               point++;
            }
         }

         RGB *pixel = setOffset(imageOut, headerOut, row, col);

         quick_sort(color_blue, 0, point - 1);
         pixel->blue = color_blue[size/2];
         quick_sort(color_green, 0, point - 1);
         pixel->green = color_green[size/2];
         quick_sort(color_red, 0, point - 1);
         pixel->red = color_red[size/2];
      }
   }   

   create_image(fileOut, imageOut, headerOut);

   fclose(fileIn);
   fclose(fileOut);
   free(imageIn);
   free(imageOut);
}