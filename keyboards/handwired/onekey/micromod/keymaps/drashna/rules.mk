ifeq ($(strip $(KEYBOARD)), handwired/onekey/micromod/stm32f405)
    BOOTLOADER = tinyuf2
endif
OLED_ENABLE = yes
HAPTIC_ENABLE = yes
HAPTIC_DRIVER = drv2605l
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = is31fl3741
ENCODER_ENABLE = yes
ENCODER_MAP_ENABLE = yes
RTC_ENABLE = yes
RTC_DRIVER = pcf8523
WPM_ENABLE = yes
CONSOLE_ENABLE = yes
BLUETOOTH_DRIVER = bluefruit_le
