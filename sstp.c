//ctatli 640427
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <pthread.h>
#include "sstp.h"
#include "log.h"
#include "hash.h"


struct Message_s{
   uint8_t buffer[BUF_LEN];
   uint8_t* end;
};


uint8_t* parse_buffer(uint8_t* start, uint8_t* end);
void parse_input(uint8_t* src, uint8_t* message, header_t header);
header_t check_header(uint8_t *src);
void build_message(uint8_t* message,header_t head);
void to_uint256(BYTE* src, BYTE* dst);
void create_data(work_t* work, data_t* data);
void init_data(data_t* data);
void *do_work(data_t* data);
BYTE* solve_work(data_t *data);

void receive_client(int fd){
   struct Message_s message;
   message.end = &message.buffer[0];
   uint8_t received = 0;
   int n;
   while((n =  recv(fd,message.end,BUF_LEN,0))){

      received += n;
      message.end += n;
      uint8_t *str;
      uint8_t sent[1024];
      if((str = parse_buffer(message.buffer, message.end)) != NULL){
         log_sstp_in(fd,str);

         header_t header = check_header(str);
         parse_input(str,sent,header);
         uint8_t m = strlen((char*)sent);
         send(fd,sent,m,0);
         log_sstp_out(fd,sent);
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
   } else if(strncmp((char*)src,"WORK",HEADER_LEN)==0){
      return header = WORK;
   }
   return header = MALF;
}


void parse_input(uint8_t* src, uint8_t* message, header_t head){
   if(head != MALF){
      if(head == SOLN){
         soln_t sol;
         if(parse_soln(src,&sol)){
            char* full = strcat(sol.seed,sol.sol);
            BYTE seednonce[40];
            convert_seed_nonce(full,seednonce);
            if(check_solution(sol.diff,seednonce)){
               build_message(message,CORRECT);
            } else {
               build_message(message,WRONG);
            }

         } else{
            build_message(message,MALF);
         }
      } else if(strlen((char*)src) != HEADER_LEN+DELIM){
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
      strncpy((char*)message,"ERRO PONG is reserved for server      \r\n",HEADER_LEN+ERROR_LEN);
   } else if (head == OKAY){
      strncpy((char*)message,"ERRO OKAY is reserved for server      \r\n",HEADER_LEN+ERROR_LEN);
   } else if (head == ERRO){
      strncpy((char*)message,"ERRO ERRO is reserved for server      \r\n",HEADER_LEN+ERROR_LEN);
   } else if (head == MALF){
      strncpy((char*)message,"ERRO Invalid message                  \r\n",HEADER_LEN+ERROR_LEN);
   } else if(head ==CORRECT){
      strcpy((char*)message,"OKAY\r\n");
   }else if (head == WRONG){
      strncpy((char*)message,"ERRO SOLN was wrong                   \r\n",HEADER_LEN+ERROR_LEN);
   }
}

//returns 1 on success, 0 on failure
int parse_soln(uint8_t* src, soln_t* soln){
   if(strlen((char*)src)!=97){
      return 0;
   }
   char* token = strtok((char*)src," \r\n");
   while (token != NULL) {
      if(strlen(token) == 4){
         strncpy(soln->head,token,sizeof(soln->head));
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 8){
         strncpy(soln->diff,token,sizeof(soln->diff));
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 64){
         strncpy(soln->seed,token,sizeof(soln->seed));
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 16){
         strncpy(soln->sol,token,sizeof(soln->sol));
         token = strtok(NULL," \r\n");
      }
      else{
         return 0;
      }
   }
   return 1;
}

int parse_work(uint8_t* src, work_t* work){
   if(strlen((char*)src)!=100){
      return 0;
   }
   char* token = strtok((char*)src," \r\n");
   while (token != NULL) {
      if(strlen(token) == 4){
         strncpy(work->head,token,sizeof(work->head));
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 8){
         strncpy(work->diff,token,sizeof(work->diff));
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 64){
         strncpy(work->seed,token,sizeof(work->seed));
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 16){
         strncpy(work->sol,token,sizeof(work->sol));
         token = strtok(NULL," \r\n");
      } else if(strlen(token) == 2){
         strncpy(work->count,token,sizeof(work->count));
         token = strtok(NULL," \r\n");
      }
      else{
         return 0;
      }
   }
   return 1;
}

void to_uint256(BYTE* src, BYTE* dst){
   int i,j;
   for(i = strlen(src)-2, j=0;i>=0;i-=2,j++){
      char temp[3];
      temp[0] = src[i];
      temp[1] = src[i+1];
      temp[2] = '\0';
      dst[31-j] = strtoul(temp,NULL,16);
   }
}

void create_data(work_t* work, data_t* data){
   strcpy(data->difficulty,work->diff);
   to_uint256(work->seed, data->seed);
   to_uint256(work->sol,data->start);
}

void init_data(data_t* data){
   uint256_init(data->seed);
   uint256_init(data->start);
}

void *do_work(data_t* data){
   print_uint256(data->seed,32);
   solve_work(data);
}

BYTE* solve_work(data_t *data){
   BYTE one[32];
   uint256_init(one);
   one[31] = 0x1;
   print_uint256(data->seed,32);
   BYTE x_value[40];
   uint256_init(x_value);
   for(int i=39;i>=0;i--){
      if(i>31){
         x_value[i]=data->start[i-8];
      }else{
         x_value[i]=data->seed[i];
      }
   }



   return NULL;

}
