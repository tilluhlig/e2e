#include "pch.h"
#include "../src/e2e.h"
#include "testUtils.h"

rte_error_state var1_write(uint8_t *data, uint32_t length)
{
	return RTE_E_OK;
}

rte_error_state var1_read(uint8_t *data, uint32_t *length)
{
	return RTE_E_OK;
}

TEST(E2E_PW, E2EPW_Init) {
	struct e2e_config config;
	e2e_protectState var1_protectState;
	e2e_checkState var1_checkState;
	e2e_sm_state var1_smState;
	e2e_error_state ret;
	E2E_P11Config(&config, 64, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	ret = E2EPW_Init(&config, &var1_checkState, &var1_protectState, &var1_smState, &var1_write, &var1_read);
	EXPECT_EQ(ret, E2E_E_OK);
}

TEST(E2E_PW, E2EPW_Read) {
	// TODO
	SUCCEED();
}

TEST(E2E_PW, E2EPW_ReadInit) {
	// TODO
	SUCCEED();
}

TEST(E2E_PW, E2EPW_Write) {
	// TODO
	SUCCEED();
}

TEST(E2E_PW, E2EPW_WriteInit) {
	// TODO
	SUCCEED();
}