cmake_minimum_required(VERSION 3.13)
# set(CMAKE_BUILD_TYPE Debug)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)
project(my_project)
# target_include_directories(my_project PRIVATE ${CMAKE_CURRENT_LIST_DIR}/lib)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

if (TARGET tinyusb_device)
    add_executable(pio_testing)
    
    pico_generate_pio_header(pio_testing ${CMAKE_CURRENT_LIST_DIR}/pio/jump.pio)

    target_sources(pio_testing PRIVATE pio_testing.cpp)

    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(pio_testing PRIVATE pico_stdlib hardware_pio)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(pio_testing 1)
    pico_enable_stdio_uart(pio_testing 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(pio_testing)

    # # add url via pico_set_program_url
    # example_auto_set_url(pio_testing)
elseif(PICO_ON_DEVICE)
    message(WARNING "not building pio_testing because TinyUSB submodule is not initialized in the SDK")
endif()