/*
 * e2e.c
 *
 *  Created on: 21.10.2020
 *      Author: till
 */

#include "e2e.h"

/*
 * @brief a generic check function that is internally used to call the specific profile
 * check function (most not be used)
 *
 * @param config: Pointer to static configuration.
 * @param smachineState: Pointer to port/data communication state.
 * @param checkstate: Pointer to port/data communication state.
 * @param data: Pointer to received data.
 * @param length: Length of the data in bytes.
 * @return
 */
e2e_error_state E2E_check(const struct e2e_config *config,
                          e2e_sm_state *smachineState, e2e_checkState* checkstate,
                          uint8_t *data, uint32_t length)
{
    e2e_error_state result = E2E_E_OK;
    e2e_profile_status profileStatus = E2E_P_OK;

    result |= config->callback_check(config, checkstate, data, length);
    profileStatus = config->callback_mapStatusToSM(result, checkstate->status);
    result = E2E_SMCheck(profileStatus, config, smachineState);

    return result;
}

/*
 * @brief a generic check function that is internally used to call the specific profile
 * protect function (most not be used)
 *
 * @param config: Pointer to static configuration.
 * @param protectstate: Pointer to port/data communication state.
 * @param data: Pointer to received data.
 * @param length: Length of the data in bytes.
 * @return
 */
e2e_error_state E2E_protect(const struct e2e_config *config,
                            e2e_protectState *protectstate, uint8_t *data,
                            uint32_t length)
{
    e2e_error_state result = E2E_E_OK;
    result |= config->callback_protect(config, protectstate, data, length);
    return result;
}
