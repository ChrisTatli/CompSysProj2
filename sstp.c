//ctatli 640427
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include "sstp.h"


// SSTP Protocol Header and Payload lengths
#define HEADER_LEN 4
#define ERROR_LEN 40
#define SEED_LEN 64
#define DELIM 2
#define BUF_LEN 1024

uint8_t* parse_buffer(uint8_t* start, uint8_t* end);
void parse_input(uint8_t* src);
void check_header(uint8_t *src);

struct Message_s{
   uint8_t buffer[BUF_LEN];
   uint8_t* end;
};

void receive_client(int fd){
   struct Message_s message;
   memset(message.buffer,'\0',BUF_LEN);
   message.end = &message.buffer[0];
   uint8_t received = 0;
   for(;;){
      uint8_t n =  recv(fd,message.end,BUF_LEN,0);
      received += n;
      message.end += n;
      uint8_t *str;
      if((str = parse_buffer(message.buffer, message.end)) != NULL){
         parse_input(str);
         message.end -= received;
         received = 0;
         //Gotta Free
         free(str);
      }
   }
}


uint8_t* parse_buffer(uint8_t buffer[], uint8_t* end){
   printf("%p %p\n", &buffer[0], end);
   int i = 0;
   uint8_t* ret = malloc(sizeof(uint8_t)*BUF_LEN);
   while (&buffer[i] != end) {
      if(buffer[i] == '\r' && buffer[i+1] == '\n'){
         //Do i need to cast these?
         strncpy((char*)ret,(char*)buffer, BUF_LEN);
         //Keep eye on this
         memmove(&buffer[0], end, BUF_LEN - (i + 2));

         return ret;
      } else {
         i++;
      }
   }
   return NULL;
}

void parse_input(uint8_t* src){
   check_header(src);
}

void check_header(uint8_t *src){

   if(strncmp((char*)src,"PING",HEADER_LEN)==0){
      printf("PONG\n");
   } else if (strncmp((char*)src,"ERRO",HEADER_LEN)==0) {
      printf("ERRO\n" );
   }
}
