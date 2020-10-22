#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#pragma pack(1)

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

typedef struct pixel{
    unsigned char blue; //char sem sinal para ir de 0 a 256 bytes
    unsigned char green;
    unsigned char red;
} RGB;

unsigned char **allocate_image(int rows, int colums)
{
   unsigned char **image=NULL;

   image = (unsigned char **) malloc(rows * sizeof(unsigned char *));
   
   for(int row = 0;row < rows;row++)
      image[row] = (unsigned char *) malloc(colums * sizeof(unsigned char));

   return image;
}

void quick_sort(unsigned char *array, int first, int last){
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
   unsigned char **image = NULL;
   FILE *in, *out;
   CABECALHO c;
   RGB p;
    
   if (argc != 4) {
       printf("%s <imagem_de_entrada> <imagem_de_saida> <tamanho_mascara> <numero_threads>\n", argv[0]);
       exit(0);
   }
   
   if((in = fopen(argv[1], "rb")) == NULL){
      printf("Erro ao abrir a imagem de entrada.\n");
      exit(0);
   }

   if((out = fopen(argv[2], "wb")) == NULL) {
      printf("Erro ao abrir a imagem de saida.\n");
      exit(0);
   }

   fread(&c, sizeof(CABECALHO), 1, in);
   image = allocate_image(c.altura, c.largura);
   for (int row = 0;row < c.altura;row++) {
      fread((image)[row], 1, c.largura, in);
   }

   int maskSize = atoi(argv[3]);

   fwrite(&c, sizeof(CABECALHO), 1, out);     
   unsigned char *pixel_values=NULL;
 
   pixel_values = (unsigned char *) malloc(maskSize * maskSize * sizeof(unsigned char));  
 
    for(int row = 0;row < c.altura;row++){
       fread(&p, sizeof(RGB), 1, in);
       for(int col = 0;col < c.largura;col++){
          int point = 0;
          for(int rowAux = row - (maskSize/2);rowAux < (row-(maskSize/2) + maskSize);rowAux++){
             for(int colAux = col-(maskSize/2);colAux < (col-(maskSize/2) + maskSize);colAux++){
                if((rowAux >= 0) && (rowAux < c.altura) && (colAux >= 0) && (colAux < c.largura)){
                   pixel_values[point] = image[rowAux][colAux];
                   point++;
	             }
             }
          }
 
          quick_sort(pixel_values, 0, point - 1);         
          p.blue = pixel_values[point/2];
          p.green = pixel_values[point/2];
          p.red = pixel_values[point/2];
          fwrite(&p, sizeof(RGB), 1, out);
       }
    } 		
   
   fclose(out);
   fclose(in);
   free(image);
}