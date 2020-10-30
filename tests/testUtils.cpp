
#include "testUtils.h"
#include <stdio.h>
#include <string.h>

uint8_t testdata[8];
uint8_t *getTestData() {
	return testdata;
}

uint8_t *outTestDataA() {
	uint8_t temp[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 };
	memcpy(testdata, temp, sizeof(testdata));
	return testdata;
}

uint8_t *inTestDataA(uint8_t crc, uint8_t counter) {
	uint8_t temp[8] = { crc,(uint8_t)(counter & 0x0Fu),0x00,0x00,0x00,0x00,0x00,0x00 };
	memcpy(testdata, temp, sizeof(testdata));
	return testdata;
}

uint8_t *outTestDataB() {
	uint8_t temp[8] = { 0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
	memcpy(testdata, temp, sizeof(testdata));
	return testdata;
}

uint8_t *inTestDataB(uint8_t crc, uint8_t counter) {
	uint8_t temp[8] = { crc,(uint8_t)(counter & 0x0Fu),0xFF,0xFF,0xFF,0xFF,0xFF,0xFF };
	memcpy(testdata, temp, sizeof(testdata));
	return testdata;
}

uint8_t *outTestDataC() {
	uint8_t temp[8] = { 0x00,0x10,0x02,0x03,0x04,0x05,0x06,0x07 };
	memcpy(testdata, temp, sizeof(testdata));
	return testdata;
}

uint8_t *inTestDataC(uint8_t crc, uint8_t counter) {
	uint8_t temp[8] = { crc,(uint8_t)(counter & 0x0Fu),0x02,0x03,0x04,0x05,0x06,0x07 };
	memcpy(testdata, temp, sizeof(testdata));
	return testdata;
}

uint8_t *outTestDataD() {
	uint8_t temp[8] = { 0xFF,0xF0,0x00,0x00,0x00,0x00,0x00,0x00 };
	memcpy(testdata, temp, sizeof(testdata));
	return testdata;
}