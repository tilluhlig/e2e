#include "pch.h"
#include "src/e2e.h"

#ifdef __cplusplus
extern "C" {
#endif
	void E2E_SMTransition(const struct e2e_config *config, e2e_sm_state *state,
		e2e_sm nextState);
	uint32_t E2E_NumberOfOKElementsInWindow(const struct e2e_config *config,
		const e2e_sm_state *state);
	uint32_t E2E_NumberOfErrorElementsInWindow(const struct e2e_config *config,
		const e2e_sm_state *state);
#ifdef __cplusplus
}
#endif /* extern "C" */

TEST(E2E_SM, E2E_SMConfig) {
	struct e2e_config config;
	e2e_error_state ret;

	ret = E2E_SMConfig(NULL, 5, 3, 3, 3, 1, 1, 1);
	EXPECT_EQ(ret, E2E_E_INPUTERR_NULL);

	ret = E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(config.windowSize, 5);
	EXPECT_EQ(config.maxErrorStateInit, 3);
	EXPECT_EQ(config.maxErrorStateInvalid, 3);
	EXPECT_EQ(config.maxErrorStateValid, 3);
	EXPECT_EQ(config.minOkStateInit, 1);
	EXPECT_EQ(config.minOkStateInvalid, 1);
	EXPECT_EQ(config.minOkStateValid, 1);
}

TEST(E2E_SM, E2E_SMTransition) {
	struct e2e_config config;
	e2e_sm_state smState;
	e2e_error_state ret;

	E2E_SMTransition(&config, &smState, E2E_SM_VALID);
	EXPECT_EQ(smState.smState, E2E_SM_VALID);

	E2E_SMTransition(&config, &smState, E2E_SM_INVALID);
	EXPECT_EQ(smState.smState, E2E_SM_INVALID);
}

TEST(E2E_SM, E2E_NumberOfOKElementsInWindow) {
	struct e2e_config config;
	e2e_sm_state smState;
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);

	EXPECT_EQ(E2E_NumberOfOKElementsInWindow(&config, &smState), 0);

	smState.profileStatusWindow[0] = E2E_P_OK;
	EXPECT_EQ(E2E_NumberOfOKElementsInWindow(&config, &smState), 1);

	smState.profileStatusWindow[0] = E2E_P_OK;
	smState.profileStatusWindow[1] = E2E_P_ERROR;
	EXPECT_EQ(E2E_NumberOfOKElementsInWindow(&config, &smState), 1);

	smState.profileStatusWindow[0] = E2E_P_ERROR;
	EXPECT_EQ(E2E_NumberOfOKElementsInWindow(&config, &smState), 0);

	smState.profileStatusWindow[0] = E2E_P_OK;
	smState.profileStatusWindow[1] = E2E_P_OK;
	EXPECT_EQ(E2E_NumberOfOKElementsInWindow(&config, &smState), 2);
}

TEST(E2E_SM, E2E_NumberOfErrorElementsInWindow) {
	struct e2e_config config;
	e2e_sm_state smState;
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);

	EXPECT_EQ(E2E_NumberOfErrorElementsInWindow(&config, &smState), 0);

	smState.profileStatusWindow[0] = E2E_P_ERROR;
	EXPECT_EQ(E2E_NumberOfErrorElementsInWindow(&config, &smState), 1);

	smState.profileStatusWindow[0] = E2E_P_ERROR;
	smState.profileStatusWindow[1] = E2E_P_OK;
	EXPECT_EQ(E2E_NumberOfErrorElementsInWindow(&config, &smState), 1);

	smState.profileStatusWindow[0] = E2E_P_OK;
	EXPECT_EQ(E2E_NumberOfErrorElementsInWindow(&config, &smState), 0);

	smState.profileStatusWindow[0] = E2E_P_ERROR;
	smState.profileStatusWindow[1] = E2E_P_ERROR;
	EXPECT_EQ(E2E_NumberOfErrorElementsInWindow(&config, &smState), 2);
}

TEST(E2E_SM, E2E_SMInit) {
	struct e2e_config config;
	e2e_sm_state smState;
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);
	EXPECT_EQ(smState.smState, E2E_SM_DEINIT);
	EXPECT_EQ(E2E_NumberOfOKElementsInWindow(&config, &smState), 0);
	EXPECT_EQ(E2E_NumberOfErrorElementsInWindow(&config, &smState), 0);
}

TEST(E2E_SM, E2E_SMCheck) {
	struct e2e_config config;
	e2e_sm_state smState;
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);
	FAIL();
}

TEST(E2E_SM, E2E_SMClearStatus) {
	struct e2e_config config;
	e2e_sm_state smState;
	e2e_error_state ret;
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);

	EXPECT_EQ(E2E_SMClearStatus(NULL, &smState), E2E_E_INPUTERR_NULL);
	EXPECT_EQ(E2E_SMClearStatus(&config, NULL), E2E_E_INPUTERR_NULL);

	ret = E2E_SMClearStatus(&config, &smState);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(smState.okCount, 0);
	EXPECT_EQ(smState.errorCount, 0);
	EXPECT_EQ(smState.windowTopIndex, 0);
	for (int i = 0u; i < config.windowSize; i++)
	{
		EXPECT_EQ(smState.profileStatusWindow[i], E2E_P_NOTAVAILABLE);
	}
}

TEST(E2E_SM, E2E_SMCheckInit) {
	struct e2e_config config;
	e2e_sm_state smState;
	e2e_error_state ret;
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);

	EXPECT_EQ(E2E_SMCheckInit(NULL, &smState), E2E_E_INPUTERR_NULL);
	EXPECT_EQ(E2E_SMCheckInit(&config, NULL), E2E_E_INPUTERR_NULL);

	ret = E2E_SMCheckInit(&config, &smState);
	EXPECT_EQ(ret, E2E_E_OK);
	EXPECT_EQ(smState.smState, E2E_SM_NODATA);
}

TEST(E2E_SM, E2E_SMAddStatus) {
	struct e2e_config config;
	e2e_sm_state smState;
	e2e_error_state ret;
	E2E_SMConfig(&config, 5, 3, 3, 3, 1, 1, 1);
	E2E_SMInit(&smState, &config);

	EXPECT_EQ(smState.okCount, 0);
	EXPECT_EQ(smState.errorCount, 0);

	E2E_SMAddStatus(E2E_P_OK, &config, &smState);
	EXPECT_EQ(smState.okCount, 1);
	EXPECT_EQ(smState.errorCount, 0);

	E2E_SMAddStatus(E2E_P_ERROR, &config, &smState);
	EXPECT_EQ(smState.okCount, 1);
	EXPECT_EQ(smState.errorCount, 1);

	E2E_SMAddStatus(E2E_P_OK, &config, &smState);
	EXPECT_EQ(smState.okCount, 2);
	EXPECT_EQ(smState.errorCount, 1);

	E2E_SMAddStatus(E2E_P_OK, &config, &smState);
	E2E_SMAddStatus(E2E_P_OK, &config, &smState);
	E2E_SMAddStatus(E2E_P_OK, &config, &smState);
	EXPECT_EQ(smState.okCount, 4);
	EXPECT_EQ(smState.errorCount, 1);

	E2E_SMAddStatus(E2E_P_OK, &config, &smState);
	EXPECT_EQ(smState.okCount, 5);
	EXPECT_EQ(smState.errorCount, 0);

	E2E_SMAddStatus(E2E_P_ERROR, &config, &smState);
	EXPECT_EQ(smState.okCount, 4);
	EXPECT_EQ(smState.errorCount, 1);

	E2E_SMAddStatus(E2E_P_NOTAVAILABLE, &config, &smState);
	E2E_SMAddStatus(E2E_P_NONEWDATA, &config, &smState);
	E2E_SMAddStatus(E2E_P_WRONGSEQUENCE, &config, &smState);
	E2E_SMAddStatus(E2E_P_REPEATED, &config, &smState);
	EXPECT_EQ(smState.okCount, 0);
	EXPECT_EQ(smState.errorCount, 3);
}