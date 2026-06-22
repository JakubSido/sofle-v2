/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * nice!view (MIP / Sharp memory LCD) VCOM keepalive.
 *
 * Sharp memory-in-pixel displays need their VCOM polarity toggled periodically
 * or DC bias builds up and the image decays into garbage ("messy rows"). The
 * Zephyr ls0xx driver only toggles VCOM when it sends an SPI frame, and ZMK
 * only sends a frame when an LVGL widget is invalidated. The nice!view shield
 * wires no extcomin-gpios, so there is no hardware VCOM toggle to fall back on.
 * On a split *peripheral* the nice!view widget updates very rarely (no
 * WPM/clock, unlike the central/dongle), so VCOM nearly stops toggling and the
 * screen decays until the next redraw - and a partial redraw only refreshes the
 * dirty rows, leaving the decayed garbage on the rest.
 *
 * This module forces a full-screen redraw on a fixed interval, which makes the
 * ls0xx driver re-issue a frame and toggle VCOM, keeping the image clean. The
 * work is scheduled on the ZMK display work queue, so it is serialized with
 * lv_task_handler() and is safe to call LVGL from.
 *
 * Enabled in config/sofle.conf via CONFIG_NICE_VIEW_VCOM_KEEPALIVE=y. The
 * dongle disables CONFIG_ZMK_DISPLAY, so the option (which depends on it) stays
 * off there.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>

#include <lvgl.h>

#include <zmk/display.h>

#define KEEPALIVE_INTERVAL K_MSEC(CONFIG_NICE_VIEW_VCOM_KEEPALIVE_INTERVAL_MS)

/* nice!view panel width in pixels (Sharp LS011B7DH03). The strip we invalidate
 * spans the full width because the ls0xx driver only accepts full-width writes. */
#define LS0XX_STRIP_WIDTH 160

static void vcom_keepalive_tick(struct k_work *work);
static K_WORK_DELAYABLE_DEFINE(vcom_keepalive_work, vcom_keepalive_tick);

static void vcom_keepalive_tick(struct k_work *work) {
    if (zmk_display_is_initialized()) {
        /* Force a SMALL real redraw, not a full-screen one. A full-screen
         * redraw (lv_obj_invalidate(lv_scr_act())) blanks this nice!view - the
         * same failure mode as CONFIG_LV_Z_FULL_REFRESH on the stock panel.
         *
         * We don't need a full redraw: VCOM is a global panel property, and the
         * ls0xx driver toggles it on ANY SPI frame it sends. So invalidating a
         * thin full-width strip makes the driver re-send just those rows'
         * current pixels (no visible change) while toggling VCOM for the whole
         * panel - which is what stops the decay/garbage. The ls0xx rounder
         * forces writes to full width anyway, so a few-row strip is the minimum
         * real write. */
        lv_area_t area = {.x1 = 0, .y1 = 0, .x2 = LS0XX_STRIP_WIDTH - 1, .y2 = 7};
        lv_obj_invalidate_area(lv_scr_act(), &area);
    }

    k_work_schedule_for_queue(zmk_display_work_q(), &vcom_keepalive_work, KEEPALIVE_INTERVAL);
}

static int vcom_keepalive_init(void) {
    k_work_schedule_for_queue(zmk_display_work_q(), &vcom_keepalive_work, KEEPALIVE_INTERVAL);
    return 0;
}

SYS_INIT(vcom_keepalive_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
