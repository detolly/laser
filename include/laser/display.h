#pragma once

#include <stddef.h>

void display_init();
void display_write_string(const char* top, size_t top_len,
                          const char* bottom, size_t bottom_len);
