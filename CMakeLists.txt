cmake_minimum_required(VERSION 3.16)

project(OBC2 C CXX ASM)

set(CMAKE_CXX_STANDARD 17)
set(COMMON_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
)

add_compile_options(${COMMON_FLAGS})
add_link_options(${COMMON_FLAGS})

add_subdirectory(lib)

set(HAL_DIR nucleo_l476rg/Drivers/STM32L4xx_HAL_Driver)
set(CMSIS_DIR nucleo_l476rg/Drivers/CMSIS)
set(CORE_DIR nucleo_l476rg/Core)

aux_source_directory(${HAL_DIR}/Src/ HAL_SRC)
aux_source_directory(${CORE_DIR}/Src/ CORE_SRC)

add_executable(
    ${PROJECT_NAME}
        ${HAL_SRC}
        ${CORE_SRC}
        nucleo_l476rg/startup_stm32l476xx.s
        src/run.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
        ${HAL_DIR}/Inc
        ${CMSIS_DIR}/Include
        ${CMSIS_DIR}/Device/ST/STM32L4xx/Include
        ${CORE_DIR}/Inc
        src
)

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE
        -fdata-sections
        -ffunction-sections
        $<$<CONFIG:Debug>:-g -gdwarf-2 -Og, -O2 -DNDEBUG>
)

target_link_options(
    ${PROJECT_NAME}
    PRIVATE
        -specs=nosys.specs
        -T${CMAKE_SOURCE_DIR}/nucleo_l476rg/STM32L476RGTX_FLASH.ld
        -lc
        -lm
        -lnosys 
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
)
        
target_compile_definitions(
    ${PROJECT_NAME}
    PRIVATE
        USE_HAL_DRIVER
        STM32L476xx
)

target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
        ccl
)