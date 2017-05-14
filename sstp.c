//ctatli 640427
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "unint256.h"

// SSTP Protocol Header and Payload lengths
#define HEADER_LEN 4
#define ERROR_LEN 40
#define SEED_LEN 64
#define DELIM 2
#define BUF_LEN 1024

/* Function Declarations */
int receive_length(int socket, BYTE *buffer, unint16_t len);


/*
** socket: fd to get byte stream from
** buffer: store the message in a buffer
** len: number of bytes to read in
** returns the amount of bytes read in
*/
int receive_length(int socket, BYTE *buffer, uint16_t len){
   BYTE *current = buffer;
   uint16_t remaining = len;
   while (remaining > 0) {
      unint16_t num_bytes = recv(socket,current,remaining,0);
      //Something went wrong
      if (num_bytes < 0) {
         return num_bytes;
      } else {
         //Advance our point in the buffer by the number of bytes read in
         current += num_bytes;
         remaining -= num_bytes;
      }
   }
   return len;
}

int receive_message(int socket, BYTE **buffer){
   BYTE* message_buf;

   int n = receive_length(socket,message_buf,BUFF_LEN);
   payload_len = handle_header(message);

}
