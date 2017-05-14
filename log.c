#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#include "log.h"

FILE *log_f = NULL;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;



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
         fprintf(log_f, "Server Initialised" );
      }
   } else {
      fprintf(stderr, "File %s is already open\n",filepath);
   }
   pthread_mutex_unlock(&lock);
}
