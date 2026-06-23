# Copyright (c) 2024 The ZMK Contributors
# SPDX-License-Identifier: MIT

target_sources_ifdef(CONFIG_NICE_VIEW_VCOM_KEEPALIVE app PRIVATE src/vcom_keepalive.c)

target_sources_ifdef(CONFIG_DEDA_DISPLAY app PRIVATE
    src/deda_display/custom_status_screen.c
    src/deda_display/peripheral_status.c
    src/deda_display/util.c
    src/deda_display/deda_art.c)
