set(LIB_NAME ccl)

add_library(
    ${LIB_NAME}
    STATIC
        src/panic.cpp
)

target_include_directories(${LIB_NAME} PUBLIC include)

target_compile_options(${LIB_NAME} PUBLIC -fconcepts)