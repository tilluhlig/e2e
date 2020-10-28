add_executable(testcase testUtils.cpp test_E2E.cpp test_E2E_P11.cpp test_E2E_PW.cpp test_E2E_SM.cpp $<TARGET_OBJECTS:e2e>)
target_link_libraries(testcase CONAN_PKG::gtest)
add_test(NAME testcase COMMAND $<TARGET_FILE:testcase>)
