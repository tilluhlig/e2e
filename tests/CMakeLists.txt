add_executable(
    unit_tests
    test_E2E.cpp
    test_E2E_P11.cpp
    test_E2E_PW.cpp
    test_E2E_SM.cpp
    )

target_link_libraries(
    unit_tests
    gtest_main
    e2e
    )

add_test(
  NAME
    unit
  COMMAND
    ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/unit_tests
  )
