[![Build Status](https://travis-ci.com/tilluhlig/e2e.svg?branch=main)](https://travis-ci.com/tilluhlig/e2e)

Attention: This project is work in progress!!!

# Description
This is an almost exact Autosar 4.4.0 and Misra 2004 compatible implementation of an E2E-Lib, an E2E-Protection Wrapper and an E2E-State-Machine. The usage is provided for private purposes.

The read and write functions are more generic than expected by the standard.

# Profiles
The currently supported profiles are:
- Profile 11

# Tests
TODO

# Usage
TODO

``` C
struct e2e_config config;

e2e_protectState var1_protectState;
e2e_checkState var1_checkState;
e2e_sm_state var1_smState;

rte_error_state var1_write(uint8_t *data, uint32_t length)
{
 return RTE_E_OK;
}

rte_error_state var1_read(uint8_t *data, uint32_t *length)
{
 return RTE_E_OK;
}
 
void main(void)
{
	E2E_P11Config(&config, 8, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2EPW_Init(&config, &var1_checkState, &var1_protectState, &var1_smState, &var1_write, &var1_read);
}
```
