#include "e2e_p11.h"

uint8_t E2E_P11Crc(const struct e2e_config *config, const uint8_t *data,
                   uint32_t length);

/*
 * @brief initializes a config with the given values
 *
 * @param config: Pointer to static configuration.
 * @param dataLength: the data length in bytes
 * @param dataIdMode: the data ID-Mode
 * @param dataIdNibbleOffset: data-ID-nibble offset in bits (is required if dataIdMode == E2E_DATAID_NIBBLE)
 * @param counterOffset: the counter offset in bits
 * @param crcOffset: the crc offset
 * @param dataID: the unique data-ID
 * @param maxDeltaCounter: the maximum acceptable counter delta (between two transmissions)
 * @return E2E_E_OK, E2E_E_INPUTERR_NULL
 */
e2e_error_state E2E_P11Config(struct e2e_config *config, uint32_t dataLength,
                              e2e_dataIdMode dataIdMode,
                              uint32_t dataIdNibbleOffset,
                              uint32_t counterOffset, uint32_t crcOffset,
                              uint32_t dataID, uint32_t maxDeltaCounter)
{
    e2e_error_state result = E2E_E_OK;

    if (config == NULL)
    {
        result = E2E_E_INPUTERR_NULL;
    }
    else
    {

        config->dataLength = dataLength;
        config->dataIdMode = dataIdMode;
        config->dataIdNibbleOffset = dataIdNibbleOffset;
        config->counterOffset = counterOffset;
        config->crcOffset = crcOffset;
        config->dataID = dataID;
        config->maxDeltaCounter = maxDeltaCounter;
        config->profileName = E2E_PROFILE_11;
        config->callback_check = &E2E_P11Check;
        config->callback_protect = &E2E_P11Protect;
        config->callback_mapStatusToSM = &E2E_P11MapStatusToSM;
    }

    return result;
}

/*
 * @brief Initializes the profile
 *
 * @param config: Pointer to static configuration.
 * @param checkState: Pointer to port/data communication state.
 * @param protectState: Pointer to port/data communication state.
 * @return E2E_E_OK, E2E_E_INPUTERR_NULL
 */
e2e_error_state E2E_P11Init(const struct e2e_config *config,
                            e2e_checkState *checkState,
                            e2e_protectState* protectState)
{
    e2e_error_state result = E2E_E_OK;

    if ((checkState == NULL) || (config == NULL) || (protectState == NULL))
    {
        result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        E2E_P11CheckInit(checkState);
        E2E_P11ProtectInit(protectState);
    }

    return result;
}

/*
 * @brief Checks the Data received using the E2E profile 11.
 * This includes CRC calculation, handling of Counter.
 * The function checks only one single data in one cycle,
 * it does not determine/compute the accumulated state of the communication link.
 *
 * @param config: Pointer to static configuration.
 * @param state: Pointer to port/data communication state.
 * @param data: Pointer to received data.
 * @param length: Length of the data in bytes.
 * @return E2E_E_INPUTERR_NULL, E2E_E_OK, E2E_E_INPUTERR_WRONG.
 */
e2e_error_state E2E_P11Check(const struct e2e_config *config,
                             e2e_checkState* state, const uint8_t *data,
                             uint32_t length)
{
    e2e_error_state result = E2E_E_OK;
    bool newDataAvailable = false;

    /* Verify inputs of the check function */
    if ((state == NULL) || (config == NULL))
    {
        result = E2E_E_INPUTERR_NULL;
    }
    else if ((length != 0) && (data == NULL))
    {
        result = E2E_E_INPUTERR_WRONG;
    }
    else
    {
        if (data != NULL)
        {

            if (length == (config->dataLength / 8))
            {
                newDataAvailable = true;
            }
            else
            {
                result = E2E_E_INPUTERR_WRONG;
            }
        }

        if (result == E2E_E_OK)
        {
            uint8_t receivedCrc = 0u;
            uint8_t computedCrc = 0u;
            uint8_t receivedCounter = 0u;
            uint32_t receivedNibble = 0u;

            if (newDataAvailable == true)
            {
                /* read DataIDNibble */
                receivedNibble = 0u;
                if (config->dataIdMode == E2E_DATAID_NIBBLE)
                {
                    receivedNibble = (data[config->dataIdNibbleOffset >> 3]
                            >> (config->dataIdNibbleOffset & 0x7)) & 0x0F;
                }

                /* Read Counter */
                receivedCounter = (data[config->counterOffset >> 3]
                        << (config->counterOffset & 0x7)) & 0xF;

                /* Read CRC */
                receivedCrc = data[config->crcOffset / 8];

                /* Compute CRC */
                computedCrc = E2E_P11Crc(config, data, length);
            }

            /* Do Checks */
            if (newDataAvailable == false)
            {
                state->status = E2E_STATUS_NONEWDATA;
            }
            else if (receivedCrc != computedCrc)
            {
                state->status = E2E_STATUS_ERROR;
            }
            else if (config->dataIdMode == E2E_DATAID_NIBBLE
                    && (receivedNibble != (config->dataID >> 8) & 0xFF))
            {
                state->status = E2E_STATUS_ERROR;
            }
            else
            {
                int deltaCounter = ((receivedCounter) & 0xE)
                        - ((state->c_counter) & 0xE);

                if (deltaCounter > config->maxDeltaCounter || deltaCounter < 0)
                {
                    state->status = E2E_STATUS_WRONGSEQUENCE;
                }
                else if (deltaCounter == 0)
                {
                    state->status = E2E_STATUS_REPEATED;
                }
                else if (deltaCounter == 1)
                {
                    state->status = E2E_STATUS_OK;
                }
                else
                {
                    state->status = E2E_STATUS_OKSOMELOST;
                }

                state->c_counter = receivedCounter;
            }
        }
    }

    return E2E_E_OK;
}

/*
 * @brief Initializes the check state
 *
 * @param statePtr: Pointer to port/data communication state.
 * @return E2E_E_INPUTERR_NULL - null pointer passed E2E_E_OK
 */
e2e_error_state E2E_P11CheckInit(e2e_checkState* statePtr)
{
    e2e_error_state result = E2E_E_OK;

    if (statePtr == NULL)
    {
        result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        statePtr->c_counter = 0xE;
        statePtr->status = E2E_STATUS_ERROR;
    }

    return result;
}

/*
 * @brief The function maps the check status of Profile 11 to a generic check status,
 * which can be used by E2E state machine check function. The E2E Profile 11 delivers
 * a more fine-granular status, but this is not relevant for the E2E state machine.
 *
 * @param checkReturn: Return value of the E2E_P11Check function
 * @param e2estatus: Status determined by E2E_P11Check function
 * @return Profile-independent status of the reception on one single Data in one cycle.
 */
e2e_profile_status E2E_P11MapStatusToSM(e2e_error_state checkReturn,
                                        e2e_status e2estatus)
{
    e2e_profile_status result = E2E_P_ERROR;

    if (checkReturn == E2E_E_OK)
    {
        switch (e2estatus)
        {
        case E2E_STATUS_OK:
            result = E2E_P_OK;
            break;
        case E2E_STATUS_OKSOMELOST:
            result = E2E_P_OK;
            break;
        case E2E_STATUS_REPEATED:
            result = E2E_P_REPEATED;
            break;
        case E2E_STATUS_NONEWDATA:
            result = E2E_P_NONEWDATA;
            break;
        case E2E_STATUS_WRONGSEQUENCE:
            result = E2E_P_WRONGSEQUENCE;
            break;
        case E2E_STATUS_ERROR:
            result = E2E_P_ERROR;
            break;
        default:
            result = E2E_P_ERROR;
            break;
        }
    }

    return result;
}

/*
 * @brief computes a crc over the given data
 *
 * @param config: Pointer to static configuration.
 * @param data: Pointer to received data.
 * @param length: Length of the data in bytes.
 * @return the crc
 */
uint8_t E2E_P11Crc(const struct e2e_config *config, const uint8_t *data,
                   uint32_t length)
{
    uint8_t crc = 0u;
    uint32_t i = 0u;
    uint32_t offset = config->crcOffset / 8u;

    if (config->dataIdMode == E2E_DATAID_BOTH)
    {
        crc = update_crc8_sae_j1850(0xFF, config->dataID);
        crc = update_crc8_sae_j1850(crc, config->dataID >> 8u & 0xFF);
    }
    else if (config->dataIdMode == E2E_DATAID_NIBBLE)
    {
        crc = update_crc8_sae_j1850(0xFF, config->dataID);
        crc = update_crc8_sae_j1850(crc, 0u);
    }
    else
    {
        /* keine Aktion */
    }

    if (offset > 0u)
    {
        for (i = 0; i <= offset; i++)
        {
            crc = update_crc8_sae_j1850(crc, data[i]);
        }

        if (length > offset + 1u)
        {
            for (i = offset + 1; i < length; i++)
            {
                crc = update_crc8_sae_j1850(crc, data[i]);
            }
        }
    }
    else
    {
        for (i = 1; i < length; i++)
        {
            crc = update_crc8_sae_j1850(crc, data[i]);
        }
    }

    return crc;
}

/*
 * @brief Protects the array/buffer to be transmitted using the E2E profile 11.
 * This includes checksum calculation, handling of counter.
 *
 * @param config: Pointer to static configuration.
 * @param statePtr: Pointer to port/data communication state.
 * @param data: Pointer to Data to be transmitted.
 * @param length: Length of the data in bytes
 * @return E2E_E_INPUTERR_NULL, E2E_E_OK, E2E_E_INPUTERR_WRONG.
 */
e2e_error_state E2E_P11Protect(const struct e2e_config *config,
                               e2e_protectState *statePtr, uint8_t *data,
                               uint32_t length)
{
    e2e_error_state result = E2E_E_OK;

    /* Verify inputs of the protect function */
    if ((statePtr == NULL) || (config == NULL) || (data == NULL))
    {
        result = E2E_E_INPUTERR_NULL;
    }
    else if (length != (config->dataLength / 8))
    {
        result = E2E_E_INPUTERR_WRONG;
    }
    else
    {
        /* Write DataIDNibble */
        if (config->dataIdMode == (uint32_t) E2E_DATAID_NIBBLE)
        {
            data[config->dataIdNibbleOffset >> 3] |= ((config->dataID & 0x0F00)
                    >> 8) << (config->dataIdNibbleOffset & 0x7);
        }

        /* Write Counter */
        data[config->counterOffset >> 3] = (statePtr->p_counter & 0xF)
                << (config->counterOffset & 0x7);

        /* Compute CRC */
        uint8_t computedCrc = E2E_P11Crc(config, data, length);

        /* Write CRC */
        data[config->crcOffset / 8] = computedCrc;

        /* Increment Counter */
        statePtr->p_counter++;
    }

    return result;
}

/*
 * @brief Initializes the protection state.
 *
 * @param statePtr: Pointer to port/data communication state.
 * @return E2E_E_INPUTERR_NULL - null pointer passed, E2E_E_OK.
 */
e2e_error_state E2E_P11ProtectInit(e2e_protectState* statePtr)
{
    e2e_error_state result = E2E_E_OK;

    if (statePtr == NULL)
    {
        result = E2E_E_INPUTERR_NULL;
    }
    else
    {
        statePtr->p_counter = 0u;
    }

    return result;
}
