/*
 * e2e_utils.h
 *
 *  Created on: 25.10.2020
 *      Author: till
 */

#ifndef E2E_E2E_UTILS_H_
#define E2E_E2E_UTILS_H_

#include "../include/types.h"

uint8_t update_crc8_sae_j1850(unsigned char inpcrc, const uint8_t data);
uint8_t crc8_sae_j1850(const uint8_t *data, uint8_t length);

#endif /* E2E_E2E_UTILS_H_ */
