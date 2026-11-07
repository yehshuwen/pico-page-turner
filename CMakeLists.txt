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
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2_w CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(page_turner C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(page_turner page_turner.c )

pico_set_program_name(page_turner "page_turner")
pico_set_program_version(page_turner "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(page_turner 0)
pico_enable_stdio_usb(page_turner 1)

# Add the standard library to the build
target_link_libraries(page_turner
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(page_turner PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(page_turner 
        pico_cyw43_arch_none
        pico_btstack_ble        # BTstack 的 BLE 功能
        pico_btstack_cyw43      # BTstack 和 CYW43 無線晶片之間的橋接
        )

pico_add_extra_outputs(page_turner)

# second exe: polling version
# build/button_poll.uf2
add_executable(button_poll button_poll.c)

target_link_libraries(button_poll
    pico_stdlib            # 基本功能
    pico_cyw43_arch_none   # 控制板載 LED 需要
)

pico_enable_stdio_usb(button_poll 1)    # printf 走 USB
pico_enable_stdio_uart(button_poll 0)

pico_add_extra_outputs(button_poll)     # 產生 .uf2