//ctatli 640427
#include <stdint.h>

//Initialise a new file to log server interactions with
void log_init(char *filepath);
void log_connect(int socket);
void log_sstp_in(int socket,uint8_t *msg);
void log_sstp_out(int socket, uint8_t *msg);
void log_terminate();
