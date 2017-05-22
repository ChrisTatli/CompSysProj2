// ctatli 640427

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <pthread.h>

#include "log.h"
#include "sstp.h"

#define CONNECT_QUEUE 100


typedef struct{
   char *port;
}options_t;

/* Function Declarations */
options_t get_options(int argc, char **argv);
void print_usage(char *prog_name);
int init_server(char *port);
void block_sig();
void *handle_sig(void* n);
void end_server(int signum);
void *handle_client(int n);

/****** MAIN ******/
int main(int argc, char **argv){

   //Load options in from the command line
   options_t options = get_options(argc, argv);

   //Initialise our log file
   log_init("log.txt");

   //Initialise our server
   int servfd = init_server(options.port);

   pthread_t sighandler;
   pthread_create(&sighandler,NULL,handle_sig,NULL);
   block_sig();

   // Main Server Loop
   for(;;){
      struct sockaddr_storage client_addr;
      socklen_t sin_size = sizeof client_addr;
      int client_fd = accept(servfd, (struct sockaddr *) &client_addr, &sin_size);
      if(client_fd == -1){
         fprintf(stderr, "Error accepting new client\n");
         continue;
      }
      log_connect(client_fd);

      pthread_t client_worker;
      pthread_create(&client_worker, NULL, handle_client, client_fd);
   }

   return 0;
}


/*
** Starts our server listening in on a specified port, returns a file descriptor
*/
int init_server(char *port){

   //Stuff for getaddrinfo
   struct addrinfo hints;
   struct addrinfo *server_info;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE; // Fill in IP automatically

   int status = getaddrinfo(NULL,port,&hints,&server_info);
   if(status != 0){
      fprintf(stderr, "error finding port %s\n",port );
      fprintf(stderr, "%s\n", gai_strerror(status));
      exit(EXIT_FAILURE);
   }

   //Open a new socket
   int file_des =  socket(server_info->ai_family,server_info->ai_socktype,0);
   if(file_des < 0){
      fprintf(stderr, "error creating socket\n");
      exit(EXIT_FAILURE);
   }

   //Associate the new socket with a port
   int binded = bind(file_des,server_info->ai_addr,server_info->ai_addrlen);
   if(binded < 0){
      fprintf(stderr, "error binding to port\n");
      exit(EXIT_FAILURE);
   }

   //Listen on a the specified port
   int listening = listen(file_des, CONNECT_QUEUE);
   if(listening < 0){
      fprintf(stderr, "error listening on port\n");
      exit(EXIT_FAILURE);
   }
   freeaddrinfo(server_info);

   return file_des;
}

void *handle_client(int fd){
   receive_client(fd);
   close(fd);
}

void *handle_sig(void* n){
   struct sigaction action;
   action.sa_handler = end_server;
   sigemptyset(&action.sa_mask);
   action.sa_flags = 0;

   sigaction(SIGINT, &action, NULL);
   sigaction(SIGTERM, &action, NULL);

   sigset_t ss;
   sigemptyset(&ss);
   while(1){
      sigsuspend(&ss);
   }
}

void block_sig(){
   sigset_t ss;
   sigemptyset(&ss);
   sigaddset(&ss,SIGINT);
   sigaddset(&ss,SIGTERM);
   pthread_sigmask(SIG_BLOCK, &ss, NULL);
}

void end_server(int signum){
   printf("\nServer terminated\n");
   log_terminate();
   exit(EXIT_SUCCESS);
}


void print_usage(char *prog_name){
   fprintf(stderr, "usage: %s [port_number]\n", prog_name);
   fprintf(stderr, "\tport_number: a valid port number to listen on\n");
   exit(EXIT_FAILURE);
}

options_t get_options(int argc, char **argv){
   options_t options;

   if(argc < 2){
      print_usage(argv[0]);
   } else{
      options.port = argv[1];
   }

   return options;
}
