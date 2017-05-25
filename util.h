#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

uint8_t hex2bin(uint8_t c);
uint8_t to_hex(uint8_t c1, uint8_t c2);

uint8_t to_hex(uint8_t c1, uint8_t c2){
   uint8_t i = hex2bin(c1);
   uint8_t j = hex2bin(c2);
   return i<<4 | j;
}

uint8_t hex2bin(uint8_t c){
  if (c >= '0' && c <= '9') return      c - '0';
  if (c >= 'A' && c <= 'F') return 10 + c - 'A';
  if (c >= 'a' && c <= 'f') return 10 + c - 'a';
  return -1;
}
