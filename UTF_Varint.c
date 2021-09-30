#include<stdio.h>
#include<stdlib.h>

//---------------------------------------------->header<----------------------------------------------
//-------------->apena fazendo isso pois o .h do trabalho não pode ser modificado<--------------------

int utf_varint(FILE *arq_entrada, FILE *arq_saida);
int varint_utf(FILE *arq_entrada, FILE *arq_saida);
FILE *openFile(char *path, char *mode);
void PrintContent_test(FILE * f_test);
//---------------------------------------------->main<-------------------------------------------------

int main(void){
   int res = 0;

   FILE *fin = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/Unicode_pequeno.txt","rb");
   FILE *fout = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/saida_teste.txt","wb");
   res = utf_varint(fin,fout);
   fclose(fout);
   fclose(fin);

   printf("\n ---------------> fim: %d <--------------- \n",res);
   return 0;
}

//----------------------------------------------------------------------------------------------------

/*
void dump (void *p, int n) {
	unsigned char *p1 = p;
	while (n--) {
		printf("%p - %02x\n", p1, *p1);
		p1++;
	}
}
*/


void PrintContent_test(FILE * f_test){
   printf("\n ------------------> leitura do arquivo <------------------ \n ");
   unsigned char c;
   printf("\nChar <-> UTF8\n_______________\n");

   do
   {
      c = getc(f_test);
      printf("   %c <-> %02X \n", c, c );

      if(feof(f_test)) break;
   } while (1); 

   
}



FILE *openFile(char *path, char *mode)
{
   FILE *f = fopen(path,mode);
   if(f == NULL)
   {
      printf("arquivo %s não pode ser aberto\n",path);
      exit(1);   
   }

   return f;
}

int utf_varint(FILE *arq_entrada, FILE *arq_saida){
   

   PrintContent_test(arq_entrada);
   

   return 0;
}