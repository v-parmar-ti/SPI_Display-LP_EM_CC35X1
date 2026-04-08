/*
 * Copyright (c) 2018-2024, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== spi_display.c ========
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

/* Driver Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/SPI.h>
#include <ti/display/Display.h>

/* Driver configuration */
#include "ti_drivers_config.h"
#include "display_config.h"

/* LCD driver selection - configure in display_config.h */
#ifdef USE_ST7789
#include "st7789_lcd.h"
#elif defined(USE_ST7735)
#include "st7735_lcd.h"
#else
#error "No display configured! Edit display_config.h to select ST7789 or ST7735"
#endif

#define THREADSTACKSIZE (1024)

#ifdef DeviceFamily_CC35XX
    #define CONFIG_GPIO_LED_0 GPIO_INVALID_INDEX
    #define CONFIG_GPIO_LED_1 GPIO_INVALID_INDEX
#endif

static Display_Handle display;

/* ========== Animated Pattern Demo ========== */
static uint32_t frame_count = 0;

/* Color palette */
static const uint16_t color_palette[] = {
    0xF800,  /* RED */
    0xF840,  /* YELLOW */
    0x07E0,  /* GREEN */
    0x07FF,  /* CYAN */
    0x001F,  /* BLUE */
    0xF81F   /* MAGENTA */
};
#define PALETTE_SIZE (sizeof(color_palette) / sizeof(color_palette[0]))

/* Simple color bar animation - just cycles through colors filling the screen */
void draw_color_bar_pattern(void)
{
    int color_idx = (frame_count / 30) % PALETTE_SIZE;
    uint16_t color = color_palette[color_idx];
    LCD_fillScreen(color);
}

/*
 *  ======== displayThread ========
 */
void *displayThread(void *arg0)
{
    /* Initialize the LCD */
    LCD_init();

    Display_printf(display, 0, 0, "");
    Display_printf(display, 0, 0, "========================================");
    Display_printf(display, 0, 0, "SPI Display Color Animation Demo");
    Display_printf(display, 0, 0, "========================================");
    Display_printf(display, 0, 0, "MCU: CC3551E");
    #ifdef USE_ST7789
    Display_printf(display, 0, 0, "Display: ST7789 (240x320 pixels)");
    #endif 
    #ifdef USE_ST7735
    Display_printf(display, 0, 0, "Display: ST7733 (128x128 pixels)");
    #endif
    Display_printf(display, 0, 0, "Status: Initializing...");
    Display_printf(display, 0, 0, "========================================");
    Display_printf(display, 0, 0, "");

    /* Demo counters */
    uint32_t last_logged_frame = 0;

    /* Target frame rate: 24 FPS = ~41.67ms per frame */
    uint32_t target_frame_ms = 40;  /* milliseconds per frame */
    struct timespec frame_start, frame_end, demo_start_time;
    uint32_t frame_elapsed_ms;
    uint64_t total_elapsed_ms;

    /* Record demo start time for accurate runtime reporting */
    clock_gettime(CLOCK_MONOTONIC, &demo_start_time);

    /* Animation loop */
    while (1)
    {
        /* Get frame start time */
        clock_gettime(CLOCK_MONOTONIC, &frame_start);

        /* Draw simple color cycling pattern (no need to clear, pattern fills entire screen) */
        draw_color_bar_pattern();

        /* Increment local frame counter */
        frame_count++;

        /* Log statistics every 100 frames */
        if (frame_count - last_logged_frame >= 100) {
            last_logged_frame = frame_count;

            clock_gettime(CLOCK_MONOTONIC, &frame_end);
            total_elapsed_ms = (frame_end.tv_sec - demo_start_time.tv_sec) * 1000 +
                               (frame_end.tv_nsec - demo_start_time.tv_nsec) / 1000000;

            double elapsed_seconds = total_elapsed_ms / 1000.0;
            double fps = (elapsed_seconds > 0) ? frame_count / elapsed_seconds : 0;

            /* Print status update */
            Display_printf(display, 0, 0, "");
            Display_printf(display, 0, 0, "Status: Runtime: %.1f sec | Frames: %d | FPS: %.2f",
                          elapsed_seconds, frame_count, fps);
        }

        /* Frame rate limiting: maintain consistent 24 FPS */
        clock_gettime(CLOCK_MONOTONIC, &frame_end);
        frame_elapsed_ms = (frame_end.tv_sec - frame_start.tv_sec) * 1000 +
                           (frame_end.tv_nsec - frame_start.tv_nsec) / 1000000;

        if (frame_elapsed_ms < target_frame_ms) {
            uint32_t sleep_ms = target_frame_ms - frame_elapsed_ms;
            usleep(sleep_ms * 1000);
        }
    }

    return (NULL);
}

/*
 *  ======== mainThread ========
 */
void *mainThread(void *arg0)
{
    pthread_t thread0;
    pthread_attr_t attrs;
    struct sched_param priParam;
    int retc;
    int detachState;

    /* Call driver init functions. */
    Display_init();
    GPIO_init();
    SPI_init();

    /* Configure the LED pins */
    GPIO_setConfig(CONFIG_GPIO_LED_0, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LED_1, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);

    /* Configure LCD control pins as outputs */
    GPIO_setConfig(CONFIG_GPIO_LCD_CS, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);
    GPIO_setConfig(CONFIG_GPIO_LCD_DC, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_LCD_RST, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_HIGH);

    /* Open the display for output */
    display = Display_open(Display_Type_UART, NULL);
    if (display == NULL)
    {
        /* Failed to open display driver */
        while (1) {}
    }

    /* Turn on user LED */
    GPIO_write(CONFIG_GPIO_LED_0, CONFIG_GPIO_LED_ON);


    /* Create application threads */
    pthread_attr_init(&attrs);

    detachState = PTHREAD_CREATE_DETACHED;
    /* Set priority and stack size attributes */
    retc        = pthread_attr_setdetachstate(&attrs, detachState);
    if (retc != 0)
    {
        /* pthread_attr_setdetachstate() failed */
        while (1) {}
    }

    retc |= pthread_attr_setstacksize(&attrs, THREADSTACKSIZE);
    if (retc != 0)
    {
        /* pthread_attr_setstacksize() failed */
        while (1) {}
    }

    /* Create display thread */
    priParam.sched_priority = 1;
    pthread_attr_setschedparam(&attrs, &priParam);

    retc = pthread_create(&thread0, &attrs, displayThread, NULL);
    if (retc != 0)
    {
        /* pthread_create() failed */
        while (1) {}
    }

    return (NULL);
}
