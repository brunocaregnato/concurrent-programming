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
   unsigned char blue;
   unsigned char green;
   unsigned char red;
} RGB;

/* parametros utilizados no processo de thread */
typedef struct parametros {
	int id;
	int n;
	int nthr;
	CABECALHO headerIn;
	CABECALHO headerOut;
	RGB *imageIn;
   RGB *imageOut;
} PARAMETROS;

/*--------------------------------------------------------------------*/

RGB *allocate_image(FILE *file, int altura, int largura) {
   RGB *image = (RGB*) malloc(sizeof(RGB));

   image = (RGB*) malloc(sizeof(RGB) * (altura * largura));

   int offset = 0, ali = (largura * 3) % 4;
   char aux;

   if (ali != 0) ali = 4 - ali;

   for (int i = 0; i < altura; i++) {
      fread(image + offset, sizeof(RGB), largura, file);
      offset += largura;

      if (ali) fread(&aux, sizeof(unsigned char), ali, file);
   }

   return image;
}

RGB *copy_image(RGB *image, int image_size) {
   RGB *imageAux = (RGB*) malloc(sizeof(RGB) * image_size);
   memcpy(imageAux, image, sizeof(RGB) * image_size);

   return imageAux;
}

RGB *set_offset(RGB *image, int largura, int row, int col) {
   return image + ((row * largura) + col); //desloca para o pixel da imagem
}

void create_image(FILE *file, RGB *image, CABECALHO header) {    
   fwrite(&header, sizeof(CABECALHO), 1, file);

   int offset = 0, ali = (header.largura * 3) % 4;
   char aux;

   if (ali != 0) ali = 4 - ali;

   for (int i = 0; i < header.altura; i++) {
      fwrite(image + offset, sizeof(RGB), header.largura, file);
      offset += header.largura;
       
      if (ali) fwrite(&aux, sizeof(unsigned char), ali, file);
   }
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

void *generate_image(void *args) {   
   PARAMETROS *par = (PARAMETROS *) args;
   int maskSize = par->n;

   unsigned char color_blue[maskSize * maskSize], color_green[maskSize * maskSize], color_red[maskSize * maskSize];

    for (int row = par->id; row < par->headerIn.altura; row += par->nthr) {
      for (int col = 0; col < par->headerIn.largura; col++) {
         if (row < (maskSize/2) || row >= par->headerIn.altura - (maskSize/2)
            || col < (maskSize/2) || col >= par->headerIn.largura - (maskSize/2)) { 
               RGB *pixel = set_offset(par->imageOut, par->headerOut.largura, row, col);               
               pixel->blue = par->imageOut->blue;
               pixel->green = par->imageOut->green;
               pixel->red = par->imageOut->red;
               continue;
         }

         int point = 0;
         for (int rowAux =- (maskSize/2); rowAux <= (maskSize/2); rowAux++) {
            for (int colAux =- (maskSize/2); colAux <= (maskSize/2); colAux++) {
               RGB *pixel = set_offset(par->imageIn, par->headerIn.largura, row + rowAux, col + colAux);
               color_blue[point] = pixel->blue;
               color_green[point] = pixel->green;
               color_red[point] = pixel->red;
               point++;
            }
         }

         RGB *pixel = set_offset(par->imageOut, par->headerOut.largura, row, col);
         quick_sort(color_blue, 0, point - 1);
         pixel->blue = color_blue[point/2];
         quick_sort(color_green, 0, point - 1);
         pixel->green = color_green[point/2];
         quick_sort(color_red, 0, point - 1);
         pixel->red = color_red[point/2];
      }
   }   
}

int main(int argc, char **argv) {
   FILE *fileIn, *fileOut;
   CABECALHO headerIn, headerOut;
   RGB *imageIn, *imageOut;
   pthread_t *tid = NULL;
	PARAMETROS *par = NULL;
   int nthr, maskSize;
    
   if (argc != 5) {
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
   
   maskSize = atoi(argv[3]);
   nthr = atoi(argv[4]);

   tid = (pthread_t *)malloc(nthr * sizeof(pthread_t));
	par = (PARAMETROS *)malloc(nthr * sizeof(PARAMETROS));

   fread(&headerIn, sizeof(CABECALHO), 1, fileIn);

   imageIn = allocate_image(fileIn, headerIn.altura, headerIn.largura);

   headerOut = headerIn; //copia cabecalho de entrada pro de saida
   imageOut = copy_image(imageIn, headerIn.largura * headerIn.altura);

   int i = 0;
	for(; i < nthr; i++) {
		par[i].id = i;
		par[i].n = maskSize;
		par[i].nthr = nthr;
		par[i].headerIn = headerIn;
		par[i].headerOut = headerOut;
		par[i].imageIn = imageIn;
      par[i].imageOut = imageOut;
		pthread_create(&tid[i], NULL, generate_image, (void *) &par[i]); 
	} 
	
	for (i = 0; i < nthr; i++) {
		pthread_join(tid[i], NULL);
	}

   create_image(fileOut, imageOut, headerOut);

   fclose(fileIn);
   fclose(fileOut);
   free(imageIn);
   free(imageOut);
}