#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "sstp.h"

uint64_t deserialize_uint64(unsigned char *buf);
unsigned char *serialize_uint64(unsigned char *buffer, uint64_t *value);

int main(int argc, char const *argv[]) {
   char str[] = "WORK 1fffffff 0000000019d6689c085ae165831e934ff763ae46a218a6c172b3f1b60a8ce26f 1000000023212000 01\r\n";
   //100
   work_t work;


   if(parse_work(str,&work)){
      printf("great\n");
   }else{
      printf("Bad\n");
   }
   printf("%s\n",work.seed );
   printf("%s\n",work.sol );
   char *p;
   uint64_t solution = strtoull(work.sol,&p,16);
   printf("%llu\n",solution );



   //convert_seed_nonce();

   return 0;
}

unsigned char *serialize_uint64(unsigned char *buffer, uint64_t *value) {
    *((uint64_t *) buffer) = htonl(*value);
    return buffer;
}
