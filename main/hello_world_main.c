#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "CAMPO_IOT";

// ==== CONFIGURACIÓN DE HARDWARE ====
// Usamos el ADC1 del ESP32-C6
#define ADC_UNIT            ADC_UNIT_1

// Canales ADC (ESP32-C6 DevKitC-1: ADC1_CH0=GPIO0, CH1=GPIO1, CH2=GPIO2, CH3=GPIO3)
#define LDR_ADC_CHANNEL     ADC_CHANNEL_1   // LDR en GPIO1
#define RAIN_ADC_CHANNEL    ADC_CHANNEL_2   // AO lluvia en GPIO2
#define SOIL_ADC_CHANNEL    ADC_CHANNEL_3   // AO suelo en GPIO3

// GPIO digital para DO del MH-RD (lluvia SI/NO)
#define RAIN_DO_GPIO        10             // DO lluvia en GPIO10

// Handle global del ADC
static adc_oneshot_unit_handle_t adc_handle;

// ==== Inicialización del ADC en modo oneshot ====
static void adc_init(void)
{
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = ADC_ATTEN_DB_11,   // hasta ~3.3 V
    };

    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, LDR_ADC_CHANNEL,  &chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, RAIN_ADC_CHANNEL, &chan_cfg));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, SOIL_ADC_CHANNEL, &chan_cfg));

    ESP_LOGI(TAG, "ADC inicializado (LDR, lluvia, suelo)");
}

// ==== Inicialización del pin digital de lluvia (DO) ====
static void rain_do_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = 1ULL << RAIN_DO_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io_conf));
    ESP_LOGI(TAG, "GPIO lluvia DO inicializado (GPIO%d)", RAIN_DO_GPIO);
}

// ==== Tarea que lee periódicamente los sensores ====
static void sensors_task(void *arg)
{
    while (1) {
        int raw_ldr  = 0;
        int raw_rain = 0;
        int raw_soil = 0;
        int rain_do  = 0;

        // LDR (ADC)
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, LDR_ADC_CHANNEL, &raw_ldr));

        // Lluvia (AO, ADC)
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, RAIN_ADC_CHANNEL, &raw_rain));

        // Suelo (AO, ADC)
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, SOIL_ADC_CHANNEL, &raw_soil));

        // Lluvia (DO, digital: 0 = mojado, 1 = seco normalmente)
        rain_do = gpio_get_level(RAIN_DO_GPIO);

        ESP_LOGI(TAG, "LDR   raw = %d", raw_ldr);
        ESP_LOGI(TAG, "RAIN  raw = %d  (AO), DO = %d", raw_rain, rain_do);
        ESP_LOGI(TAG, "SOIL  raw = %d", raw_soil);
        ESP_LOGI(TAG, "--------------------------------------\n");

        vTaskDelay(pdMS_TO_TICKS(2000));   // cada 2 s
    }
}

// ==== app_main: punto de entrada ====
void app_main(void)
{
    // NVS requerido por varios componentes del IDF (Wi-Fi, etc.)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_LOGI(TAG, "Arrancando CAMPO IOT (LDR + lluvia + suelo)");

    adc_init();
    rain_do_init();

    // Creamos la tarea que lee sensores
    xTaskCreate(sensors_task, "sensors_task", 4096, NULL, 5, NULL);
}
