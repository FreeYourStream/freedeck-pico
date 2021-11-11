file(GLOB FILES src/*.cpp include/*.hpp)

add_library(ssd1306 ${FILES})
target_include_directories(ssd1306 PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_link_libraries(ssd1306 hardware_i2c pico_stdlib)