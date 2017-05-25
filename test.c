#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sstp.h"
#include "uint256.h"
#include "util.h"
#include "sha256.h"

uint32_t convert_diff(char* src);
uint64_t convert_solution(char* src);
void convert_seed_nonce(char* src, BYTE* dst);
int parse_soln(uint8_t* src,soln_t* soln);
uint32_t get_alpha(uint32_t src);
void get_beta(uint32_t src, BYTE* res);

int main(int argc, char const *argv[]) {
   unsigned char str[] = "SOLN 1fffffff 0000000019d6689c085ae165831e934ff763ae46a218a6c172b3f1b60a8ce26f 1000000023212147\r\n";
   soln_t soln;
   int n = parse_soln(str, &soln);
   uint32_t diff = convert_diff(soln.diff);
   uint32_t alpha;
   alpha = get_alpha(diff);
   BYTE beta[32];
   uint256_init(beta);
   get_beta(diff,beta);

   char* full = strcat(soln.seed,soln.sol);
   BYTE seednonce[40];
   convert_seed_nonce(full,seednonce);

   BYTE targ[32];
   uint256_init(targ);
   BYTE temp[32];
   uint256_init(temp);

   BYTE base[32];
   uint256_init(base);
   base[31] = 0x2;
   uint32_t n_a = 8*(alpha-3);

   uint256_exp(temp,base, n_a);

   BYTE beta256[32];
   uint256_init(beta256);

   uint256_mul(targ,beta,temp);


   BYTE hash1[32];
   BYTE hash2[32];
   uint256_init(hash1);
   uint256_init(hash2);

   SHA256_CTX ctx;
   sha256_init(&ctx);
   sha256_update(&ctx,seednonce,40);
   sha256_final(&ctx,hash1);

   sha256_init(&ctx);
   sha256_update(&ctx,hash1,32);
   sha256_final(&ctx,hash2);

   if(sha256_compare(hash2,targ)==-1){
      printf("OKAY\n" );
   } else{
      printf("WRONG\n" );
   }



   return 0;
}


int parse_soln(uint8_t* src, soln_t* soln){
   char* token = strtok(src," \r\n");
   while (token != NULL) {
      if(strlen(token) == 4){
         strncpy(soln->head,token,4);
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 8){
         strncpy(soln->diff,token,8);
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 64){
         strncpy(soln->seed,token,64);
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 16){
         strncpy(soln->sol,token,16);
         token = strtok(NULL," \r\n");
      }
      else{
         return -1;
      }
   }
   return 0;
}

uint32_t convert_diff(char* src){
   char* p;
   return (uint32_t)strtoul(src,&p,16);
}

uint64_t convert_solution(char* src){
   char* p;
   return (uint64_t)strtoull(src, &p,16);
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
