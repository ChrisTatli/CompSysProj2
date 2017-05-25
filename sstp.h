#include "uint256.h"


// SSTP Protocol Header and Payload lengths
#define HEADER_LEN 4
#define DIFF_LEN 8
#define ERROR_LEN 40
#define SEED_LEN 64
#define DELIM 2
#define BUF_LEN 1024
#define SOLN_LEN 16
#define WORK_LEN 2

typedef enum header_e{
   PING,
   PONG,
   OKAY,
   ERRO,
   SOLN,
   WRONG,
   CORRECT,
   MALF
}header_t;

//Stores the components of the solution message separately as nul terminated strings
typedef struct soln_s{
   char head[HEADER_LEN+1];
   char diff[DIFF_LEN+1];
   char seed[SEED_LEN+SOLN_LEN+1];
   char sol[SOLN_LEN];
}soln_t;

typedef struct work_s{
   char head[HEADER_LEN+1];
   char diff[DIFF_LEN+1];
   char seed[SEED_LEN+SOLN_LEN+1];
   char sol[SOLN_LEN+1];
   char count[WORK_LEN];
}work_t;

void receive_client(int fd);
int parse_soln(uint8_t* src,soln_t* soln);
int parse_work(uint8_t* src, work_t* work);
