/*
 * e2e_pw.c
 *
 *  Created on: 22.10.2020
 *      Author: till
 */

#include "e2e_pw.h"
e2e_error_state E2EPW_ReadInit(const struct e2e_config *config,
                               e2e_checkState* state);
e2e_error_state E2EPW_WriteInit(const struct e2e_config *config,
                                e2e_protectState *state);

/*
 * @brief Initializes the E2E-PW
 *
 * @param config: Pointer to static configuration.
 * @param checkState: Pointer to port/data communication state.
 * @param protectState: Pointer to port/data communication state.
 * @param smState: Pointer to port/data communication state.
 * @param callback_write: the data related write function
 * @param callback_read: the data related read function
 * @return any possible E2E-Error-State
 */
e2e_error_state E2EPW_Init(
        const struct e2e_config *config, e2e_checkState* checkState,
        e2e_protectState *protectState, e2e_sm_state *smState,
        rte_error_state (*callback_write)(uint8_t *data, uint32_t length),
        rte_error_state (*callback_read)(uint8_t *data, uint32_t *length))
{
    e2e_error_state enum_result = E2E_E_OK;

    switch (config->profileName)
    {
    case E2E_PROFILE_11:
        enum_result |= E2E_P11Init(config, checkState, protectState);
        break;
    default:
        enum_result = E2E_E_INPUTERR_WRONG;
        break;
    }

    if (enum_result == E2E_E_OK)
    {
        checkState->callback_read = callback_read;
        protectState->callback_write = callback_write;
        enum_result |= E2EPW_ReadInit(config, checkState);
        enum_result |= E2EPW_WriteInit(config, protectState);
        enum_result |= E2E_SMInit(smState, config);
    }

    return enum_result;
}

/*
 * @brief Performs a safe explicit read on a sender-receiver safety-related
 * communication data element with data semantics. The function calls the
 * corresponding function RTE_Read, and then checks received data with E2E_PXXCheck.
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @param smState: Pointer to port/data communication state.
 * @param data: Parameter to pass back the received data.
 * @param length: Length of the data in bytes
 * @return
 * The byte 0 (lowest byte) is the status of Rte_Read function:
 * RTE_E_INVALID - data element invalid
 * RTE_E_MAX_AGE_EXCEEDED - data element outdated
 * RTE_E_NEVER_RECEIVED - No data received since system start or partition restart
 * RTE_E_UNCONNECTED - Indicates that the receiver port is not connected.
 * RTE_E_OK - data read successfully
 *
 * The byte 1 is the status of runtime checks done within E2E Protection Wrapper function, plus including bit extension checks:
 * E2E_E_INPUTERR_NULL - At least one pointer parameter of E2EPW_Read is a NULL pointer
 * E2E_E_INPUTERR_WRONG - At least one input parameter of E2EPW_Read is erroneous, e.g. out of range
 * E2E_E_INTERR - An internal error has occurred in E2EPW_Read (e.g. error detected by
 *                program flow monitoring, violated invariant or postcondition)
 * E2EPW_E_DESERIALIZATION - extension/expansion error(s) occurred. It is the status
 *                           if bit extension (conversion of shortened I-PDU representation into
 *                           data elements) is correct. For example, if 12 bits from I-PDU are
 *                           expanded into 16-bit uint, then the top most 4 bits shall be 0.
 * E2E_E_OK - Function E2EPW_Read completed successfully
 *
 * The byte 2 is the return value of E2E_PXXCheck function:
 * E2E_E_INPUTERR_NULL - At least one pointer parameter of E2E_PXXCheck is a NULL pointer
 * E2E_E_INPUTERR_WRONG - At least one input parameter of E2E_PXXCheck is erroneous,
 *                        e.g. out of range
 * E2E_E_INTERR - An internal error has occurred in E2E_PXXCheck (e.g. error detected by program
 *                flow monitoring, violated invariant or postcondition)
 * E2E_E_OK - Function E2E_PXXCheck completed successfully
 *
 * The byte 3 is the value of E2E_PXXCheckStatusType Enumeration, representing the result of
 *          the verification of the Data in E2E Profile XX, determined by the Check function:
 * E2EPW_STATUS_NONEWDATA - Error: the Check function has been invoked but no new Data is not
 *          available since the last call, according to communication medium (e.g. RTE, COM).
 *          As a result, no E2E checks of Data have been consequently executed.
 * E2EPW_STATUS_WRONGCRC - Error: The data has been received according to communication medium,
 *          but the CRC or Data or part of Data is incorrect/corrupted. This may be caused by
 *          corruption, insertion or by addressing faults.
 * E2EPW_STATUS_INITIAL - Error: The new data has been received according to communication medium,
 *          the CRC is correct, but this is the first Data since the receiver's initialization or
 *          reinitialization, so the Counter cannot be verified yet.
 * E2EPW_STATUS_REPEATED - Error: The new data has been received according to communication medium,
 *          the CRC is correct, but the Counter is identical to the most recent Data received with
 *          Status _INITIAL, _OK, or _OKSOMELOST.
 * E2EPW_STATUS_OK - OK: The new data has been received according to communication medium, the CRC
 *          is correct, the Counter is incremented by 1 with respect to the most recent Data
 *          received with Status _INITIAL, _OK, or _OKSOMELOST. This means that no Data has been
 *          lost since the last correct data reception.
 * E2EPW_STATUS_OKSOMELOST - OK: The new data has been received according to communication medium,
 *          the CRC is correct, the Counter is incremented by DeltaCounter
 *          (1 < DeltaCounter = MaxDeltaCounter) with respect to the most recent Data received with
 *          Status _INITIAL, _OK, or _OKSOMELOST. This means that some Data in the sequence have
 *          been probably lost since the last correct/initial reception, but this is within the
 *          configured tolerance range
 * E2EPW_STATUS_WRONGSEQUENCE - Error: The new data has been received according to communication
 *          medium, the CRC is correct, but the Counter Delta is too big
 *          (DeltaCounter > MaxDeltaCounter) with respect to the most recent Data received with
 *          Status _INITIAL, _OK, or _OKSOMELOST. This means that too many Data in the sequence
 *          have been probably lost since the last correct/initial reception.
 * E2EPW_STATUS_SYNC - NOT VALID: The new data has been received after detection of an unexpected
 *          behaviour of counter. The data has a correct CRC and a counter within the expected range
 *          with respect to the most recent Data received, but the determined continuity check for
 *          the counter is not finalized yet.
 */
uint32_t E2EPW_Read(const struct e2e_config *config, e2e_checkState* state,
                    e2e_sm_state *smState, uint8_t *data, uint32_t *length)
{
    rte_error_state retRteRead = RTE_E_OK;
    e2e_error_state retE2EProtect = E2E_E_OK;
    e2e_error_state plausibilityChecks = E2E_E_OK;
    uint32_t ret = 0;

    if (config == NULL || state == NULL || data == NULL)
    {
        plausibilityChecks = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* read data from Rte */
        retRteRead = state->callback_read(data, length);

        if (retRteRead == RTE_E_OK)
        {
            state->newDataAvailable = true;
        }
        else
        {
            state->newDataAvailable = false;
        }

        /* Deserialize Data */
        bool validDeserialization = true;

        if (validDeserialization == true)
        {
            /* run E2E_PXXCheck() */
            retE2EProtect = E2E_check(config, smState, state, data, *length);
        }
        else
        {
            plausibilityChecks = E2EPW_E_DESERIALIZATION;
        }
    }

    ret = (uint32_t) retRteRead | ((uint32_t) (retE2EProtect) << 8)
            | ((uint32_t) (plausibilityChecks) << 16)
            | ((uint32_t) (state->status) << 24);
    return ret;
}

/*
 * @brief The function reinitializes the corresponding data structure after
 * a detected error or at startup.
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @return E2E_E_OK
 */
e2e_error_state E2EPW_ReadInit(const struct e2e_config *config,
                               e2e_checkState* state)
{
    e2e_error_state enum_result = E2E_E_OK;
    state->newDataAvailable = false;
    state->status = E2E_STATUS_NONEWDATA;
    return enum_result;
}

/*
 * @brief Initiates a safe explicit sender-receiver transmission of a safety-related
 * data element with data semantic. It protects data with E2E Library function
 * E2E_PXXProtect and then it calls the corresponding RTE_Write function.
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @param data: Data element to be protected and sent. The parameter is inout,
 * because this function invokes E2E_PXXProtect function, which updates
 * the values of control fields. The name and data type are the same as
 * in the corresponding Rte_Write function.
 * @param length: Length of the data in bytes
 *
 * @return
 * The byte 0 (lowest byte) is the status of Rte_Write function:
 * RTE_E_COM_STOPPED - the RTE could not perform the operation because the COM service
 *                     is currently not available (inter ECU communication only)
 * RTE_E_SEG_FAULT - a segmentation violation is detected in the handed over parameters
 *                   to the RTE API. No transmission is executed
 * RTE_E_OK - data passed to communication service successfully
 *
 * The byte 1 is the status of runtime checks done within E2E Protection Wrapper function:
 * E2E_E_INPUTERR_NULL - At least one pointer parameter of E2EPW_Write is a NULL pointer
 * E2E_E_INPUTERR_WRONG - At least one input parameter of E2EPW_Write is erroneous, e.g.
 *                        out of range
 * E2E_E_INTERR - An internal error has occurred in E2EPW_Write (e.g. error detected by
 *                program flow monitoring, violated invariant or postcondition)
 * E2E_E_OK - Function E2EPW_Write completed successfully
 *
 * The byte 2 is the return value of E2E_PXXProtect function:
 * E2E_E_INPUTERR_NULL - At least one pointer parameter of E2E_PXXProtect is a
 *                       NULL pointer
 * E2E_E_INPUTERR_WRONG - At least one input parameter of E2E_PXXProtect is erroneous,
 *                        e.g. out of range
 * E2E_E_INTERR - An internal error has occurred in E2E_PXXProtect (e.g. error detected
 *                by program flow monitoring, violated invariant or postcondition)
 * E2E_E_OK - Function E2E_PXXProtect completed successfully
 *
 * The byte 3 is a placeholder for future use and takes the following values:
 * E2E_E_OK - default case
 */
uint32_t E2EPW_Write(const struct e2e_config *config, e2e_protectState *state,
                     uint8_t *data, uint32_t length)
{
    rte_error_state retRteWrite = RTE_E_OK;
    e2e_error_state retE2EProtect = E2E_E_OK;
    e2e_error_state plausibilityChecks = E2E_E_OK;
    uint32_t ret = 0;

    if ((config == NULL) || (state == NULL) || (data == NULL))
    {
        plausibilityChecks = E2E_E_INPUTERR_NULL;
    }
    else
    {
        /* SerializeAppDataEl to IPDU representation */
        /* keine Aktion noetig */

        /* run E2E_PXXProtect() */
        retE2EProtect = E2E_protect(config, state, data, length);

        if (retE2EProtect == E2E_E_OK)
        {
            /* write data to Rte */
            retRteWrite = state->callback_write(data, length);
        }
    }

    ret = (uint32_t) retRteWrite | ((uint32_t) (retE2EProtect) << 8)
            | ((uint32_t) (plausibilityChecks) << 16);
    return ret;
}

/*
 * @brief Initializes the Write-PW
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @return E2E_E_OK
 */
e2e_error_state E2EPW_WriteInit(const struct e2e_config *config,
                                e2e_protectState *state)
{
    e2e_error_state enum_result = E2E_E_OK;
    state->p_counter = 0u;
    return enum_result;
}
