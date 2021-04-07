cmake_minimum_required(VERSION 3.13)
# set(CMAKE_BUILD_TYPE Debug)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

if (TARGET tinyusb_device)
    add_executable(i2c_7seg)
    
    pico_generate_pio_header(i2c_7seg ${CMAKE_CURRENT_LIST_DIR}/pio/tm1637.pio)

    target_sources(i2c_7seg PRIVATE i2c_7seg.cpp)

    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(i2c_7seg PRIVATE pico_stdlib hardware_pio)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(i2c_7seg 1)
    pico_enable_stdio_uart(i2c_7seg 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(i2c_7seg)

    # # add url via pico_set_program_url
    # example_auto_set_url(i2c_7seg)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building i2c_7seg because TinyUSB submodule is not initialized in the SDK")
endif()