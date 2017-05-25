#include "sha256.h"
#include "util.h"
#include "sstp.h"

int check_solution(char* difficulty, char* seed_cat_sol);
void convert_seed_nonce(char* src, BYTE* dst);
void create_x(char* src, uint64_t nonce, BYTE* dst);
