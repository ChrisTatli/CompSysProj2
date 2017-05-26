#include "util.h"
uint8_t to_hex(uint8_t c1, uint8_t c2){
   uint8_t i = hex2bin(c1);
   uint8_t j = hex2bin(c2);
   return i<<4 | j;
}
/*
void to_hex(BYTE* src, BYTE* dst,int len){
   int n =0;
   for(int i=0;i<len;i+=2){
      dst[n] = (hex2bin(src[i])>>4) | hex2bin(src[i+1]);
      n++;
   }
}
*/

void from_hex(BYTE* src,BYTE* dst,int len){
   int n=0;
   uint8_t mask = 15;
   for(int i = 0; i<len;i++){
      dst[n] = bin2hex(src[i]>>4);
      dst[n+1] = bin2hex(src[i] & mask);
      n+=2;
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
