add_library(e2e "")

target_sources(
    e2e
    PRIVATE
        e2e.c
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/e2e.h
    )

target_include_directories(
    e2e
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
    )
