/*
 * e2e_sm.h
 *
 *  Created on: 21.10.2020
 *      Author: till
 */

#ifndef E2E_E2E_SM_H_
#define E2E_E2E_SM_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "rte.h"
#include "e2e_config.h"

typedef enum
{
    E2E_SM_VALID = 0u,
    E2E_SM_DEINIT = 1u,
    E2E_SM_NODATA = 2u,
    E2E_SM_INIT = 3u,
    E2E_SM_INVALID = 4u,
} e2e_sm;

typedef struct
{
    e2e_sm smState;
    uint32_t okCount;
    uint32_t errorCount;
    uint32_t windowTopIndex;
    e2e_profile_status profileStatusWindow[16];
} e2e_sm_state;

e2e_error_state E2E_SMClearStatus(const struct e2e_config *config,
                                  e2e_sm_state *state);
e2e_error_state E2E_SMCheckInit(const struct e2e_config *config,
                                e2e_sm_state *state);
e2e_error_state E2E_SMAddStatus(e2e_profile_status profileStatus,
                                const struct e2e_config *config,
                                e2e_sm_state *state);
e2e_error_state E2E_SMCheck(e2e_profile_status profileStatus,
                            const struct e2e_config *config,
                            e2e_sm_state *state);
e2e_error_state E2E_SMInit(e2e_sm_state *state,
                           const struct e2e_config *config);
e2e_error_state E2E_SMConfig(struct e2e_config *config, uint32_t new_windowSize,
                             uint32_t new_maxErrorStateInit,
                             uint32_t new_maxErrorStateInvalid,
                             uint32_t new_maxErrorStateValid,
                             uint32_t new_minOkStateInit,
                             uint32_t new_minOkStateInvalid,
                             uint32_t new_minOkStateValid);

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* E2E_E2E_SM_H_ */
