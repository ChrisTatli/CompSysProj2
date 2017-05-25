//ctatli 640427
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include "sstp.h"
#include "log.h"


struct Message_s{
   uint8_t buffer[BUF_LEN];
   uint8_t* end;
};


uint8_t* parse_buffer(uint8_t* start, uint8_t* end);
void parse_input(uint8_t* src, uint8_t* message, header_t header);
header_t check_header(uint8_t *src);
void build_message(uint8_t* message,header_t head);
int parse_soln(uint8_t* src, soln_t* soln);




void receive_client(int fd){
   struct Message_s message;
   message.end = &message.buffer[0];
   uint8_t received = 0;
   for(;;){
      uint8_t n =  recv(fd,message.end,BUF_LEN,0);
      received += n;
      message.end += n;
      uint8_t *str;
      uint8_t sent[1024];
      if((str = parse_buffer(message.buffer, message.end)) != NULL){
         log_sstp(fd,str);

         header_t header = check_header(str);
         parse_input(str,sent,header);
         uint8_t m = strlen((char*)sent);
         send(fd,sent,m,0);
         log_sstp(fd,sent);
         memset(sent,'\0',BUF_LEN);
         message.end -= received;
         received = 0;
         free(str);
      }
   }
}

uint8_t* parse_buffer(uint8_t buffer[], uint8_t* end){
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

header_t check_header(uint8_t *src){
   header_t header;
   if(strncmp((char*)src,"PING",HEADER_LEN)==0){
      return header = PING;
   } else if(strncmp((char*)src, "PONG",HEADER_LEN)==0){
      return header = PONG;
   } else if(strncmp((char*)src, "OKAY",HEADER_LEN)==0){
      return header = OKAY;
   } else if(strncmp((char*)src,"ERRO",HEADER_LEN)==0) {
      return header = ERRO;
   } else if(strncmp((char*)src,"SOLN",HEADER_LEN)==0){
      return header = SOLN;
   }
   return header = MALF;
}


void parse_input(uint8_t* src, uint8_t* message, header_t head){
   if(head != MALF){
      if(head == SOLN){
         soln_t sol;
         if(parse_soln(src,&sol)){
            build_message(message,head);
            printf("%s\n",sol.seed );
         } else{
            build_message(message,MALF);
         }
      }
      else if(strlen((char*)src) != HEADER_LEN+DELIM){
         build_message(message,MALF);
      } else {
         build_message(message,head);
      }
   } else{
      build_message(message,head);
   }
}


void build_message(uint8_t* message, header_t head){
   if(head == PING){
      strcpy((char*)message,"PONG\r\n");
   } else if (head == PONG){
      strncpy((char*)message,"ERRO PONG is reserved for server\r\n",HEADER_LEN+ERROR_LEN);
   } else if (head == OKAY){
      strncpy((char*)message,"ERRO OKAY is reserved for server\r\n",HEADER_LEN+ERROR_LEN);
   } else if (head == ERRO){
      strncpy((char*)message,"ERRO ERRO is reserved for server\r\n",HEADER_LEN+ERROR_LEN);
   } else if (head == MALF){
      strncpy((char*)message,"ERRO Invalid message\r\n",HEADER_LEN+ERROR_LEN);
   } else if (head == SOLN){
      //temp
      strncpy((char*)message,"Chill'n\r\n",30);
   }
}


int parse_soln(uint8_t* src, soln_t* soln){
   if(strlen((char*)src)!=97){
      return 0;
   }
   char* token = strtok((char*)src," \r\n");
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
         return 0;
      }
   }
   return 1;
}
