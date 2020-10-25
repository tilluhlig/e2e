/*
 * e2e_pw.h
 *
 *  Created on: 22.10.2020
 *      Author: till
 */

#ifndef E2E_E2E_PW_H_
#define E2E_E2E_PW_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "../include/types.h"
#include "e2e_config.h"
#include "e2e_sm.h"
#include "e2e.h"
#include "e2e_p11.h"

typedef enum
{
    E2EPW_STATUS_OK = 0x00,
    E2EPW_STATUS_NONEWDATA = 0x1,
    E2EPW_STATUS_WRONGCRC = 0x2,
    E2EPW_STATUS_SYNC = 0x3,
    E2EPW_STATUS_INITIAL = 0x4,
    E2EPW_STATUS_REPEATED = 0x8,
    E2EPW_STATUS_OKSOMELOST = 0x20,
    E2EPW_STATUS_WRONGSEQUENCE = 0x40,
} e2epw_status;

e2e_error_state E2EPW_Init(
        const struct e2e_config *config, e2e_checkState* checkState,
        e2e_protectState *protectState, e2e_sm_state *smState,
        rte_error_state (*callback_write)(uint8_t *data, uint32_t length),
        rte_error_state (*callback_read)(uint8_t *data, uint32_t *length));
uint32_t E2EPW_Read(const struct e2e_config *config, e2e_checkState* state,
                    e2e_sm_state *smState, uint8_t *data, uint32_t *length);
uint32_t E2EPW_Write(const struct e2e_config *config, e2e_protectState *state,
                     uint8_t *data, uint32_t length);

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* E2E_E2E_PW_H_ */
