/*
 * e2e_config.h
 *
 *  Created on: 21.10.2020
 *      Author: till
 */

#ifndef E2E_E2E_CONFIG_H_
#define E2E_E2E_CONFIG_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include "types.h"

struct e2e_config;

typedef enum
{
    E2E_PROFILE_1 = 1u,
    E2E_PROFILE_2 = 2u,
    E2E_PROFILE_4 = 4u,
    E2E_PROFILE_5 = 5u,
    E2E_PROFILE_6 = 6u,
    E2E_PROFILE_7 = 7u,
    E2E_PROFILE_11 = 11u,
    E2E_PROFILE_22 = 22u,
} e2e_profile;

typedef enum
{
    E2E_STATUS_NOTAVAILABLE = 0x04,
    E2E_STATUS_OK = 0x00,
    E2E_STATUS_REPEATED = 0x08,
    E2E_STATUS_WRONGSEQUENCE = 0x40,
    E2E_STATUS_NONEWDATA = 0x01,
    E2E_STATUS_OKSOMELOST = 0x20,
    E2E_STATUS_ERROR = 0x07,
} e2e_status;

typedef enum
{
    E2E_P_ERROR = 0u,
    E2E_P_OK = 1u,
    E2E_P_NOTAVAILABLE = 2u,
    E2E_P_NONEWDATA = 3u,
    E2E_P_WRONGSEQUENCE = 4u,
    E2E_P_REPEATED = 5u,
} e2e_profile_status;

typedef enum
{
    E2E_E_OK = 0x0,
    E2E_E_NOT_OK = 0x1,
    E2EPW_E_DESERIALIZATION = 0x3,
    E2EPW_E_REDUNDANCY = 0x5,
    E2E_E_INPUTERR_NULL = 0x13,
    E2E_E_INPUTERR_WRONG = 0x17,
    E2E_E_INTERR = 0x19,
    E2E_E_WRONGSTATE = 0x1A,
} e2e_error_state;

typedef enum
{
    E2E_DATAID_BOTH = 0u,
    E2E_DATAID_ALT = 1u,
    E2E_DATAID_LOW = 2u,
    E2E_DATAID_NIBBLE = 3u,
} e2e_dataIdMode;

typedef struct
{
    uint8_t p_counter;

    /* functions */
    rte_error_state (*callback_write)(uint8_t *data, uint32_t length);
} e2e_protectState;

typedef struct
{
    uint8_t c_counter;
    e2e_status status;
    bool newDataAvailable;

    /* functions */
    rte_error_state (*callback_read)(uint8_t *data, uint32_t *length);
} e2e_checkState;

struct e2e_config
{
    /* parameters */
    uint32_t dataID;
    e2e_profile profileName;
    uint32_t dataLength;
    uint32_t minDataLength;
    uint32_t maxDataLength;
    uint32_t dataIdList[16];
    e2e_dataIdMode dataIdMode;
    uint32_t offset;
    uint32_t counterOffset;
    uint32_t crcOffset;
    uint32_t dataIdNibbleOffset;
    uint32_t maxDeltaCounter;

    /* parameters of legacy profiles */
    uint32_t maxDeltaCounterInit;
    uint32_t maxNoNewOrRepeatedData;
    uint32_t syncCounterInit;
    uint32_t profileBehavior;

    /* parameters of E2E state machine */
    uint32_t windowSize;
    uint32_t maxErrorStateInit;
    uint32_t maxErrorStateInvalid;
    uint32_t maxErrorStateValid;
    uint32_t minOkStateInit;
    uint32_t minOkStateInvalid;
    uint32_t minOkStateValid;

    /* functions */
    e2e_error_state (*callback_check)(const struct e2e_config *config,
                                      e2e_checkState* state,
                                      const uint8_t *data, uint32_t length);
    e2e_profile_status (*callback_mapStatusToSM)(e2e_error_state returnStatus,
                                                 e2e_status e2estatus);
    e2e_error_state (*callback_protect)(const struct e2e_config *config,
                                        e2e_protectState *state, uint8_t *data,
                                        uint32_t length);
};

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /* E2E_E2E_CONFIG_H_ */
