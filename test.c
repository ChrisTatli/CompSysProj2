#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct work_s{
   uint32_t diff;
   uint8_t seed[32];
   uint64_t start;
   uint8_t count;
}work_t;

typedef unsigned char BYTE;
uint8_t hex2bin(uint8_t c);
void to_hex(BYTE* src, BYTE* dst,int len);


int parse_work(char* src,work_t* work);

int main(int argc, char const *argv[]) {
   char str[] = "WORK 1fffffff 0000000019d6689c085ae165831e934ff763ae46a218a6c172b3f1b60a8ce26f 1000000023212000 01\r\n";
   //100
   work_t work;

   parse_work(str, &work);
   printf("%lu\n", work.diff );

   return 0;
}

int parse_work(char* src,work_t* work){
   if(strlen(src) != 100){
      return 0;
   } else if(*(src+4) != ' ' || *(src+13) != ' ' || *(src+78) !=' ' || *(src+95) != ' '){
      return 0;
   } else{
      to_hex(&src[5], work->diff,8);
   }
}

void to_hex(BYTE* src, BYTE* dst,int len){
   int n =0;
   for(int i=0;i<len;i+=2){
      printf("%d\n", i);
      uint8_t c1 = hex2bin(src[i]);
      printf("%c\n",c1 );
      uint8_t c2 = hex2bin(src[i+1]);
      printf("%c\n",c2 );
      printf("%u\n",c1<<4|c2 );
      dst[n]= (c1<<4|c2);
      printf("%u\n" ,dst[n]);
      n++;
   }
}

uint8_t hex2bin(uint8_t c){
  if (c >= '0' && c <= '9') return      c - '0';
  if (c >= 'A' && c <= 'F') return 10 + c - 'A';
  if (c >= 'a' && c <= 'f') return 10 + c - 'a';
  return -1;
}

uint8_t bin2hex(uint8_t c){
   char table[16]="0123456789abcdef";
   //printf("%c\n",table[c] );
   return table[c];
}
