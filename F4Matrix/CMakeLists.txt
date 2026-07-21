cmake_minimum_required(VERSION 3.2.0)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Set cross compiler
project(F4Matrix C ASM)
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")
find_program(OBJCOPY "arm-none-eabi-objcopy")
find_program(ST_FLASH "st-flash")

set(STM32F4_DSP_PERIPH_LIB "${CMAKE_CURRENT_SOURCE_DIR}/STM32F4xx_DSP_StdPeriph_Lib_V1.9.0")
set(STM32F4_STDPERIPH_DRIVER_DIR "${STM32F4_DSP_PERIPH_LIB}/Libraries/STM32F4xx_StdPeriph_Driver")
set(CMSIS_DIR "${STM32F4_DSP_PERIPH_LIB}/Libraries/CMSIS")
set(MATRIX_FILES
    src/colorcorr.c
    src/main.c
    src/control_uart.c
    src/framebuffer.c
    src/main.c
    src/matrix.c
    src/testimage.c
    src/stm32f4xx_it.c
    src/system_stm32f4xx.c
    )

# set(STARTUP_FILES
#     startup_src/startup_stm32f40xx.S
#     )
set(STARTUP_FILES
    startup_src/startup_stm32f429xx.s
    )

set(STDPERIPH_FUNCTIONS
    "rcc"
    "usart"
    "dma"
    "tim"
    "gpio"
    )

set(STDPERIPH_FILES "${STM32F4_STDPERIPH_DRIVER_DIR}/src/misc.c")
foreach(STDPERIPH_FUNCTION ${STDPERIPH_FUNCTIONS})
    list(APPEND STDPERIPH_FILES ${STM32F4_STDPERIPH_DRIVER_DIR}/src/stm32f4xx_${STDPERIPH_FUNCTION}.c)
endforeach()

# get_filename_component(LINKERSCRIPT startup_src/stm32_flash.ld ABSOLUTE)
get_filename_component(LINKERSCRIPT startup_src/STM32F429ZI_FLASH.ld ABSOLUTE)

# Set linker script
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKERSCRIPT}")
# Set gc-sections to cleanup unused sections
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
# Processor features

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mcpu=${CMAKE_SYSTEM_PROCESSOR} -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=softfp")

# add_definitions(-DSTM32F40_41xxx)
# add_definitions(-DHSE_VALUE=80000000)

# Set USE_HSE_BYPASS for NUCLEO-F429ZI board! ST-LINK supplies an external 8 MHz clock to PH0/PF0-OSC_IN pin.
add_definitions(-DUSE_HSE_BYPASS)
add_definitions(-DHSE_VALUE=8000000)
add_definitions(-DSTM32F429_439xx)
add_definitions(-DUSE_STDPERIPH_DRIVER)

add_executable(F4Matrix
    ${MATRIX_FILES}
    ${STARTUP_FILES}
    ${STDPERIPH_FILES}
)

target_include_directories(F4Matrix PRIVATE
    src
    ${STM32F4_STDPERIPH_DRIVER_DIR}/inc
    ${CMSIS_DIR}/Include
    ${CMSIS_DIR}/Device/ST/STM32F4xx/Include
)

target_link_libraries(F4Matrix PRIVATE
    m
    c
    gcc
)

add_custom_target(F4Matrix.bin ALL
    COMMAND ${OBJCOPY} -S -O binary F4Matrix F4Matrix.bin
    DEPENDS F4Matrix
    )

add_custom_target(flash
    COMMAND ${ST_FLASH} write F4Matrix.bin 0x08000000
    DEPENDS F4Matrix.bin
    )
