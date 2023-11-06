// Copyright 2023 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#define CIRQUE_PINNACLE_SPI_DIVISOR 8
#define CIRQUE_PINNACLE_SPI_CS_PIN  POINTING_DEVICE_CS_PIN

#define CIRQUE_PINNACLE_TAP_ENABLE
#define POINTING_DEVICE_GESTURES_CURSOR_GLIDE_ENABLE
#define POINTING_DEVICE_GESTURES_SCROLL_ENABLE

#define OLED_DISPLAY_128X128
#define OLED_UPDATE_PROCESS_LIMIT 2

#define ENCODER_RESOLUTION 2

#define I2C_DRIVER       I2CD1
#define I2C1_SDA_PIN     GP2
#define I2C1_SCL_PIN     GP3
#define I2C1_CLOCK_SPEED 400000

#undef RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 750U

#define SERIAL_USART_FULL_DUPLEX
#define SERIAL_USART_PIN_SWAP
#define SERIAL_USART_FULL_DUPLEX // Enable full duplex operation mode.
#define SERIAL_USART_RX_PIN      GP13

#define QP_DC    GP11
#define QP_CS    GP12
#define QP_RESET GP13

#define DILEMMA_DEBUG_1 GP16
#define DILEMMA_DEBUG_2 GP18
