/*
 * e2e_p11.h
 *
 *  Created on: 21.10.2020
 *      Author: till
 */

#ifndef E2E_E2E_P11_H_
#define E2E_E2E_P11_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "../include/types.h"
#include "e2e_utils.h"
#include "e2e_config.h"
#include "e2e.h"

e2e_error_state E2E_P11Config(struct e2e_config *config, uint32_t dataLength,
                              e2e_dataIdMode dataIdMode,
                              uint32_t dataIdNibbleOffset,
                              uint32_t counterOffset, uint32_t crcOffset,
                              uint32_t dataID, uint32_t maxDeltaCounter);
e2e_error_state E2E_P11Init(const struct e2e_config *config,
                            e2e_checkState *checkState,
                            e2e_protectState* protectState);
e2e_error_state E2E_P11Check(const struct e2e_config *config,
                             e2e_checkState* state, const uint8_t *data,
                             uint32_t length);
e2e_error_state E2E_P11CheckInit(e2e_checkState* state);
e2e_profile_status E2E_P11MapStatusToSM(e2e_error_state returnStatus,
                                        e2e_status status);
e2e_error_state E2E_P11Protect(const struct e2e_config *config,
                               e2e_protectState *state, uint8_t *data,
                               uint32_t length);
e2e_error_state E2E_P11ProtectInit(e2e_protectState* state);

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* E2E_E2E_P11_H_ */
