set(ROOT_BIN_NAME ${CMAKE_PROJECT_NAME})

file(GLOB_RECURSE TEST_SRC_FILES ./*.c)


foreach(TEST ${TEST_SRC_FILES})
    get_filename_component(TEST_NAME ${TEST} NAME_WE)
    set(BINARY ${ROOT_BIN_NAME}_${TEST_NAME})

    add_executable(${BINARY} ${TEST})

    add_test(NAME ${BINARY} COMMAND ${BINARY})

    target_link_libraries(${BINARY} m ${DBG_LIB_NAME} CONAN_PKG::libcheck)

    target_compile_options(${BINARY} PUBLIC -Wall -Werror -Wpedantic -Wextra -g)
    target_include_directories(${BINARY} PUBLIC ${SOURCE_DIR}/include)
endforeach()

