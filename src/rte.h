/*
 * rte.h
 *
 *  Created on: 26.10.2020
 *      Author: till
 */

#ifndef E2E_SRC_RTE_H_
#define E2E_SRC_RTE_H_

typedef enum
{
    RTE_E_OK = 0u,
    RTE_E_COM_STOPPED = 1u,
    RTE_E_SEG_FAULT = 2u,
} rte_error_state;

#endif /* E2E_SRC_RTE_H_ */
