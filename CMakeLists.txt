cmake_minimum_required(VERSION 3.16)

project(OBC2 C CXX ASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(COMMON_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
)
set(COMPILE_FLAGS ${COMMON_FLAGS} -fdata-sections -ffunction-sections)
set(DEBUG_FLAGS "-g;-gdwarf-2;-Og")
set(RELEASE_FLAGS "-O2;-DNDEBUG")
set(LINK_FLAGS
    ${COMMON_FLAGS}
    -specs=nosys.specs
    -T${CMAKE_SOURCE_DIR}/nucleo_l476rg/STM32L476RGTx_FLASH.ld
    -lc
    -lm
    -lnosys 
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)
set(DEFINITIONS -DUSE_HAL_DRIVER -DSTM32L476xx)

set(HAL_DIR nucleo_l476rg/Drivers/STM32L4xx_HAL_Driver)
set(CMSIS_DIR nucleo_l476rg/Drivers/CMSIS)
set(CORE_DIR nucleo_l476rg/Core)

include_directories(
    ${HAL_DIR}/Inc
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/Device/ST/STM32L4xx/Include
    ${CORE_DIR}/Inc
)

add_compile_options(${COMPILE_FLAGS})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_compile_options(${DEBUG_FLAGS})
else ()
    add_compile_options(${RELEASE_FLAGS})
endif ()

add_link_options(${LINK_FLAGS})
add_definitions(${DEFINITIONS})

add_subdirectory(lib)
add_subdirectory(src)

aux_source_directory(${HAL_DIR}/Src/ HAL_SRC)
aux_source_directory(${CORE_DIR}/Src/ CORE_SRC)

add_executable(
    nucleo_l476rg
        ${HAL_SRC}
        ${CORE_SRC}
        nucleo_l476rg/startup_stm32l476xx.s
)

target_link_libraries(
    nucleo_l476rg
    PRIVATE
        obc2_lib
)