// Copyright 2018-2024 Nick Brassel (@tzarc)
// SPDX-License-Identifier: GPL-2.0-or-later
#include "drashna.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <printf.h>
#include <qp.h>
#include "display/painter/painter.h"
#include "process_keycode/process_unicode_common.h"
#include "unicode.h"

typedef enum _menu_flags_t {
    menu_flag_is_parent = (1 << 0),
    menu_flag_is_value  = (1 << 1),
} menu_flags_t;

typedef enum _menu_input_t {
    menu_input_exit,
    menu_input_back,
    menu_input_enter,
    menu_input_up,
    menu_input_down,
    menu_input_left,
    menu_input_right,
} menu_input_t;

typedef struct _menu_entry_t {
    menu_flags_t flags;
    const char  *text;
    union {
        struct {
            struct _menu_entry_t *children;
            size_t                child_count;
        } parent;
        struct {
            bool (*menu_handler)(menu_input_t input);
            void (*display_handler)(char *text_buffer, size_t buffer_len);
        } child;
    };
} menu_entry_t;

static bool menu_handler_unicode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            unicode_input_mode_step_reverse();
            return false;
        case menu_input_right:
            unicode_input_mode_step();
            return false;
        default:
            return true;
    }
}

void display_handler_unicode(char *text_buffer, size_t buffer_len) {
    switch (get_unicode_input_mode()) {
        case UNICODE_MODE_MACOS:
            strncpy(text_buffer, "macOS", buffer_len - 1);
            return;
        case UNICODE_MODE_LINUX:
            strncpy(text_buffer, "Linux", buffer_len - 1);
            return;
        case UNICODE_MODE_BSD:
            strncpy(text_buffer, "BSD", buffer_len - 1);
            return;
        case UNICODE_MODE_WINDOWS:
            strncpy(text_buffer, "Windows", buffer_len - 1);
            return;
        case UNICODE_MODE_WINCOMPOSE:
            strncpy(text_buffer, "WinCompose", buffer_len - 1);
            return;
        case UNICODE_MODE_EMACS:
            strncpy(text_buffer, "Emacs", buffer_len - 1);
            return;
    }

    strncpy(text_buffer, "Unknown", buffer_len);
}
static bool menu_handler_unicode_typing(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            unicode_typing_mode = (unicode_typing_mode - 1) % UNICODE_MODE_COUNT;
            return false;
        case menu_input_right:
            unicode_typing_mode = (unicode_typing_mode + 1) % UNICODE_MODE_COUNT;
            return false;
        default:
            return true;
    }
}
void display_handler_unicode_typing(char *text_buffer, size_t buffer_len) {
    switch (unicode_typing_mode) {
        case UCTM_NO_MODE:
            strncpy(text_buffer, "Normal", buffer_len - 1); // Normal
            return;
        case UCTM_WIDE:
            strncpy(text_buffer, "Wide", buffer_len - 1); // ｗｉｄｅ
            return;
        case UCTM_SCRIPT:
            strncpy(text_buffer, "Script", buffer_len - 1); // 𝓢𝓬𝓻𝓲𝓹𝓽
            return;
        case UCTM_BLOCKS:
            strncpy(text_buffer, "Blocks", buffer_len - 1); // 🅱🅻🅾🅲🅺🆂
            return;
        case UCTM_REGIONAL:
            strncpy(text_buffer, "Regional",
                    buffer_len - 1); // 🇷‌‌🇪‌‌🇬‌‌🇮‌‌🇴‌‌🇳‌‌🇦‌‌🇱‌‌
            return;
        case UCTM_AUSSIE:
            strncpy(text_buffer, "Aussie", buffer_len - 1); // ǝᴉssnɐ
            return;
        case UCTM_ZALGO:
            strncpy(text_buffer, "Zalgo", buffer_len - 1); // z̴̬̙̐̋͢ā̸̧̺͂ͥ͐͟l̵̪̻͎̈ͭ̋͠g̦ͥo͚ͫͣ
            return;
        case UCTM_SUPER:
            strncpy(text_buffer, "SuperScript", buffer_len - 1); // ˢᵘᵖᵉʳˢᶜʳᶦᵖᵗ
            return;
        case UCTM_COMIC:
            strncpy(text_buffer, "Comic", buffer_len - 1); // ƈơɱıƈ
            return;
        case UCTM_FRAKTUR:
            strncpy(text_buffer, "Fraktur", buffer_len - 1); // 𝔉𝔯𝔞𝔨𝔱𝔲𝔯
            return;
        case UCTM_DOUBLE_STRUCK:
            strncpy(text_buffer, "Double Struck", buffer_len - 1); // 𝕯𝖔𝖚𝖇𝖑𝖊 𝕾𝖙𝖗𝖚𝖈𝖐
            return;
    }

    strncpy(text_buffer, "Unknown", buffer_len);
}

static bool menu_handler_rgbenabled(menu_input_t input) {
    switch (input) {
        case menu_input_left:
        case menu_input_right:
            rgb_matrix_toggle();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbenabled(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%s", rgb_matrix_is_enabled() ? "on" : "off");
}

static bool menu_handler_rgbmode(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_step_reverse();
            return false;
        case menu_input_right:
            rgb_matrix_step();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbmode(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_mode());
}

static bool menu_handler_rgbhue(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_hue();
            return false;
        case menu_input_right:
            rgb_matrix_increase_hue();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbhue(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_hue());
}

static bool menu_handler_rgbsat(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_sat();
            return false;
        case menu_input_right:
            rgb_matrix_increase_sat();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbsat(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_sat());
}

static bool menu_handler_rgbval(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_val();
            return false;
        case menu_input_right:
            rgb_matrix_increase_val();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbval(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_val());
}

static bool menu_handler_rgbspeed(menu_input_t input) {
    switch (input) {
        case menu_input_left:
            rgb_matrix_decrease_speed();
            return false;
        case menu_input_right:
            rgb_matrix_increase_speed();
            return false;
        default:
            return true;
    }
}

void display_handler_rgbspeed(char *text_buffer, size_t buffer_len) {
    snprintf(text_buffer, buffer_len - 1, "%d", (int)rgb_matrix_get_speed());
}

menu_entry_t unicode_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Unicode mode",
        .child.menu_handler    = menu_handler_unicode,
        .child.display_handler = display_handler_unicode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Unicode Typing Mode",
        .child.menu_handler    = menu_handler_unicode_typing,
        .child.display_handler = display_handler_unicode_typing,
    },
};

menu_entry_t rgb_matrix_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB enabled",
        .child.menu_handler    = menu_handler_rgbenabled,
        .child.display_handler = display_handler_rgbenabled,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB mode",
        .child.menu_handler    = menu_handler_rgbmode,
        .child.display_handler = display_handler_rgbmode,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB hue",
        .child.menu_handler    = menu_handler_rgbhue,
        .child.display_handler = display_handler_rgbhue,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB saturation",
        .child.menu_handler    = menu_handler_rgbsat,
        .child.display_handler = display_handler_rgbsat,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB value",
        .child.menu_handler    = menu_handler_rgbval,
        .child.display_handler = display_handler_rgbval,
    },
    {
        .flags                 = menu_flag_is_value,
        .text                  = "RGB speed",
        .child.menu_handler    = menu_handler_rgbspeed,
        .child.display_handler = display_handler_rgbspeed,
    },
};

menu_entry_t root_entries[] = {
    {
        .flags                 = menu_flag_is_value,
        .text                  = "Unicode mode",
        .child.menu_handler    = menu_handler_unicode,
        .child.display_handler = display_handler_unicode,
    },
    {
        .flags              = menu_flag_is_parent,
        .text               = "Unicode Settings",
        .parent.children    = unicode_entries,
        .parent.child_count = ARRAY_SIZE(unicode_entries),
    },
    {
        .flags              = menu_flag_is_parent,
        .text               = "RGB Matrix Settings",
        .parent.children    = rgb_matrix_entries,
        .parent.child_count = ARRAY_SIZE(rgb_matrix_entries),
    },
};

menu_entry_t root = {
    .flags              = menu_flag_is_parent,
    .text               = "Configuration",
    .parent.children    = root_entries,
    .parent.child_count = ARRAY_SIZE(root_entries),
};

typedef struct _menu_state_t {
    bool    dirty;
    bool    is_in_menu;
    uint8_t selected_child;
    uint8_t menu_stack[8];
} menu_state_t;

static menu_state_t state = {.dirty          = false,
                             .is_in_menu     = false,
                             .menu_stack     = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                             .selected_child = 0xFF};

menu_entry_t *get_current_menu(void) {
    if (state.selected_child == 0xFF) {
        return NULL;
    }

    menu_entry_t *entry = &root;
    for (int i = 0; i < sizeof(state.menu_stack); ++i) {
        if (state.menu_stack[i] == 0xFF) {
            return entry;
        }
        entry = &entry->parent.children[state.menu_stack[i]];
    }

    return entry;
}

menu_entry_t *get_selected_menu_item(void) {
    return &(get_current_menu()->parent.children[state.selected_child]);
}

bool menu_handle_input(menu_input_t input) {
    menu_entry_t *menu     = get_current_menu();
    menu_entry_t *selected = get_selected_menu_item();
    switch (input) {
        case menu_input_exit:
            state.is_in_menu = false;
            memset(state.menu_stack, 0xFF, sizeof(state.menu_stack));
            state.selected_child = 0xFF;
            return false;
        case menu_input_back:
            // Iterate backwards through the stack and remove the last entry
            for (int i = 0; i < sizeof(state.menu_stack); ++i) {
                if (state.menu_stack[sizeof(state.menu_stack) - 1 - i] != 0xFF) {
                    state.selected_child = state.menu_stack[sizeof(state.menu_stack) - 1 - i];
                    state.menu_stack[sizeof(state.menu_stack) - 1 - i] = 0xFF;
                    break;
                }

                // If we've dropped out of the last entry in the stack, exit the menu
                if (i == sizeof(state.menu_stack) - 1) {
                    state.is_in_menu     = false;
                    state.selected_child = 0xFF;
                }
            }
            return false;
        case menu_input_enter:
            // Only attempt to enter the next menu if we're a parent object
            if (selected->flags & menu_flag_is_parent) {
                // Iterate forwards through the stack and add the selected entry
                for (int i = 0; i < sizeof(state.menu_stack); ++i) {
                    if (state.menu_stack[i] == 0xFF) {
                        state.menu_stack[i]  = state.selected_child;
                        state.selected_child = 0;
                        break;
                    }
                }
            }
            return false;
        case menu_input_up:
            state.selected_child = (state.selected_child + menu->parent.child_count - 1) % menu->parent.child_count;
            return false;
        case menu_input_down:
            state.selected_child = (state.selected_child + menu->parent.child_count + 1) % menu->parent.child_count;
            return false;
        case menu_input_left:
        case menu_input_right:
            if (selected->flags & menu_flag_is_value) {
                state.dirty = true;
                return selected->child.menu_handler(input);
            }
            return false;
        default:
            return false;
    }
}

bool process_record_menu(uint16_t keycode, keyrecord_t *record) {
    if (keycode == DISPLAY_MENU && record->event.pressed && !state.is_in_menu) {
        state.is_in_menu     = true;
        state.selected_child = 0;
        return false;
    }

    if (state.is_in_menu) {
        if (record->event.pressed) {
            switch (keycode) {
                case DISPLAY_MENU:
                    return menu_handle_input(menu_input_exit);
                case KC_ESC:
                    return menu_handle_input(menu_input_back);
                case KC_SPACE:
                case KC_ENTER:
                case KC_RETURN:
                    return menu_handle_input(menu_input_enter);
                case KC_UP:
                case KC_W:
                    return menu_handle_input(menu_input_up);
                case KC_DOWN:
                case KC_S:
                    return menu_handle_input(menu_input_down);
                case KC_LEFT:
                case KC_A:
                    return menu_handle_input(menu_input_left);
                case KC_RIGHT:
                case KC_D:
                    return menu_handle_input(menu_input_right);
                default:
                    return false;
            }
        }
        return false;
    }

    return true;
}

extern painter_font_handle_t font_thintel, font_mono, font_oled;

bool render_menu(painter_device_t display, uint16_t width, uint16_t height) {
    static menu_state_t last_state;
    if (memcmp(&last_state, &state, sizeof(menu_state_t)) == 0) {
        return state.is_in_menu;
    }

    state.dirty = false;
    memcpy(&last_state, &state, sizeof(menu_state_t));

    if (state.is_in_menu) {
        qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);

        uint8_t       hue      = rgb_matrix_get_hue();
        menu_entry_t *menu     = get_current_menu();
        menu_entry_t *selected = get_selected_menu_item();

        int y = 80;
        qp_rect(display, 0, y, width, y + 3, hue, 255, 255, true);
        y += 8;
        qp_drawtext(display, 8, y, font_oled, menu->text);
        y += font_oled->line_height + 4;
        qp_rect(display, 0, y, width, y + 3, hue, 255, 255, true);
        y += 8;
        for (int i = 0; i < menu->parent.child_count; ++i) {
            menu_entry_t *child = &menu->parent.children[i];
            uint16_t      x;
            if (child == selected) {
                x = qp_drawtext_recolor(display, 8, y, font_oled, child->text, HSV_GREEN, 85, 255, 0);
            } else {
                x = qp_drawtext_recolor(display, 8, y, font_oled, child->text, HSV_RED, 0, 255, 0);
            }
            if (child->flags & menu_flag_is_parent) {
                qp_drawtext(display, 8 + x, y, font_oled, "  >");
            }
            if (child->flags & menu_flag_is_value) {
                char buf[32] = {0};
                child->child.display_handler(buf, sizeof(buf));
                qp_drawtext(display, 8 + x, y, font_oled, buf);
            }
            y += font_oled->line_height + 4;
            qp_rect(display, 0, y, width - 1, y, hue, 255, 255, true);
            y += 5;
        }
        return true;
    } else {
        // qp_rect(display, 0, 0, width - 1, height - 1, 0, 0, 0, true);
        return false;
    }
}
