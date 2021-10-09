#include<stdio.h>
#include<stdlib.h>

//---------------------------------------------->header<----------------------------------------------
//--------->apenas fazendo isso pois não sei se o .h do trabalho não pode ser modificado<-------------

//conversão de UTF-8 para Varint
int utf_varint(FILE *arq_entrada, FILE *arq_saida);

//conversão de Varint para UTF-8
int varint_utf(FILE *arq_entrada, FILE *arq_saida);

//função de abertura do arquivo
FILE *openFile(char *path, char *mode);

//função exibe na tela conteúdo do arquivo
void PrintContent_test(FILE * f_test);

//função converte sequencia de bytes em Hex para Varint e escreve no arquivo
void write_SeqVarint(unsigned char *ByteSeq, int lenght, FILE* arq_saida);

//função alinha os bits mais significantes no array de bytes advintos de varint
unsigned char *alignSeq(unsigned char *ByteSeq,int lenght);
//---------------------------------------------->main<-------------------------------------------------

int main(void){
   int res = 0;

   FILE *fin = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/Unicode_pequeno.txt","rb");
   FILE *fout = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/saida_teste.txt","wb");
   //PrintContent_test(fin);
   res = utf_varint(fin,fout);
   fclose(fout);
   fclose(fin);

   printf("\n ---------------> fim: %d <--------------- \n",res);
   return 0;
}

//----------------------------------------------------------------------------------------------------


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

void write_SeqVarint(unsigned char *ByteSeq, int lenght, FILE* arq_saida){
   int finalIndex = lenght - 1;
   unsigned char msb1 = ByteSeq[finalIndex] >> 7;
   unsigned char msb2; 
   unsigned char flag = 0x80;

   printf("\n>>>>>> inicio da sequencia varintf <<<<<<<<\n");
   printf("\ntamanho da sequencia: %d\n",lenght);
   for (int i = finalIndex; i >= 0; i--)
   {        
      msb2 = ByteSeq[i] >> 7;
      
      if (i == 0 ) { flag = 0x00; }
      
      if (i == lenght - 1) { ByteSeq[i] |= flag; }
      else
      {
         ByteSeq[i] <<= 1;
         ByteSeq[i] |= (msb1 | flag);
      }
      msb1 = msb2;
      fwrite(&ByteSeq[i],sizeof(unsigned char), 1, arq_saida);
      printf("byte: %02X ",ByteSeq[i]);
   }
   printf("\n--------------------------------------------------------------\n\n");
}



int utf_varint(FILE *arq_entrada, FILE *arq_saida){
   
   printf("\n------------------> inicio da função <------------------\n");
   unsigned char byteCurrent;
   unsigned char byteSeq[4];
   unsigned char byteBefore = 0x00;
   int SeqLenght;
   int index = 0;
   
   //loop leitura do arquivo
   for (byteCurrent = getc(arq_entrada); !feof(arq_entrada); byteCurrent = getc(arq_entrada))
   {
      printf("\nbyte entrada loop: %02X \n",byteCurrent);  

      if (byteCurrent >= 0x80)
      {
         printf("----> maior que 128bits !!!");
         int i = 0;

         while (byteCurrent >= 0x80)
         {
            byteCurrent <<= 1;
            i++;
         }
         printf(" ----> nº de 1s: %d \n",i);
         byteCurrent >>= i;
         byteCurrent |= (byteBefore << 6); 
         byteBefore = (byteCurrent & 0x03);
         
         if (i>1) { SeqLenght = i; }

         byteSeq[index] = (byteCurrent);
         
         if (index == SeqLenght - 1)
         {
            byteBefore = 0x00;
            write_SeqVarint(byteSeq,SeqLenght,arq_saida);
            index = 0;
         }
         else
         {
            byteSeq[index] >>= 2;
            index++;
         }
      }
      else { fwrite(&byteCurrent,sizeof(unsigned char), 1, arq_saida); }  
   }
   return 0;
}

unsigned char *alignSeq(unsigned char *ByteSeq,int lenght){
   unsigned char lsb;
   unsigned char msb = 0x00;
   for (int i = lenght - 1; i >= 0; i--)
   {
      lsb = ByteSeq[i] & 0x01;
      ByteSeq[i] >>= 1;
      ByteSeq[i] |= msb; 
      msb = (lsb<<7);
   }
   



}

int varint_utf(FILE *arq_entrada, FILE *arq_saida){
   printf("\n------------------> inicio da função 2 <------------------\n");
   unsigned char byteCurrent;
   unsigned char byteSeq[4];
   unsigned char msbBefore;
   int SeqLenght = 0;
   int index = 0;
   
   for (byteCurrent = getc(arq_entrada); !feof(arq_entrada); byteCurrent = getc(arq_entrada))
   {
      
      if(byteCurrent >= 0x80)
      {
         printf("----> maior que 128bits !!!");
         byteSeq[index] = byteCurrent & 0x7F;
         index++;
      }
      else if(index =! 0)
      {
         byteSeq[index] = byteCurrent;
         index = 0;
         unsigned char *alignSeq(ByteSeq, index);
         //chama função que converte array de hex em UTF-8
      }
      else { fwrite(&byteCurrent,sizeof(unsigned char), 1, arq_saida); } 

   return 0;
}
