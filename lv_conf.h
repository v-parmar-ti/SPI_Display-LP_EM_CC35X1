/**
 * lv_conf.h
 * LVGL v9 configuration for CC3551E / SPI_Display project.
 *
 * Place this file at the project root (same level as the lvgl/ submodule).
 * Build system must define LV_CONF_INCLUDE_SIMPLE so that lvgl.h resolves
 * this file with  #include "lv_conf.h"  rather than a relative path.
 */

#if 1 /* Set to 1 to enable LVGL configuration (do not remove this guard) */

#ifndef LV_CONF_H
#define LV_CONF_H

/*
 * Tell LVGL to skip its Kconfig discovery chain (lv_conf_kconfig.h).
 * Without this, lv_conf_internal.h tries to include lv_conf_kconfig.h
 * from the submodule's src/ directory, which can interfere with our
 * lv_conf.h being recognised as the active configuration.
 */
#define LV_KCONFIG_IGNORE

/*
 * Disable example builds. Without this, lv_conf_internal.h defaults
 * LV_BUILD_EXAMPLES to 1 (when not using Kconfig), which causes all
 * files under lvgl/examples/ to compile and produce errors on this
 * toolchain. Setting it to 0 makes those files compile to nothing.
 */
#define LV_BUILD_EXAMPLES 0

/*====================
 * COLOR SETTINGS
 *====================*/

/* Color depth matching ST7789/ST7735 RGB565 format */
#define LV_COLOR_DEPTH 16

/* Swap the bytes of RGB565 color: needed because SPI sends MSB first
 * and the display expects big-endian but LVGL produces little-endian
 * 16-bit words on most architectures. The lvgl_port flush callback
 * handles the swap manually, so keep this 0. */
#define LV_COLOR_16_SWAP 0

/*====================
 * MEMORY SETTINGS
 *====================*/

/* LVGL internal heap size (bytes).
 * CC3551E has 256 KB SRAM. Allocate 48 KB for LVGL.
 * Increase if you get "lv_mem: couldn't allocate memory" assertions. */
#define LV_MEM_SIZE (48 * 1024U)

/* Use built-in memory allocator */
#define LV_MEM_CUSTOM 0

/*====================
 * HAL SETTINGS
 *====================*/

/* Tick source: manual lv_tick_inc() calls from a FreeRTOS timer task.
 * See lvgl_port.c for the tick task implementation. */
#define LV_TICK_CUSTOM 0

/* Default display refresh period in ms (how often lv_timer_handler is called) */
#define LV_DEF_REFR_PERIOD 33  /* ~30 Hz refresh */

/* Dot-per-inch of the display for DPI-aware sizing */
#define LV_DPI_DEF 130

/*====================
 * DRAW SETTINGS
 *====================*/

/* Draw buffer strategy: LVGL renders into a partial buffer, then flushes.
 * The buffer size is defined in lvgl_port.c (LV_DRAW_BUF_LINES). */
#define LV_DRAW_SW_SHADOW_CACHE_SIZE 0

/* Enable SW rendering */
#define LV_USE_DRAW_SW 1

/*====================
 * LOGGING
 *====================*/

/* Disable logging in production to save code/RAM */
#define LV_USE_LOG 0

/* If you enable logging (LV_USE_LOG 1), set the level here:
 * LV_LOG_LEVEL_TRACE / INFO / WARN / ERROR / USER / NONE
 * Note: when LV_USE_LOG is 0, lv_conf_internal.h forces this to
 * LV_LOG_LEVEL_NONE unconditionally, so do not define it here. */

/*====================
 * ASSERTS
 *====================*/

/* Enable asserts during development; disable for release builds */
#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

/* Custom assert handler — map to while(1) halt for embedded debug */
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);

/*====================
 * FONTS
 *====================*/

/* Built-in fonts (Montserrat subset bitmaps, included in lvgl/src/font/).
 * Enable only what you need to keep flash usage low. */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 0
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Default font — used by widgets unless overridden in a style */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Enable built-in symbols (material design icons subset) */
#define LV_FONT_MONTSERRAT_12_SUBPX 0
#define LV_USE_FONT_PLACEHOLDER 1

/*====================
 * WIDGETS
 *====================*/

/* Core widgets (enable only what you use to minimize flash) */
#define LV_USE_ARC          1
#define LV_USE_BAR          1
#define LV_USE_BTN          1
#define LV_USE_BTNMATRIX    0
#define LV_USE_CANVAS       0
#define LV_USE_CHECKBOX     0
#define LV_USE_DROPDOWN     0
#define LV_USE_IMG          1
#define LV_USE_LABEL        1
#define LV_USE_LINE         1
#define LV_USE_ROLLER       0
#define LV_USE_SLIDER       1
#define LV_USE_SWITCH       0
#define LV_USE_TEXTAREA     0
#define LV_USE_TABLE        0

/*====================
 * THEMES
 *====================*/

/* Default theme */
#define LV_USE_THEME_DEFAULT    1
#define LV_THEME_DEFAULT_DARK   0    /* 0 = light, 1 = dark */
#define LV_THEME_DEFAULT_GROW   1

#define LV_USE_THEME_SIMPLE     0
#define LV_USE_THEME_MONO       0

/*====================
 * LAYOUTS
 *====================*/

#define LV_USE_FLEX  1
#define LV_USE_GRID  0

/*====================
 * EXTRA COMPONENTS
 *====================*/

#define LV_USE_ANIMIMG      0
#define LV_USE_CALENDAR     0
#define LV_USE_CHART        0
#define LV_USE_COLORWHEEL   0
#define LV_USE_IMGBTN       0
#define LV_USE_KEYBOARD     0
#define LV_USE_LED          1
#define LV_USE_LIST         0
#define LV_USE_MENU         0
#define LV_USE_METER        0
#define LV_USE_MSGBOX       0
#define LV_USE_SPAN         0
#define LV_USE_SPINBOX      0
#define LV_USE_SPINNER      1
#define LV_USE_TABVIEW      0
#define LV_USE_TILEVIEW     0
#define LV_USE_WIN          0

/*====================
 * SquareLine Studio
 *====================*/

/* SquareLine Studio exports use lv_i18n for translations.
 * Enable if your SquareLine project uses multi-language support. */
#define LV_USE_I18N 0

/*====================
 * MISC
 *====================*/

/* Garbage collector: not needed on bare-metal */
#define LV_ENABLE_GC 0

/* Enable sprintf-based number formatting in labels */
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/* Image caching: cache decoded images in heap.
 * Set to 0 to disable and save RAM. */
#define LV_IMG_CACHE_DEF_SIZE 0

/* Compiler attribute for large constant arrays in flash */
#define LV_ATTRIBUTE_LARGE_CONST __attribute__((section(".rodata")))

/* Place draw buffers in specific RAM section if needed.
 * Leave empty to use default heap. */
#define LV_ATTRIBUTE_MEM_FAST

#endif /* LV_CONF_H */
#endif /* lv_conf.h end */
