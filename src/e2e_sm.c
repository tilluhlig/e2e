/*
 * e2e_sm.c
 *
 *  Created on: 21.10.2020
 *      Author: till
 */

#include "e2e_sm.h"

void E2E_SMTransition(const struct e2e_config *config, e2e_sm_state *state,
                      e2e_sm nextState);
uint32_t E2E_NumberOfOKElementsInWindow(const struct e2e_config *config,
                                        const e2e_sm_state *state);
uint32_t E2E_NumberOfErrorElementsInWindow(const struct e2e_config *config,
                                           const e2e_sm_state *state);

/*
 * @brief initializes a config with the given values
 *
 * @param config: Pointer to static configuration.
 * @param new_windowSize: the window size
 * @param new_maxErrorStateInit: the maximum allowed errors in Init
 * @param new_maxErrorStateInvalid: the maximum allowed errors in Invalid
 * @param new_maxErrorStateValid: the maximum allowed errors in Valid
 * @param new_minOkStateInit: the minimum allowed errors in Init
 * @param new_minOkStateInvalid: the minimum allowed errors in Invalid
 * @param new_minOkStateValid: the minimum allowed errors in Valid
 * @return E2E_E_OK, E2E_E_INPUTERR_NULL
 */
e2e_error_state E2E_SMConfig(struct e2e_config *config, uint32_t new_windowSize,
                             uint32_t new_maxErrorStateInit,
                             uint32_t new_maxErrorStateInvalid,
                             uint32_t new_maxErrorStateValid,
                             uint32_t new_minOkStateInit,
                             uint32_t new_minOkStateInvalid,
                             uint32_t new_minOkStateValid)
{
    e2e_error_state e2e_smconfig_result = E2E_E_OK;

    if (config == NULL)
    {
        e2e_smconfig_result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        config->windowSize = new_windowSize;
        config->maxErrorStateInit = new_maxErrorStateInit;
        config->maxErrorStateInvalid = new_maxErrorStateInvalid;
        config->maxErrorStateValid = new_maxErrorStateValid;
        config->minOkStateInit = new_minOkStateInit;
        config->minOkStateInvalid = new_minOkStateInvalid;
        config->minOkStateValid = new_minOkStateValid;
    }

    return e2e_smconfig_result;
}

/*
 * @brief Initializes the SM
 *
 * @param state: Pointer to port/data communication state.
 * @param config: Pointer to static configuration.
 * @return E2E_E_OK, E2E_E_INPUTERR_NULL
 */
e2e_error_state E2E_SMInit(e2e_sm_state *state, const struct e2e_config *config)
{
    e2e_error_state e2e_sminit_result = E2E_E_OK;
    uint32_t i;

    if ((state == NULL) || (config == NULL))
    {
        e2e_sminit_result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        E2E_SMClearStatus(config, state);
        state->smState = E2E_SM_DEINIT;

        for (i = 0u; i < config->windowSize; i++)
        {
            state->profileStatusWindow[i] = E2E_P_NOTAVAILABLE;
        }
    }

    return e2e_sminit_result;
}

/*
 * @brief resets the SM-Status
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @return E2E_E_OK, E2E_E_INPUTERR_NULL
 */
e2e_error_state E2E_SMClearStatus(const struct e2e_config *config,
                                  e2e_sm_state *state)
{
    uint32_t temp_var = 0u;
    e2e_error_state e2e_smclearstatus_result = E2E_E_OK;

    if ((config == NULL) || (state == NULL))
    {
        e2e_smclearstatus_result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        for (temp_var = 0u; temp_var < config->windowSize; temp_var++)
        {
            state->profileStatusWindow[temp_var] = E2E_P_NOTAVAILABLE;
        }

        state->okCount = 0u;
        state->errorCount = 0u;
        state->windowTopIndex = 0u;
    }

    return e2e_smclearstatus_result;
}

/*
 * @brief Initializes the state machine.
 *
 * @param config: Pointer to configuration of the state machine
 * @param state: Pointer to port/data communication state.
 * @return E2E_E_INPUTERR_NULL - null pointer passed or E2E_E_OK
 */
e2e_error_state E2E_SMCheckInit(const struct e2e_config *config,
                                e2e_sm_state *state)
{
    e2e_error_state result = E2E_E_OK;

    if ((config == NULL) || (state == NULL))
    {
        result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        E2E_SMTransition(config, state, E2E_SM_NODATA);
        E2E_SMClearStatus(config, state);
    }

    return result;
}

/*
 * @brief adds a profile status to the profile status window
 *
 * @param profileStatus: the profile status to add
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @return E2E_E_OK, E2E_E_INPUTERR_NULL
 */
e2e_error_state E2E_SMAddStatus(e2e_profile_status profileStatus,
                                const struct e2e_config *config,
                                e2e_sm_state *state)
{
    e2e_error_state e2e_smaddstatus_result = E2E_E_OK;

    if ((config == NULL) || (state == NULL))
    {
        e2e_smaddstatus_result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        uint32_t windowPos = state->windowTopIndex;
        state->profileStatusWindow[windowPos] = profileStatus;

        /* State->OKCount = number of elements in State->ProfileStatusWindow[] with values E2E_P_OK */
        state->okCount = E2E_NumberOfOKElementsInWindow(config, state);

        /* State->ErrorCount = number of elements in State->ProfileStatusWindow[] with values E2E_P_ERROR */
        state->errorCount = E2E_NumberOfErrorElementsInWindow(config, state);

        if (state->windowTopIndex == (config->windowSize - 1u))
        {
            state->windowTopIndex = 0u;
        }
        else
        {
            state->windowTopIndex++;
        }
    }

    return e2e_smaddstatus_result;
}

/*
 * @brief performs a transition
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @param nextState: the new sm state
 */
void E2E_SMTransition(const struct e2e_config *config, e2e_sm_state *state,
                      e2e_sm nextState)
{
    state->smState = nextState;
}

/*
 * @brief Checks the communication channel.
 * It determines if the data can be used for safety-related application, based on
 * history of checks performed by a corresponding E2E_P0XCheck() function.
 *
 * @param profileStatus: Profile-independent status of the reception on one single Data in one cycle
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @return E2E_E_INPUTERR_NULL, E2E_E_INPUTERR_WRONG, E2E_E_INTERR, E2E_E_OK, E2E_E_WRONGSTATE
 */
e2e_error_state E2E_SMCheck(e2e_profile_status profileStatus,
                            const struct e2e_config *config,
                            e2e_sm_state *state)
{
    e2e_error_state e2e_smcheck_result = E2E_E_OK;

    if ((config == NULL) || (state == NULL))
    {
        e2e_smcheck_result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        switch (state->smState)
        {
        case E2E_SM_DEINIT:
            E2E_SMCheckInit(config, state);
            e2e_smcheck_result = E2E_E_WRONGSTATE;
            break;
        case E2E_SM_NODATA:
            if ((profileStatus != E2E_P_ERROR)
                    && (profileStatus != E2E_P_NOTAVAILABLE))
            {
                E2E_SMTransition(config, state, E2E_SM_INIT);
            }
            e2e_smcheck_result = E2E_E_NOT_OK;
            break;
        case E2E_SM_INIT:
            E2E_SMAddStatus(profileStatus, config, state);

            if ((state->errorCount <= config->maxErrorStateInit)
                    && (state->okCount >= config->minOkStateInit))
            {
                E2E_SMTransition(config, state, E2E_SM_VALID);
                e2e_smcheck_result = E2E_E_OK;
            }
            else if (state->errorCount > config->maxErrorStateInit)
            {
                E2E_SMTransition(config, state, E2E_SM_INVALID);
                e2e_smcheck_result = E2E_E_NOT_OK;
            }
            else
            {
                /* keine Aktion */
                e2e_smcheck_result = E2E_E_NOT_OK;
            }

            break;
        case E2E_SM_VALID:
            E2E_SMAddStatus(profileStatus, config, state);

            if ((state->errorCount <= config->maxErrorStateValid)
                    && (state->okCount >= config->minOkStateValid))
            {
                /* keine Aktion */
                e2e_smcheck_result = E2E_E_OK;
            }
            else
            {
                E2E_SMTransition(config, state, E2E_SM_INVALID);
                e2e_smcheck_result = E2E_E_NOT_OK;
            }

            break;
        case E2E_SM_INVALID:
            E2E_SMAddStatus(profileStatus, config, state);

            if ((state->errorCount <= config->maxErrorStateInvalid)
                    && (state->okCount >= config->minOkStateInvalid))
            {
                E2E_SMTransition(config, state, E2E_SM_VALID);
                e2e_smcheck_result = E2E_E_OK;
            }
            else
            {
                e2e_smcheck_result = E2E_E_NOT_OK;
            }

            break;
        default:
            E2E_SMTransition(config, state, E2E_SM_DEINIT);
            e2e_smcheck_result = E2E_E_NOT_OK;
            break;
        }
    }

    return e2e_smcheck_result;
}

/*
 * @brief counts all valid checks in the status window
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @return the number of valid checks
 */
uint32_t E2E_NumberOfOKElementsInWindow(const struct e2e_config *config,
                                        const e2e_sm_state *state)
{
    uint32_t ok_cntVar = 0u;
    uint32_t ok_result = 0u;

    for (ok_cntVar = 0u; ok_cntVar < config->windowSize; ok_cntVar++)
    {
        if (state->profileStatusWindow[ok_cntVar] == E2E_P_OK)
        {
            ok_result++;
        }
    }
    return ok_result;
}

/*
 * @brief counts all invalid checks in the status window
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @return the number of invalid checks
 */
uint32_t E2E_NumberOfErrorElementsInWindow(const struct e2e_config *config,
                                           const e2e_sm_state *state)
{
    uint32_t error_cntVar = 0u;
    uint32_t error_result = 0u;
    e2e_profile_status currentVal;

    for (error_cntVar = 0u; error_cntVar < config->windowSize; error_cntVar++)
    {
        currentVal = state->profileStatusWindow[error_cntVar];
        if ((currentVal == E2E_P_ERROR) || (currentVal == E2E_P_REPEATED)
                || (currentVal == E2E_P_WRONGSEQUENCE))
        {
            error_result++;
        }
    }
    return error_result;
}
