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

//função converte sequencia de bytes em Hex para Varint e escreve no arquivo de saída
void write_SeqVarint(unsigned char *ByteSeq, int lenght, FILE* arq_saida);

//função alinha os bits mais significantes no array de bytes advintos de varint
void alignSeq(unsigned char *ByteSeq,int lenght);

//função converte array de bytes em Hex para UTF-8 e escreve no arquivo de saída
void write_SeqUTF(unsigned char *ByteSeq, int lenght, FILE* arq_saida);

//---------------------------------------------->main<-------------------------------------------------

int main(void){
   int res = 0;

   //FILE *fin1 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/Unicode_pequeno.txt","rb");
   FILE *fin2 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/var_peq","rb");
   FILE *fout = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/saida_teste.txt","wb");
   //PrintContent_test(fin);
   //res = utf_varint(fin,fout);
   //printf("\n ---------------> fim função 1: %d <--------------- \n",res);
   
   res = varint_utf(fin2,fout);
   printf("\n ---------------> fim 2: %d <--------------- \n",res);

   fclose(fout);
   fclose(fin2);
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
   
   printf("\n------------------> inicio da função UTF <------------------\n");
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

void alignSeq(unsigned char *ByteSeq,int lenght){
   unsigned char lsb;
   unsigned char msb = 0x00;
   printf("sequencia em hex(invertido): ");
   for (int i = lenght; i >= 0; i--)
   {  
      lsb = ByteSeq[i] & 0x01;

      if(i != 0){ ByteSeq[i] >>= 1; } 
      
      ByteSeq[i] |= msb; 
      msb = (lsb<<7);
      printf("%02X ",ByteSeq[i]);
   }
}

void write_SeqUTF(unsigned char *ByteSeq, int lenght, FILE* arq_saida){
   unsigned char next;
   unsigned char signSeq = 0x80;
   unsigned char last = 0x00;
   printf("\n\n teste tamanho do array: %d\n",lenght);
   unsigned char end = 0x00;
   

   for (int i = 0, k = 2; i <= lenght; i++, k+=2)
   {
      next = ByteSeq[i] >> (8-k);
      if (i != 0)
      {
         
         ByteSeq[i] <<= (k-2);
         ByteSeq[i] |= last;  
      }
      ByteSeq[i] &= 0x3F;
      ByteSeq[i] |= signSeq;
      last = next;
      
      printf("index: %d - %02X\n", i,ByteSeq[i]);

   }
   printf("oq tem no next final: %02X\n\n",next);
   
   if( next != 0x00 || ByteSeq[lenght] >= end ) { lenght += 1; }
   end = 0xF0 << (4 - (lenght+1));
   ByteSeq[lenght] = next | end;

   for (int j = lenght; j >= 0; j--)
   {
       printf("teste UTF -> index: %d - %02X\n", j,ByteSeq[j]);
   }
   
}





int varint_utf(FILE *arq_entrada, FILE *arq_saida){
   printf("\n------------------> inicio da função 2 <------------------\n");
   unsigned char byteCurrent;
   unsigned char byteSeq[4];
   int index = 0;
   
   for (byteCurrent = getc(arq_entrada); !feof(arq_entrada); byteCurrent = getc(arq_entrada))
   {
      printf("\n\nbyte da vez: %02X\n",byteCurrent);
      if(byteCurrent >= 0x80)
      {
         printf(" ----> maior que 128bits !!!\n");
         byteSeq[index] = byteCurrent & 0x7F;
         index++;
      }
      else if(index != 0)
      {
         byteSeq[index] = byteCurrent;
         printf(" -----> byte final do array\n");
         alignSeq(byteSeq, index);
         if(byteSeq[index] == 0x00) { index -= 1;}
         write_SeqUTF(byteSeq,index,arq_saida);
         index = 0;
      }
      else { fwrite(&byteCurrent,sizeof(unsigned char), 1, arq_saida); } 
   }
   return 0;
}
