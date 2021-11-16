#pragma once
#include "pico/stdlib.h"
#include <stdio.h>
uint32_t readSerialBinary(void);
void writeSerialLine(const char *line);
void writeSerial(const char *line);
void writeSerialChar(const char chr);
void cdc_task(void);