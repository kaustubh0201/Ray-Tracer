set(BINARY ${CMAKE_PROJECT_NAME}e)

file(GLOB_RECURSE SRC_FILES ./sources/*.c)

set(SOURCES ${SRC_FILES})

# release 
add_library(${LIB_NAME} STATIC ${SOURCES})
target_link_libraries(${LIB_NAME} m OpenMP::OpenMP_C)
#target_link_options(${LIB_NAME} PUBLIC -fsanitize=undefined)
target_compile_options(${LIB_NAME} PUBLIC -Wall -Werror -Wpedantic -Wextra -DNDEBUG -O3)
target_include_directories(${LIB_NAME} PUBLIC ${SOURCE_DIR}/include)

add_executable(${BINARY} main.c)
target_link_libraries(${BINARY} m ${LIB_NAME} OpenMP::OpenMP_C)
#target_link_options(${BINARY} PUBLIC -fsanitize=undefined)
target_compile_options(${BINARY} PUBLIC -Wall -Werror -Wpedantic -Wextra -DNDEBUG -O3)
target_include_directories(${BINARY} PUBLIC ${SOURCE_DIR}/include)

# debug
set(DBG_BINARY ${BINARY}_dbg)

add_library(${DBG_LIB_NAME} STATIC ${SOURCES})
target_link_libraries(${DBG_LIB_NAME} m OpenMP::OpenMP_C)
target_link_options(${DBG_LIB_NAME} PUBLIC -fsanitize=undefined,address)
target_compile_options(${DBG_LIB_NAME} PUBLIC -Wall -Werror -Wpedantic -Wextra -fsanitize=undefined,address -g)
target_include_directories(${DBG_LIB_NAME} PUBLIC ${SOURCE_DIR}/include)

add_executable(${DBG_BINARY} main.c)
target_link_libraries(${DBG_BINARY} m ${DBG_LIB_NAME} OpenMP::OpenMP_C)
target_link_options(${DBG_BINARY} PUBLIC -fsanitize=undefined,address)
target_compile_options(${DBG_BINARY} PUBLIC -Wall -Werror -Wpedantic -Wextra -fsanitize=undefined,address -g)
target_include_directories(${DBG_BINARY} PUBLIC ${SOURCE_DIR}/include)
