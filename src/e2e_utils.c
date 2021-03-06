/*
 * e2e_utils.c
 *
 *  Created on: 25.10.2020
 *      Author: till
 */

#include "e2e_utils.h"

uint8_t update_crc8_sae_j1850(unsigned char inpcrc, const uint8_t data)
{
    unsigned long crc;
    int i, bit;

    crc = inpcrc;
    crc ^= data;
    for (bit = 0u; bit < 8; bit++)
    {
        if ((crc & 0x80) != 0u)
        {
            crc <<= 1u;
            crc ^= 0x1D;
        }
        else
        {
            crc <<= 1;
        }
    }

    return (uint8_t) (~crc) & 0xFF;
}

uint8_t crc8_sae_j1850(const uint8_t *data, uint8_t length)
{
    unsigned long crc;
    uint8_t i, bit;

    crc = 0xFF;
    for (i = 0u; i < length; i++)
    {
        crc ^= data[i];
        for (bit = 0u; bit < 8; bit++)
        {
            if ((crc & 0x80) != 0u)
            {
                crc <<= 1u;
                crc ^= 0x1D;
            }
            else
            {
                crc <<= 1u;
            }
        }
    }

    return (~crc) & 0xFF;
}
