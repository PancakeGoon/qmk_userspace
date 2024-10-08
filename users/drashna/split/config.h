// Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define SPLIT_TRANSPORT_MIRROR
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_LED_STATE_ENABLE
#define SPLIT_MODS_ENABLE
#ifdef SPLIT_USB_DETECT
#    define SPLIT_WATCHDOG_ENABLE
#endif
#define SPLIT_WPM_ENABLE
#define SPLIT_ACTIVITY_ENABLE
#define SPLIT_DETECTED_OS_ENABLE
#define SPLIT_HAPTIC_ENABLE
#ifdef SPLIT_OLED_ENABLE
#    undef SPLIT_OLED_ENABLE
#endif
#if defined(__AVR__) && !defined(SELECT_SOFT_SERIAL_SPEED)
#    define SELECT_SOFT_SERIAL_SPEED 1
#endif
#ifdef CUSTOM_SPLIT_TRANSPORT_SYNC
#    define SPLIT_TRANSACTION_IDS_USER                                                                         \
        RPC_ID_USER_STATE_SYNC, RPC_ID_USER_KEYMAP_SYNC, RPC_ID_USER_CONFIG_SYNC, RPC_ID_USER_AUTOCORRECT_STR, \
            RPC_ID_USER_DISPLAY_KEYLOG_STR, RPC_ID_USER_SUSPEND_STATE_SYNC

// autocorrect and the like require larger data sets, so we need to increase the buffer size
#    define RPC_M2S_BUFFER_SIZE 64
#    define RPC_S2M_BUFFER_SIZE 64
#endif
