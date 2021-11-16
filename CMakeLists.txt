cmake_minimum_required(VERSION 3.13)
include(./DownloadProject/DownloadProject.cmake)
# set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)
project(freedeck-pico C CXX ASM)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

download_project(
  PROJ fatfs_pico
  GIT_REPOSITORY https://github.com/carlk3/no-OS-FatFS-SD-SPI-RPi-Pico.git
  GIT_TAG        fe71973accb34783840ad59443c023e4b5eccf03
  UPDATE_DISCONNECTED 1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# rest of your project

if (TARGET tinyusb_device)
    
    add_library(settings INTERFACE)
    target_include_directories(settings INTERFACE ${CMAKE_CURRENT_LIST_DIR})

    add_executable(${PROJECT_NAME} main.cpp)
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
    add_subdirectory(lib/freedeck)
    add_subdirectory(lib/ssd1306)
    add_subdirectory(lib/usb_com)
    # add_subdirectory(lib/no-OS-FatFS/FatFs_SPI)
    add_subdirectory(${fatfs_pico_SOURCE_DIR}/FatFs_SPI ${fatfs_pico_BINARY_DIR})
    

    # pico_generate_pio_header(${PROJECT_NAME})

    # target_sources(${PROJECT_NAME} PRIVATE main.cpp)
    # target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    # Pull in our pico_stdlib which aggregates commonly used features
    target_link_libraries(${PROJECT_NAME} PUBLIC pico_stdlib pico_multicore hardware_i2c ssd1306 usb_com freedeck_lib )

    # enable usb output, disable uart output
    # pico_enable_stdio_usb(${PROJECT_NAME} 1)
    # pico_enable_stdio_uart(${PROJECT_NAME} 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(${PROJECT_NAME})

    # # add url via pico_set_program_url
    # example_auto_set_url(${PROJECT_NAME})
elseif(PICO_ON_DEVICE)
    message(WARNING "not building ${PROJECT_NAME} because TinyUSB submodule is not initialized in the SDK")
endif()