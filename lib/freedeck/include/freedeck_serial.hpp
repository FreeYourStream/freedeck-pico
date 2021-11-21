#pragma once
#include <pico/stdlib.h>
#include <stdio.h>
uint32_t read_serial_binary(void);
void write_serial_line(const char *line);
void write_serial(const char *line);
void write_serial_char(const char chr);
void cdc_task(void);