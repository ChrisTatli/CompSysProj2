#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "uint256.h"

uint8_t hex2bin(uint8_t c);
uint8_t to_hex(uint8_t c1, uint8_t c2);
uint8_t bin2hex(uint8_t c);

//converts a hex string to the same string but ascii
void from_hex(BYTE* src,BYTE* dst,int len);
