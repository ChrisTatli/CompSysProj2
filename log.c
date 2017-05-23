//ctatli 640427
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "log.h"

#define TIME_BUFFER 26

FILE *log_f = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* Fucntion Declarations*/
void log_datetime();
void log_client_ip(int socket);
void log_sstp(int socket);


/*
** Initialise a new file to log server and client interactions
*/
void log_init(char *filepath){

   pthread_mutex_lock(&lock);

   // If we dont have an open log file open one
   if(log_f == NULL){
      FILE *file = fopen(filepath,"w");
      if(file == NULL){
         fprintf(stderr, "Error opening file %s\n", filepath);
      } else {
         log_f = file;
         log_datetime();
         fprintf(log_f, " Server Initialised\n");
      }
   } else {
      fprintf(stderr, "File %s is already open\n",filepath);
   }
   pthread_mutex_unlock(&lock);
}

void log_terminate(){
   pthread_mutex_lock(&lock);
   if(log_f == NULL){
      printf("error with logfile\n");
   } else {
      log_datetime();
      fprintf(log_f, " (0.0.0.0) Closing Log File\n");
      log_datetime();
      fprintf(log_f, " (0.0.0.0) Server Terminated\n" );
      fclose(log_f);
   }
   pthread_mutex_unlock(&lock);
}

/*
** Logs the date and time that a client/server action takes place
*/
void log_datetime(){
   // Based on code from : http://bit.ly/2qiYlIO
   time_t datetime;
   char buffer[TIME_BUFFER];
   struct tm* tm_info;

   time(&datetime);
   tm_info = localtime(&datetime);
   strftime(buffer,TIME_BUFFER,"%Y-%m-%d %H:%M:%S",tm_info);
   fprintf(log_f, "[%s]",buffer);
}

void log_client_ip(int socket){
   struct sockaddr_in cli;
   socklen_t cli_len = sizeof cli;
   if(getpeername(socket,(struct sockaddr*) &cli, &cli_len) == 0){
      char ip4[INET_ADDRSTRLEN];
      if(inet_ntop(AF_INET,&cli,ip4,INET_ADDRSTRLEN)){
         fprintf(log_f, " (%s) New client connected with socket id %d\n", ip4,socket);
      }else{
         fprintf(stderr, " Ip invalid\n");
         fprintf(log_f, "Error with ip address of client\n");
      }
   } else {
      fprintf(log_f, "Error with ip address of client\n" );
   }
}

void log_connect(int socket){
   pthread_mutex_lock(&lock);
   if(log_f){
      log_datetime();
      log_client_ip(socket);
   }
   pthread_mutex_unlock(&lock);
}


void log_sstp(int socket){

}
