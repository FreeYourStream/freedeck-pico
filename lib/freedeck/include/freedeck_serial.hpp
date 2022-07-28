#pragma once
#include <pico/stdlib.h>
#include <stdio.h>
uint32_t read_serial_binary(void);
void write_serial_line(const char *line);
void write_serial(const char *line);
void write_serial(char byte);
void write_serial_number(uint32_t number);
void cdc_task(void);