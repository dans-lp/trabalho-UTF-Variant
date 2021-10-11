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

//função converte sequencia de bytes em Hex para Varint e escreve no arquivo de saída
int write_SeqVarint(unsigned char *ByteSeq, int lenght, FILE* arq_saida);

//função alinha os bits mais significantes no array de bytes advintos de varint
void alignSeq(unsigned char *ByteSeq,int lenght);

//função converte array de bytes em Hex para UTF-8 e escreve no arquivo de saída
int write_SeqUTF(unsigned char *ByteSeq, int lenght, FILE* arq_saida);

//---------------------------------------------->main<-------------------------------------------------

int main(void){
   int res = 0;

   FILE *fin1 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/Unicode_pequeno.txt","rb");
   FILE *fin2 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/var_peq","rb");
   FILE *fin3 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/Unicode_medio.txt","rb");
   FILE *fin4 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/var_medio.txt","rb");
   
   FILE *fout1 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/saida_teste1.txt","wb");
   FILE *fout2 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/saida_teste2.txt","wb");
   FILE *fout3 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/saida_teste3.txt","wb");
   FILE *fout4 = openFile("/mnt/d/Trabalhos PUC/Software Basico/TrabalhoUTF-Variant/saida_teste4.txt","wb");
   
   
   

   res = utf_varint(fin1,fout1);
   printf("\n ---------------> fim utf_varint: %d <--------------- \n",res);
   
   res = varint_utf(fin2,fout2);
   printf("\n ---------------> fim varint_utf: %d <--------------- \n",res);

   res = utf_varint(fin3,fout3);
   printf("\n ---------------> fim utf_varint grande: %d <--------------- \n",res);

   res = varint_utf(fin4,fout4);
   printf("\n ---------------> fim varint_utf grande: %d <--------------- \n",res);

   return 0;
}

//----------------------------------------------------------------------------------------------------

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

int write_SeqVarint(unsigned char *ByteSeq, int lenght, FILE* arq_saida){
   int finalIndex = lenght - 1;
   unsigned char msb1 = ByteSeq[finalIndex] >> 7;
   unsigned char msb2; 
   unsigned char flag = 0x80;

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
      //fwrite(&ByteSeq[i],sizeof(unsigned char), 1, arq_saida);
      if(fwrite(&ByteSeq[i],sizeof(unsigned char), 1, arq_saida) != 1)
      {
         fprintf(stderr, "erro durante a escrita no arquivo de saída\n");
         return -1;
      } 
   }
   return 0;
}


int utf_varint(FILE *arq_entrada, FILE *arq_saida){
   
   unsigned char byteCurrent;
   unsigned char byteSeq[4];
   unsigned char byteBefore = 0x00;
   int SeqLenght;
   int index = 0;
   int res;
   
   for (byteCurrent = getc(arq_entrada); !feof(arq_entrada); byteCurrent = getc(arq_entrada))
   {
      if (byteCurrent >= 0x80)
      {
         int i = 0;
         while (byteCurrent >= 0x80)
         {
            byteCurrent <<= 1;
            i++;
         }
   
         byteCurrent >>= i;
         byteCurrent |= (byteBefore << 6); 
         byteBefore = (byteCurrent & 0x03);
         
         if (i>1) { SeqLenght = i; }

         byteSeq[index] = (byteCurrent);
         
         if (index == SeqLenght - 1)
         {
            byteBefore = 0x00;
            res = write_SeqVarint(byteSeq,SeqLenght,arq_saida);
            if (res!= 0){ return res; }
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
   
   if(ferror(arq_entrada)) 
   {
      fprintf(stderr, "Erro ao ler arquivo de entrada\n");
      return -1;
   }
   return 0;

   fclose(arq_entrada);
   fclose(arq_saida);

}

void alignSeq(unsigned char *ByteSeq,int lenght){
   unsigned char lsb;
   unsigned char msb = 0x00;
   
   for (int i = lenght; i >= 0; i--)
   {  
      lsb = ByteSeq[i] & 0x01;

      if(i != 0){ ByteSeq[i] >>= 1; } 
      
      ByteSeq[i] |= msb; 
      msb = (lsb<<7);
   }
}

int write_SeqUTF(unsigned char *ByteSeq, int lenght, FILE* arq_saida){
   unsigned char next;
   unsigned char signSeq = 0x80;
   unsigned char last = 0x00;
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
   }
   
   if( next != 0x00 || ByteSeq[lenght] >= end ) 
   { 
      lenght += 1; 
      if (lenght >= 5)
      {
         fprintf(stderr,"tamanho da sequência de bytes incompatível para varint\n");
         return -1;
      }
   }
   
   end = 0xF0 << (4 - (lenght+1));

   ByteSeq[lenght] = next | end;

   for (int j = lenght; j >= 0; j--)
   {
      if(fwrite(&ByteSeq[j],sizeof(unsigned char), 1, arq_saida) != 1)
      {
         fprintf(stderr, "erro durante a escrita no arquivo de saída\n");
         return -1;
      } 
   }
   return 0;
}

int varint_utf(FILE *arq_entrada, FILE *arq_saida){

   unsigned char byteCurrent;
   unsigned char byteSeq[4];
   int index = 0;
   int res;
   
   for (byteCurrent = getc(arq_entrada); !feof(arq_entrada); byteCurrent = getc(arq_entrada))
   {
      if(byteCurrent >= 0x80)
      {
         byteSeq[index] = byteCurrent & 0x7F;
         index++;
      }
      else if(index != 0)
      {
         byteSeq[index] = byteCurrent;
         alignSeq(byteSeq, index);
         if(byteSeq[index] == 0x00) { index -= 1;}
         res = write_SeqUTF(byteSeq,index,arq_saida);
         if (res!= 0){ return res; }
         index = 0;
      }
      else { fwrite(&byteCurrent,sizeof(unsigned char), 1, arq_saida); } 
   }

   if(ferror(arq_entrada)) 
   {
      fprintf(stderr, "\nErro ao ler arquivo de entrada");
      return -1;
   }

   fclose(arq_entrada);
   fclose(arq_saida);

   return 0;
}
