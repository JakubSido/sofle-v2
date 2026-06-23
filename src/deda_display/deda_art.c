/*
 * nice!view art image, blanked to solid white per request. The image keeps its
 * original 140x68 INDEXED_1BIT format and is still drawn by peripheral_status.c,
 * but BOTH palette entries are white, so every pixel renders white regardless of
 * the bitmap bits (which are left zero). This leaves the left side of the screen
 * blank/white while the battery + connection status still shows on the right.
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_DEDA
#define LV_ATTRIBUTE_IMG_DEDA
#endif

/* 8 bytes palette (index 0 + index 1, both white) + 1224 bitmap bytes (140x68
 * at 1 bpp = ceil(140/8)*68 = 1224). The bitmap bytes are implicitly zeroed; it
 * does not matter since both palette colors are white. */
const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_DEDA uint8_t deda_map[1232] = {
    0xff, 0xff, 0xff, 0xff, /*Color of index 0 -> white*/
    0xff, 0xff, 0xff, 0xff, /*Color of index 1 -> white*/
};

const lv_img_dsc_t deda = {
    .header.cf = LV_IMG_CF_INDEXED_1BIT,
    .header.always_zero = 0,
    .header.reserved = 0,
    .header.w = 140,
    .header.h = 68,
    .data_size = 1232,
    .data = deda_map,
};
