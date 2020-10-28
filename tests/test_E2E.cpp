#include "pch.h"
#include "../src/e2e.h"
#include "testUtils.h"

TEST(E2E, E2E_check) {
	FAIL();
}

TEST(E2E, E2E_protect) {
	struct e2e_config config;
	e2e_protectState protectState;
	e2e_error_state ret;
	uint8_t data[8];
	E2E_P11Config(&config, 64, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	E2E_P11ProtectInit(&protectState);

	E2E_protect(&config, &protectState, outTestDataA(), 8);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(testdata[0], 93);
	EXPECT_EQ(testdata[1], 0);
	EXPECT_EQ(protectState.p_counter, 1);
}