cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
set(PICO_SDK_PATH ~/opt/pico-sdk)
# set(PICO_BOARD seeed_xiao_rp2040)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_BUILD_TYPE Debug)
#set(LINKER_SCRIPT memmap_mod.ld)
set(MY_TARGET firmware)


include(pico_sdk_import.cmake)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(${MY_TARGET}
   src/main.c
   src/sim7600.c
)

#Enable printing over USB and disable printing over UART
pico_enable_stdio_usb(${MY_TARGET} 1)
pico_enable_stdio_uart(${MY_TARGET} 0)


# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${MY_TARGET} pico_stdlib hardware_gpio 
hardware_uart
hardware_dma
hardware_irq
)



target_include_directories(${MY_TARGET} PUBLIC lib)


# Use custom linker script
#pico_set_linker_script(${MY_TARGET} ${LINKER_SCRIPT})

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(${MY_TARGET})
