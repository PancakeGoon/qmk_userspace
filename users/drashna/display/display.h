// Copyright 2024 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <stdio.h>
#include "action.h"
#include "progmem.h"

extern bool               keylogger_has_changed;
extern bool               layer_map_has_updated;
extern const char PROGMEM code_to_name[256];

bool process_record_display_driver(uint16_t keycode, keyrecord_t* record);
void keyboard_post_init_display_driver(void);

#ifdef DISPLAY_KEYLOGGER_ENABLE
#    ifndef DISPLAY_KEYLOGGER_LENGTH
#        if defined(OLED_ENABLE)
#            if defined(OLED_DISPLAY_128X128) || defined(OLED_DISPLAY_128X64)
#                define DISPLAY_KEYLOGGER_LENGTH 14
#            else
#                define DISPLAY_KEYLOGGER_LENGTH 5
#            endif // OLED_DISPLAY_128X128 || OLED_DISPLAY_128X64
#        else      // OLED_ENABLE
#            define DISPLAY_KEYLOGGER_LENGTH 20
#        endif // DISPLAY_KEYLOGGER_ENABLE
#    endif     // DISPLAY_KEYLOGGER_LENGTH
extern char display_keylogger_string[DISPLAY_KEYLOGGER_LENGTH + 1];
extern bool keylogger_has_changed;
#endif
