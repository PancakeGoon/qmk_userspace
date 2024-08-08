// Copyright 2021 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "action.h"
#include "drashna.h"
#include "keyboard.h"
#ifdef LAYER_MAP_ENABLE
#    include "layer_map.h"
#endif
#ifdef DISPLAY_DRIVER_ENABLE
#    include "display/display.h"
#endif // DISPLAY_DRIVER_ENABLE
#ifdef QUANTUM_PAINTER_ENABLE
#    include "display/painter/painter.h"
#endif

#ifdef CUSTOM_DYNAMIC_MACROS_ENABLE
#    include "keyrecords/custom_dynamic_macros.h"
#endif
#ifdef I2C_SCANNER_ENABLE
void housekeeping_task_i2c_scanner(void);
void keyboard_post_init_i2c(void);
#endif
#ifdef RTC_ENABLE
#    include "rtc.h"
#endif
#ifdef CUSTOM_UNICODE_ENABLE
void keyboard_post_init_unicode(void);
#endif
#ifdef WATCHDOG_ENABLE
#    include "watchdog.h"
#endif
#if defined(LAYER_LOCK_ENABLE) && defined(LAYER_LOCK_IDLE_TIMEOUT)
#    include "layer_lock.h"
#endif

user_runtime_config_t user_state;

__attribute__((weak)) void keyboard_pre_init_keymap(void) {}
void                       keyboard_pre_init_user(void) {
    eeconfig_read_user_config(&userspace_config.raw);
    if (!userspace_config.check) {
        eeconfig_init_user();
    }
    keyboard_pre_init_keymap();
}
// Add reconfigurable functions here, for keymap customization
// This allows for a global, userspace functions, and continued
// customization of the keymap.  Use _keymap instead of _user
// functions in the keymaps
// Call user matrix init, set default RGB colors and then
// call the keymap's init function

#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
void keyboard_post_init_qp(void);
#endif

#if defined(OS_DETECTION_ENABLE) && defined(DEFERRED_EXEC_ENABLE)
uint32_t startup_exec(uint32_t trigger_time, void *cb_arg);
#endif

__attribute__((weak)) void keyboard_post_init_keymap(void) {}
void                       keyboard_post_init_user(void) {
#ifdef DISPLAY_DRIVER_ENABLE
    keyboard_post_init_display_driver();
#endif // DISPLAY_DRIVER_ENABLE
#if defined(CUSTOM_RGBLIGHT)
    keyboard_post_init_rgb_light();
#endif
#if defined(CUSTOM_RGB_MATRIX)
    keyboard_post_init_rgb_matrix();
#endif
#if defined(SPLIT_KEYBOARD) && defined(SPLIT_TRANSACTION_IDS_USER)
    keyboard_post_init_transport_sync();
#endif
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    keyboard_post_init_quantum_painter();
#endif
#ifdef I2C_SCANNER_ENABLE
    keyboard_post_init_i2c();
#endif
#ifdef CUSTOM_UNICODE_ENABLE
    keyboard_post_init_unicode();
#endif

#ifdef DEBUG_MATRIX_SCAN_RATE_ENABLE
    userspace_config.matrix_scan_print = true;
#endif

#if defined(BOOTLOADER_CATERINA) && defined(__AVR__) && defined(__AVR_ATmega32U4__)
    DDRD &= ~(1 << 5);
    PORTD &= ~(1 << 5);

    DDRB &= ~(1 << 0);
    PORTB &= ~(1 << 0);
#endif
#ifdef CUSTOM_DYNAMIC_MACROS_ENABLE
    dynamic_macro_init();
#endif
#ifdef RTC_ENABLE
    rtc_init();
#endif
#ifdef WATCHDOG_ENABLE
    watchdog_init();
#endif

    keyboard_post_init_keymap();
}

__attribute__((weak)) bool shutdown_keymap(bool jump_to_bootloader) {
    return true;
}

bool shutdown_user(bool jump_to_bootloader) {
#ifdef WATCHDOG_ENABLE
    watchdog_shutdown();
#endif
    if (!shutdown_keymap(jump_to_bootloader)) {
        return false;
    }
#ifdef RGBLIGHT_ENABLE
    rgblight_shutdown(jump_to_bootloader);
#endif // RGBLIGHT_ENABLE
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_shutdown(jump_to_bootloader);
#endif // RGB_MATRIX_ENABLE
#if defined(OLED_ENABLE) && defined(CUSTOM_OLED_DRIVER)
    oled_shutdown(jump_to_bootloader);
#endif
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    shutdown_quantum_painter();
#endif
    return true;
}

__attribute__((weak)) void suspend_power_down_keymap(void) {}

void suspend_power_down_user(void) {
    set_is_device_suspended(true);
#ifdef WATCHDOG_ENABLE
    suspend_power_down_watchdog();
#endif
    if (layer_state_is(_GAMEPAD)) {
        layer_off(_GAMEPAD);
    }
    if (layer_state_is(_DIABLO)) {
        layer_off(_DIABLO);
    }
    if (layer_state_is(_DIABLOII)) {
        layer_off(_DIABLOII);
    }
#ifdef OLED_ENABLE
    oled_off();
#endif

#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    suspend_power_down_quantum_painter();
#endif

    suspend_power_down_keymap();
}

__attribute__((weak)) void suspend_wakeup_init_keymap(void) {}
void                       suspend_wakeup_init_user(void) {
#ifdef WATCHDOG_ENABLE
    suspend_wakeup_init_watchdog();
#endif
    void last_matrix_activity_trigger(void);
    last_matrix_activity_trigger();

    set_is_device_suspended(false);
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    suspend_wakeup_init_quantum_painter();
#endif
    suspend_wakeup_init_keymap();
}

#ifdef AUDIO_ENABLE
float doom_song[][2] = SONG(E1M1_DOOM);
#endif

// on layer change, no matter where the change was initiated
// Then runs keymap's layer change check
__attribute__((weak)) layer_state_t layer_state_set_keymap(layer_state_t state) {
    return state;
}
layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, _RAISE, _LOWER, _ADJUST);
#if defined(CUSTOM_POINTING_DEVICE)
    state = layer_state_set_pointing(state);
#endif
#if defined(CUSTOM_RGBLIGHT)
    state = layer_state_set_rgb_light(state);
#endif // CUSTOM_RGBLIGHT
#if defined(AUDIO_ENABLE)
    static bool is_gamepad_on = false;
    if (layer_state_cmp(state, _GAMEPAD) != is_gamepad_on) {
        static bool is_click_on = false;
        is_gamepad_on           = layer_state_cmp(state, _GAMEPAD);
        if (is_gamepad_on) {
            is_click_on = is_clicky_on();
            if (is_click_on) {
                clicky_off();
            }
            PLAY_LOOP(doom_song);
        } else {
            if (is_click_on) {
                clicky_on();
            }
            audio_stop_all();
        }
    }
#endif
    state = layer_state_set_keymap(state);

#ifndef NO_PRINT
    char layer_buffer[16 + 5];
    format_layer_bitmap_string(layer_buffer, state, default_layer_state);
    dprintf("layer state: %s\n", layer_buffer);
#endif

#ifdef LAYER_MAP_ENABLE
    set_layer_map();
#endif

#ifdef SWAP_HANDS_ENABLE
    if (is_gaming_layer_active(state) && is_swap_hands_on()) {
        swap_hands_off();
    }
#endif
    return state;
}

// Runs state check and changes underglow color and animation
__attribute__((weak)) layer_state_t default_layer_state_set_keymap(layer_state_t state) {
    return state;
}

#if defined(AUDIO_ENABLE) && defined(DEFAULT_LAYER_SONGS)
static float default_layer_songs[][16][2] = DEFAULT_LAYER_SONGS;
#endif

layer_state_t default_layer_state_set_user(layer_state_t state) {
    if (!is_keyboard_master()) {
        return state;
    }

    state = default_layer_state_set_keymap(state);
#if defined(CUSTOM_RGBLIGHT)
    state = default_layer_state_set_rgb_light(state);
#endif

    static bool has_init_been_ran = false;
    // We don't want to run this the first time it's called, since it's read from eeeprom and called
    // as part of the startup process. But after that, it's okay.
    if (has_init_been_ran) {
#if defined(AUDIO_ENABLE) && defined(DEFAULT_LAYER_SONGS)
        if (get_highest_layer(state) < MAX_LAYER) {
            PLAY_SONG(default_layer_songs[get_highest_layer(state)]);
        }
#endif
        eeconfig_update_default_layer(state);
    } else {
        has_init_been_ran = true;
    }

#ifdef LAYER_MAP_ENABLE
    set_layer_map();
#endif
    return state;
}

__attribute__((weak)) void led_set_keymap(uint8_t usb_led) {}
void                       led_set_user(uint8_t usb_led) {
    led_set_keymap(usb_led);
}

__attribute__((weak)) void eeconfig_init_keymap(void) {}
void                       eeconfig_init_user(void) {
    userspace_config.raw              = 0;
    userspace_config.rgb_layer_change = true;
    userspace_config.check            = true;
#if defined(OLED_ENABLE)
    userspace_config.oled_brightness = OLED_BRIGHTNESS;
#else
    userspace_config.oled_brightness = 255;
#endif

    // ensure that nkro is enabled
    keymap_config.raw  = eeconfig_read_keymap();
    keymap_config.nkro = true;
    eeconfig_update_keymap(keymap_config.raw);

    eeconfig_update_user_config(&userspace_config.raw);
    eeconfig_init_keymap();
}

void eeconfig_init_user_datablock(void) {
#if (EECONFIG_USER_DATA_SIZE) > 4
    uint8_t eeconfig_empty_temp[(EECONFIG_USER_DATA_SIZE)-4] = {0};
    eeconfig_update_user_data(eeconfig_empty_temp);
#endif
}

void matrix_scan_user(void) {
    matrix_scan_rate_task();
}

#ifdef SPLIT_KEYBOARD
__attribute__((weak)) void matrix_slave_scan_keymap(void) {}
void                       matrix_slave_scan_user(void) {
    matrix_scan_rate_task();
#    if defined(AUDIO_ENABLE)
#        ifdef AUDIO_INIT_DELAY
    if (!is_keyboard_master()) {
        static bool     delayed_tasks_run  = false;
        static uint16_t delayed_task_timer = 0;
        if (!delayed_tasks_run) {
            if (!delayed_task_timer) {
                delayed_task_timer = timer_read();
            } else if (timer_elapsed(delayed_task_timer) > 300) {
                audio_startup();
                delayed_tasks_run = true;
            }
        }
    }
#        endif
#    endif
    matrix_slave_scan_keymap();
}
#endif

__attribute__((weak)) void housekeeping_task_keymap(void) {}
void                       housekeeping_task_user(void) {
    if (is_keyboard_master()) {
#ifdef AUDIO_ENABLE
        user_state.audio_enable        = is_audio_on();
        user_state.audio_clicky_enable = is_clicky_on();
#endif
#if defined(POINTING_DEVICE_ENABLE) && defined(POINTING_DEVICE_AUTO_MOUSE_ENABLE)
        user_state.tap_toggling = get_auto_mouse_toggle();
#endif
#ifdef UNICODE_COMMON_ENABLE
        user_state.unicode_mode        = unicode_config.input_mode;
        user_state.unicode_typing_mode = unicode_typing_mode;
#endif
#ifdef SWAP_HANDS_ENABLE
        user_state.swap_hands = swap_hands;
#endif
        user_state.host_driver_disabled = get_keyboard_lock();
#ifdef CAPS_WORD_ENABLE
        user_state.is_caps_word = is_caps_word_on();
#endif
    }

#ifdef WATCHDOG_ENABLE
    watchdog_task();
#endif // WATCHDOG_ENABLE
#if defined(LAYER_LOCK_ENABLE) && defined(LAYER_LOCK_IDLE_TIMEOUT)
    layer_lock_task();
#endif
#if defined(CUSTOM_TAP_DANCE_ENABLE) // Run Diablo 3 macro checking code.
    run_diablo_macro_check();
#endif // CUSTOM_TAP_DANCE_ENABLE
#if defined(CUSTOM_RGB_MATRIX)
    housekeeping_task_rgb_matrix();
#endif
#ifdef I2C_SCANNER_ENABLE
    housekeeping_task_i2c_scanner();
#endif
#ifdef CUSTOM_OLED_DRIVER
    housekeeping_task_oled();
#endif
#if defined(SPLIT_KEYBOARD) && defined(SPLIT_TRANSACTION_IDS_USER)
    housekeeping_task_transport_sync();
#endif
#ifdef CUSTOM_QUANTUM_PAINTER_ENABLE
    housekeeping_task_quantum_painter();
#endif
#ifdef RTC_ENABLE
    rtc_task();
#endif
#ifdef SENTENCE_CASE_ENABLE
    sentence_case_task();
#endif
#ifdef SELECT_WORD_ENABLE
    select_word_task();
#endif
#ifdef ACHORDION_ENABLE
    achordion_task();
#endif
#ifdef ORBITAL_MOUSE_ENABLE
    orbital_mouse_task();
#endif
#ifdef LAYER_MAP_ENABLE
    housekeeping_task_layer_map();
#endif
    housekeeping_task_keymap();
}
