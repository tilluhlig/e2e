#ifndef TESTUTILS_H_
#define TESTUTILS_H_

#include "../src/types.h"

uint8_t *outTestDataA();
uint8_t *inTestDataA(uint8_t crc, uint8_t counter);
uint8_t *outTestDataB();
uint8_t *inTestDataB(uint8_t crc, uint8_t counter);
uint8_t *outTestDataC();
uint8_t *inTestDataC(uint8_t crc, uint8_t counter);
uint8_t *outTestDataD();
uint8_t *getTestData();

#endif