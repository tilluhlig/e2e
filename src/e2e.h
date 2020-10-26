/*
 * e2e.h
 *
 *  Created on: 21.10.2020
 *      Author: till
 */

#ifndef E2E_E2E_H_
#define E2E_E2E_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"
#include "rte.h"
#include "e2e_config.h"
#include "e2e_sm.h"
#include "e2e_pw.h"

e2e_error_state E2E_check(const struct e2e_config *config,
                          e2e_sm_state *smachineState, e2e_checkState* checkstate,
                          uint8_t *data, uint32_t length);
e2e_error_state E2E_protect(const struct e2e_config *config,
                            e2e_protectState *protectstate, uint8_t *data,
                            uint32_t length);
#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* E2E_E2E_H_ */
