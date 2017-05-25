#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "hash.h"

uint32_t convert_diff(char* src);
void convert_solution(char* src, BYTE* dst);
uint32_t get_alpha(uint32_t src);
void get_beta(uint32_t src, BYTE* res);

uint32_t convert_diff(char* src){
   char* p;
   return (uint32_t)strtoul(src,&p,16);
}

void convert_solution(char* src, BYTE* dst){
   int n=0;
   for(int i = 0;i<8;i++){
      dst[i] = to_hex(src[n],src[n+1]);
      n+=2;
   }
}

void convert_seed_nonce(char* src, BYTE* dst){
   int n=0;
   for(int i = 0;i<40;i++){
      dst[i] = to_hex(src[n],src[n+1]);
      n+=2;
   }
}

uint32_t get_alpha(uint32_t src){
   return src>>=24;
}

void get_beta(uint32_t src, BYTE* res){
   src <<= 8;
   src >>=8;
   memcpy(&res[31],&src,1);
   src>>=8;
   memcpy(&res[30],&src,1);
   src>>=8;
   memcpy(&res[29],&src,1);
   src>>=8;
   memcpy(&res[28],&src,1);
}


int check_solution(char* difficulty, char* seed_cat_sol){
   uint32_t diff = convert_diff(difficulty);

   uint32_t alpha = get_alpha(diff);
   BYTE beta[32];
   uint256_init(beta);
   get_beta(diff,beta);

   BYTE targ[32], temp[32], base[32];
   uint256_init(targ);
   uint256_init(temp);
   uint256_init(base);
   base[31] = 0x2;

   uint256_exp(temp,base,8*(alpha-3));
   uint256_mul(targ,beta,temp);

   BYTE hash1[32];
   BYTE hash2[32];
   uint256_init(hash1);
   uint256_init(hash2);

   SHA256_CTX ctx;
   sha256_init(&ctx);
   sha256_update(&ctx,seed_cat_sol,40);
   sha256_final(&ctx,hash1);

   sha256_init(&ctx);
   sha256_update(&ctx,hash1,32);
   sha256_final(&ctx,hash2);

   if(sha256_compare(hash2,targ)==-1){
      return 1;
   } else{
      return 0;
   }
}
