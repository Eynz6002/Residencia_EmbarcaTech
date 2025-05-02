# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(U2_C3_Question2 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(U2_C3_Question2 U2_C3_Question2.c source/adc_controller.c)

pico_set_program_name(U2_C3_Question2 "U2_C3_Question2")
pico_set_program_version(U2_C3_Question2 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(U2_C3_Question2 0)
pico_enable_stdio_usb(U2_C3_Question2 1)

# Add the standard include files to the build
target_include_directories(U2_C3_Question2 PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}libs
        ${CMAKE_CURRENT_LIST_DIR}source
        ${PICO_SDK_PATH}/lib/lwip/src/include
        ${PICO_SDK_PATH}/lib/lwip/src/include/arch
        ${PICO_SDK_PATH}/lib/lwip/src/include/lwip
)

# Add any user requested libraries
target_link_libraries(U2_C3_Question2 
        hardware_adc
        pico_stdlib
        pico_cyw43_arch_lwip_threadsafe_background
        )


target_sources(U2_C3_Question2 PRIVATE
        ${PICO_SDK_PATH}/lib/lwip/src/apps/http/httpd.c
        ${PICO_SDK_PATH}/lib/lwip/src/apps/http/fs.c
)
pico_add_extra_outputs(U2_C3_Question2)