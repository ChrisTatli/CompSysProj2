#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "log.h"

#define TIME_BUFFER 26

FILE *log_f = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* Fucntion Declarations*/
void log_datetime();


/*
** Initialise a new file to log server and client interactions
*/
void init_log(char *filepath){

   pthread_mutex_lock(&lock);

   // If we dont have an open log file open one
   if(log_f == NULL){
      FILE *file = fopen(filepath,"w");
      if(file == NULL){
         fprintf(stderr, "Error opening file %s\n", filepath);
      } else {
         log_f = file;
         log_datetime();
         fprintf(log_f, " Server Initialised" );
      }
   } else {
      fprintf(stderr, "File %s is already open\n",filepath);
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
