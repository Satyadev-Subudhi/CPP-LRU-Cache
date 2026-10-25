cmake_minimum_required(VERSION 3.16)

project(LRUCacheProject LANGUAGES CXX)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)

if(ENABLE_ASAN)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
        )
        add_link_options(-fsanitize=address)
    else()
        message(FATAL_ERROR "AddressSanitizer is unsupported by this configuration")
    endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lru_cache
    src/main.cpp
)

target_include_directories(lru_cache PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

enable_testing()

add_executable(lru_tests
    tests/test_lru.cpp
)

target_include_directories(lru_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_test(NAME LRUCacheTests COMMAND lru_tests)