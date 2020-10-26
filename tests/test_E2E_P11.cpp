#include "pch.h"
#include "e2e.h"
#include "testUtils.h"

#ifdef __cplusplus
extern "C" {
#endif
	uint8_t E2E_P11Crc(const struct e2e_config *config, uint8_t *data,
		uint32_t length);
#ifdef __cplusplus
}
#endif /* extern "C" */

TEST(E2E_P11, E2E_P11Config) {
	struct e2e_config config;
	e2e_error_state ret;

	ret = E2E_P11Config(NULL, 8, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	EXPECT_EQ(ret, E2E_E_INPUTERR_NULL);

	ret = E2E_P11Config(&config, 8, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(config.dataLength, 8);
	EXPECT_EQ(config.dataIdMode, E2E_DATAID_BOTH);
	EXPECT_EQ(config.dataIdNibbleOffset, 0);
	EXPECT_EQ(config.counterOffset, 8);
	EXPECT_EQ(config.crcOffset, 0);
	EXPECT_EQ(config.dataID, 0x129);
	EXPECT_EQ(config.maxDeltaCounter, 2);
	EXPECT_EQ(config.profileName, E2E_PROFILE_11);
	EXPECT_EQ(config.callback_check, &E2E_P11Check);
	EXPECT_EQ(config.callback_protect, &E2E_P11Protect);
	EXPECT_EQ(config.callback_mapStatusToSM, &E2E_P11MapStatusToSM);
}

TEST(E2E_P11, E2E_P11Init) {
	struct e2e_config config;
	e2e_protectState protectState;
	e2e_checkState checkState;
	e2e_error_state ret;

	ret = E2E_P11Init(&config, &checkState, NULL);
	EXPECT_EQ(ret, E2E_E_INPUTERR_NULL);

	ret = E2E_P11Init(&config, NULL, &protectState);
	EXPECT_EQ(ret, E2E_E_INPUTERR_NULL);

	ret = E2E_P11Init(NULL, &checkState, &protectState);
	EXPECT_EQ(ret, E2E_E_INPUTERR_NULL);

	ret = E2E_P11Init(&config, &checkState, &protectState);
	EXPECT_EQ(ret, E2E_E_OK);
}

TEST(E2E_P11, E2E_P11Check) {
	struct e2e_config config;
	e2e_checkState checkState;
	e2e_error_state ret;
	E2E_P11Config(&config, 64, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	E2E_P11CheckInit(&checkState);

	EXPECT_EQ(E2E_P11Check(&config, &checkState, inTestDataA(0, 0), 8), E2E_E_OK);
	EXPECT_EQ(checkState.status, E2E_STATUS_ERROR);
	EXPECT_EQ(checkState.c_counter, 0xE);
	EXPECT_EQ(E2E_P11Check(&config, &checkState, inTestDataA(0, 7), 8), E2E_E_OK);
	EXPECT_EQ(checkState.status, E2E_STATUS_ERROR);
	EXPECT_EQ(checkState.c_counter, 0xE);

	EXPECT_EQ(E2E_P11Check(&config, &checkState, inTestDataA(0x5D, 0), 8), E2E_E_OK);
	EXPECT_EQ(checkState.status, E2E_STATUS_WRONGSEQUENCE);
	EXPECT_EQ(checkState.c_counter, 0);

	EXPECT_EQ(E2E_P11Check(&config, &checkState, inTestDataA(0, 1), 8), E2E_E_OK);
	EXPECT_EQ(checkState.status, E2E_STATUS_OK);
	EXPECT_EQ(checkState.c_counter, 1);

	EXPECT_EQ(E2E_P11Check(&config, &checkState, inTestDataA(0, 1), 8), E2E_E_OK);
	EXPECT_EQ(checkState.status, E2E_STATUS_REPEATED);
	EXPECT_EQ(checkState.c_counter, 1);

	EXPECT_EQ(E2E_P11Check(&config, &checkState, inTestDataA(186, 3), 8), E2E_E_OK);
	EXPECT_EQ(checkState.status, E2E_STATUS_OKSOMELOST);
	EXPECT_EQ(checkState.c_counter, 3);

	EXPECT_EQ(E2E_P11Check(&config, &checkState, NULL, 0), E2E_E_OK);
	EXPECT_EQ(checkState.status, E2E_STATUS_NONEWDATA);
	EXPECT_EQ(checkState.c_counter, 3);
}

TEST(E2E_P11, E2E_P11CheckInit) {
	e2e_checkState checkState;

	EXPECT_EQ(E2E_P11CheckInit(NULL), E2E_E_INPUTERR_NULL);
	EXPECT_EQ(E2E_P11CheckInit(&checkState), E2E_E_OK);
	EXPECT_EQ(checkState.c_counter, (uint8_t)0xE);
	EXPECT_EQ(checkState.status, E2E_STATUS_ERROR);
}

TEST(E2E_P11, E2E_P11MapStatusToSM) {
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_OK, E2E_STATUS_NOTAVAILABLE), E2E_P_ERROR);
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_OK, E2E_STATUS_OK), E2E_P_OK);
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_OK, E2E_STATUS_REPEATED), E2E_P_REPEATED);
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_OK, E2E_STATUS_WRONGSEQUENCE), E2E_P_WRONGSEQUENCE);
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_OK, E2E_STATUS_NONEWDATA), E2E_P_NONEWDATA);
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_OK, E2E_STATUS_OKSOMELOST), E2E_P_OK);
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_OK, E2E_STATUS_ERROR), E2E_P_ERROR);

	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_NOT_OK, E2E_STATUS_OK), E2E_P_ERROR);
	EXPECT_EQ(E2E_P11MapStatusToSM(E2E_E_NOT_OK, E2E_STATUS_ERROR), E2E_P_ERROR);
}

TEST(E2E_P11, E2E_P11Crc) {
	struct e2e_config config;
	uint8_t crc;

	E2E_P11Config(&config, 8, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);

	crc = E2E_P11Crc(&config, outTestDataA(), 8);
	EXPECT_EQ(crc, 0x5D);

	crc = E2E_P11Crc(&config, outTestDataA(), 8);
	EXPECT_EQ(crc, 0x5D);

	crc = E2E_P11Crc(&config, outTestDataA(), 1);
	EXPECT_NE(crc, 0x5D);

	crc = E2E_P11Crc(&config, outTestDataA(), 0);
	EXPECT_NE(crc, 0x5D);

	crc = E2E_P11Crc(&config, outTestDataB(), 8);
	EXPECT_EQ(crc, 150);

	crc = E2E_P11Crc(&config, outTestDataC(), 8);
	EXPECT_EQ(crc, 247);

	crc = E2E_P11Crc(&config, outTestDataD(), 8);
	EXPECT_EQ(crc, 244);
}

TEST(E2E_P11, E2E_P11Protect) {
	struct e2e_config config;
	e2e_protectState protectState;
	e2e_error_state ret;
	uint8_t data[8];
	E2E_P11Config(&config, 64, E2E_DATAID_BOTH, 0, 8, 0, 0x129, 2);
	E2E_P11ProtectInit(&protectState);

	EXPECT_EQ(E2E_P11Protect(NULL, &protectState, testdata, 8), E2E_E_INPUTERR_NULL);
	EXPECT_EQ(E2E_P11Protect(&config, NULL, testdata, 8), E2E_E_INPUTERR_NULL);

	memcpy(data, outTestDataA(), sizeof(testdata));
	ret = E2E_P11Protect(&config, &protectState, testdata, 8);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_TRUE(0 == memcmp(data + 2, testdata + 2, 6));
	EXPECT_EQ(testdata[0], 93);
	EXPECT_EQ(testdata[1], 0);
	EXPECT_EQ(protectState.p_counter, 1);

	ret = E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(testdata[0], 203);
	EXPECT_EQ(testdata[1], 1);
	EXPECT_EQ(protectState.p_counter, 2);

	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);
	E2E_P11Protect(&config, &protectState, outTestDataB(), 8);

	ret = E2E_P11Protect(&config, &protectState, outTestDataC(), 8);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(testdata[0], 0x4E);
	EXPECT_EQ(testdata[1], 0);
	EXPECT_EQ(protectState.p_counter, 17);
}

TEST(E2E_P11, E2E_P11ProtectInit) {
	e2e_protectState protectState;

	EXPECT_EQ(E2E_P11ProtectInit(NULL), E2E_E_INPUTERR_NULL);
	EXPECT_EQ(E2E_P11ProtectInit(&protectState), E2E_E_OK);
	EXPECT_EQ(protectState.p_counter, 0u);
}