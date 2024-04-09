/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

static const char *TAG = "example";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static uint8_t s_led_state = 1;

#ifdef CONFIG_BLINK_LED_STRIP

static led_strip_handle_t led_strip;

static void blink_led(uint8_t red,uint8_t green,uint8_t blue)
{
    /* If the addressable LED is enabled */
    if (s_led_state) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        led_strip_set_pixel(led_strip, 0, red, green, blue);
        /* Refresh the strip to send data */
        led_strip_refresh(led_strip);
    } else {
        /* Set all LED off to clear all pixels */
        led_strip_clear(led_strip);
    }
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink addressable LED!");
    /* LED strip initialization with the GPIO and pixels number*/
    led_strip_config_t strip_config = {
        .strip_gpio_num = BLINK_GPIO,
        .max_leds = 1, // at least one LED on board
    };
#if CONFIG_BLINK_LED_STRIP_BACKEND_RMT
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
        .flags.with_dma = false,
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
#elif CONFIG_BLINK_LED_STRIP_BACKEND_SPI
    led_strip_spi_config_t spi_config = {
        .spi_bus = SPI2_HOST,
        .flags.with_dma = true,
    };
    ESP_ERROR_CHECK(led_strip_new_spi_device(&strip_config, &spi_config, &led_strip));
#else
#error "unsupported LED strip backend"
#endif
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

#elif CONFIG_BLINK_LED_GPIO

static void blink_led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(BLINK_GPIO, s_led_state);
}

static void configure_led(void)
{
    ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

#else
#error "unsupported LED type"
#endif


void app_main(void)
{
    /* Configure the peripheral according to the LED type */
    configure_led();
    while (1) {
        uint16_t hue = 0;
        for (int i = 0; i < 360; i++) {
            led_strip_set_pixel_hsv(led_strip, 0, hue, 255, 10);
            led_strip_refresh(led_strip);
            vTaskDelay(pdMS_TO_TICKS(50)); // 50ms delay for smooth transition
            hue++;
            if (hue == 255) hue = 0;
        }
    }
}

void app_main2(void)
{

    /* Configure the peripheral according to the LED type */
    configure_led();

    while (1) {
        
        //blink_led();
        for (int i = 0; i < 255; i++) {
            // 计算红、绿、蓝三个通道的颜色值，实现渐变效果
            uint8_t red = i;
            uint8_t green = 255 - red;
            uint8_t blue = 0;
            ESP_LOGI(TAG, "Turning the LED %s!,r=%d,g=%d", s_led_state == true ? "ON" : "OFF",red,green);
            // 设置 LED 的颜色
            blink_led( red, green, blue);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        
        /* Toggle the LED state */
        //s_led_state = !s_led_state;
        //vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }
}
void app_main3(void)
{
    uint8_t red = 255, green = 0, blue = 0;
    //int direction = 1;
    /* Configure the peripheral according to the LED type */
    configure_led();
    while (1) {
        // 从红色到青色的过渡
        for (red = 255, green = 0, blue = 0; green < 255; green++) {
            // 设置 LED 的颜色
            blink_led( red, green, blue);
            // 延时一段时间，观察渐变效果
            vTaskDelay(pdMS_TO_TICKS(15));
            green++;
        }
        // 从青色到蓝色的过渡
        for (red = 255, green = 255, blue = 0; red > 0; red--) {
            // 设置 LED 的颜色
            blink_led(red, green, blue);
            // 延时一段时间，观察渐变效果
            vTaskDelay(pdMS_TO_TICKS(15));
            red--;
        }
        // 从蓝色到红色的过渡
        for (red = 0, green = 255, blue = 255; green > 0; green--) {
            // 设置 LED 的颜色
            blink_led( red, green, blue);
            // 延时一段时间，观察渐变效果
            vTaskDelay(pdMS_TO_TICKS(15));
            green--;
        }
    }
}
