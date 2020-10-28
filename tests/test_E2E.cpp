#include "pch.h"
#include "../src/e2e.h"
#include "testUtils.h"

TEST(E2E, E2E_check) {
	struct e2e_config config;
	e2e_checkState checkState;
	e2e_error_state ret;
	e2e_sm_state smState;
	E2E_P11Config(&config, 64, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	E2E_P11CheckInit(&checkState);
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);

	EXPECT_EQ(E2E_check(&config, &smState, &checkState, inTestDataA(0, 1), 8), E2E_E_WRONGSTATE);
	EXPECT_EQ(checkState.status, E2E_STATUS_WRONGSEQUENCE);
	EXPECT_EQ(checkState.c_counter, 1);
}

TEST(E2E, E2E_protect) {
	struct e2e_config config;
	e2e_protectState protectState;
	e2e_error_state ret;
	uint8_t data[8];
	E2E_P11Config(&config, 64, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	E2E_P11ProtectInit(&protectState);

	ret = E2E_protect(&config, &protectState, outTestDataA(), 8);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(getTestData()[0], 93);
	EXPECT_EQ(getTestData()[1], 0);
	EXPECT_EQ(protectState.p_counter, 1);
}